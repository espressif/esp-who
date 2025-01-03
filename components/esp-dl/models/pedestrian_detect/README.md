| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |

# List of Pedestrian Detect Models

Pedestrian Detect now support one model pico.

input_shape : h * w * c  
pico : 224 * 224 * 3  

|                   | preprocess(us) | model(us)  | postprocess(us) |
| ----------------- | -------------- | ---------- | --------------- |
| pico_s8_v1_s3     | 27787          | 109200     | 2135            |
| pico_s8_v1_p4     | 14363          | 51450      | 1220            |

