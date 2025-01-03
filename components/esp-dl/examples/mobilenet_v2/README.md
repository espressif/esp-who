| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |


# MobileNet_v2 Example

Deploy [MobileNet_v2](https://arxiv.org/abs/1801.04381) model from [torchvision](https://pytorch.org/vision/0.18/models/generated/torchvision.models.mobilenet_v2.html).

See [Deploying MobileNet_v2 Using ESP-DL](https://docs.espressif.com/projects/esp-dl/en/latest/tutorials/how_to_deploy_mobilenet.html) for more information.

# Example Output
After the flashing you should see the output at idf monitor:

```
I (1228) MOBILENET_V2_EXAMPLE: get into app_main
I (1238) FbsLoader: The storage free size is 65536 KB
I (1238) FbsLoader: The partition size is 7900 KB
I (1258) dl::Model: model:torch_jit, version:1

I (1258) dl::Model: /features/features.0/features.0.0/Conv: Conv
I (1258) dl::Model: /features/features.0/features.0.2/Clip: Clip
I (1268) dl::Model: /features/features.1/conv/conv.0/conv.0.0/Conv: Conv
I (1268) dl::Model: /features/features.1/conv/conv.0/conv.0.2/Clip: Clip
I (1278) dl::Model: /features/features.1/conv/conv.1/Conv: Conv
I (1288) dl::Model: /features/features.2/conv/conv.0/conv.0.0/Conv: Conv
I (1298) dl::Model: /features/features.2/conv/conv.0/conv.0.2/Clip: Clip
I (1298) dl::Model: /features/features.2/conv/conv.1/conv.1.0/Conv: Conv
I (1308) dl::Model: /features/features.2/conv/conv.1/conv.1.2/Clip: Clip
I (1318) dl::Model: /features/features.2/conv/conv.2/Conv: Conv
I (1328) dl::Model: /features/features.3/conv/conv.0/conv.0.0/Conv: Conv
I (1328) dl::Model: PPQ_Operation_0: RequantizeLinear
I (1338) dl::Model: /features/features.3/conv/conv.0/conv.0.2/Clip: Clip
I (1348) dl::Model: /features/features.3/conv/conv.1/conv.1.0/Conv: Conv
I (1348) dl::Model: /features/features.3/conv/conv.1/conv.1.2/Clip: Clip
I (1358) dl::Model: /features/features.3/conv/conv.2/Conv: Conv
I (1368) dl::Model: /features/features.3/Add: Add
I (1368) dl::Model: /features/features.4/conv/conv.0/conv.0.0/Conv: Conv
I (1378) dl::Model: /features/features.4/conv/conv.0/conv.0.2/Clip: Clip
I (1388) dl::Model: /features/features.4/conv/conv.1/conv.1.0/Conv: Conv
I (1388) dl::Model: /features/features.4/conv/conv.1/conv.1.2/Clip: Clip
I (1398) dl::Model: /features/features.4/conv/conv.2/Conv: Conv
I (1408) dl::Model: /features/features.5/conv/conv.0/conv.0.0/Conv: Conv
I (1418) dl::Model: /features/features.5/conv/conv.0/conv.0.2/Clip: Clip
I (1418) dl::Model: /features/features.5/conv/conv.1/conv.1.0/Conv: Conv
I (1428) dl::Model: /features/features.5/conv/conv.1/conv.1.2/Clip: Clip
I (1438) dl::Model: /features/features.5/conv/conv.2/Conv: Conv
I (1438) dl::Model: /features/features.5/Add: Add
I (1448) dl::Model: /features/features.6/conv/conv.0/conv.0.0/Conv: Conv
I (1458) dl::Model: /features/features.6/conv/conv.0/conv.0.2/Clip: Clip
I (1458) dl::Model: /features/features.6/conv/conv.1/conv.1.0/Conv: Conv
I (1468) dl::Model: /features/features.6/conv/conv.1/conv.1.2/Clip: Clip
I (1478) dl::Model: /features/features.6/conv/conv.2/Conv: Conv
I (1488) dl::Model: /features/features.6/Add: Add
I (1488) dl::Model: /features/features.7/conv/conv.0/conv.0.0/Conv: Conv
I (1498) dl::Model: /features/features.7/conv/conv.0/conv.0.2/Clip: Clip
I (1508) dl::Model: /features/features.7/conv/conv.1/conv.1.0/Conv: Conv
I (1508) dl::Model: /features/features.7/conv/conv.1/conv.1.2/Clip: Clip
I (1518) dl::Model: /features/features.7/conv/conv.2/Conv: Conv
I (1528) dl::Model: /features/features.8/conv/conv.0/conv.0.0/Conv: Conv
I (1538) dl::Model: /features/features.8/conv/conv.0/conv.0.2/Clip: Clip
I (1538) dl::Model: /features/features.8/conv/conv.1/conv.1.0/Conv: Conv
I (1548) dl::Model: /features/features.8/conv/conv.1/conv.1.2/Clip: Clip
I (1558) dl::Model: /features/features.8/conv/conv.2/Conv: Conv
I (1568) dl::Model: /features/features.8/Add: Add
I (1568) dl::Model: /features/features.9/conv/conv.0/conv.0.0/Conv: Conv
I (1578) dl::Model: PPQ_Operation_1: RequantizeLinear
I (1578) dl::Model: /features/features.9/conv/conv.0/conv.0.2/Clip: Clip
I (1588) dl::Model: /features/features.9/conv/conv.1/conv.1.0/Conv: Conv
I (1598) dl::Model: /features/features.9/conv/conv.1/conv.1.2/Clip: Clip
I (1598) dl::Model: /features/features.9/conv/conv.2/Conv: Conv
I (1608) dl::Model: /features/features.9/Add: Add
I (1618) dl::Model: /features/features.10/conv/conv.0/conv.0.0/Conv: Conv
I (1628) dl::Model: /features/features.10/conv/conv.0/conv.0.2/Clip: Clip
I (1628) dl::Model: /features/features.10/conv/conv.1/conv.1.0/Conv: Conv
I (1638) dl::Model: /features/features.10/conv/conv.1/conv.1.2/Clip: Clip
I (1648) dl::Model: /features/features.10/conv/conv.2/Conv: Conv
I (1648) dl::Model: /features/features.10/Add: Add
I (1658) dl::Model: /features/features.11/conv/conv.0/conv.0.0/Conv: Conv
I (1668) dl::Model: /features/features.11/conv/conv.0/conv.0.2/Clip: Clip
I (1668) dl::Model: /features/features.11/conv/conv.1/conv.1.0/Conv: Conv
I (1678) dl::Model: /features/features.11/conv/conv.1/conv.1.2/Clip: Clip
I (1688) dl::Model: /features/features.11/conv/conv.2/Conv: Conv
I (1698) dl::Model: /features/features.12/conv/conv.0/conv.0.0/Conv: Conv
I (1708) dl::Model: /features/features.12/conv/conv.0/conv.0.2/Clip: Clip
I (1708) dl::Model: /features/features.12/conv/conv.1/conv.1.0/Conv: Conv
I (1718) dl::Model: /features/features.12/conv/conv.1/conv.1.2/Clip: Clip
I (1718) dl::Model: /features/features.12/conv/conv.2/Conv: Conv
I (1728) dl::Model: /features/features.12/Add: Add
I (1738) dl::Model: /features/features.13/conv/conv.0/conv.0.0/Conv: Conv
I (1748) dl::Model: /features/features.13/conv/conv.0/conv.0.2/Clip: Clip
I (1748) dl::Model: /features/features.13/conv/conv.1/conv.1.0/Conv: Conv
I (1758) dl::Model: /features/features.13/conv/conv.1/conv.1.2/Clip: Clip
I (1768) dl::Model: /features/features.13/conv/conv.2/Conv: Conv
I (1778) dl::Model: /features/features.13/Add: Add
I (1778) dl::Model: /features/features.14/conv/conv.0/conv.0.0/Conv: Conv
I (1788) dl::Model: /features/features.14/conv/conv.0/conv.0.2/Clip: Clip
I (1788) dl::Model: /features/features.14/conv/conv.1/conv.1.0/Conv: Conv
I (1798) dl::Model: /features/features.14/conv/conv.1/conv.1.2/Clip: Clip
I (1808) dl::Model: /features/features.14/conv/conv.2/Conv: Conv
I (1818) dl::Model: /features/features.15/conv/conv.0/conv.0.0/Conv: Conv
I (1828) dl::Model: /features/features.15/conv/conv.0/conv.0.2/Clip: Clip
I (1828) dl::Model: /features/features.15/conv/conv.1/conv.1.0/Conv: Conv
I (1838) dl::Model: /features/features.15/conv/conv.1/conv.1.2/Clip: Clip
I (1848) dl::Model: /features/features.15/conv/conv.2/Conv: Conv
I (1858) dl::Model: /features/features.15/Add: Add
I (1858) dl::Model: /features/features.16/conv/conv.0/conv.0.0/Conv: Conv
I (1878) dl::Model: PPQ_Operation_2: RequantizeLinear
I (1878) dl::Model: /features/features.16/conv/conv.0/conv.0.2/Clip: Clip
I (1878) dl::Model: /features/features.16/conv/conv.1/conv.1.0/Conv: Conv
I (1888) dl::Model: /features/features.16/conv/conv.1/conv.1.2/Clip: Clip
I (1888) dl::Model: /features/features.16/conv/conv.2/Conv: Conv
I (1908) dl::Model: /features/features.16/Add: Add
I (1908) dl::Model: /features/features.17/conv/conv.0/conv.0.0/Conv: Conv
I (1918) dl::Model: /features/features.17/conv/conv.0/conv.0.2/Clip: Clip
I (1918) dl::Model: /features/features.17/conv/conv.1/conv.1.0/Conv: Conv
I (1928) dl::Model: /features/features.17/conv/conv.1/conv.1.2/Clip: Clip
I (1928) dl::Model: /features/features.17/conv/conv.2/Conv: Conv
I (1958) dl::Model: /features/features.18/features.18.0/Conv: Conv
I (1988) dl::Model: /features/features.18/features.18.2/Clip: Clip
I (1988) dl::Model: /GlobalAveragePool: GlobalAveragePool
I (1988) dl::Model: PPQ_Operation_3: Transpose
I (1998) dl::Model: /Flatten: Flatten
I (1998) dl::Model: /classifier/classifier.1/Gemm: Gemm
I (2108) MemoryManagerGreedy: Maximum mermory size: 1705984

I (2698) MOBILENET_V2_EXAMPLE: infer_output, name: 536, shape: [1, 1000]
I (2698) MOBILENET_V2_EXAMPLE: output size: 1000
I (2708) MOBILENET_V2_EXAMPLE: exit app_main
I (2708) main_task: Returned from app_main()

```