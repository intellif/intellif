/*  
   Description: Facial Feature Representation
   Authors:     Yongqiang.Mou(mou.yongqiang@intellif.com), Dihong.Tian(tian.dihong@intellif.com)

   Do not redistribute without permission 
   For any questions please contact Yongqiang.Mou or Dihong.Tian
   Copyright(C) 2016, Shenzhen Intellifusion Technologies Co., Ltd., all rights reserved.
*/

#ifndef __H_H_FEATURES_CONTAINER_H_H__
#define __H_H_FEATURES_CONTAINER_H_H__

#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
//#include "FeatureDescriptor_v2.h"
//#include "SDMLocator.h"
//#include "FaceAligner.h"

#ifdef WIN32
#ifdef FEATURESCONTAINER_EXPORTS
#define FEATURES_CONTAINER_API _declspec(dllexport)
#else 
#define FEATURES_CONTAINER_API _declspec(dllimport)
#endif
#else
#define FEATURES_CONTAINER_API
#endif

using namespace std;
using namespace cv;

class FeatureDescriptor_v2;
class SDMLocator;
class FaceAligner;
class AlignerParam;

class FEATURES_CONTAINER_API Tuna17FeaturesContainer
{
public:
	typedef struct tagAlignerParam
	{
		int m_nLMCount;
		int m_nType;
		int m_nCanonicalCoord;
		int m_nAlg;
		int m_nTSize[2];
		float m_fBorder[4];
	}AlignerParam;

public:
        static int LibStaticInit(void);

	Tuna17FeaturesContainer(std::string strModelPth,int nMode=-1,int nT=1);
	~Tuna17FeaturesContainer();
	void Release();
	int Init();
	int ComputingDescriptor(cv::Mat& mtImg, cv::Rect rtBB, float*& fDescriptor);
	float MatchingDescriptor(float* fDescriptor1, float* fDescriptor2, int nFeatLen);
	int ComputingDescriptor(cv::Mat& mtImg, cv::Rect rtBB, cv::Mat& mtDescriptor);
	int ComputingDescriptor(std::vector<cv::Mat>& mtImg, std::vector<cv::Rect> rtBB, std::vector<cv::Mat>& mtDescriptor);
	int ComputingDescriptor(cv::Mat& mtImg, cv::Mat& mtDescriptor);
	float MatchingDescriptor(cv::Mat& mtDescriptor1, cv::Mat& mtDescriptor2, int nAction=1);

        #if 0
	cv::Rect ComputingDescriptor(cv::Mat& mtImg, cv::Mat& mtDescriptor);
        #endif
        
private:
	std::vector<std::vector<FeatureDescriptor_v2*>> m_pFeatDesc;
	SDMLocator* m_pSDMLocator;
	std::vector<std::vector<FaceAligner*>> m_pFaceAligner;

	std::vector<std::vector<AlignerParam>> m_stAlignerParam;

	std::string m_strModelPth;
	static const std::string m_strSDMModelFile;

	std::vector<std::vector<std::string>> m_strAlignerFile;
	std::vector<std::vector<std::string>> m_strFeatFile;
	cv::Mat m_mtProj;
	cv::Mat m_mtProjMean;
	int m_nMode;
	int m_dwThreads;

};

#endif
