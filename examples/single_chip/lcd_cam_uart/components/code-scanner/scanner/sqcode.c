/*Copyright (C) 2018 Javier Serrano Polo <javier@jasp.net>
  You can redistribute this library and/or modify it under the terms of the
   GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2.1 of the License, or (at your option) any later
   version.*/
#include "sqcode.h"

#include <stdbool.h>
#include <stdlib.h>

#include "image.h"
#include "img_scanner.h"

typedef enum {
    SHAPE_DOT,
    SHAPE_CORNER,
    SHAPE_OTHER,
    SHAPE_VOID
} shape_t;

typedef struct {
    float x;
    float y;
} sq_point;

typedef struct {
    shape_t type;
    unsigned x0;
    unsigned y0;
    unsigned width;
    unsigned height;
    sq_point center;
} sq_dot;

struct sq_reader {
    bool enabled;
};

/*Initializes a client reader handle.*/
static void sq_reader_init(sq_reader *reader)
{
    reader->enabled = true;
}

/*Allocates a client reader handle.*/
sq_reader *_zbar_sq_create(void)
{
    // sq_reader *reader = malloc(sizeof(sq_reader));
    sq_reader *reader = heap_caps_malloc(sizeof(sq_reader), MALLOC_CAP_SPIRAM);
    if (reader)
        sq_reader_init(reader);
    return reader;
}

/*Frees a client reader handle.*/
void _zbar_sq_destroy(sq_reader *reader)
{
    free(reader);
}

/* reset finder state between scans */
void _zbar_sq_reset (sq_reader *reader)
{
    reader->enabled = true;
}

int _zbar_sq_new_config(sq_reader *reader,
                        unsigned config)
{
    reader->enabled = config;
    return 0;
}

static const char base64_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
	'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

static char *base64_encode_buffer(const char *s, size_t size) {
	size_t encoded_size = (size + 2) / 3 * 4 + 1;
	char *encoded = malloc(encoded_size);
	if (!encoded)
		return NULL;
	char *e = encoded;
	for (;;) {
		unsigned char c = (*s >> 2) & 0x3f;
		*e++ = base64_table[c];
		c = (*s++ << 4) & 0x30;
		if (!--size) {
			*e++ = base64_table[c];
			*e++ = '=';
			*e++ = '=';
			break;
		}
		c |= (*s >> 4) & 0x0f;
		*e++ = base64_table[c];
		c = (*s++ << 2) & 0x3c;
		if (!--size) {
			*e++ = base64_table[c];
			*e++ = '=';
			break;
		}
		c |= (*s >> 6) & 0x03;
		*e++ = base64_table[c];
		c = *s++ & 0x3f;
		*e++ = base64_table[c];
		if (!--size)
			break;
	}
	*e = '\0';
	return encoded;
}

static bool sq_extract_text(zbar_image_scanner_t *iscn,
                            const char *buf,
                            size_t len)
{
    zbar_symbol_t *sym = _zbar_image_scanner_alloc_sym(iscn, ZBAR_SQCODE, 0);
    sym->data = base64_encode_buffer(buf, len);
    if (!sym->data) {
        _zbar_image_scanner_recycle_syms(iscn, sym);
        return true;
    }
    size_t b64_len = (len + 2) / 3 * 4;
    sym->data_alloc = b64_len + 1;
    sym->datalen = b64_len;
    _zbar_image_scanner_add_sym(iscn, sym);
    return false;
}

static bool is_black_color(const unsigned char c)
{
    return c <= 0x7f;
}

static bool is_black(zbar_image_t *img, int x, int y)
{
    if (x < 0 || (unsigned) x >= img->width || y < 0
        || (unsigned) y >= img->height)
        return false;
    const unsigned char *data = img->data;
    return is_black_color(data[y * img->width + x]);
}

static void set_dot_center(sq_dot *dot, float x, float y)
{
    dot->center.x = x;
    dot->center.y = y;
}

