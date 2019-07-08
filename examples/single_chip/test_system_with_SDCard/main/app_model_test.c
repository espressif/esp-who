#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "app_httpd.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "rom/ets_sys.h"
#include "esp_system.h"
#include "app_model_test.h"
#include "fr_forward.h"
#include "fd_forward.h"
#include "dl_lib_matrix3d.h"
#include "esp_camera.h"

static const char *TAG = "Model_Test";

static char *val_path = "/sdcard/transcribed/cleaned.val";
static char *val_img_path = "/sdcard/transcribed/";
static char *rec_val_result_path = "/sdcard/val_result/recogtion_result.txt";

#define ENROLL_CONFIRM_TIMES 3
#define FACE_ID_SAVE_NUMBER 250
face_id_list g_id_list = {0};

static mtmn_config_t mtmn_config = {0};



data_info *data_info_alloc(int n, uint32_t caps)
{
	data_info *r = heap_caps_calloc(1, sizeof(data_info), caps);

	int *enroll = heap_caps_calloc(n, sizeof(int), caps);
	if(NULL == enroll){
		ets_printf("enroll alloc failed.\n");
    	free(r);
    	return NULL;
	}
	
    int *img_id = heap_caps_calloc(n, sizeof(int), caps);
    if(NULL == img_id){
		ets_printf("img_id alloc failed.\n");
    	free(r);
    	return NULL;
	}

	int *face_id = heap_caps_calloc(n, sizeof(int), caps);
    if(NULL == face_id){
		ets_printf("face_id alloc failed.\n");
    	free(r);
    	return NULL;
	}

    char **path = heap_caps_calloc(n, sizeof(char *), caps);
    for(int i=0;i<n;i++){
    	path[i] = heap_caps_calloc(50, sizeof(char), caps);
    	if(NULL == path[i]){
    		ets_printf("path alloc failed.\n");
    		free(r);
    		return NULL;
    	}
    }

    r->img_num = n;
    r->face_id_num = 0;
    r->path = path;
    r->enroll = enroll;
    r->img_id = img_id;
    r->face_id = face_id;

    return r;     
}


void data_info_free(data_info *d)
{
	if (NULL == d)
        return;

    free(d->enroll);
    d->enroll = NULL;
    free(d->img_id);
    d->img_id = NULL;
    free(d->face_id);
    d->face_id = NULL;

    for(int i=0;i<d->img_num;i++){
    	free(d->path[i]);
    }
    free(d->path);
    d->path = NULL;
    free(d);
}


int getLineNum(char *filename)
{
  	int line=0;
  	char buf[1024];
  	FILE* f = fopen(val_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open cleaned.val");
        return NULL;
    }

   	while(fgets(buf,sizeof(buf),f) != NULL){
    	if(buf[strlen(buf) -1] == '\n'){
    		line++;
    	}
    }

    fclose(f);
   	return line;
}

int getFileSize(char *filename)
{
	struct stat st;
	stat(filename, &st);
	int size = st.st_size;
	return size;
}

data_info *get_val_data_info(char *val_path)
{
	FILE* f = fopen(val_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open cleaned.val");
        return NULL;
    }
    char buf[1024];
    char *line_info;
    int num = getLineNum(val_path);
    ets_printf("cleaned.val total lines : %d\n",num);
    data_info *img_info = data_info_alloc(num,MALLOC_CAP_SPIRAM);

    int line_index = 0;
    int face_id_index = -1;
    int last_id = -1;
    while(fgets(buf,sizeof(buf),f) != NULL){
    	line_info = strtok(buf, "|");
    	if(line_info!=NULL){

    		img_info->img_id[line_index] = atoi(line_info);

    		line_info = strtok(NULL, "|");
    		strcpy(img_info->path[line_index],val_img_path);
    		strcat(img_info->path[line_index],line_info);

    		int new_id = atoi(strtok(line_info, "/"));
    		if(new_id != last_id){
    			face_id_index++;
    			last_id = new_id;
    		}
    		img_info->face_id[line_index] = face_id_index;

    		line_index++;
    	}
    }
    img_info->face_id_num = face_id_index+1;

    fclose(f);

    return img_info;
}

