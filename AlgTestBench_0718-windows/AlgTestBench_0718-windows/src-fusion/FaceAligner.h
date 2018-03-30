/*  
   Description: Facial Image Normalization
   Authors:     Yongqiang.Mou(mou.yongqiang@intellif.com), Dihong.Tian(tian.dihong@intellif.com)

   Do not redistribute without permission 
   For any questions please contact Yongqiang.Mou or Dihong.Tian
   Copyright(C) 2016, Shenzhen Intellifusion Technologies Co., Ltd., all rights reserved.
*/


#ifndef __H_H_FACE_ALIGNER_H_H__
#define __H_H_FACE_ALIGNER_H_H__

#include <opencv.hpp>

#ifdef WIN32
#ifndef STATIC_LIB
#ifdef FACEALIGNER_EXPORTS
#define FACE_ALIGNER_API _declspec(dllexport)
#else 
#define FACE_ALIGNER_API _declspec(dllimport)
#endif
#else
#define FACE_ALIGNER_API
#endif
#else
#define FACE_ALIGNER_API
#endif


class FACE_ALIGNER_API FaceAligner
{
public:
	FaceAligner(int i, int j=1, int k=1, int* pTSize=NULL, float* pTShift=NULL);
	~FaceAligner();
	void Release();
	void Init();
	cv::Mat Compute(const cv::Mat& mtSrcLM, const cv::Mat& mtSrcImg, cv::Mat& mtSrcLMTed, cv::Mat& mtT);
private:
	void* m_pData;
	int g_nTSize[2];
	float g_nTShift[4];
	cv::Mat g_mtMeanShape;
	cv::Mat g_mtFundamentalPts;
	int g_nAlgorithm;
};


#endif