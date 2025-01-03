| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |


# Human Face Detect Example

Two stage human face detection model.

# Example Output

After the flashing you should see the output at idf monitor:

```
I (1351) dl::Model: model:torch-jit-export, version:0

I (1351) dl::Model: Conv_0: Conv
I (1351) dl::Model: Conv_2: Conv
I (1351) dl::Model: Conv_4: Conv
I (1361) dl::Model: Conv_5: Conv
I (1361) dl::Model: Conv_7: Conv
I (1361) dl::Model: Conv_8: Conv
I (1371) dl::Model: Conv_10: Conv
I (1371) dl::Model: Conv_26: Conv
I (1381) dl::Model: Conv_28: Conv
I (1381) dl::Model: Conv_29: Conv
I (1381) dl::Model: Conv_32: Conv
I (1391) dl::Model: Conv_48: Conv
I (1391) dl::Model: Conv_31: Conv
I (1401) dl::Model: Conv_34: Conv
I (1401) dl::Model: Conv_50: Conv
I (1401) dl::Model: Conv_35: Conv
I (1411) dl::Model: Conv_38: Conv
I (1411) dl::Model: Conv_66: Conv
I (1421) dl::Model: Conv_37: Conv
I (1421) dl::Model: Conv_40: Conv
I (1421) dl::Model: Conv_68: Conv
I (1431) dl::Model: Conv_41: Conv
I (1431) dl::Model: Conv_69: Conv
I (1441) dl::Model: Conv_72: Conv
I (1441) dl::Model: Conv_43: Conv
I (1441) dl::Model: Conv_71: Conv
I (1451) dl::Model: Conv_74: Conv
I (1451) dl::Model: Concat_44: Concat
I (1461) dl::Model: Conv_75: Conv
I (1461) dl::Model: Conv_78: Conv
I (1461) dl::Model: Conv_45: Conv
I (1471) dl::Model: Conv_46: Conv
I (1471) dl::Model: Conv_47: Conv
I (1481) dl::Model: Conv_77: Conv
I (1481) dl::Model: Conv_80: Conv
I (1481) dl::Model: Conv_81: Conv
I (1491) dl::Model: Conv_83: Conv
I (1491) dl::Model: Concat_84: Concat
I (1501) dl::Model: Conv_85: Conv
I (1501) dl::Model: Conv_86: Conv
I (1511) dl::Model: Conv_87: Conv
I (1511) MemoryManagerGreedy: Maximum mermory size: 202400

I (1521) dl::Model: model:torch-jit-export, version:0

I (1521) dl::Model: Conv_0: Conv
I (1531) dl::Model: PRelu_1: PRelu
I (1531) dl::Model: Conv_2: Conv
I (1531) dl::Model: PRelu_3: PRelu
I (1541) dl::Model: Conv_4: Conv
I (1541) dl::Model: PRelu_5: PRelu
I (1551) dl::Model: Conv_6: Conv
I (1551) dl::Model: Conv_7: Conv
I (1551) dl::Model: PRelu_8: PRelu
I (1561) dl::Model: Conv_9: Conv
I (1561) dl::Model: Conv_10: Conv
I (1571) dl::Model: PRelu_11: PRelu
I (1571) dl::Model: Conv_12: Conv
I (1571) dl::Model: PRelu_13: PRelu
I (1581) dl::Model: Conv_14: Conv
I (1581) dl::Model: Conv_15: Conv
I (1591) dl::Model: PRelu_16: PRelu
I (1591) dl::Model: Conv_17: Conv
I (1591) dl::Model: PRelu_18: PRelu
I (1601) dl::Model: Conv_19: Conv
I (1601) dl::Model: Conv_20: Conv
I (1611) dl::Model: Conv_21: Conv
I (1611) dl::Model: Conv_22: Conv
I (1621) MemoryManagerGreedy: Maximum mermory size: 49232

I (1641) human_face_detect: [score: 0.880797, x1: 101, y1: 64, x2: 197, y2: 191]

I (1641) human_face_detect: left_eye: [116, 113], left_mouth: [119, 159], nose: [130, 143], right_eye: [157, 113], right_mouth: [151, 159]]
```