static void sq_scan_shape(zbar_image_t *img, sq_dot *dot, int start_x,
    int start_y)
{
    if (!is_black(img, start_x, start_y)) {
        dot->type = SHAPE_VOID;
        dot->x0 = start_x;
        dot->y0 = start_y;
        dot->width = 0;
        dot->height = 0;
        set_dot_center(dot, start_x, start_y);
        return;
    }

    unsigned x0 = start_x;
    unsigned y0 = start_y;
    unsigned width = 1;
    unsigned height = 1;

new_point:
    for (int x = x0 - 1; x < (int) (x0 + width + 1); x++) {
        if (is_black(img, x, y0 - 1)) {
            y0 = y0 - 1;
            height++;
            goto new_point;
        }
        if (is_black(img, x, y0 + height)) {
            height++;
            goto new_point;
        }
    }
    for (int y = y0; y < (int) (y0 + height); y++) {
        if (is_black(img, x0 - 1, y)) {
            x0 = x0 - 1;
            width++;
            goto new_point;
        }
        if (is_black(img, x0 + width, y)) {
            width++;
            goto new_point;
        }
    }

    dot->x0 = x0;
    dot->y0 = y0;
    dot->width = width;
    dot->height = height;

    /* Is it a corner? */
    if (is_black(img, x0 + 0.25 * width, y0 + 0.25 * height)
        && !is_black(img, x0 + 0.75 * width, y0 + 0.25 * height)
        && !is_black(img, x0 + 0.25 * width, y0 + 0.75 * height)
        && is_black(img, x0 + 0.75 * width, y0 + 0.75 * height)) {
        dot->type = SHAPE_CORNER;
        set_dot_center(dot, x0 + 0.5 * width, y0 + 0.5 * height);
        return;
    }

    /* Set dot center */
    const unsigned char *data = img->data;
    unsigned x_sum = 0;
    unsigned y_sum = 0;
    unsigned total_weight = 0;
    for (int y = y0; y < (int) (y0 + height); y++) {
        for (int x = x0; x < (int) (x0 + width); x++) {
            if (!is_black(img, x, y))
                continue;
            unsigned char weight = 0xff - data[y * img->width + x];
            x_sum += weight * x;
            y_sum += weight * y;
            total_weight += weight;
        }
    }
    dot->type = SHAPE_DOT;
    set_dot_center(dot, x_sum / (float) total_weight + 0.5,
        y_sum / (float) total_weight + 0.5);

    /* TODO: Is it other shape? White hole? Really a dot? */
}

static void set_middle_point(sq_point *middle, const sq_point *start,
    const sq_point *end)
{
    middle->x = (start->x + end->x) / 2;
    middle->y = (start->y + end->y) / 2;
}

bool find_left_dot(zbar_image_t *img, sq_dot *dot, unsigned *found_x,
    unsigned *found_y)
{
    for (int y = dot->y0; y < (int) (dot->y0 + dot->height); y++) {
        for (int x = dot->x0 - 1; x >= (int) (dot->x0 - 2 * dot->width); x--) {
            if (is_black(img, x, y)) {
                *found_x = x;
                *found_y = y;
                return true;
            }
        }
    }
    return false;
}

bool find_right_dot(zbar_image_t *img, sq_dot *dot, unsigned *found_x,
    unsigned *found_y)
{
    for (int y = dot->y0; y < (int) (dot->y0 + dot->height); y++) {
        for (int x = dot->x0 + dot->width; x < (int) (dot->x0 + 3 * dot->width);
             x++) {
            if (is_black(img, x, y)) {
                *found_x = x;
                *found_y = y;
                return true;
            }
        }
    }
    return false;
}

bool find_bottom_dot(zbar_image_t *img, sq_dot *dot, unsigned *found_x,
    unsigned *found_y)
{
    for (int x = dot->x0 + dot->width - 1; x >= (int) dot->x0; x--) {
        for (int y = dot->y0 + dot->height;
             y < (int) (dot->y0 + 3 * dot->height); y++) {
            if (is_black(img, x, y)) {
                *found_x = x;
                *found_y = y;
                return true;
            }
        }
    }
    return false;
}

