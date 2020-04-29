import cv2
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-i")
arg = parser.parse_args()

image = cv2.imread(arg.i)

image = cv2.resize(image, (320, 240))
image = cv2.cvtColor(image, cv2.COLOR_BGR2BGR565)
image = image[:, :, ::-1]

with open("input.h", 'w') as f:
    f.write('#include <stdint.h>\n')
    f.write('const static uint8_t input_item[] = {\n')
    for h in range(240):
        for w in range(320):
            for c in range(2):
                f.write(f'{image[h, w, c]}, ')
            if w % 8 == 0:
                f.write('\n')

    f.write('\n};\n')
    f.write('const uint8_t *get_input() { return input_item; }\n')
