/*
Description: Landmark Locator
Authors:     Yongqiang.Mou(mou.yongqiang@intellif.com), Dihong.Tian(tian.dihong@intellif.com)

Do not redistribute without permission
For any questions please contact Yongqiang.Mou or Dihong.Tian
Copyright(C) 2016, Shenzhen Intellifusion Technologies Co., Ltd., all rights reserved.
*/

#ifndef __H_H_SDM_LOCATOR_H_H__
#define __H_H_SDM_LOCATOR_H_H__

#ifdef WIN32
#ifndef STATIC_LIB
#ifdef SDMLOCATOR_EXPORTS
#define SDM_LOCATOR_API __declspec(dllexport)
#else
#define SDM_LOCATOR_API __declspec(dllimport)
#endif
#else
#define SDM_LOCATOR_API
#endif
#else
#define SDM_LOCATOR_API
#endif

#include <fstream>
#include <iostream>
#include "PitorHOGDescriptor.h"

class SDM_LOCATOR_API SDMLocator
{
public:
	SDMLocator(std::string strPth);
	~SDMLocator();
	void Init();
	void Release();
	bool compute(cv::Mat& mtImg, cv::Rect rtBB, std::vector<cv::Point2f>& ptLMs);
private:
    //template class SDM_LOCATOR_API std::allocator;
	std::string m_strFileDescription;
	int m_nLmCount;
	int m_nFaceSize;
	int m_nCascadeSteps;

	std::vector<string> m_vecStrFeatType;
	std::vector<int> m_vecFeatLen;
	std::vector<PitorHOGParam> m_vecParam;

	cv::Mat m_mtMeanShape;
	vector<Mat> m_mtRegressor;
	vector<Mat> m_mtB;
	float* m_fAMBRet;

	static const float m_fShrinkge;	

	cv::Mat getInitShape(cv::Mat mtMeanShape, cv::Rect rtBoundtingBox);
};

#endif
