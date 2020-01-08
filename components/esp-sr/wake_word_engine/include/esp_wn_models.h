#pragma once
#include "esp_wn_iface.h"

//Contains declarations of all available speech recognion models. Pair this up with the right coefficients and you have a model that can recognize
//a specific phrase or word.

extern const esp_wn_iface_t esp_sr_wakenet5_quantized;


/*
 Configure network to use based on what's selected in menuconfig.
*/
#define WAKENET_MODEL esp_sr_wakenet5_quantized

/*
 Configure wake word to use based on what's selected in menuconfig.
*/

#include "hilexin_wn5.h"
#define WAKENET_COEFF get_coeff_hilexin_wn5



/* example

static const sr_model_iface_t *model = &WAKENET_MODEL;

//Initialize wakeNet model data
static model_iface_data_t *model_data=model->create(DET_MODE_90);

//Set parameters of buffer
int audio_chunksize=model->get_samp_chunksize(model_data);
int frequency = model->get_samp_rate(model_data);
int16_t *buffer=malloc(audio_chunksize*sizeof(int16_t));

//Detect
int r=model->detect(model_data, buffer);
if (r>0) {
    printf("Detection triggered output %d.\n",  r);
}

//Destroy model
model->destroy(model_data)

*/