void face_enrollment(dl_matrix3du_t *image_matrix, box_array_t *net_boxes,int8_t *left_sample_face)
{
    char tag[] = "face_enrollment";

    if (net_boxes->len > 1)
    {
        ESP_LOGE(tag, "One face only, when doing enrollment.");
    }
    else
    {
        dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
        if (!aligned_face)
        {
            ESP_LOGE(tag, "Could not allocate aligned_face");
        }
        else
        {
            if (ESP_FAIL == align_face2(net_boxes->landmark[0].landmark_p, image_matrix, aligned_face))
            {
                ESP_LOGE(tag, "Face is not aligned.");
            }
            else
            {
                ESP_LOGI(tag, "Face is aligned.");

                uint64_t timestamp = esp_timer_get_time();
                *left_sample_face = enroll_face2(&g_id_list, aligned_face);
                int duration = (uint32_t)((esp_timer_get_time() - timestamp) / 1000);
                ESP_LOGW(tag,"Recognition duration: %dms",duration);

                switch (*left_sample_face)
                {
                // Enrolled first sample
                case ENROLL_CONFIRM_TIMES - 1:
                    ESP_LOGW(tag, "Enrolling Face ID: %d", g_id_list.tail);
                    break;
                // Enrolled last sample
                case 0:
                    ESP_LOGW(TAG, "Enrolled Face ID: %d", g_id_list.tail - 1);
                    if (g_id_list.id_list[g_id_list.tail - 1]->c != 512)
                    {
                        ESP_LOGE(TAG, "Enrolled Face ID size: %d", g_id_list.id_list[g_id_list.tail - 1]->c);
                        assert(g_id_list.id_list[g_id_list.tail - 1]->c == 512);
                    }
                    break;
                // Was enrolling
                default:
                    ESP_LOGW(tag,
                             "Enrolling Face ID: %d sample %d",
                             g_id_list.tail,
                             ENROLL_CONFIRM_TIMES - *left_sample_face);
                    break;
                }
            }
            // Free aligned face
            dl_matrix3du_free(aligned_face);
        }
    }
}


esp_err_t face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes, int *matched_ids, uint32_t *duration)
{
    char tag[] = "face_recognition";

    if (net_boxes->len > 1)
    {
        ESP_LOGE(tag, "One face only, when doing recognition test.");
        return ESP_FAIL;
    }

    dl_matrix3du_t *aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);

    if (!aligned_face)
    {
        ESP_LOGE(tag, "Could not allocate aligned_face");
        return ESP_FAIL;
    }
    else
    {
        uint64_t duration_n = 0;
        int aligned_face_counter = 0;

        for (size_t i = 0; i < net_boxes->len; i++)
        {
            // Align face
            if (ESP_FAIL == align_face2(net_boxes->landmark[i].landmark_p, image_matrix, aligned_face))
            {
                ESP_LOGE(TAG, "Face is not aligned.");
                matched_ids[i] = -2;
            }
            else
            {
                //ESP_LOGI(tag, "Face is aligned.");

                uint64_t timestamp = esp_timer_get_time();
                matched_ids[i] = recognize_face2(&g_id_list, aligned_face);
                duration_n += (esp_timer_get_time() - timestamp);
                aligned_face_counter++;

                //ESP_LOGI(tag, "Match Face ID: %u", matched_ids[i]);
            }
        }

        *duration = (aligned_face_counter == 0) ? 0 : (uint32_t)(duration_n / aligned_face_counter / 1000);

        ESP_LOGI(tag, "Recognition duration: %ums", *duration);

        // Free aligned face
        dl_matrix3du_free(aligned_face);
    }

    return ESP_OK;
}


