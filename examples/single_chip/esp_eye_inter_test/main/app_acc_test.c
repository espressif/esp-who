#include "app_acc_test.h"
#include <math.h>

static const char *TAG = "ACC TEST";

static face_id_list id_list = {0};
uint8_t face_cap_flag = 0;
dl_matrix3du_t *aligned_face = NULL;


void accuracy_process(int mode,int face_num)
{
	if(mode == 0){
		int pairs_num = face_num;
		int pairs_count = 0;
		float *dist = calloc(pairs_num,sizeof(float));
		uint8_t *issame_list = calloc(pairs_num,sizeof(uint8_t));
		int positive_num = face_num/2;
		int negative_num = pairs_num-positive_num;
		ESP_LOGE("PAIRS INFO","Total: %d,  Pos: %d,  Neg: %d",pairs_num,positive_num,negative_num);
		for(int i=0;i<positive_num;i++)
		{
			dist[pairs_count] = cos_sim(id_list.id_list[i],id_list.id_list[i+positive_num]);
			issame_list[pairs_count] = 1;
			pairs_count++;
		}
		for(int i=0;i<negative_num;i++)
		{
			dist[pairs_count] = cos_sim(id_list.id_list[2*i],id_list.id_list[2*i+1]);
			issame_list[pairs_count] = 0;
			pairs_count++;
		}
		assert(pairs_count == pairs_num);
		acc_cal(dist,issame_list,pairs_num);
	}
	else if(mode == 1){
		int pairs_num = face_num*(face_num-1)/2;
		float *dist = calloc(pairs_num,sizeof(float));
		uint8_t *issame_list = calloc(pairs_num,sizeof(uint8_t));
		int positive_num = face_num/2;
		int negative_num = pairs_num - positive_num;
		int pairs_count = 0;
		ESP_LOGE("PAIRS INFO","Total: %d,  Pos: %d,  Neg: %d",pairs_num,positive_num,negative_num);
		for(int i=0;i<(face_num-1);i++){
			for(int j=i+1;j<face_num;j++){
				dist[pairs_count] = cos_sim(id_list.id_list[i],id_list.id_list[j]);
				issame_list[pairs_count] = ((j-i)==(face_num/2))?1:0;
				pairs_count++;
			}
		}
		assert(pairs_count == pairs_num);
		acc_cal(dist,issame_list,pairs_num);
	}
}

void acc_cal(float *dist, uint8_t *issame_list, int pairs_num)
{
	int tp = 0;
	int fp = 0;
	int tn = 0;
	int fn = 0;
	float thresh = -1.0;
	float tpr[200] = {0.0};
	float fpr[200] = {0.0};
	float acc[200] = {0.0};
	float auc = 0.0;
	float max_acc = 0.0;
	float max_acc_thresh = 0.0;

	for(int i=0;i<200;i++){
		for(int j=0;j<pairs_num;j++){
			if((dist[j]>thresh)&&(issame_list[j]==1)){
				tp++;
			}else if((dist[j]>thresh)&&(issame_list[j]==0)){
				fp++;
			}else if((dist[j]<=thresh)&&(issame_list[j]==0)){
				tn++;
			}else if((dist[j]<=thresh)&&(issame_list[j]==1)){
				fn++;
			}
		}
		assert((tp+fp+tn+fn) == pairs_num);
		tpr[i] = ((tp+fn)>0)?((float)(tp)/(tp+fn)):0.0;
		fpr[i] = ((tn+fp)>0)?((float)(fp)/(tn+fp)):0.0;
		acc[i] = (float)(tp+tn)/pairs_num;
		if(acc[i]>max_acc){
			max_acc = acc[i];
			max_acc_thresh = thresh;
		}
		if(thresh > 0.4 && thresh < 0.9){
			ESP_LOGI("ACC INFO","Thresh: %.2f,  TPR: %.5f, FPR: %.5f, ACC: %.5f%%",thresh,tpr[i],fpr[i],100.0*acc[i]);
		}
		thresh += 0.01;
		tp = 0;
		fp = 0;
		tn = 0;
		fn = 0;
	}
	for(int i=0;i<199;i++){
		auc += ((fpr[i] - fpr[i+1])*(tpr[i] + tpr[i+1])/2);
	}
	ESP_LOGE("ACC INFO","Max ACC: %.5f%%, Thresh: %.2f, AUC: %.5f",100.0*max_acc,max_acc_thresh,auc);
}

