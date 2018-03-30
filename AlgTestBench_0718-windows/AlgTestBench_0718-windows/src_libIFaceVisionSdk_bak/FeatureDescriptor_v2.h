/*
Description: Features Extractor
Authors:     Yongqiang.Mou(mou.yongqiang@intellif.com), Dihong.Tian(tian.dihong@intellif.com)

Do not redistribute without permission
For any questions please contact Yongqiang.Mou or Dihong.Tian
Copyright(C) 2016, Shenzhen Intellifusion Technologies Co., Ltd., all rights reserved.
*/

#ifndef __H_H_FEATURE_DESCRIPTOR_H_H__
#define __H_H_FEATURE_DESCRIPTOR_H_H__

#include <opencv2/opencv.hpp>
#include <cstring>

#ifdef WIN32
#ifdef FEATUREDESCRIPTOR_V2_EXPORTS
#define FEATURE_DEACRIPTOR_V2_API _declspec(dllexport)
#else
#define FEATURE_DEACRIPTOR_V2_API _declspec(dllimport)
#endif
#else
#define FEATURE_DEACRIPTOR_V2_API 
#endif

using namespace std;

class FEATURE_DEACRIPTOR_V2_API FeatureDescriptor_v2
{
public:

	FeatureDescriptor_v2(std::string strPth, int model = -1, int dwThreads = 1);
	~FeatureDescriptor_v2();
	void Init();
	void Release();
	cv::Mat GenDescriptor(cv::Mat& mtImg);
	cv::Size GetSize();

	void global_blas_init(int dwThreads);

private:
	void* m_pData;
};

#endif