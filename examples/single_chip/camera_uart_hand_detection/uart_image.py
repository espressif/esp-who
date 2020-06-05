import cv2
import struct
import serial
import numpy as np
from io import BytesIO
from threading import Thread
from PIL import Image, ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES = True


class UartImage:
    def __init__(self, dev="/dev/ttyUSB0", baud_rate=2000000, timeout=2.0, flag=b"get", size=(640, 480), save=False, name="v0"):
        self.dev = dev
        self.baud_rate = baud_rate
        self.timeout = timeout
        self.flag = flag
        self.size = size
        self.save = save
        self.name = name

    def create_uart(self):
        ser = serial.Serial(self.dev, self.baud_rate, timeout=self.timeout)
        if ser.isOpen():
            print("open success")
        else:
            print("open failed")

        cv2.namedWindow(self.dev, 0)
        cv2.resizeWindow(self.dev, self.size[0], self.size[1])

        count = 0

        fourcc = cv2.VideoWriter_fourcc('X', 'V', 'I', 'D')
        fps = 5.0
        out = cv2.VideoWriter('./outVideo.avi', fourcc, fps, self.size)

        while 1:
            count += 1
            ser.write(self.flag)
            data = ser.read(5)
            while b"start" not in data:
                data = ser.read(5)

            data = ser.read(4)
            # print(data.decode(errors="ignore"))
            while not len(data):
                data = ser.read(4)

            length = struct.unpack("I", data)[0]
            print(length)
            data = ser.read(length)
            jpeg_img = data
            try:
                image = Image.open(BytesIO(jpeg_img))
            except (IOError, ValueError, TypeError):
                continue
            image_bgr = np.asarray(image)[:, :, [2, 1, 0]]
            # out.write(image_bgr)
            cv2.imshow(self.dev, image_bgr)
            if self.save:
                cv2.imwrite("./imgs/%s_%d.jpg" % (self.name, count), image_bgr)
                # cv2.imwrite("./imgs/%s_%d_80.jpg" % (self.name, count), cv2.resize(image_bgr, (80, 60)))
                # cv2.imwrite("./imgs/%s_%d_160.jpg" % (self.name, count), cv2.resize(image_bgr, (160, 120)))
                # cv2.imwrite("./imgs/%s_%d_240.jpg" % (self.name, count), cv2.resize(image_bgr, (240, 180)))
                # cv2.imwrite("./imgs/%s_%d_320.jpg" % (self.name, count), cv2.resize(image_bgr, (320, 240)))
                # cv2.imwrite("./imgs/%s_%d_400.jpg" % (self.name, count), cv2.resize(image_bgr, (400, 296)))
                # cv2.imwrite("./imgs/%s_%d_640.jpg" % (self.name, count), cv2.resize(image_bgr, (640, 480)))
            if cv2.waitKey(1) & 0xFF == ord('q'):
                out.release()
                cv2.destroyAllWindows()
                break


if __name__ == "__main__":
    uart_img = UartImage(size=(320, 240), save=True, name="v525")
    t = Thread(target=uart_img.create_uart)
    t.start()
