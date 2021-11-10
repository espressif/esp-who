# ESP-S3-EYE V2.1 默认固件说明



### 产测流程
**产测分为6个测试项，分别为：**
- SD 卡读写测试
- 按键测试
- LED 灯测试
- LCD 显示屏测试
- 摄像头测试

#### 进入产测模式
开机上电后5秒内在终端输入testmode并按下回车，进入测试模式。
开发板打印为：
```
E (1956) ESP_MAC_WIFI_STA ADDRESS: 7cdfa1e0ac88

ESP32-S3-EYE > testmode
I (5106) gpio: GPIO[0]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2 
W (5106) eFuse: calibration efuse version does not match, set default version: 0
I (5116) gpio: GPIO[3]| InputEn: 0| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
Initializing SPIFFS
Partition size: total: 2857886, used: 2629727
model_name: hilexin7q8 model_data: /srmodel/hilexin7q8/wn7q8_data
accelate model: 1
0x3d800974
MC Quantized-8 wakeNet7: wakeNet7Q8_v2_hilexin_5_0.97_0.90, mode:2, p:3, (Oct 28 2021 17:15:22)
Initial ONE-MIC auido front-end for speech recognition, mode:0, (Oct 22 2021 10:56:28)
I (6276) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=3
I (6706) ESP32-S3-EYE: --------------- Enter Test Mode ---------------

ESP32-S3-EYE V2.0 Firmware V0.1.0
FACTORY Test Mode

auto: 	auto test
all: 	test all
id0: 	SD Card test
id1: 	Button test
id2: 	LED test
id3: 	LCD test
id4: 	CAMERA test
id5: 	MIC test

ESP32-S3-EYE > 

```

#### SD卡测试
- 操作：确保SD卡在开发板卡槽内，输入id0
- 打印：
```
ESP32-S3-EYE > id0
I (1027856) ESP32-S3-EYE: --------------- Enter SD CARD Test ---------------

I (1027856) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1027856) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1027856) gpio: GPIO[40]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
Name: SC16G
Type: SDHC/SDXC
Speed: 20 MHz
Size: 15193MB
I (1028076) SD_CARD: --------------- SD CARD Test PASS ---------------

```

#### 按键测试
- 操作：输入id1，根据提示按下对应按键
- 打印：
```
ESP32-S3-EYE > id1
I (1141816) ESP32-S3-EYE: --------------- Enter Button Test ---------------

Please press BOOT Button
W (1144516) BUTTON: --------------- BOOT Button Test PASS ---------------

Please press MENU Button
W (1145856) BUTTON: --------------- MENU Button Test PASS ---------------

Please press PLAY Button
W (1146456) BUTTON: --------------- PLAY Button Test PASS ---------------

Please press UP+ Button
W (1147696) BUTTON: --------------- UP+ Button Test PASS ---------------

Please press DN- Button
W (1148416) BUTTON: --------------- DN- Button Test PASS ---------------

I (1148416) BUTTON: --------------- Button Test PASS ---------------


```

#### LED测试
- 操作：输入id2，根据开发板LED是否闪烁按下对应按键
- 打印：
```
ESP32-S3-EYE > id2
I (1133116) ESP32-S3-EYE: --------------- Enter LED Test ---------------

Please check if the LED works
Press UP+ if works, Press MENU if failed
I (1139106) LED: --------------- LED Test PASS ---------------

```

#### LCD测试
- 操作：输入id3，根据开发板LCD是否依次显示红绿蓝白颜色按下对应按键
- 打印：
```
ESP32-S3-EYE > id3
I (1391936) ESP32-S3-EYE: --------------- Enter LCD Test ---------------

I (1391946) spi_bus: SPI2 bus created
I (1391946) lcd st7789: MADCTL=0vice added, CS=44 Mode=0 Speed=40000000
I (1391946) LCD: Screen name:ST7789 | width:240 | height:240
W (1391946) LCD: Display Red 

W (1393016) LCD: Display Green 

W (1394086) LCD: Display Blue 

W (1395156) LCD: Display White 

Please check if the LCD works
Press UP+ if works, Press MENU if failed
I (1399796) LCD: --------------- LCD Test PASS ---------------

```

#### 摄像头测试
- 操作：输入id4，将摄像头正放置在人脸前
- 打印：
```
ESP32-S3-EYE > id4
I (1548636) s3 ll_cam: DMA Channel=4
I (1548636) cam_hal: cam init ok
I (1548636) sccb: pin_sda 4 pin_scl 5
I (1548646) camera: Detected camera at address=0x30
I (1548646) camera: Detected OV2640 camera
I (1548646) camera: Camera PID=0x26 VER=0x42 MIDL=0x7f MIDH=0xa2
I (1548716) s3 ll_cam: node_size: 3840, nodes_per_line: 1, lines_per_node: 8
I (1548716) s3 ll_cam: dma_half_buffer_min:  3840, dma_half_buffer: 11520, lines_per_half_buffer: 24, dma_buffer_size: 23040
I (1548716) cam_hal: buffer_size: 23040, half_buffer_size: 11520, node_buffer_size: 3840, node_cnt: 6, total_cnt: 10
I (1548716) cam_hal: Allocating 115200 Byte frame buffer in PSRAM
I (1548716) cam_hal: Allocating 115200 Byte frame buffer in PSRAM
I (1548726) cam_hal: cam config ok
I (1548726) ov2640: Set PLL: clk_2x: 1, clk_div: 3, pclk_auto: 1, pclk_div: 8
W (1548796) CAMERA: Please place the camera in front of the human face

I (1548966) CAMERA: --------------- CAMERA Test PASS ---------------

```

#### 麦克风测试
- 操作：输入id5，说出或播放“Hi，乐鑫”
- 打印：
```
ESP32-S3-EYE > id5
I (1632756) ESP32-S3-EYE: --------------- Enter MIC Test ---------------

W (1632756) MIC: Please Say "Hi, Lexin"

I (1632756) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1632756) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1632756) gpio: GPIO[40]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1634996) MIC: --------------- MIC Test PASS ---------------

```

#### 快速测试选项
- auto： 测试所有未过项。
- all： 测试全部测试项。

#### 测试结果
- 当所有项都已测试过一遍后会打印测试结果
- 所有项都通过：
```
I (1634996) ESP32-S3-EYE: *************** Test PASS ***************
```

- 存在未过项：
```
E (2048066) ESP32-S3-EYE: *************** Test FAIL ***************
```
