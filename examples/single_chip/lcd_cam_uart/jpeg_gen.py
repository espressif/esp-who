import struct

f1 = open('jpegtest.txt', 'r')
f2 = open("jpegtest.jpeg", 'wb')
res = f1.read()
cc = res.split(' ')
numbers = [int(n) for n in cc]
for i in numbers:
    num = struct.pack('>B', i)
    f2.write(num)
f1.close()
f2.close()