int _zbar_sq_decode (sq_reader *reader,
                     zbar_image_scanner_t *iscn,
                     zbar_image_t *img)
{
    if (!reader->enabled)
        return 0;

    if (img->format != fourcc('Y','8','0','0')) {
        fputs("Unexpected image format for SQCODE\n", stderr);
        return 1;
    }

    /* Starting pixel */
    unsigned scan_y;
    unsigned scan_x;
    for (scan_y = 0; scan_y < img->height; scan_y++) {
        for (scan_x = 0; scan_x < img->width; scan_x++) {
            if (is_black(img, scan_x, scan_y))
                goto found_start;
        }
    }
    return 1;

found_start: ;
    /* Starting dot */
    sq_dot start_dot;
    sq_scan_shape(img, &start_dot, scan_x, scan_y);

    bool start_corner = start_dot.type == SHAPE_CORNER;

    bool error = true;

    sq_point *top_border = NULL;
    sq_point *left_border = NULL;
    sq_point *right_border = NULL;
    sq_point *bottom_border = NULL;

    size_t border_len;
    if (start_corner) {
        border_len = 0;
    }
    else {
        border_len = 1;
        top_border = malloc(sizeof(sq_point));
        if (!top_border)
            return 1;
        top_border[0] = start_dot.center;
    }

    sq_dot top_left_dot = start_dot;
    while (find_left_dot(img, &top_left_dot, &scan_x, &scan_y)) {
        sq_scan_shape(img, &top_left_dot, scan_x, scan_y);
        if (top_left_dot.type != SHAPE_DOT)
            goto free_borders;
        if (border_len) {
            border_len += 2;
            void *ptr = realloc(top_border, border_len * sizeof(sq_point));
            if (!ptr)
                goto free_borders;
            top_border = ptr;
            for (size_t i = border_len - 1; i >= 2; i--)
                top_border[i] = top_border[i - 2];
            top_border[0] = top_left_dot.center;
            set_middle_point(&top_border[1], &top_border[0], &top_border[2]);
        }
        else {
            border_len = 1;
            top_border = malloc(sizeof(sq_point));
            if (!top_border)
                return 1;
            top_border[0] = top_left_dot.center;
        }
    }
    if (top_left_dot.type != SHAPE_DOT)
        goto free_borders;

    sq_dot top_right_dot = start_dot;
    if (!start_corner) {
        while (find_right_dot(img, &top_right_dot, &scan_x, &scan_y)) {
            sq_scan_shape(img, &top_right_dot, scan_x, scan_y);
            if (top_right_dot.type == SHAPE_CORNER)
                break;
            if (top_right_dot.type != SHAPE_DOT)
                goto free_borders;
            border_len += 2;
            void *ptr = realloc(top_border, border_len * sizeof(sq_point));
            if (!ptr)
                goto free_borders;
            top_border = ptr;
            top_border[border_len - 1] = top_right_dot.center;
            set_middle_point(&top_border[border_len - 2],
                &top_border[border_len - 3],
                &top_border[border_len - 1]);
        }
    }
    if (border_len < 3)
        goto free_borders;
    float inc_x = top_border[border_len - 1].x - top_border[border_len - 3].x;
    float inc_y = top_border[border_len - 1].y - top_border[border_len - 3].y;
    border_len += 3;
    void *ptr = realloc(top_border, border_len * sizeof(sq_point));
    if (!ptr)
        goto free_borders;
    top_border = ptr;
    top_border[border_len - 3].x = top_border[border_len - 4].x + 0.5 * inc_x;
    top_border[border_len - 3].y = top_border[border_len - 4].y + 0.5 * inc_y;
    top_border[border_len - 2].x = top_border[border_len - 4].x + inc_x;
    top_border[border_len - 2].y = top_border[border_len - 4].y + inc_y;
    top_border[border_len - 1].x = top_border[border_len - 4].x + 1.5 * inc_x;
    top_border[border_len - 1].y = top_border[border_len - 4].y + 1.5 * inc_y;

    left_border = malloc(border_len * sizeof(sq_point));
    if (!left_border)
        goto free_borders;
    left_border[0] = top_border[0];

    sq_dot bottom_left_dot = top_left_dot;
    size_t cur_len = 1;
    while (find_bottom_dot(img, &bottom_left_dot, &scan_x, &scan_y)) {
        sq_scan_shape(img, &bottom_left_dot, scan_x, scan_y);
        if (bottom_left_dot.type == SHAPE_CORNER)
            break;
        if (bottom_left_dot.type != SHAPE_DOT)
            goto free_borders;
        cur_len += 2;
        if (cur_len > border_len)
            goto free_borders;
        left_border[cur_len - 1] = bottom_left_dot.center;
        set_middle_point(&left_border[cur_len - 2],
            &left_border[cur_len - 3],
            &left_border[cur_len - 1]);
    }
    if (cur_len != border_len - 3 || bottom_left_dot.type != SHAPE_CORNER)
        goto free_borders;
    inc_x = left_border[cur_len - 1].x - left_border[cur_len - 3].x;
    inc_y = left_border[cur_len - 1].y - left_border[cur_len - 3].y;
    left_border[border_len - 3].x = left_border[border_len - 4].x + 0.5 * inc_x;
    left_border[border_len - 3].y = left_border[border_len - 4].y + 0.5 * inc_y;
    left_border[border_len - 2].x = left_border[border_len - 4].x + inc_x;
    left_border[border_len - 2].y = left_border[border_len - 4].y + inc_y;
    left_border[border_len - 1].x = left_border[border_len - 4].x + 1.5 * inc_x;
    left_border[border_len - 1].y = left_border[border_len - 4].y + 1.5 * inc_y;

    right_border = malloc(border_len * sizeof(sq_point));
    if (!right_border)
        goto free_borders;

    sq_dot bottom_right_dot = top_right_dot;
    cur_len = 3;
    while (find_bottom_dot(img, &bottom_right_dot, &scan_x, &scan_y)) {
        sq_scan_shape(img, &bottom_right_dot, scan_x, scan_y);
        if (bottom_right_dot.type != SHAPE_DOT)
            goto free_borders;
        if (cur_len == 3) {
            cur_len++;
            if (cur_len > border_len)
                goto free_borders;
            right_border[cur_len - 1] = bottom_right_dot.center;
        }
        else {
            cur_len += 2;
            if (cur_len > border_len)
                goto free_borders;
            right_border[cur_len - 1] = bottom_right_dot.center;
            set_middle_point(&right_border[cur_len - 2],
                &right_border[cur_len - 3],
                &right_border[cur_len - 1]);
        }
    }
    if (cur_len != border_len || border_len < 6)
        return 1;
    inc_x = right_border[5].x - right_border[3].x;
    inc_y = right_border[5].y - right_border[3].y;
    right_border[2].x = right_border[3].x - 0.5 * inc_x;
    right_border[2].y = right_border[3].y - 0.5 * inc_y;
    right_border[1].x = right_border[3].x - inc_x;
    right_border[1].y = right_border[3].y - inc_y;
    right_border[0].x = right_border[3].x - 1.5 * inc_x;
    right_border[0].y = right_border[3].y - 1.5 * inc_y;

    bottom_border = malloc(border_len * sizeof(sq_point));
    if (!bottom_border)
        goto free_borders;
    bottom_border[border_len - 1] = right_border[border_len - 1];

    sq_dot bottom_left2_dot = bottom_right_dot;
    size_t offset = border_len - 1;
    while (find_left_dot(img, &bottom_left2_dot, &scan_x, &scan_y)) {
        sq_scan_shape(img, &bottom_left2_dot, scan_x, scan_y);
        if (bottom_left2_dot.type == SHAPE_CORNER)
            break;
        if (bottom_left2_dot.type != SHAPE_DOT)
            goto free_borders;
        if (offset < 2)
            goto free_borders;
        offset -= 2;
        bottom_border[offset] = bottom_left2_dot.center;
        set_middle_point(&bottom_border[offset + 1],
            &bottom_border[offset],
            &bottom_border[offset + 2]);
    }
    if (offset != 3 || bottom_left2_dot.type != SHAPE_CORNER)
        goto free_borders;
    inc_x = bottom_border[5].x - bottom_border[3].x;
    inc_y = bottom_border[5].y - bottom_border[3].y;
    bottom_border[2].x = bottom_border[3].x - 0.5 * inc_x;
    bottom_border[2].y = bottom_border[3].y - 0.5 * inc_y;
    bottom_border[1].x = bottom_border[3].x - inc_x;
    bottom_border[1].y = bottom_border[3].y - inc_y;
    bottom_border[0].x = bottom_border[3].x - 1.5 * inc_x;
    bottom_border[0].y = bottom_border[3].y - 1.5 * inc_y;

    /* Size check */
    if (border_len < 8 + 2 * (1 + 2) || border_len > 65535)
        goto free_borders;
    size_t bit_side_len = border_len - 2 * (1 + 2);

    size_t bit_len = bit_side_len * bit_side_len;
    if (bit_len % 8)
        goto free_borders;
    size_t byte_len = bit_len / 8;
    char *buf = calloc(byte_len, sizeof(char));
    if (!buf)
        goto free_borders;

    size_t idx = 0;
    for (unsigned y = 3; y <= border_len - 4; y++) {
        for (unsigned x = 3; x <= border_len - 4; x++) {
            float bottom_weight = y / (float) (border_len - 1);
            float top_weight = 1 - bottom_weight;
            float right_weight = x / (float) (border_len - 1);;
            float left_weight = 1 - right_weight;

            sq_point top_left_source =
                {top_border[x].x + left_border[y].x - left_border[0].x,
                top_border[x].y + left_border[y].y - left_border[0].y};

            sq_point bottom_right_source =
                {bottom_border[x].x + right_border[y].x
                - right_border[border_len - 1].x,
                bottom_border[x].y + right_border[y].y
                - right_border[border_len - 1].y};

            const unsigned char *data = img->data;
            unsigned sample_x = top_left_source.x;
            unsigned sample_y = top_left_source.y;
            unsigned char top_left_color =
                data[sample_y * img->width + sample_x];
            sample_x = bottom_right_source.x;
            sample_y = bottom_right_source.y;
            unsigned char bottom_right_color =
                data[sample_y * img->width + sample_x];

            unsigned char mixed_color =
                ((top_weight + left_weight) * top_left_color
                + (bottom_weight + right_weight) * bottom_right_color) / 2;

            if (is_black_color(mixed_color))
                buf[idx / 8] |= 1 << ( 7 - idx % 8 ) ;
            idx++;
        }
    }
    error = sq_extract_text(iscn, buf, byte_len);
    free(buf);

free_borders:
    free(top_border);
    free(left_border);
    free(right_border);
    free(bottom_border);
    return error? 1: 0;
}
