/*------------------------------------------------------------------------
 *  Copyright 2007-2010 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#include <config.h>
#include <stdlib.h>     /* malloc, calloc, free */
#include <stdio.h>      /* snprintf */
#include <string.h>     /* memset, strlen */

#include <zbar.h>

#if defined(DEBUG_DECODER) || defined(DEBUG_EAN) || defined(DEBUG_CODE93) || \
    defined(DEBUG_CODE39) || defined(DEBUG_CODABAR) || defined(DEBUG_I25) || \
    defined(DEBUG_DATABAR) || defined(DEBUG_CODE128) || \
    defined(DEBUG_SQ_FINDER) || \
    defined(DEBUG_QR_FINDER) || (defined(DEBUG_PDF417) && (DEBUG_PDF417 >= 4))
# define DEBUG_LEVEL 1
#endif
#include "debug.h"
#include "decoder.h"

zbar_decoder_t *zbar_decoder_create ()
{
    // zbar_decoder_t *dcode = calloc(1, sizeof(zbar_decoder_t));
    zbar_decoder_t *dcode = heap_caps_calloc(1, sizeof(zbar_decoder_t), MALLOC_CAP_SPIRAM);
    dcode->buf_alloc = BUFFER_MIN;
    // dcode->buf = malloc(dcode->buf_alloc);
    dcode->buf = heap_caps_malloc(dcode->buf_alloc, MALLOC_CAP_SPIRAM);

    /* initialize default configs */
#if ENABLE_EAN == 1
    dcode->ean.enable = 1;
    dcode->ean.ean13_config = ((1 << ZBAR_CFG_ENABLE) |
                               (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->ean.ean8_config = ((1 << ZBAR_CFG_ENABLE) |
                              (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->ean.upca_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.upce_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.isbn10_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.isbn13_config = 1 << ZBAR_CFG_EMIT_CHECK;
# ifdef FIXME_ADDON_SYNC
    dcode->ean.ean2_config = 1 << ZBAR_CFG_ENABLE;
    dcode->ean.ean5_config = 1 << ZBAR_CFG_ENABLE;
# endif
#endif
#if ENABLE_I25 == 1
    dcode->i25.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->i25, ZBAR_CFG_MIN_LEN) = 6;
#endif
#if ENABLE_DATABAR == 1
    dcode->databar.config = ((1 << ZBAR_CFG_ENABLE) |
                             (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->databar.config_exp = ((1 << ZBAR_CFG_ENABLE) |
                                 (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->databar.csegs = 4;
    // dcode->databar.segs = calloc(4, sizeof(*dcode->databar.segs));
    dcode->databar.segs = heap_caps_calloc(4, sizeof(*dcode->databar.segs), MALLOC_CAP_SPIRAM);
#endif
#if ENABLE_CODABAR == 1
    dcode->codabar.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->codabar, ZBAR_CFG_MIN_LEN) = 4;
#endif
#if ENABLE_CODE39 == 1
    dcode->code39.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->code39, ZBAR_CFG_MIN_LEN) = 1;
#endif
#if ENABLE_CODE93 == 1
    dcode->code93.config = 1 << ZBAR_CFG_ENABLE;
#endif
#if ENABLE_CODE128 == 1
    dcode->code128.config = 1 << ZBAR_CFG_ENABLE;
#endif
#if ENABLE_PDF417 == 1
    dcode->pdf417.config = 1 << ZBAR_CFG_ENABLE;
#endif
#if ENABLE_QRCODE == 1
    dcode->qrf.config = 1 << ZBAR_CFG_ENABLE;
#endif
#if ENABLE_SQCODE == 1
    dcode->sqf.config = 1 << ZBAR_CFG_ENABLE;
#endif

    zbar_decoder_reset(dcode);
    return(dcode);
}

void zbar_decoder_destroy (zbar_decoder_t *dcode)
{
#if ENABLE_DATABAR == 1
    if(dcode->databar.segs)
        free(dcode->databar.segs);
#endif
    if(dcode->buf)
        free(dcode->buf);
    free(dcode);
}

void zbar_decoder_reset (zbar_decoder_t *dcode)
{
    memset(dcode, 0, (long)&dcode->buf_alloc - (long)dcode);
#if ENABLE_EAN == 1
    ean_reset(&dcode->ean);
#endif
#if ENABLE_I25 == 1
    i25_reset(&dcode->i25);
#endif
#if ENABLE_DATABAR == 1
    databar_reset(&dcode->databar);
#endif
#if ENABLE_CODABAR == 1
    codabar_reset(&dcode->codabar);
#endif
#if ENABLE_CODE39 == 1
    code39_reset(&dcode->code39);
#endif
#if ENABLE_CODE93 == 1
    code93_reset(&dcode->code93);
#endif
#if ENABLE_CODE128 == 1
    code128_reset(&dcode->code128);
#endif
#if ENABLE_PDF417 == 1
    pdf417_reset(&dcode->pdf417);
#endif
#if ENABLE_QRCODE == 1
    qr_finder_reset(&dcode->qrf);
#endif
}

void zbar_decoder_new_scan (zbar_decoder_t *dcode)
{
    /* soft reset decoder */
    memset(dcode->w, 0, sizeof(dcode->w));
    dcode->lock = 0;
    dcode->idx = 0;
    dcode->s6 = 0;
#if ENABLE_EAN == 1
    ean_new_scan(&dcode->ean);
#endif
#if ENABLE_I25 == 1
    i25_reset(&dcode->i25);
#endif
#if ENABLE_DATABAR == 1
    databar_new_scan(&dcode->databar);
#endif
#if ENABLE_CODABAR == 1
    codabar_reset(&dcode->codabar);
#endif
#if ENABLE_CODE39 == 1
    code39_reset(&dcode->code39);
#endif
#if ENABLE_CODE93 == 1
    code93_reset(&dcode->code93);
#endif
#if ENABLE_CODE128 == 1
    code128_reset(&dcode->code128);
#endif
#if ENABLE_PDF417 == 1
    pdf417_reset(&dcode->pdf417);
#endif
#if ENABLE_QRCODE == 1
    qr_finder_reset(&dcode->qrf);
#endif
}


zbar_color_t zbar_decoder_get_color (const zbar_decoder_t *dcode)
{
    return(get_color(dcode));
}

const char *zbar_decoder_get_data (const zbar_decoder_t *dcode)
{
    return((char*)dcode->buf);
}

unsigned int zbar_decoder_get_data_length (const zbar_decoder_t *dcode)
{
    return(dcode->buflen);
}

int zbar_decoder_get_direction (const zbar_decoder_t *dcode)
{
    return(dcode->direction);
}

zbar_decoder_handler_t *
zbar_decoder_set_handler (zbar_decoder_t *dcode,
                          zbar_decoder_handler_t *handler)
{
    zbar_decoder_handler_t *result = dcode->handler;
    dcode->handler = handler;
    return(result);
}

void zbar_decoder_set_userdata (zbar_decoder_t *dcode,
                                void *userdata)
{
    dcode->userdata = userdata;
}

void *zbar_decoder_get_userdata (const zbar_decoder_t *dcode)
{
    return(dcode->userdata);
}

zbar_symbol_type_t zbar_decoder_get_type (const zbar_decoder_t *dcode)
{
    return(dcode->type);
}

unsigned int zbar_decoder_get_modifiers (const zbar_decoder_t *dcode)
{
    return(dcode->modifiers);
}

zbar_symbol_type_t zbar_decode_width (zbar_decoder_t *dcode,
                                      unsigned w)
{
    zbar_symbol_type_t tmp, sym = ZBAR_NONE;

    dcode->w[dcode->idx & (DECODE_WINDOW - 1)] = w;
    // printf("    decode[%x]: w=%d (%g)\n", dcode->idx, w, (w / 32.));

    /* update shared character width */
    dcode->s6 -= get_width(dcode, 7);
    dcode->s6 += get_width(dcode, 1);

    /* each decoder processes width stream in parallel */
#if ENABLE_QRCODE == 1
    if(TEST_CFG(dcode->qrf.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_find_qr(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_EAN == 1
    if((dcode->ean.enable) &&
       (tmp = _zbar_decode_ean(dcode)))
        sym = tmp;
#endif
#if ENABLE_CODE39 == 1
    if(TEST_CFG(dcode->code39.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_code39(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_CODE93 == 1
    if(TEST_CFG(dcode->code93.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_code93(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_CODE128 == 1
    if(TEST_CFG(dcode->code128.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_code128(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_DATABAR == 1
    if(TEST_CFG(dcode->databar.config | dcode->databar.config_exp,
                ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_databar(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_CODABAR == 1
    if(TEST_CFG(dcode->codabar.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_codabar(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_I25 == 1
    if(TEST_CFG(dcode->i25.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_i25(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif
#if ENABLE_PDF417 == 1
    if(TEST_CFG(dcode->pdf417.config, ZBAR_CFG_ENABLE) &&
       (tmp = _zbar_decode_pdf417(dcode)) > ZBAR_PARTIAL)
        sym = tmp;
#endif

    dcode->idx++;
    dcode->type = sym;
    if(sym) {
        if(dcode->lock && sym > ZBAR_PARTIAL && sym != ZBAR_QRCODE)
            release_lock(dcode, sym);
        if(dcode->handler)
            dcode->handler(dcode);
    }
    return(sym);
}

static inline const unsigned int*
decoder_get_configp (const zbar_decoder_t *dcode,
                     zbar_symbol_type_t sym)
{
    const unsigned int *config;
    switch(sym) {
#if ENABLE_EAN == 1
    case ZBAR_EAN13:
        config = &dcode->ean.ean13_config;
        break;

    case ZBAR_EAN2:
        config = &dcode->ean.ean2_config;
        break;

    case ZBAR_EAN5:
        config = &dcode->ean.ean5_config;
        break;

    case ZBAR_EAN8:
        config = &dcode->ean.ean8_config;
        break;

    case ZBAR_UPCA:
        config = &dcode->ean.upca_config;
        break;

    case ZBAR_UPCE:
        config = &dcode->ean.upce_config;
        break;

    case ZBAR_ISBN10:
        config = &dcode->ean.isbn10_config;
        break;

    case ZBAR_ISBN13:
        config = &dcode->ean.isbn13_config;
        break;
#endif

#if ENABLE_I25 == 1
    case ZBAR_I25:
        config = &dcode->i25.config;
        break;
#endif

#if ENABLE_DATABAR == 1
    case ZBAR_DATABAR:
        config = &dcode->databar.config;
        break;
    case ZBAR_DATABAR_EXP:
        config = &dcode->databar.config_exp;
        break;
#endif

#if ENABLE_CODABAR == 1
    case ZBAR_CODABAR:
        config = &dcode->codabar.config;
        break;
#endif

#if ENABLE_CODE39 == 1
    case ZBAR_CODE39:
        config = &dcode->code39.config;
        break;
#endif

#if ENABLE_CODE93 == 1
    case ZBAR_CODE93:
        config = &dcode->code93.config;
        break;
#endif

#if ENABLE_CODE128 == 1
    case ZBAR_CODE128:
        config = &dcode->code128.config;
        break;
#endif

#if ENABLE_PDF417 == 1
    case ZBAR_PDF417:
        config = &dcode->pdf417.config;
        break;
#endif

#if ENABLE_QRCODE == 1
    case ZBAR_QRCODE:
        config = &dcode->qrf.config;
        break;
#endif

#if ENABLE_SQCODE == 1
    case ZBAR_SQCODE:
        config = &dcode->sqf.config;
        break;
#endif

    default:
        config = NULL;
    }
    return(config);
}

unsigned int zbar_decoder_get_configs (const zbar_decoder_t *dcode,
                                       zbar_symbol_type_t sym)
{
    const unsigned *config = decoder_get_configp(dcode, sym);
    if(!config)
        return(0);
    return(*config);
}

static inline int decoder_set_config_bool (zbar_decoder_t *dcode,
                                           zbar_symbol_type_t sym,
                                           zbar_config_t cfg,
                                           int val)
{
    unsigned *config = (void*)decoder_get_configp(dcode, sym);
    if(!config || cfg >= ZBAR_CFG_NUM)
        return(1);

    if(!val)
        *config &= ~(1 << cfg);
    else if(val == 1)
        *config |= (1 << cfg);
    else
        return(1);

#if ENABLE_EAN == 1
    dcode->ean.enable = TEST_CFG(dcode->ean.ean13_config |
                                 dcode->ean.ean2_config |
                                 dcode->ean.ean5_config |
                                 dcode->ean.ean8_config |
                                 dcode->ean.upca_config |
                                 dcode->ean.upce_config |
                                 dcode->ean.isbn10_config |
                                 dcode->ean.isbn13_config,
                                 ZBAR_CFG_ENABLE);
#endif

    return(0);
}

static inline int decoder_set_config_int (zbar_decoder_t *dcode,
                                          zbar_symbol_type_t sym,
                                          zbar_config_t cfg,
                                          int val)
{
    switch(sym) {

#if ENABLE_I25 == 1
    case ZBAR_I25:
        CFG(dcode->i25, cfg) = val;
        break;
#endif
#if ENABLE_CODABAR == 1
    case ZBAR_CODABAR:
        CFG(dcode->codabar, cfg) = val;
        break;
#endif
#if ENABLE_CODE39 == 1
    case ZBAR_CODE39:
        CFG(dcode->code39, cfg) = val;
        break;
#endif
#if ENABLE_CODE93 == 1
    case ZBAR_CODE93:
        CFG(dcode->code93, cfg) = val;
        break;
#endif
#if ENABLE_CODE128 == 1
    case ZBAR_CODE128:
        CFG(dcode->code128, cfg) = val;
        break;
#endif
#if ENABLE_PDF417 == 1
    case ZBAR_PDF417:
        CFG(dcode->pdf417, cfg) = val;
        break;
#endif

    default:
        return(1);
    }
    return(0);
}

int zbar_decoder_get_config(zbar_decoder_t *dcode,
                            zbar_symbol_type_t sym,
                            zbar_config_t cfg,
                            int *val)
{
    const unsigned *config = decoder_get_configp(dcode, sym);

    /* Return error if symbol doesn't have config */
    if(sym <= ZBAR_PARTIAL || sym > ZBAR_CODE128 || sym == ZBAR_COMPOSITE)
        return 1;

    /* Return decoder boolean configs */
    if (cfg < ZBAR_CFG_NUM) {
        *val = (*config & (1 << cfg)) != 0;
        return 0;
    }

    /* Return decoder integer configs */
    if(cfg >= ZBAR_CFG_MIN_LEN && cfg <= ZBAR_CFG_MAX_LEN) {
        switch(sym) {
#if ENABLE_I25 == 1
        case ZBAR_I25:
            *val = CFG(dcode->i25, cfg);
            return 0;
#endif
    #if ENABLE_CODABAR == 1
        case ZBAR_CODABAR:
            *val = CFG(dcode->codabar, cfg);
            return 0;
    #endif
    #if ENABLE_CODE39 == 1
        case ZBAR_CODE39:
            *val = CFG(dcode->code39, cfg);
            return 0;
    #endif
    #if ENABLE_CODE93 == 1
        case ZBAR_CODE93:
            *val = CFG(dcode->code93, cfg);
            return 0;
    #endif
    #if ENABLE_CODE128 == 1
        case ZBAR_CODE128:
            *val = CFG(dcode->code128, cfg);
            return 0;
    #endif
    #if ENABLE_PDF417 == 1
        case ZBAR_PDF417:
            *val = CFG(dcode->pdf417, cfg);
            return 0;
    #endif
        default:
            return 1;
        }
    }
    return 1;
}

int zbar_decoder_set_config (zbar_decoder_t *dcode,
                             zbar_symbol_type_t sym,
                             zbar_config_t cfg,
                             int val)
{
    if(sym == ZBAR_NONE) {
        static const zbar_symbol_type_t all[] = {
        ZBAR_EAN13, ZBAR_EAN2, ZBAR_EAN5, ZBAR_EAN8,
            ZBAR_UPCA, ZBAR_UPCE, ZBAR_ISBN10, ZBAR_ISBN13,
            ZBAR_I25, ZBAR_DATABAR, ZBAR_DATABAR_EXP, ZBAR_CODABAR,
        ZBAR_CODE39, ZBAR_CODE93, ZBAR_CODE128, ZBAR_QRCODE, 
            ZBAR_SQCODE,
        ZBAR_PDF417, 0
        };
        const zbar_symbol_type_t *symp;
        for(symp = all; *symp; symp++)
            zbar_decoder_set_config(dcode, *symp, cfg, val);
        return(0);
    }

    if(cfg >= 0 && cfg < ZBAR_CFG_NUM)
        return(decoder_set_config_bool(dcode, sym, cfg, val));
    else if(cfg >= ZBAR_CFG_MIN_LEN && cfg <= ZBAR_CFG_MAX_LEN)
        return(decoder_set_config_int(dcode, sym, cfg, val));
    else
        return(1);
}


static char *decoder_dump = NULL;
static unsigned decoder_dumplen = 0;

const char *_zbar_decoder_buf_dump (unsigned char *buf,
                                    unsigned int buflen)
{
    int dumplen = (buflen * 3) + 12;
    char *p;
    int i;

    if(!decoder_dump || dumplen > decoder_dumplen) {
        if(decoder_dump)
            free(decoder_dump);
        // decoder_dump = malloc(dumplen);
        decoder_dump = heap_caps_malloc(dumplen, MALLOC_CAP_SPIRAM);
        decoder_dumplen = dumplen;
    }
    p = decoder_dump +
        snprintf(decoder_dump, 12, "buf[%04x]=",
                 (buflen > 0xffff) ? 0xffff : buflen);
    for(i = 0; i < buflen; i++)
        p += snprintf(p, 4, "%s%02x", (i) ? " " : "",  buf[i]);
    return(decoder_dump);
}
