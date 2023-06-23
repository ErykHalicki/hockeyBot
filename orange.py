from collections import deque
from imutils.video import VideoStream
import numpy as np
import argparse

ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video", help="path to the (optional) video file")#cli arguments
ap.add_argument("-b", "--buffer", type=int, default=64,help="max buffer size")

args = vars(ap.parse_args())
if not args.get("video", False):
	vs = VideoStream(src=0).start()#take the camera if there is no video file specified
else:
	vs = cv2.VideoCapture(args["video"])
time.sleep(2.0)#wait for camera or video to load in

if not args.get("video", False):
    vs.stop()
else:
    vs.release()
cv2.destroyAllWindows()


#find the number of pixels inside the contour, then how many of those pixels overalap with the mask
