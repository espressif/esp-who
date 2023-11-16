# ESP-S3-EYE [[中文](./README_CN.md)]

Note that this example is not compatible with idf5.0 yet.

This example is designed for [**ESP32-S3-EYE**](https://www.espressif.com/zh-hans/products/devkits) development kit **only**, which can help to get started with it easily and to experience the functions of speech wake-up, speech command recognition, human face detection and recognition.



## Run Example

### Step 1: Set wake-up word and command words

This example presets two sets of wake-up words and command words in Chinese and English.

#### Chinese:

If you use Chinese, you need to mute [Line49~Line60](./sdkconfig.defaults#L49) in `sdkconfig.defaults` by adding "#" in front, and delete the "#" in the front of [Line35~Line46](./sdkconfig.defaults#L35). *Chinese is set by defaults. So in defaults setting, nothing should be modified.*

The Chinese wake-up word is "Hi, Lexin", and the command words include: "Ting Zhi Gong Zuo", "Jin Xian Shi", "Ren Lian Shi Bie", "Yi Dong Zhen Ce", "Tian Jia Ren Lian", "Shan Chu Ren Lian" and "Shi Bie Yi Xia".

#### English:

If you use English, you need to mute [Line35~Line46](./sdkconfig.defaults#L35) in `sdkconfig.defaults` by adding "#" in front, and delete the "#" in the front of [Line49~Line60](./sdkconfig.defaults#L49).

The English wake-up word is "Hi, ESP", and the command words include: "Stop Working", "Display Only", "Face Recognition", "Motion Detection", "Enter Face", "Delete Face" and "Recognize Face".

> Of course, you can also choose other wake-up words and command words in menuconfig.



### Step 2: Set target chip

Open the terminal and go to this examples. Run the following command to set the target chip to ESP32-S3.

```shell
idf.py set-target esp32s3
```



### Step 3: Launch and monitor

Flash the program and launch IDF Monitor:

```shell
idf.py flash monitor
```



## Example Description

### Working modes

This example includes four working modes: "Standby Mode", "Real-time Display", "Face Recognition" and "Motion Detection". After power on, it enters "Standby Mode" by default:

1. Standby Mode: in this mode, only the espressif logo is displayed on the LCD screen. This mode is also the default mode after the development kit is powered on.
1. Real-time Display: in this mode, the pictures collected by the camera will be displayed on the LCD screen in real time.
1. Face Recognition: in this mode, the pictures collected by the camera will be displayed on the LCD screen in real time. The face in the picture is detected in real time, and the detection box and key points are displayed. Operations related to face recognition can be realized through button press and speech interaction. See the following table for details:

| Operations     | Description                                               | Button | Speech                                          |
| :------------- | :-------------------------------------------------------- | :----- | :---------------------------------------------- |
| Recognize Face | Recognize the face in picture and display result          | "PLAY" | Wake up by "Hi, ESP", then say "Recognize Face" |
| Enter Face     | Enter the face in picture and display its ID number       | "UP+"  | Wake up by "Hi, ESP", then say "Enter Face"     |
| Delete Face    | Delete the last ID in database and display left ID number | "DN-"  | Wake up by "Hi, ESP", then say "Delete Face"    |

4. Motion detection: in this mode, the pictures collected by the camera will be displayed on the LCD screen in real time. And detect whether the object moves in the screen in real time. If the object moves, the blue dot will be displayed in the upper left corner of the screen, otherwise it will not.



### Mode Switching

Mode switching can be realized by speech and button interaction.



#### By speech

The mode can be switched by speech at any time. The corresponding detailed methods are shown in the table below:

| Mode              | Chinese                                              | English                                           |
| :---------------- | ---------------------------------------------------- | ------------------------------------------------- |
| Standby Mode      | Wake up by "Hi, Lexin", then say "Ting Zhi Gong Zuo" | Wake up by "Hi, ESP", then say "Stop Working"     |
| Real-time Display | Wake up by "Hi, Lexin", then say "Jin Xian Shi"      | Wake up by "Hi, ESP", then say "Display Only"     |
| Face Recognition  | Wake up by "Hi, Lexin", then say "Ren Lian Shi Bie"  | Wake up by "Hi, ESP", then say "Face Recognition" |
| Motion Detection  | Wake up by "Hi, Lexin", then say "Yi Dong Zhen Ce"   | Wake up by "Hi, ESP", then say "Motion Detection" |



#### By button

At any time, you can switch modes by pressing the "MENU" button. The modes will be switched in the following order: "Standby Mode" > "Real-time Display" > "Face Recognition" > "Motion Detection". Pressing the "MENU" button again will return to the "Standby Mode" and repeat.



### LED interaction

#### Feedback for speech

When the wake-up word is spoken and the wake-up is successful, the LED light on the development kit lights up to wait for the command word. At this point,

- If the command word is spoken within 6 seconds and recognized correctly, the LED will flash for one second and then go out to return to the state of waiting to wake up,
- If the command word is not spoken or recognized correctly within 6 seconds, the LED goes out after 6 seconds and returns to the state to of waiting to wake up.



#### Feedback for button

When any button is pressed (excluding "boot" and "reset"), the LED will flash for one second.