esp_err_t recognition_model_test(data_info *test_data_info)
{
#if CONFIG_FRMN1_QUANT
        char *model = "frmn1";
#elif CONFIG_FRMN2_QUANT
        char *model = "frmn2";
#elif CONFIG_FRMN2P_QUANT
        char *model = "frmn2p";
#else
        char *model = "frmn2c";
#endif
    char tag[50] = "recognition_test ";
    strcat(tag,model);
    uint32_t detection_duration = 0;
	face_id_init(&g_id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
	//enrollment
	
	ESP_LOGE(tag, "start enrollment\n");
	int img_index = 0;
	int enrollment_num = 0;
	for(int i=0;i<test_data_info->face_id_num;i++){
		while(test_data_info->face_id[img_index] != i){
			img_index++;
			if(img_index>test_data_info->img_num){
				ESP_LOGE(tag, "id information error!!!");
				return ESP_FAIL;
			}
		}
		int start_id_index = img_index;
		int8_t left_sample_face = ENROLL_CONFIRM_TIMES;
		while(left_sample_face){
			if(test_data_info->face_id[img_index] != i){
				ESP_LOGE(tag, "id enrollment error!!!");
				//return ESP_FAIL;
				img_index = start_id_index;
			}
			ets_printf("\nimage: %s\n",test_data_info->path[img_index]);
			FILE* f = fopen(test_data_info->path[img_index], "r");
			if (f == NULL) {
        		ESP_LOGE(TAG, "Failed to open %s",test_data_info->path[img_index]);
        		return ESP_FAIL;
    		}

    		test_data_info->enroll[img_index] = 1;

			camera_fb_t jpeg;
			jpeg.len = getFileSize(test_data_info->path[img_index]);
			jpeg.width = 320;
			jpeg.height = 240;
			jpeg.format = PIXFORMAT_JPEG;
			jpeg.buf = (uint8_t *)heap_caps_malloc(jpeg.len * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
			fread(jpeg.buf,1,jpeg.len,f);
			fclose(f);

			dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, jpeg.width, jpeg.height, 3);
    		if (!image_matrix)
    		{
        		ESP_LOGE(tag, "dl_matrix3du_alloc failed");
        		return ESP_FAIL;
    		}

    		if (!fmt2rgb888(jpeg.buf, jpeg.len, jpeg.format, image_matrix->item))
    		{
        		ESP_LOGE(tag, "to rgb888 failed");
        		dl_matrix3du_free(image_matrix);
        		return ESP_FAIL;
   	 		}
   	 		free(jpeg.buf);

   	 		int64_t timestamp = esp_timer_get_time();
        	box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        	detection_duration = (uint32_t)((esp_timer_get_time() - timestamp) / 1000);
        	ESP_LOGI(tag, "Detection duration: %ums", detection_duration);

        	if (net_boxes)
        	{
            	ESP_LOGI(tag, "DETECTED.");
            	face_enrollment(image_matrix,net_boxes,&left_sample_face);

            	free(net_boxes->score);
        		free(net_boxes->box);
        		free(net_boxes->landmark);
        		free(net_boxes);
            }else{
            	ESP_LOGE(tag, "NOT DETECTED.");
            }
            dl_matrix3du_free(image_matrix);
            enrollment_num++;
            img_index++;
		}
	}
	ESP_LOGI(tag, "enrollment finished\n");

	ESP_LOGE(tag, "start recognition\n");

	int recognition_total_num = 0;
	int true_num = 0;
	int false_num = 0;
	uint64_t recognition_total_time = 0;
	uint32_t recognition_once_time = 0;
	for(int i=0;i<test_data_info->img_num;i++){
		ets_printf("\nimage: %s\n",test_data_info->path[i]);
		if(test_data_info->enroll[i] == 0){
			int matched_id = -3;
			FILE* f = fopen(test_data_info->path[i], "r");
			if (f == NULL) {
        		ESP_LOGE(TAG, "Failed to open %s",test_data_info->path[i]);
        		return ESP_FAIL;
    		}
    		camera_fb_t jpeg;
			jpeg.len = getFileSize(test_data_info->path[i]);
			jpeg.width = 320;
			jpeg.height = 240;
			jpeg.format = PIXFORMAT_JPEG;
			jpeg.buf = (uint8_t *)heap_caps_malloc(jpeg.len * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
			fread(jpeg.buf,1,jpeg.len,f);
			fclose(f);

			dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, jpeg.width, jpeg.height, 3);
    		if (!image_matrix)
    		{
        		ESP_LOGE(tag, "dl_matrix3du_alloc failed");
        		return ESP_FAIL;
    		}

    		if (!fmt2rgb888(jpeg.buf, jpeg.len, jpeg.format, image_matrix->item))
    		{
        		ESP_LOGE(tag, "to rgb888 failed");
        		dl_matrix3du_free(image_matrix);
        		return ESP_FAIL;
   	 		}
   	 		free(jpeg.buf);

   	 		int64_t timestamp = esp_timer_get_time();
        	box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
        	detection_duration = (uint32_t)((esp_timer_get_time() - timestamp) / 1000);
        	ESP_LOGI(tag, "Detection duration: %ums", detection_duration);

        	if (net_boxes)
        	{
            	ESP_LOGI(tag, "DETECTED.");
            	if(face_recognition(image_matrix,net_boxes,&matched_id,&recognition_once_time)==ESP_OK){
            		if(matched_id > -2){
            			recognition_total_num++;
            			recognition_total_time += recognition_once_time;
            			ESP_LOGI(tag,"Recognition duration: %ums",recognition_once_time);
            			if(matched_id == test_data_info->face_id[i]){
            				true_num++;
            				ESP_LOGI(tag, "GT/Pred: %d/%d",test_data_info->face_id[i],matched_id);
            			}else{
            				false_num++;
            				ESP_LOGE(tag, "GT/Pred: %d/%d",test_data_info->face_id[i],matched_id);
            			}
            		}

            	}

            	free(net_boxes->score);
        		free(net_boxes->box);
        		free(net_boxes->landmark);
        		free(net_boxes);
            }else{
            	ESP_LOGE(tag, "NOT DETECTED.");
            }
            dl_matrix3du_free(image_matrix);
		}else{
			ESP_LOGW(tag,"Image has been enrolled !");
		}
	}

	ESP_LOGI(tag, "recoginition finished\n");

	float rec_acc = true_num/(float)recognition_total_num;
	int rec_time = recognition_total_time/recognition_total_num;

	ESP_LOGI(tag, "%s model accuracy: %f(%d/%d), time: %dms\n",model,rec_acc,true_num,recognition_total_num,rec_time);

	FILE* f = fopen(rec_val_result_path, "a+");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open %s",rec_val_result_path);
        return ESP_FAIL;
    }
    fprintf(f, "%s model accuracy: %f(%d/%d), time: %dms\n", model,rec_acc,true_num,recognition_total_num,rec_time);
    fclose(f);

	return ESP_OK;
}

void app_model_test()
{
	mtmn_config.type = FAST;
	mtmn_config.pyramid = 0.707;/// if mtmn_config.type == FAST, the pyramid is set to sqrt(0.5) in default
	mtmn_config.min_face = 48;
	mtmn_config.pyramid_times = 5;
	mtmn_config.p_threshold.score = 0.6;
	mtmn_config.p_threshold.nms = 0.7;
	mtmn_config.p_threshold.candidate_number = 100;
	mtmn_config.r_threshold.score = 0.7;
	mtmn_config.r_threshold.nms = 0.7;
	mtmn_config.r_threshold.candidate_number = 100;
	mtmn_config.o_threshold.score = 0.7;
	mtmn_config.o_threshold.nms = 0.7;
	mtmn_config.o_threshold.candidate_number = 1;
	app_sd_card_init();
	data_info *test_data_info = get_val_data_info(val_path);
	recognition_model_test(test_data_info);  
	if(test_data_info != NULL){
		data_info_free(test_data_info);
		ets_printf("freed\n");
	}
	esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG, "Card unmounted");
}