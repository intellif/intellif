/*
* @file utilsfunction.cpp
* @date 2016-03-08
* @brief some utilis function in tracking.
* @author ZhangZhaofeng(zhang.zhaofeng@intellif.com)
*
* Copyright (c) 2016, Shenzhen Intellifusion Technologies Co., Ltd.
*/

#include "utilsfunction.h"
float RectangleIntersect(cv::Rect *rect1, cv::Rect *rect2)
{
    if(rect1 == NULL || rect2 == NULL)
    {
        return 0;
    }
    int minX, minY, maxX, maxY, nArea;
    int nArea1, nArea2, nAreaMin;

    nArea1 = rect1->width * rect1->height;
    nArea2 = rect2->width * rect2->height;
    nAreaMin = nArea1 < nArea2 ? nArea1 : nArea2;

    minX = MAX(rect1->x, rect2->x);
    minY = MAX(rect1->y, rect2->y);
    maxX = MIN(rect1->br().x, rect2->br().x);
    maxY = MIN(rect1->br().y, rect2->br().y);

    if(!(minX > maxX || minY > maxY))
    {
        nArea = (maxX - minX)*(maxY - minY);
        return nArea / (float)nAreaMin;
    }

    return 0;
}

bool BoxPosFilter(cv::Rect* rect, int nWidth, int nHeight, int nMargin)
{
    if(rect == NULL)
    {
        return false;
    }

    if(rect->br().y + nMargin > nHeight)
    {
        return true;
    }

    if(rect->br().x + nMargin > nWidth)
    {
        return true;
    }

    if(rect->tl().x < nMargin)
    {
        return true;
    }

    if(rect->tl().y < nMargin)
    {
        return true;
    }

    return false;
}

bool BoxSizeFilter(cv::Rect* rect, int nDim)
{
    if(rect == NULL)
    {
        return false;
    }

    if(rect->width > nDim)
    {
        return true;
    }

    if(rect->height > nDim)
    {
        return true;
    }
    
    return false;
}

bool Rule1(int R, int G, int B) {
    int diff = 15;//orig: diff = 15;
    //bool e1 = (R>95) && (G>40) && (B>20) && ((max(R,max(G,B)) - min(R, min(G,B)))>diff) && (abs(R-G)>diff) && (R>G) && (R>B);
    bool e1 = (R>95) && (G>40) && (B>20) && ((std::max(R,std::max(G,B)) - std::min(R, std::min(G,B)))>diff) && (abs(R-G)>diff) && (R>G) && (R>B);
    //bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
    bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
    return (e1||e2);
}

int getSkin(cv::Mat const &src) {
    int cnt = 0;
    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {
            cv::Vec3b pix_bgr = src.ptr<cv::Vec3b>(i)[j];
             int B = pix_bgr.val[0];
             int G = pix_bgr.val[1];
             int R = pix_bgr.val[2];
             bool a = Rule1(R,G,B);
             if (a)
			 {
                 cnt++;
             }
        }
    }
    return cnt;
}

//人脸框转opencv rect
void ifrt2Rect(cv::Rect *tracker_rect, if_rect_t *detect_rect, int image_width, int image_height)
{
    int iamge_left = (detect_rect->left < 0) ? 0 : detect_rect->left;
    int image_right = (detect_rect->right > (image_width-1)) ? (image_width-1) : detect_rect->right;
    int image_top = (detect_rect->top < 0) ? 0 : detect_rect->top;
    int image_bottom = (detect_rect->bottom > (image_height - 1)) ? (image_height - 1) : detect_rect->bottom;

    tracker_rect->x = iamge_left;
    tracker_rect->y = image_top;
    tracker_rect->width = image_right - iamge_left + 1;
    tracker_rect->height = image_bottom - image_top + 1;
}

//人脸框转opencv rect
void ifrt2RectRef(cv::Rect & tracker_rect, if_rect_t & detect_rect, int image_width, int image_height)
{
    int iamge_left   = (detect_rect.left    < 0)
                     ?           0          : detect_rect.left;
    int image_right  = (detect_rect.right   > (image_width - 1))
                     ? (image_width - 1)    : detect_rect.right;
    int image_top    = (detect_rect.top     < 0)
                     ?           0          : detect_rect.top;
    int image_bottom = (detect_rect.bottom >  (image_height - 1))
                     ? (image_height - 1)   : detect_rect.bottom;

    tracker_rect.x      = iamge_left;
    tracker_rect.y      = image_top;
    tracker_rect.width  = image_right  - iamge_left + 1;
    tracker_rect.height = image_bottom - image_top  + 1;
}


