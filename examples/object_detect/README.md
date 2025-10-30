# Object Detection Example

## Supported detect models

| model                                                                                        | support or not     |
|----------------------------------------------------------------------------------------------|--------------------|
| [human_face_detect](https://components.espressif.com/components/espressif/human_face_detect) | :heavy_check_mark: |
| [pedestrian_detect](https://components.espressif.com/components/espressif/pedestrian_detect) | :heavy_check_mark: |
| [cat_detect](https://components.espressif.com/components/espressif/cat_detect)               | :heavy_check_mark: |
| [dog_detect](https://components.espressif.com/components/espressif/dog_detect)               | :heavy_check_mark: |

## How to use

Follow the [Quick Start](../../README.md#quick-start). The only difference is that besides the target SOC and the default sdkconfig configuration file, the detect model needs to be specified too.

```
idf.py -DSDKCONFIG_DEFAULTS=sdkconfig.bsp.bsp_name -DDETECT_MODEL=xxx_detect set-target esp32xx
```