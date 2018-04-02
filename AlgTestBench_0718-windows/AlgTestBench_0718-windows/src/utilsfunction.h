/*
* @file utilsfunction.h
* @date 2016-03-08
* @brief some utilis function in tracking.
* @author ZhangZhaofeng(zhang.zhaofeng@intellif.com)
*
* Copyright (c) 2016, Shenzhen Intellifusion Technologies Co., Ltd.
*/

#ifndef __H_H_UTILS_FUNCTION_H_H__

#define __H_H_UTILS_FUNCTION_H_H__

#include <opencv2/opencv.hpp>

#ifdef _MSC_VER
#include "IFaceSDK.h"
#else
#include "ifacesdk.h"
#endif

float RectangleIntersect(cv::Rect *rect1, cv::Rect *rect2);
bool  BoxPosFilter(cv::Rect* rect, int nWidth, int nHeight, int nMargin);
bool  BoxSizeFilter(cv::Rect* rect, int nDim);

bool  Rule1(int R, int G, int B);
int   getSkin(cv::Mat const &src);

//������תopencv rect
void ifrt2Rect(cv::Rect *tracker_rect, if_rect_t *detect_rect, int image_width, int image_height);

void ifrt2RectRef(cv::Rect & tracker_rect, if_rect_t & detect_rect, int image_width, int image_height);

void cycleRect2BoderRect(if_rect_t & detect_rect);

//��ȡ�������
void get_rect_center(cv::Rect *tracker_rect, CvPoint *center_point);

//��ȡ����ľ���
float get_distance(CvPoint *point1, CvPoint *point2);

//�Ŵ��
void enlarge_rect(if_rect_t &face_rect_orig, cv::Rect &roi_enlarge, int image_width, int image_height);

//�������������ٶ�
float cal_v_std(int r, int v);

#endif