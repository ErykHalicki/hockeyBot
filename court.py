
from collections import deque
from imutils.video import VideoStream
import numpy as np
import argparse
import serial
import time
import numpy as np
import cv2
import imutils
import time
import math
def PointsInCircum(r,n=100):
    return [(math.cos(2*math.pi/n*x)*r,math.sin(2*math.pi/n*x)*r) for x in range(0,n+1)]

# construct the argument parse and parse the arguments
#paramaters
thresh=60#thresh % of all points of the radius reduced minEnclosingcircle have to be within the contour
colorLower = (0, 50, 165)#lower bound for binary mask creating
colorUpper = (5, 200, 255)#upper bound for binary mask creating
blurring=2#blurring factor, how many iterations of erode and dilate to run
radMultiplier=0.6#how close to a circle does the opject have to be, 1 - perfect circle, 0 - not a circle at all
cap=cv2.VideoCapture(0)
#centimeter pixels
walls=[]#create a wall object
class wall:
    def __init__(self, l,r, Y):
        self.l = l
        self.r = r
        self.y = Y
def initialize():
    global ser
    try:
        ser = serial.Serial('/dev/ttyUSB0', 9600,timeout=0)# serial port for sensor arduino
        ser.flush()
    except Exception as e:
        try:
            ser = serial.Serial('/dev/ttyUSB1', 9600,timeout=0)
            ser.flush()
        except Exception as e:
            try:
                ser = serial.Serial('/dev/tty.usbserial-110', 9600,timeout=0)
                print("110")
                ser.flush()
            except Exception as e:
                try:
                    ser = serial.Serial('/dev/tty.usbserial-1110', 9600,timeout=0)
                    print("1110")
                    ser.flush()
                except:
                    return False
    return True

def scan():
    # grab the current frame
    ret, frame = cap.read()
    # handle the frame from VideoCapture or VideoStream
    # if we are viewing a video and we did not grab a frame,
    # then we have reached the end of the vide
    # resize the frame, blur it, and convert it to the HSV
    # color space
    frame = imutils.resize(frame, width=600)
    blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    # construct a mask
    # a series of dilations and erosions to remove any small blobs left in the mask
    mask = cv2.inRange(hsv, colorLower, colorUpper)
    mask = cv2.erode(mask, None, iterations=blurring)
    mask = cv2.dilate(mask, None, iterations=blurring)
    # find contours in the mask and initialize the current
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
    cnts = imutils.grab_contours(cnts)
    center = None
    returnable=[0,0]
    # only proceed if at least one contour was found
    if len(cnts) > 0:
        for i in range(len(cnts)):
            c = cnts[i]
            ((x, y), radius) = cv2.minEnclosingCircle(c)
            verifiedPoints=0
            circ=PointsInCircum(radius*radMultiplier,100)
            for pair in circ:
                #cv2.circle(frame, (int(pair[0]), int(pair[1])), 0,(0, 255, 255), -1)
                if cv2.pointPolygonTest(c,(pair[0]+x , pair[1]+y),False)>=0:
                    verifiedPoints+=1
            percentIn=(verifiedPoints/float(len(circ)))*100

            #print(percentIn)
            #M = cv2.moments(c)
            #center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
            if percentIn >= thresh and radius>30:
                cv2.circle(frame, (int(x), int(y)), int(radius),(150, 75, 255), 2)
                returnable=[x,y]

    cv2.imshow("Frame", cv2.flip(frame,1))
    cv2.imshow("mask", cv2.flip(mask,1))
    cv2.waitKey(2)
    return returnable,frame.shape

def getData():
    global ser
    ser.reset_input_buffer()#clearing the input buffer to get most recent readings?
    time.sleep(0.03)#wait for the output of the serial to print
    #first line might be empty, so read second line
    by = ser.readline()
    #print(by)
    by = by.decode().split()#decoding and splitign the output
    for i in range(len(by)):
        try:
            by[i] = float(by[i])
        except:
            return
    return by

def getSelfPos(data):
    return 0

def turn(deg):
    mod=""
    if(deg<0):
        mod="-"
    if(abs(deg)<10):
        #ser.write(f'100{deg}'.encode())
        print(f'sent command {mod}100{(abs(deg))}')
        return
    if(abs(deg)<100):
        #ser.write(f'10{deg}'.encode())
        print(f'sent command {mod}10{(abs(deg))}')
        return
    #ser.write(f'1{deg}'.encode())
    print(f'sent command {mod}10{abs(deg)}')
def run():
    init=initialize()#initizlizing the serial interface
    while True:#start the playing loop
        if init:
            temp=getData()# data format: left distance, right distance, back distance, front(camera) distance, rotation(0-180? redundant degrees? need to find out how itll work)
            getSelfPos(temp)#figure out xy position of robot, only really possible once the court is built
        camInfo=scan()
        pos=camInfo[0]
        s=camInfo[1]
        if(pos[0]+pos[1]!=0):
            if(pos[0]<=s[1]/2):#left side of screen turn left and vice versa, find the the center point/ size of the frame
                turn(50)
            if(pos[0]>s[1]/2):
                turn(-50)
run()
