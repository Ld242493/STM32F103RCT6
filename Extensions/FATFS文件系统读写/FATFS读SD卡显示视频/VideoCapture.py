import numpy as np
import cv2

vc = cv2.VideoCapture("BadApple.mp4")  # 读入视频文件，命名cv
n = 1  # 计数

if vc.isOpened():  # 判断是否正常打开
    rval, frame = vc.read()
else:
    rval = False

timeF = 5  # 视频帧计数间隔频率

i = 0
while rval:  # 循环读取视频帧
    rval, frame = vc.read()
    if not rval:
        break
    frame = cv2.resize(frame, (128, 64))  # 图像尺寸调整到240*240大小
    # cropped_frame=frame[120:240,0:240]
    if (n % timeF == 0):  # 每隔timeF帧进行存储操作
        i += 1
        print(i)
        cv2.imwrite("./Image/{}.jpg".format(i),
                   frame)  # 存储为图像
    n = n + 1
    cv2.waitKey(1)
vc.release()