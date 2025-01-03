| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |

# List of Human Face Detect Models

Human Face Detect now support one model msr+mnp. It's a two stage model.  
First stage model msr predicts some candidates, then every candidate go through the next stage model mnp.

input_shape : h * w * c  
msr : 120 * 160 * 3  
mnp : 48 * 48 * 3  

|                  | preprocess(us) | model(us) | postprocess(us) |
| ---------------- | -------------- | --------- | --------------- |
| msr_s8_v1_s3     | 10344          | 30609     | 344             |
| msr_s8_v1_p4     | 5328           | 13338     | 199             |
| mnp_s8_v1_s3     | 1156           | 5197      | 63             |
| mnp_s8_v1_p4     | 649            | 2478      | 41              |
