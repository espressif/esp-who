#include "sq_finder.h"
#include "decoder.h"

unsigned _zbar_decoder_get_sq_finder_config(zbar_decoder_t *dcode)
{
    return dcode->sqf.config;
}
