/* ESPRESSIF MIT License
 * 
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 * 
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "app_camera.h"
#include "app_wifi.h"
#include "app_httpd.h"
#include "app_mdns.h"

#include "cd_forward.h"
void app_main()
{
    app_wifi_main();
    app_camera_main();
    app_httpd_main();
    app_mdns_main();
    // dl_matrix3du_t *rgb = dl_matrix3du_alloc(1, 4, 4, 3);
    // rgb->item[0] = 200;
    // rgb->item[1] = 20;
    // rgb->item[2] = 180;

    // rgb->item[3] = 160;
    // rgb->item[4] = 50;
    // rgb->item[5] = 100;

    // rgb->item[6] = 120;
    // rgb->item[7] = 220;
    // rgb->item[8] = 80;

    // rgb->item[9] = 120;
    // rgb->item[10] = 120;
    // rgb->item[11] = 110;

    // rgb->item[12] = 120;
    // rgb->item[13] = 160;
    // rgb->item[14] = 160;

    // dl_matrix3du_t *hsv = rgb2hsv(rgb);
    // int n = 0;
    // printf("\n\n");
    // for(int i=0; i<4; i++){
    //     for(int j=0; j<4; j++){
    //         printf("%d, %d, %d\n", hsv->item[n], hsv->item[n+1], hsv->item[n+2]);
    //         n+=3;
    //     }
    // }

}