void face_acc_process (void *arg)
{
	size_t frame_num = 0;
    size_t detected_frame_num = 0;
    size_t detected_pass_num = 0;
    camera_fb_t * fb = NULL;
    dl_matrix3du_t *image_matrix = NULL;
    char *line = NULL;
    uint8_t left_sample_face = 0;
    int face_count = 0;

    mtmn_config_t mtmn_config = mtmn_init_config();

    const char *prompt1 = "Pictures num: ";
    const char *prompt2 = "Next> ";
    int probe_status = linenoiseProbe();
    if (probe_status) { /* zero indicates success */
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);
    }
    line = linenoise(prompt1);
    printf("\n");
   	int total_face_num = atoi(line);

   	/*prompt1 = "Mode: ";
   	line = linenoise(prompt1);
    printf("\n");
   	int mode = atoi(line);*/

   	face_id_init(&id_list, total_face_num, ENROLL_CONFIRM_TIMES);
   	aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT,3);

   	int people_num = total_face_num/2;

   	for(int i=0;i<total_face_num;i++){
   		gpio_set_level(GPIO_LED_WHITE, 0);
   		gpio_set_level(GPIO_LED_RED, 1);
   		char *line2 = linenoise(prompt2);
   		if(1){
   			gpio_set_level(GPIO_LED_WHITE, 1);
   			gpio_set_level(GPIO_LED_RED, 0);
   			face_cap_flag = 1;
   			while(face_cap_flag == 1){
   				fb = esp_camera_fb_get();
   				frame_num++;
        		if (!fb)
        		{
            		ESP_LOGE(TAG, "Camera capture failed");
            		break;
        		}
        		image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
        		if (!image_matrix)
        		{
           	 		ESP_LOGE(TAG, "dl_matrix3du_alloc failed");
            		break;
        		}
        		if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item))
        		{
            		ESP_LOGE(TAG, "fmt2rgb888 failed");
            		break;
        		}
        		esp_camera_fb_return(fb);

        		box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);

        		if (net_boxes){
        			detected_frame_num++;
        			if(align_face(net_boxes, image_matrix, aligned_face) == ESP_OK){
        				detected_pass_num++;
        				left_sample_face = enroll_face_heap(&id_list, aligned_face);
        				if(left_sample_face == 0){
        					face_count++;
        					ESP_LOGW("Face ID", "%d-%d Finished", (id_list.count-1)%people_num,(id_list.count>people_num?1:0));
        					if(id_list.count == people_num){
        						ESP_LOGE(TAG,"Please change to anothor set of images");
        					}
        					face_cap_flag = 0;
        				}
        			}
        			free(net_boxes->box);
            	free(net_boxes->landmark);
            	free(net_boxes);
        		}
        		
        		dl_matrix3du_free(image_matrix);
        		//dl_matrix3du_free(aligned_face);

        		ESP_LOGI("INFO", "Detect/ALL:  %d/%d,  Align/ALL: %d/%d", detected_frame_num,frame_num,detected_pass_num,frame_num);
            /*printf("RAM size: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024);
            printf("SPIRAM size: %d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024);
   			    printf("SRAM size: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024 - heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024);*/
        }
   		}
   		free(line2);
   	}

   	ESP_LOGE("INFO","Detection Rate: %.5f%%,  Detection Pass Rate: %.5f%%",100.0*(float)(detected_frame_num)/frame_num,100.0*(float)(detected_pass_num)/frame_num);
   	
   	gpio_set_level(GPIO_LED_WHITE, 0);
   	gpio_set_level(GPIO_LED_RED, 0);
   	
   	printf("\n");
   	accuracy_process(0,total_face_num);
   	printf("\n");
   	accuracy_process(1,total_face_num);

   	while(1){
        vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}



void face_acc_test()
{
	xTaskCreatePinnedToCore(face_acc_process, "face_acc_process", 12 * 1024, NULL, 5, NULL, 1);

}