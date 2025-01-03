| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |

# List of Human Face Recognition Models

Up to now, we support two different versions of human face recognition models. The performance of human face recognition models is as follows:

input_shape : h * w * c  
mfn : 112 * 112 * 3  
mbf : 112 * 112 * 3  

| Model                    | Params(M) | GFLOPs | TAR@FAR=1E-4 on IJB-C(%) |
| ---------------------------- | --------- | ------ | ------------------------ |
| mfn_s8_v1 | 1.2       | 0.46   | 90.03                  |
| mbf_s8_v1 | 3.4       | 0.90   | 93.94                    |

|              | preprocess(us) | model(us) | postprocess(us) |
| ------------ | -------------- | --------- | --------------- |
| mfn_s8_v1_s3 | 8586           | 245617    | 68              |
| mfn_s8_v1_p4 | 5128           | 90578     | 42              |
| mbf_s8_v1_s3 | 8570           | 1117230   | 75              |
| mbf_s8_v1_p4 | 5255           | 193842    | 43              |
