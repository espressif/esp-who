/*Copyright (C) 2018 Javier Serrano Polo <javier@jasp.net>
  You can redistribute this library and/or modify it under the terms of the
   GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2.1 of the License, or (at your option) any later
   version.*/
#ifndef _SQCODE_H_
#define _SQCODE_H_

#include <zbar.h>

typedef struct sq_reader sq_reader;

sq_reader *_zbar_sq_create(void);
void _zbar_sq_destroy(sq_reader *reader);
void _zbar_sq_reset(sq_reader *reader);

int _zbar_sq_new_config(sq_reader *reader,
                        unsigned config);
int _zbar_sq_decode(sq_reader *reader,
                    zbar_image_scanner_t *iscn,
                    zbar_image_t *img);

#endif
