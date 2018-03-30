/*
* @file    fuzzy_degree.h
* @date    2015-11-14
* @brief   Head file of function calculate_fuzzy_degree
* @author  zhang.zhaofeng (zhang.zhaofeng@intellif.com)
*
* Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
*/
#ifndef FUZZY_DEGREE_H
#define FUZZY_DEGREE_H

#include <stdio.h>
//#include <io.h>
#include <iostream>
#include <math.h>
//#include <cv.h>
//#include <opencv2/core/core.hpp>  
//#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
using namespace cv;

//void init_guass_pixel_weight(float *pixel_weight_i, int pic_size);    //将图像的权重矩阵初始化为高斯权重矩阵
void   init_pixel_weight();                                             //初始化图像的权重矩阵
float  calculate_fuzzy_degree(Mat & pic_gray);                          //使用前必须先调用init_pixel_weight()函数，输出图像的模糊度

int    face_area_detect(Mat  & imgMat, CvRect & face_area);
#endif