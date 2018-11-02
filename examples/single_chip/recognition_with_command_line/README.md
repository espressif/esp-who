# Recognition with Command Line in Single Chip

This example implements Human Face Recognition with a single ESP32 chip and without LCD. ESP32 gets input of image from camera and displays results in the Command Line after recognition.

# Preparation

To run this example, you need the following components:

* ESP32 module: this example has been tested with **ESP32-WROVER**, which is highly recommended for getting started with.
* Camera module: this example has been tested with **OV2640** camera module which is highly recommended for getting started with.
* Set up [ESP-IDF](https://github.com/espressif/esp-idf)
* Set up [ESP-WHO](https://github.com/espressif/esp-who)

Any other confusions about preparation, please see general guide in the README.md of ESP-WHO.


# Quick Start

If preparations are ready, please follow this section to **connect** the camera to ESP32 module, flash application to ESP32, finally execute human face recognition and display the **result**.

## Connect
Specific pins used in this example to connect ESP32 module and camera module are listed in table below.

| Interface | Camera Pin | Pin Mapping for ESP32-WROVER |
| :--- | :---: | :---: |
| SCCB Clock | SIOC | IO27 |
| SCCB Data | SIOD | IO26 |
| System Clock | XCLK | IO21 |
| Vertical Sync | VSYNC | IO25 |
| Horizontal Reference | HREF | IO23 |
| Pixel Clock | PCLK | IO22 |
| Pixel Data Bit 0 | D2 | IO4 |
| Pixel Data Bit 1 | D3 | IO5 |
| Pixel Data Bit 2 | D4 | IO18 |
| Pixel Data Bit 3 | D5 | IO19 |
| Pixel Data Bit 4 | D6 | IO36 |
| Pixel Data Bit 5 | D7 | IO39 |
| Pixel Data Bit 6 | D8 | IO34 |
| Pixel Data Bit 7 | D9 | IO35 |
| Camera Reset | RESET | IO2 |
| Camera Power Down | PWDN | IO0 |
| Power Supply 3.3V | 3V3 | 3V3 |
| Ground | GND | GND |


In particular, if you have a **ESP-WROVER-KIT**, camera connector is already broken out and labeled Camera / JP4. Solder 2.54 mm / 0.1" double row, 18 pin socket in provided space and plug the camera module, OV2640 for example, right into it. Line up 3V3 and GND pins on camera module and on ESP-WROVER-KIT. D0 and D1 should be left unconnected outside the socket. The image below shows **ESP-WROVER-KIT** plugged with **OV2640** camera module.

![esp_wrover_kit_with_ov2640](../../../img/esp_wrover_kit_with_ov2640.png)  


## Results

Open a serial terminal by using `make monitor` at this project, point the camera to a human face with a distance of 0.3m at least, then face entry will start after two faces are detected. The following information shows the output of the command line before the entry process:

![login_delay2](../../../img/login_delay2.png)
![login_delay1](../../../img/login_delay1.png)

#### Login

Then the face entry process will begin, during which the program will input multiple faces to get a person's ID:

![start_login](../../../img/start_login.png)

You can also reset the number of faces entered by one person, which is set to 3 by default.
When the last face of ID is entered, the process of face recognition will start:

![start_recognition](../../../img/start_recognition.png)

#### Recognition

When a face is detected, it will be recognized whether the face is the same as the entered ID. If it is the same, the corresponding ID number will be displayed:

![recognition_matched](../../../img/matched.png)

Otherwise, the command line will display `No Matched ID`:

![recognition_no_matched](../../../img/no_matched.png)
