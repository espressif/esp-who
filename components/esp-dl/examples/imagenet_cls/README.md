| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |


# IMAGENET classification Example

Mobilenetv2 is now supported.

# Example Output

After the flashing you should see the output at idf monitor:

```
W (1836) FbsLoader: There is only one model in the flatbuffers, ignore the input model name!
I (1836) dl::Model: model:main_graph, version:0

I (1836) dl::Model: /features/features.0/features.0.0/Conv: Conv
I (1846) dl::Model: /features/features.1/conv/conv.0/conv.0.0/Conv: Conv
I (1846) dl::Model: /features/features.1/conv/conv.1/Conv: Conv
I (1856) dl::Model: /features/features.2/conv/conv.0/conv.0.0/Conv: Conv
I (1856) dl::Model: /features/features.2/conv/conv.1/conv.1.0/Conv: Conv
I (1866) dl::Model: /features/features.2/conv/conv.2/Conv: Conv
I (1876) dl::Model: /features/features.3/conv/conv.0/conv.0.0/Conv: Conv
I (1876) dl::Model: /features/features.3/conv/conv.1/conv.1.0/Conv: Conv
I (1886) dl::Model: /features/features.3/conv/conv.2/Conv: Conv
I (1896) dl::Model: /features/features.3/Add: Add
I (1896) dl::Model: /features/features.4/conv/conv.0/conv.0.0/Conv: Conv
I (1906) dl::Model: /features/features.4/conv/conv.1/conv.1.0/Conv: Conv
I (1906) dl::Model: /features/features.4/conv/conv.2/Conv: Conv
I (1916) dl::Model: /features/features.5/conv/conv.0/conv.0.0/Conv: Conv
I (1916) dl::Model: PPQ_Operation_0: RequantizeLinear
I (1926) dl::Model: /features/features.5/conv/conv.1/conv.1.0/Conv: Conv
I (1936) dl::Model: /features/features.5/conv/conv.2/Conv: Conv
I (1936) dl::Model: /features/features.5/Add: Add
I (1946) dl::Model: /features/features.6/conv/conv.0/conv.0.0/Conv: Conv
I (1946) dl::Model: /features/features.6/conv/conv.1/conv.1.0/Conv: Conv
I (1956) dl::Model: /features/features.6/conv/conv.2/Conv: Conv
I (1956) dl::Model: /features/features.6/Add: Add
I (1966) dl::Model: /features/features.7/conv/conv.0/conv.0.0/Conv: Conv
I (1966) dl::Model: /features/features.7/conv/conv.1/conv.1.0/Conv: Conv
I (1976) dl::Model: /features/features.7/conv/conv.2/Conv: Conv
I (1986) dl::Model: /features/features.8/conv/conv.0/conv.0.0/Conv: Conv
I (1986) dl::Model: /features/features.8/conv/conv.1/conv.1.0/Conv: Conv
I (1996) dl::Model: /features/features.8/conv/conv.2/Conv: Conv
I (2006) dl::Model: /features/features.8/Add: Add
I (2006) dl::Model: /features/features.9/conv/conv.0/conv.0.0/Conv: Conv
I (2016) dl::Model: /features/features.9/conv/conv.1/conv.1.0/Conv: Conv
I (2016) dl::Model: /features/features.9/conv/conv.2/Conv: Conv
I (2026) dl::Model: /features/features.9/Add: Add
I (2026) dl::Model: /features/features.10/conv/conv.0/conv.0.0/Conv: Conv
I (2036) dl::Model: /features/features.10/conv/conv.1/conv.1.0/Conv: Conv
I (2046) dl::Model: /features/features.10/conv/conv.2/Conv: Conv
I (2046) dl::Model: /features/features.10/Add: Add
I (2056) dl::Model: /features/features.11/conv/conv.0/conv.0.0/Conv: Conv
I (2056) dl::Model: /features/features.11/conv/conv.1/conv.1.0/Conv: Conv
I (2066) dl::Model: /features/features.11/conv/conv.2/Conv: Conv
I (2066) dl::Model: /features/features.12/conv/conv.0/conv.0.0/Conv: Conv
I (2076) dl::Model: PPQ_Operation_1: RequantizeLinear
I (2086) dl::Model: /features/features.12/conv/conv.1/conv.1.0/Conv: Conv
I (2086) dl::Model: /features/features.12/conv/conv.2/Conv: Conv
I (2096) dl::Model: /features/features.12/Add: Add
I (2096) dl::Model: /features/features.13/conv/conv.0/conv.0.0/Conv: Conv
I (2106) dl::Model: /features/features.13/conv/conv.1/conv.1.0/Conv: Conv
I (2116) dl::Model: /features/features.13/conv/conv.2/Conv: Conv
I (2116) dl::Model: /features/features.13/Add: Add
I (2126) dl::Model: /features/features.14/conv/conv.0/conv.0.0/Conv: Conv
I (2126) dl::Model: /features/features.14/conv/conv.1/conv.1.0/Conv: Conv
I (2136) dl::Model: /features/features.14/conv/conv.2/Conv: Conv
I (2136) dl::Model: /features/features.15/conv/conv.0/conv.0.0/Conv: Conv
I (2146) dl::Model: PPQ_Operation_2: RequantizeLinear
I (2156) dl::Model: /features/features.15/conv/conv.1/conv.1.0/Conv: Conv
I (2156) dl::Model: /features/features.15/conv/conv.2/Conv: Conv
I (2166) dl::Model: /features/features.15/Add: Add
I (2166) dl::Model: /features/features.16/conv/conv.0/conv.0.0/Conv: Conv
I (2176) dl::Model: PPQ_Operation_3: RequantizeLinear
I (2176) dl::Model: /features/features.16/conv/conv.1/conv.1.0/Conv: Conv
I (2186) dl::Model: /features/features.16/conv/conv.2/Conv: Conv
I (2196) dl::Model: /features/features.16/Add: Add
I (2196) dl::Model: /features/features.17/conv/conv.0/conv.0.0/Conv: Conv
I (2206) dl::Model: /features/features.17/conv/conv.1/conv.1.0/Conv: Conv
I (2206) dl::Model: /features/features.17/conv/conv.2/Conv: Conv
I (2216) dl::Model: /features/features.18/features.18.0/Conv: Conv
I (2226) dl::Model: /GlobalAveragePool: GlobalAveragePool
I (2226) dl::Model: PPQ_Operation_4: Transpose
I (2226) dl::Model: /Flatten: Flatten
I (2236) dl::Model: /classifier/classifier.1/Gemm: Gemm
I (2246) MemoryManagerGreedy: Maximum mermory size: 1705984

I (2626) imagenet_cls: category: tabby, score: 0.356466

I (2626) imagenet_cls: category: lynx, score: 0.216208

I (2626) imagenet_cls: category: Egyptian_cat, score: 0.168383

I (2626) imagenet_cls: category: tiger_cat, score: 0.131137

I (2636) imagenet_cls: category: window_screen, score: 0.029261

I (2646) main_task: Returned from app_main()
```