void cycleRect2BoderRect(if_rect_t & detect_rect)
{
    int dwCenterX = detect_rect.left;
    int dwCenterY = detect_rect.top;

    int dwRadius  = detect_rect.right;

    detect_rect.left   = (dwCenterX > dwRadius) ? (dwCenterX - dwRadius) : 0;
    detect_rect.top    = (dwCenterY > dwRadius) ? (dwCenterY - dwRadius) : 0;
    detect_rect.right  = dwCenterX + dwRadius;
    detect_rect.bottom = dwCenterY + dwRadius;
}

//获取框的中心
void get_rect_center(cv::Rect *tracker_rect, CvPoint *center_point)
{
    center_point->x = tracker_rect->x + tracker_rect->width / 2;
    center_point->y = tracker_rect->y + tracker_rect->height / 2;
}

//获取两点的距离
float get_distance(CvPoint *point1, CvPoint *point2)
{
    float p_dis = sqrt(pow((point1->x - point2->x), 2) + pow((point1->y - point2->y), 2));
    return p_dis;
}

//放大框
void enlarge_rect(if_rect_t &face_rect_orig, cv::Rect &roi_enlarge, int image_width, int image_height)
{
    //将图像每边扩大原来的1/4
    int width_enlarge = (face_rect_orig.right - face_rect_orig.left) / 4;
    int height_enlarge = (face_rect_orig.bottom - face_rect_orig.top) / 4;

    int rect_left_enlarge = (face_rect_orig.left > width_enlarge)
        ? (face_rect_orig.left - width_enlarge)
        : 0;
    int rect_right_enlarge = (face_rect_orig.right < (image_width - width_enlarge))
        ? (face_rect_orig.right + width_enlarge)
        : image_width;

    int rect_top_enlarge = (face_rect_orig.top  > height_enlarge)
        ? (face_rect_orig.top - height_enlarge)
        : 0;
    int rect_bottom_enlarge = (face_rect_orig.bottom < (image_height - height_enlarge))
        ? (face_rect_orig.bottom + height_enlarge)
        : image_height;

    roi_enlarge = cvRect(rect_left_enlarge,
        rect_top_enlarge,
        rect_right_enlarge - rect_left_enlarge,
        rect_bottom_enlarge - rect_top_enlarge);
}

//计算跟踪物体的速度
float cal_v_std(int r, int v)
{
    int r_std = 640;
    int b0 = 0.05;
    int v_ratio = pow((r_std/r),0.5) + b0;
    int v_std = v_ratio*v;
    return v_std;
}

//std::string GBKToUTF8(const char* strGBK)
//{
//	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
//	wchar_t* wstr = new wchar_t[len + 1];
//	memset(wstr, 0, len + 1);
//	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
//	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
//	char* str = new char[len + 1];
//	memset(str, 0, len + 1);
//	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
//	std::string strTemp = str;
//	if (wstr) delete[] wstr;
//	if (str) delete[] str;
//	return strTemp;
//}
//
//std::string UTF8ToGBK(const char* strUTF8)
//{
//	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
//	wchar_t* wszGBK = new wchar_t[len + 1];
//	memset(wszGBK, 0, len * 2 + 2);
//	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
//	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
//	char* szGBK = new char[len + 1];
//	memset(szGBK, 0, len + 1);
//	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
//	std::string strTemp(szGBK);
//	if (wszGBK) delete[] wszGBK;
//	if (szGBK) delete[] szGBK;
//	return strTemp;
//}

void quickSort(int s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r, x = s[l];
		while (i < j)
		{
			while (i < j && s[j] >= x) // 从右向左找第一个小于x的数  
				j--;
			if (i < j)
				s[i++] = s[j];
			while (i < j && s[i] < x) // 从左向右找第一个大于等于x的数  
				i++;
			if (i < j)
				s[j--] = s[i];
		}
		s[i] = x;
		quickSort(s, l, i - 1); // 递归调用  
		quickSort(s, i + 1, r);
	}
}

