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

//void init_guass_pixel_weight(float *pixel_weight_i, int pic_size);    //��ͼ���Ȩ�ؾ����ʼ��Ϊ��˹Ȩ�ؾ���
void   init_pixel_weight();                                             //��ʼ��ͼ���Ȩ�ؾ���
float  calculate_fuzzy_degree(Mat & pic_gray);                          //ʹ��ǰ�����ȵ���init_pixel_weight()���������ͼ���ģ����

int    face_area_detect(Mat  & imgMat, CvRect & face_area);
#endif