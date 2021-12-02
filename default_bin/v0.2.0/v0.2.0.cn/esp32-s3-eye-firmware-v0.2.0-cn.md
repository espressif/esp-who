# ESP-S3-EYE 默认固件说明

### 固件版本
v0.2.0-cn

该固件包含产测与应用两部分。

支持的开发板：
- ESP32-S3-EYE V2.1
- ESP32-S3-EYE V2.2

更新： 
1. 加入LCD显示测试进程。
2. 加入测试log写入sd卡功能。
3. 修复加速度传感器第一次测试失败后，第二次再次测试所读数据有误的bug。

### 固件地址
- 0x0			  bootloader.bin
- 0x8000	   partition-table.bin
- 0x10000	 default-bin.bin
- 0x3f8000    model.bin

### 产测流程
**产测分为7个测试项，分别为：**

- SD 卡读写测试
- 三轴加速度传感器测试
- 按键测试
- LED 灯测试
- LCD 显示屏测试
- 摄像头测试
- 麦克风测试

#### 进入产测模式
开机上电后程序初始化，随后打印芯片mac地址，在这之后**3秒内按下boot键**，进入测试模式。
此时LCD会显示**测试进程**，表示已进入测试模式

LCD的显示：
- 第一行为芯片MAC地址。
- 最后一行为当前固件版本，末尾CN为中文固件， 末尾EN为英文固件。
- 每一个测试项的结果： “FAIL“为测试失败， ”PASS”为测试通过， “？“ 为尚未测试，”？ *“ 为正在测试 

开发板打印为：

```
E (1080) ESP_MAC_WIFI_STA ADDRESS: 7cdfa1e0ab78

I (1080) gpio: GPIO[0]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:3 
I (2640) ESP32-S3-EYE-V2.1: --------------- Enter Test Mode ---------------

ESP32-S3-EYE V2.1 Firmware CN V0.1.3
I (2640) spi_bus: SPI2 bus created
I (2640) spi_bus: SPI2 bus device added, CS=44 Mode=0 Speed=40000000
I (2640) lcd st7789: MADCTL=0
I (2640) LCD: Screen name:ST7789 | width:240 | height:240
W (3160) eFuse: calibration efuse version does not match, set default version: 0
I (3160) i2c_bus: i2c0 bus inited
W (3160) qma7981: ID : E7
I (3160) gpio: GPIO[3]| InputEn: 0| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0 
Initializing SPIFFS
Partition size: total: 3287096, used: 2629225
model_name: hilexin7q8 model_data: /srmodel/hilexin7q8/wn7q8_data
accelate model: 1
MC Quantized-8 wakeNet7: wakeNet7Q8_v2_hilexin_5_0.97_0.90, mode:2, p:3, (Nov 23 2021 15:19:36)
Initial ONE-MIC auido front-end for speech recognition, mode:0, (Nov 23 2021 15:35:55)
I (4080) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=3
I (4160) ESP32-S3-EYE: --------------- Test Start ---------------

```

随后开发板会自动依次进入：
1. SD 卡读写测试
2. 三轴加速度传感器测试
3. 按键测试
4. LED 灯测试
5. LCD 显示屏测试
6. 摄像头测试
7. 麦克风测试

#### SD卡测试
- 操作：确保SD卡在开发板卡槽内
- 打印：
```
I (3697) ESP32-S3-EYE: --------------- Enter SD CARD Test ---------------

I (3697) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (3697) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (3697) gpio: GPIO[40]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
Name: SC16G
Type: SDHC/SDXC
Speed: 20 MHz
Size: 15193MB
I (3787) SD_CARD: --------------- SD CARD Test PASS ---------------

```

