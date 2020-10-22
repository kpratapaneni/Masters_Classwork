import cv2
import numpy as np
import sys
import os
import glob
from os import listdir
import scipy.ndimage as scindi
from os.path import isfile, join
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

def Grayscale():
    image1 = cv2.imread('meanImg.jpg')
    img = cv2.cvtColor(image1, cv2.COLOR_BGR2GRAY)
    cv2.imshow('Grayscale Image', img)
    cv2.waitKey()
    return img

def AdaptiveThreshMean(img):
    thresh1 = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_MEAN_C, 
                                          cv2.THRESH_BINARY, 199, 5)
    cv2.imshow('Adaptive Threshold Mean', thresh1)
    cv2.waitKey()

def AdaptiveThreshGaussian(img):
    thresh2 = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                              cv2.THRESH_BINARY, 199, 5)
    cv2.imshow('Adaptive Threshold Gaussian', thresh2)
    cv2.waitKey()

def GaussianFilter():
    numpy_array=cv2.imread("meanImg.jpg",0)
    image_mask = numpy_array.astype(np.uint8)
    gaussian_image = scindi.gaussian_filter(image_mask, (10,10))
    cv2.imshow('Gaussian Filter Image', gaussian_image)
    cv2.waitKey()
    
def MeanForImg(directory, imageNum):
    images = []
    count = 0
    directory = os.path.join(directory,'*g')
    images = glob.glob(directory)
    meanImg =  np.zeros((500, 500,3), dtype=float)
    print("Mean gradient for %s images is in progress"% imageNum)
    for i in images :
        if count < imageNum:
            image = cv2.imread(i)
            img = cv2.resize(image,(500,500))
            numpy_arrayimage = np.array(img,dtype=float)   
            meanImg = meanImg + (numpy_arrayimage/imageNum)
            count = count + 1
        else:
            break
    meanImg = np.array(np.round( meanImg), dtype=np.uint8)  
    cv2.imshow('Average mean Image', meanImg)
    cv2.waitKey()
    return meanImg

def MaskImg(image):
    image = cv2.normalize( image, image, 0, 255, cv2.NORM_MINMAX)
    cv2.imshow('Normalized Image',image)
    kernel = np.ones((5, 5),np.uint8)
    opening = cv2.morphologyEx(image, cv2.MORPH_OPEN, kernel, iterations = 3)
    dilated = cv2.dilate( image, kernel, iterations = 2)
    mask = cv2.erode(dilated,kernel,iterations = 2)
    mask = cv2.normalize( mask, mask, 0, 255, cv2.NORM_MINMAX)
    _, mask = cv2.threshold( mask, 100, 255,cv2.THRESH_BINARY)
    return mask

dirctory = input("Enter the directory path ")
imageNum = int(input("Enter number of images you want to process "))
#Finding out mean of images
meanImg = MeanForImg(dirctory, imageNum)
cv2.normalize(meanImg,meanImg,0,255,cv2.NORM_MINMAX,dtype=cv2.CV_32F)
cv2.imwrite("meanImg.jpg", meanImg)
#Finding out grayscale image
img = Grayscale()
#Adaptive threshold mean and gausian 
AdaptiveThreshMean(img)
AdaptiveThreshGaussian(img)
#Gaussian Blur and smoothning
smooth = cv2.GaussianBlur(img,(3,3),0)
cv2.imshow('Smooth Image', smooth)
cv2.waitKey()
#Gaussian filter
GaussianFilter()
#Finally mask and smear for camera
mask = MaskImg(smooth)
cv2.imshow('Mask Image', mask)
cv2.waitKey()
   

