/*
Description: HOG Descriptor
Authors:     Yongqiang.Mou(mou.yongqiang@intellif.com), Dihong.Tian(tian.dihong@intellif.com)

Do not redistribute without permission
For any questions please contact Yongqiang.Mou or Dihong.Tian
Copyright(C) 2016, Shenzhen Intellifusion Technologies Co., Ltd., all rights reserved.
*/

#ifndef __H_H_PITOR_HOG_DESCRIPTOR_H_H__
#define __H_H_PITOR_HOG_DESCRIPTOR_H_H__

#include <opencv.hpp>

using namespace cv;

typedef struct tagPitorHOGParam
{
	int nBinSize;
	int nOrients;
	float fClip;
	bool bCrop;
	int nUseSoftBin;
	int nUseHog;
	int nNBP;
	tagPitorHOGParam()
	{
		nBinSize = 8;
		nOrients = 9;
		fClip = .2f;
		bCrop = false;
		nUseSoftBin = -1;
		nUseHog = 2;
		nNBP = 2;
	}
}PitorHOGParam;

void getGradMag(Mat& mtImg, Mat& mtM, Mat& mtO, PitorHOGParam param);
Mat getDescriptorPitorHOG(Mat& mtM, Mat& mtO, Mat& mtLMs, PitorHOGParam param);

#endif