#### 三轴加速度传感器测试
- 操作：依次测试Z轴，X轴和Y轴。
- 测试Z轴时，请在10秒内将开发板背面朝向地面。
- 测试X轴时，请在10秒内将开发板底面朝向地面。
- 测试Y轴时，请在10秒内将开发板左面朝向地面。
- 打印：
```
I (4417) ESP32-S3-EYE: --------------- Enter IMU Test ---------------

W (4417) IMU: Test Z axis

W (6917) IMU: --------------- Z axis Test PASS ---------------

W (6917) IMU: Test X axis

W (9617) IMU: --------------- X axis Test PASS ---------------

W (9617) IMU: Test Y axis

W (11217) IMU: --------------- Y axis Test PASS ---------------

I (11217) IMU: --------------- IMU Test PASS ---------------

```

#### 按键测试
- 操作：根据提示按下对应按键，每个按键请在10秒内按下。
- 打印：
```
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
- 操作：根据开发板LED是否闪烁按下对应按键
- 打印：
```
I (1133116) ESP32-S3-EYE: --------------- Enter LED Test ---------------

Please check if the LED works
Press UP+ if works, Press MENU if failed
I (1139106) LED: --------------- LED Test PASS ---------------

```

#### LCD测试
- 操作：根据开发板LCD是否依次显示红绿蓝白颜色按下对应按键
- 打印：
```
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
- 操作：将摄像头正放置在人脸前
- 打印：
```
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
- 操作：说出或播放“Hi，乐鑫”
- 打印：
```
I (1632756) ESP32-S3-EYE: --------------- Enter MIC Test ---------------

W (1632756) MIC: Please Say "Hi, LeXin"

I (1632756) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1632756) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1632756) gpio: GPIO[40]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1634996) MIC: --------------- MIC Test PASS ---------------

```


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

#### 未过项测试
- 当所有项都测完，且存在未过测试项时，再次按下boot键，可依次对未过项再次进行测试。


### 默认固件使用

#### 进入默认固件
上电后等待3秒，不对开发板做任何操作，当LCD屏幕显示Espressif LOGO表示已进入默认固件。

#### 默认固件模式
默认固件存在四种运行模式，固件会默认先进入人脸识别模式：
1. 待机模式：该模式下开发板只有LCD工作，显示公司LOGO。
2. 实时显示模式：该模式下LCD会实时显示摄像头拍摄到的画面。
3. 人脸识别模式：该模式下LCD会实时显示摄像头拍摄到的画面，如果画面中存在人脸，LCD显示的画面上会框出人脸，并画出人脸的五个特征点。该模式下提供三种功能：录入人脸、识别人脸、删除人脸。
   - 录入人脸： 按下MENU键。或者使用“Hi，乐鑫”唤醒词唤醒，唤醒后摄像头旁的LED会被点亮，随后说出“添加人脸”。
   - 识别人脸： 按下UP键。或者使用“Hi，乐鑫”唤醒词唤醒，唤醒后摄像头旁的LED会被点亮，随后说出“识别一下”。
   - 删除人脸： 按下PLAY键。或者使用“Hi，乐鑫”唤醒词唤醒，唤醒后摄像头旁的LED会被点亮，随后说出“删除人脸”。
4. 移动侦测模式： 该模式下LCD会实时显示摄像头拍摄到的画面。若画面中存在物体移动，LCD中左上角会显示蓝色色块。

#### 模式切换
在任何时候都可以使用“Hi，乐鑫”唤醒词唤醒开发板，四种模式分别对应四个命令词。说出相应的命令词，识别成功后随即切换到对应的模式。工作模式与命令词的对应关系如下：
1. 待机模式：“停止工作”
2. 实时显示模式：“仅显示”
3. 人脸识别模式：“人脸识别”
4. 移动侦测模式：“移动侦测”

#### 语音系统
- 语音唤醒：默认固件工作时，任何时刻都可以使用“Hi，乐鑫”唤醒词唤醒开发板。开发板被唤醒后，摄像头旁的LED会常亮。
- 命令词识别：唤醒后会等待用户说出命令词。若识别成功，摄像头旁的LED会从常亮变为闪烁一秒, LED会随后熄灭。