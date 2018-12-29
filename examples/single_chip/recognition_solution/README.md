# Face Detection and Recognition with ESP-EYE

This example is supposed to demonstrate the elementry function of the board [ESP-EYE](), which is based on ESP32 chip and intergrated with camera, led, and digital microphone modules[detection_with_command_line example](./detection_with_command_line), but displaies the image stream through http server.

## General Information

用户可以通过连接esp32建立的softap，在局域网内访问其特定的网址，不管是手机端还是电脑端，都可以在网页上看到图像拍摄和检测的效果。

## 安装及使用

- 通过`make defconfig`使用默认配置
- 通过`make menuconfig`设置Wi-Fi的用户名和密码
- 烧入程序`make flash`
- 找到一个手机或者电脑，连接上建立的softap
- 在浏览器中输入`192.168.4.1/face_stream`
- Please enjoy it

## 图像格式及尺寸

我们建议使用默认的320x240的像素大小，因为更大的像素会带来更多的计算。

输入输出的图像格式都使用jpeg，比一般的RGB小很多，适合网络的传输。

## 配置(Configuration)

- 可增加自己定制的handler，例如本例子使用的就是
```
httpd_uri_t _face_stream_handler = {
    .uri       = "/face_stream",
    .method    = HTTP_GET,
    .handler   = facenet_stream_handler,
    .user_ctx  = NULL
};
```
在初始化过程中进行注册:
`httpd_register_uri_handler(camera_httpd, &_face_stream_handler);`

- 可增加camera的缓存队列，提高并行的速度
`camera_config_t config.fb_count = 2`


