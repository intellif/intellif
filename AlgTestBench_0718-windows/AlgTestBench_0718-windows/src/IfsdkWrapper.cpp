/*
 * @file    IfsdkWrapper.cpp
 * @date    2016-07-07
 * @brief   the wrapper impelement of intellifusion facesdk
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#include "TestCaseCommon.h"

#include "IfsdkWrapper.h"

#include "utilsfunction.h"

using namespace cv;

extern std::string g_strAlgModelPath;
extern int         g_mdwAlgFeatureLen;
extern int         g_mdwDetectMode;

extern int         g_mdwDetectHfProfile;
extern int         g_mdwDetectProfile;
extern int         g_mdwDetectRot;

extern int         g_mdwAlgVersion;
extern int         g_flipExtratFeature;

extern int         g_mdwIfCompFuncSel;

extern vector<int>    g_vecIntDetect;

boost::shared_mutex   EagleMtcnnDetectManager::mMtcnnMapLocker;
TMtcnnDetectorMap     EagleMtcnnDetectManager::mMtcnnDetectMap;
int                   EagleMtcnnDetectManager::mMtcnnHandleSeq = 0x12121212;

boost::shared_mutex   FaceRecSDKManager::mFaceRecMapLocker;
TFaceRecMap        FaceRecSDKManager::mFaceRecMap;
int                   FaceRecSDKManager::mFaceRecHandleSeq = 0x3c3c3c3c;

boost::shared_mutex   IfMutilAlgExtManger::mMapLocker;
TMutilAlgMap          IfMutilAlgExtManger::mIfAlgMap;
int                   IfMutilAlgExtManger::mHandleAlgSeq     = 0x76767676;

boost::shared_mutex   IfMutilAlgCompareManger::mMapLocker;
TMutilAlgCmpMap       IfMutilAlgCompareManger::mIfCmpMap;
int                   IfMutilAlgCompareManger::mHandleCmpSeq = 0x45454545;

boost::shared_mutex   CascadeDetectManager::mMapLocker;
TCascadeDetectorMap   CascadeDetectManager::mIfDetectorMap;
int                   CascadeDetectManager::mHandleDetectSeq = 0x67676767;

void  FaceRecSDKManager::Init(string strLicensePath)
{
	IFaceRecSDK::Init(strLicensePath);
}

if_handle_t FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType handleType, string sModelPath)
{
	if_handle_t hRetHandle = (if_handle_t)0;

	boost::unique_lock<boost::shared_mutex> writeLocker(mFaceRecMapLocker);

	
	mFaceRecHandleSeq = mFaceRecHandleSeq + 0xa;
	hRetHandle = (if_handle_t)mFaceRecHandleSeq;

	IFaceRecHandle newFaceRecHandle = 0;
	int result = IFaceRecSDK::Create(handleType, sModelPath, newFaceRecHandle);

	std::cout << "##IFaceRecSDK::Create result:::" << result << std::endl;

	mFaceRecMap.insert(TFaceRecMap::value_type(hRetHandle, newFaceRecHandle));

	return hRetHandle;
}


void FaceRecSDKManager::DestoryIfHandle(if_handle_t  hHandle)
{
	boost::unique_lock<boost::shared_mutex> readLocker(mFaceRecMapLocker);

	TFaceRecMap::iterator findHandle = mFaceRecMap.find(hHandle);
	if (findHandle == mFaceRecMap.end())
	{
		std::cout << "FaceRecSDKManager::DestoryIfHandle, unexist handle: " << hHandle << std::endl;
	}
	else
	{
		std::cout << "FaceRecSDKManager::DestoryIfHandle, ##delete handle: " << hHandle << std::endl;
		IFaceRecSDK::Destroy(findHandle->second);
		mFaceRecMap.erase(findHandle);
	}
}

int FaceRecSDKManager::CompareImageFeature(E_IFaceRecType      handleType,
	const float     *pFeature0,
	int              dwLen0,
	const float     *pFeature1,
	int              dwLen1,
	float           *score)
{
	float fCmpScore = (float)0.0;
	if (dwLen0 != dwLen1)
	{
		*score = (float)0.0;
		return 0;
	}

	IFaceRecSDK::FeatureCompare(handleType, pFeature0, dwLen0, pFeature1, dwLen0, fCmpScore);

	*score = fCmpScore;

	return FACE_SNAPER_ERR_NONE;
}

if_result_t FaceRecSDKManager::FeedOneFramToDetector(if_handle_t       hHandle,
	const cv::Mat &       matRgbDat,
	PFACE_RST_TYPE  * pFaceArray,
	unsigned int    * pdwFaceCnt)
{
	boost::shared_lock<boost::shared_mutex> readLocker(mFaceRecMapLocker);

	if (matRgbDat.data == NULL || pFaceArray == NULL || pdwFaceCnt == NULL)
	{
		std::cout << "FeedOneFramToDetector, wrong parameter: " << hHandle << std::endl;
		return FACE_SNAPER_ERR_NONE;
	}


	TFaceRecMap::iterator findHandle = mFaceRecMap.find(hHandle);

	if (findHandle == mFaceRecMap.end())
	{
		std::cout << "FeedOneFramToDetector, unexist handle: " << hHandle << std::endl;
		pFaceArray[0] = NULL;
		pdwFaceCnt[0] = 0;
		return FACE_SNAPER_ERR_NONE;
	}

	IFaceRecHandle hDetectorHandle = findHandle->second;

	printf("###start FaceRecSDKManager::FeedOneFramToDetector:::handle:%d\n", hDetectorHandle);

	int dResult = 0;
	int dFaceCnt = 0;
	T_IFaceRecRect * pFaceRectArray = NULL;
	try{
		

		dResult = IFaceRecSDK::FaceDetect(hDetectorHandle, matRgbDat.data, matRgbDat.cols, matRgbDat.rows, dFaceCnt, &pFaceRectArray);

		std::cout << "FaceRecSDKManager::FeedOneFramToDetector,IFaceRecSDK::FaceDetect:" << dResult << ",face count:" << dFaceCnt  << endl;

		if (dResult != 0)
		{
			std::cout << "FeedOneFramToDetector, FaceDetect Error: " << dResult << std::endl;
			return FACE_SNAPER_ERR_NONE;
		}

		if (dFaceCnt > MAX_FACE_CNT) dFaceCnt = MAX_FACE_CNT;
	}
	catch (cv::Exception cvExp)
	{
		std::cout << "Opencv Exception:" << cvExp.what() << std::endl;
		std::cout << "Mat cols: " << matRgbDat.cols << ", rows:" << matRgbDat.rows << std::endl;
		pFaceArray[0] = NULL;
		pdwFaceCnt[0] = 0;
		return FACE_SNAPER_ERR_NONE;
	}
	catch (...)
	{

		std::cout << "Mat cols: " << matRgbDat.cols << ", rows:" << matRgbDat.rows << std::endl;
		pFaceArray[0] = NULL;
		pdwFaceCnt[0] = 0;
		return FACE_SNAPER_ERR_NONE;
	}

	int dwFaceDetectCnt = dFaceCnt;

	if (dwFaceDetectCnt == 0)
	{
		pFaceArray[0] = NULL;
		pdwFaceCnt[0] = 0;
		return FACE_SNAPER_ERR_NONE;
	}

	int            dwMallocLen = (int)(sizeof(FACE_RST_TYPE)* dwFaceDetectCnt);
	PFACE_RST_TYPE pFaceRect = (PFACE_RST_TYPE)malloc(dwMallocLen);

	if (pFaceRect == NULL)
	{
		std::cout << "FeedOneFramToDetector, alloc memory failed: " << hHandle << std::endl;
		pFaceArray[0] = NULL;
		pdwFaceCnt[0] = 0;
		return FACE_SNAPER_ERR_NONE;
	}

	for (std::size_t dwFaceIdx = 0; dwFaceIdx < dwFaceDetectCnt; dwFaceIdx++)
	{
		pFaceRect[dwFaceIdx].Rect.left = pFaceRectArray[dwFaceIdx].left;
		pFaceRect[dwFaceIdx].Rect.top = pFaceRectArray[dwFaceIdx].top;
		pFaceRect[dwFaceIdx].Rect.right = pFaceRectArray[dwFaceIdx].right;
		pFaceRect[dwFaceIdx].Rect.bottom = pFaceRectArray[dwFaceIdx].bottom;

		pFaceRect[dwFaceIdx].Pose = (IF_FacePose)0;
		pFaceRect[dwFaceIdx].Confidence = (float)(0);
	}

	std::cout << "FaceRecSDKManager::FeedOneFramToDetector,pFaceRectArray[0]:" << pFaceRectArray[0].left << "," << pFaceRectArray[0].top << "," << pFaceRectArray[0].right << "," << pFaceRectArray[0].bottom << endl;

	IFaceRecSDK::ReleaseDetectResult(pFaceRectArray);

	pFaceArray[0] = pFaceRect;
	pdwFaceCnt[0] = (unsigned int)dwFaceDetectCnt;

	return FACE_SNAPER_ERR_NONE;
}

void FaceRecSDKManager::ReleaseDetFaceResult(PIF_FACERECT pFaceRectArray, unsigned int dwFaceCnt)
{
	if (pFaceRectArray == NULL)
	{
		std::cout << "Try to release null result" << std::endl;
	}
	else
	{
		std::cout << "FaceRecSDKManager::ReleaseDetFaceResult::dwFaceCnt:" << dwFaceCnt << endl;

		std::cout << "FaceRecSDKManager::ReleaseDetFaceResult::pFaceRectArray:" << pFaceRectArray[0].Rect.left << pFaceRectArray[0].Rect.top << endl;

		//int            dwMallocLen = (int)(sizeof(T_IFaceRecRect)* dwFaceCnt);

		//std::cout << "dwMallocLen:" << dwMallocLen << endl;

		//T_IFaceRecRect * pFaceRect = (T_IFaceRecRect *)malloc(dwMallocLen);

		//if (pFaceRect == NULL)
		//{
		//	std::cout << "ReleaseDetFaceResult, alloc memory failed: " << std::endl;
		//	free(pFaceRectArray);
		//	pFaceRectArray = NULL;
		//	return;
		//}

		//for (std::size_t dwFaceIdx = 0; dwFaceIdx < dwFaceCnt; dwFaceIdx++)
		//{
		//	pFaceRect[dwFaceIdx].left = pFaceRectArray[dwFaceIdx].Rect.left;
		//	pFaceRect[dwFaceIdx].top = pFaceRectArray[dwFaceIdx].Rect.top;
		//	pFaceRect[dwFaceIdx].right = pFaceRectArray[dwFaceIdx].Rect.right;
		//	pFaceRect[dwFaceIdx].bottom = pFaceRectArray[dwFaceIdx].Rect.bottom;
		//	std::cout << "FaceRecSDKManager::ReleaseDetFaceResult::" << pFaceRect[dwFaceIdx].left << endl;
		//}

		//std::cout << "FaceRecSDKManager::ReleaseDetFaceResult::" << pFaceRect[0].left << pFaceRect[0].top << pFaceRect[0].right << pFaceRect[0].bottom << endl;

		//IFaceRecSDK::ReleaseDetectResult(pFaceRect);

		free(pFaceRectArray);
		pFaceRectArray = NULL;

		std::cout << "FaceRecSDKManager::ReleaseDetFaceResult::end!!" << endl;
	}
}

void  FaceRecSDKManager::ReleaseFeautreResult(float        *  pFeature)
{
	if (pFeature == NULL)
	{
		printf("Try to release null result");
	}
	else
	{
		IFaceRecSDK::ReleaseExtractResult(pFeature);
	}
}

int FaceRecSDKManager::ExtratFeatureFromImage(if_handle_t  hHandle,
	cv::Mat      &   rMatRgb,
	T_IFaceRecRect    &   pFaceRect,
	float        **  pFeature)
{
	boost::shared_lock<boost::shared_mutex> readLocker(mFaceRecMapLocker);

	int dwFeatureLen = 0;

	if (rMatRgb.data == NULL || pFeature == NULL)
	{
		std::cout << "FaceRecSDKManager::ExtratFeatureFromImage, wrong parameter: " << hHandle << std::endl;
		return dwFeatureLen;
	}

	TFaceRecMap::iterator findExtrator = mFaceRecMap.find(hHandle);

	if (findExtrator == mFaceRecMap.end())
	{
		std::cout << "FaceRecSDKManager::ExtratFeatureFromImage, unexist handle: " << hHandle << std::endl;
		pFeature[0] = NULL;
		return dwFeatureLen;
	}

	IFaceRecHandle hExtractHandle = findExtrator->second;

	float *pFaceFeature;
	int dResult = 0;

	dResult = IFaceRecSDK::FeatureExtract(hExtractHandle, rMatRgb.data, rMatRgb.cols, rMatRgb.rows, pFaceRect, dwFeatureLen, pFaceFeature);

	if (dResult != 0)
	{
		std::cout << "FaceRecSDKManager::ExtratFeatureFromImage, ExtractTest Error: " << dResult << std::endl;
		pFeature[0] = NULL;
		return dwFeatureLen;
	}

	if (dwFeatureLen == 0 || pFaceFeature == NULL)
	{
		std::cout << "FaceRecSDKManager::ExtratFeatureFromImage, failed to extrace feature: " << hHandle << std::endl;
		pFeature[0] = NULL;
		dwFeatureLen = 0;
		return dwFeatureLen;
	}

	pFeature[0] = pFaceFeature;

	return dwFeatureLen;
}

int FaceRecSDKManager::ExtratGluonFeature(if_handle_t  hHandle,
	cv::Mat      &   rMatRgb,
	T_IFaceRecRect    &   pFaceRect,
	float        **  pFeature)
{
	if (rMatRgb.data == NULL || pFeature == NULL)
	{
		printf("FaceRecSDKManager::ExtratGluonFeature, wrong parameter: ");
		return FACE_SNAPER_ERR_NONE;
	}

	float * pFeatureRet = NULL;
	int     dwFeatureLen = 0;

	//正常提取一次
	float * pNomalFeature = NULL;
	int     pNomalFeatureLen = 0;
	pNomalFeatureLen = FaceRecSDKManager::ExtratFeatureFromImage(hHandle, rMatRgb, pFaceRect, &pNomalFeature);
	//printf("FaceRecSDKManager::ExtratGluonFeature, pNomalFeatureLen=%d\n", pNomalFeatureLen);
	if ((pNomalFeatureLen != g_mdwAlgFeatureLen) || (pNomalFeature == NULL))
	{
		printf("FaceRecSDKManager::ExtratGluonFeature, Failed, FeatureLen: %d, RetLen: %d\n", g_mdwAlgFeatureLen, pNomalFeatureLen);

		if (pNomalFeature != NULL)
		{
			FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
			pNomalFeature = NULL;
		}
		pFeatureRet = pNomalFeature;
		dwFeatureLen = 0;
	}

	// Gluon特征值flip归一化处理 20171115 pengjidong
	if (g_flipExtratFeature)
	{
		//printf("FaceRecSDKManager::ExtratGluonFeature, g_flipExtratFeature=1");
		//flip 提取一次
		float * pFlipFeature = NULL;
		int     pFlipFeatureLen = 0;
		cv::Mat matFlip;
		cv::flip(rMatRgb, matFlip, 1);

		T_IFaceRecRect  pFaceRectFlip(pFaceRect);
		pFaceRectFlip.left = (rMatRgb.cols - pFaceRect.right) > 0 ? (rMatRgb.cols - pFaceRect.right) : 0;
		pFaceRectFlip.right = (rMatRgb.cols - pFaceRect.left) > 0 ? (rMatRgb.cols - pFaceRect.left) : 0;
		pFlipFeatureLen = FaceRecSDKManager::ExtratFeatureFromImage(hHandle, matFlip, pFaceRectFlip, &pFlipFeature);

		//printf("FaceRecSDKManager::ExtratGluonFeature, pFlipFeatureLen=%d\n", pFlipFeatureLen);
		if ((pFlipFeatureLen != g_mdwAlgFeatureLen) || (pFlipFeature == NULL))
		{
			printf("FaceRecSDKManager::ExtratGluonFeature, pFlipFeatureLen != g_mdwAlgFeatureLen =%d || pFlipFeature == NULL%d\n", g_mdwAlgFeatureLen);
			if (pFlipFeature != NULL)
			{
				FaceRecSDKManager::ReleaseFeautreResult(pFlipFeature);
				pFlipFeature = NULL;
			}
			pFeatureRet = pFlipFeature;
			dwFeatureLen = 0;
		}

		//两种特征值合成
		float * pRetFeature = (float*)malloc(sizeof(float)*pNomalFeatureLen);

		if (pRetFeature == NULL)
		{
			printf("FaceRecSDKManager::ExtratGluonFeature, malloc(sizeof(float)*pNomalFeatureLen) failed!!\n");
			FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
			FaceRecSDKManager::ReleaseFeautreResult(pFlipFeature);
			pFeatureRet = pNomalFeature;
			dwFeatureLen = 0;
		}

		for (int dwIndx = 0; dwIndx < pNomalFeatureLen; dwIndx++)
		{
			pRetFeature[dwIndx] = pNomalFeature[dwIndx] + pFlipFeature[dwIndx];
		}

		//归一化
		float dfNormaParam = 0.0;
		for (int dwFeatureIdx = 0; dwFeatureIdx < pNomalFeatureLen; dwFeatureIdx++)
		{
			dfNormaParam += pRetFeature[dwFeatureIdx] * pRetFeature[dwFeatureIdx];
		}
		dfNormaParam = sqrt(dfNormaParam);
		for (int dwFeatureIdx = 0; dwFeatureIdx < pNomalFeatureLen; dwFeatureIdx++)
		{
			pRetFeature[dwFeatureIdx] = pRetFeature[dwFeatureIdx] / dfNormaParam;
		}

		pFeatureRet = pRetFeature;
		dwFeatureLen = pNomalFeatureLen;

		if (pFeatureRet == NULL)
		{
			printf("FaceRecSDKManager::ExtratGluonFeature, pFeatureRet = null!!\n");
		}
		if (pRetFeature == NULL)
		{
			printf("FaceRecSDKManager::ExtratGluonFeature, pRetFeature == null!!\n");
		}

		FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
		FaceRecSDKManager::ReleaseFeautreResult(pFlipFeature);
	}
	else
	{
		pFeatureRet = pNomalFeature;
		dwFeatureLen = pNomalFeatureLen;
		FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
	}

	*pFeature = pFeatureRet;
	return dwFeatureLen;
}

void  EagleMtcnnDetectManager::StaticInit(void)
{
	MtcnnMxnetHandle::MtcnnStaticInit();
}

if_handle_t EagleMtcnnDetectManager::CreateIfDetectorHandle(int   dwThreadCnt)
{
	if_handle_t hRetHandle = (if_handle_t)0;

	boost::unique_lock<boost::shared_mutex> writeLocker(mMtcnnMapLocker);

	mMtcnnHandleSeq = mMtcnnHandleSeq + 0xa;

	hRetHandle = (if_handle_t)mMtcnnHandleSeq;

	std::vector<float> vecFloatMtcnnThr;

	vecFloatMtcnnThr.push_back((float)0.75);
	vecFloatMtcnnThr.push_back((float)0.7);
	vecFloatMtcnnThr.push_back((float)0.97);

	string strMtcnnModelPath = "model/mtcnn";

#ifndef _MSC_VER
	SP_MtcnnMxnetHandle newMtcnnHandle(new MtcnnMxnetHandle(strMtcnnModelPath,
		32,
		vecFloatMtcnnThr,
		(float)0.709,
		false,
		-1,
		true,
		(float)0.083,
		(float)0.0));

	mMtcnnDetectMap.insert(TMtcnnDetectorMap::value_type(hRetHandle, newMtcnnHandle));
#endif

	return hRetHandle;
}


void EagleMtcnnDetectManager::DestoryIfDetectoHandle(if_handle_t  hHandle)
{
#ifndef _MSC_VER
	boost::unique_lock<boost::shared_mutex> writeLocker(mMtcnnMapLocker);

	TMtcnnDetectorMap::iterator findDetector = mMtcnnDetectMap.find(hHandle);
	if (findDetector == mMtcnnDetectMap.end())
	{
		std::cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle, unexist handle: " << hHandle << std::endl;
	}
	else
	{
		std::cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle, ##delete handle: " << hHandle << std::endl;
		mMtcnnDetectMap.erase(findDetector);
	}
#endif
}


if_result_t EagleMtcnnDetectManager::FeedOneFramToDetector(if_handle_t       hHandle,
	const cv::Mat &       matRgbDat,
	const int         dwWidth,
	const int         dwHeight,
	PFACE_RST_TYPE  * pFaceRectArray,
	unsigned int    * pdwFaceCnt)
{
	boost::shared_lock<boost::shared_mutex> readLocker(mMtcnnMapLocker);

	if (matRgbDat.data == NULL || pFaceRectArray == NULL || pdwFaceCnt == NULL)
	{
		std::cout << "FeedOneFramToDetector, wrong parameter: " << hHandle << std::endl;

		return FACE_SNAPER_ERR_NONE;
	}

#ifndef _MSC_VER
	TMtcnnDetectorMap::iterator findDetector = mMtcnnDetectMap.find(hHandle);

	if (findDetector == mMtcnnDetectMap.end())
	{
		std::cout << "FeedOneFramToDetector, unexist handle: " << hHandle << std::endl;

		pFaceRectArray[0] = NULL;
		pdwFaceCnt[0] = 0;

		return FACE_SNAPER_ERR_NONE;
	}

	Vbbox vecFaceBoxes;

	SP_MtcnnMxnetHandle & spMtcnnHandle = findDetector->second;

	//std::cout << "Mtcnn Detect Befin......";

	try{
		spMtcnnHandle->MtcnnFaceDetect(matRgbDat,
			vecFaceBoxes);
	}
	catch (cv::Exception cvExp)
	{
		std::cout << "Opencv Exception:" << cvExp.what() << std::endl;
		std::cout << "Mat cols: " << matRgbDat.cols << ", rows:" << matRgbDat.rows << std::endl;

		pFaceRectArray[0] = NULL;
		pdwFaceCnt[0] = 0;

		return FACE_SNAPER_ERR_NONE;
	}
	catch (...)
	{

		std::cout << "Mat cols: " << matRgbDat.cols << ", rows:" << matRgbDat.rows << std::endl;

		pFaceRectArray[0] = NULL;
		pdwFaceCnt[0] = 0;

		return FACE_SNAPER_ERR_NONE;
	}



	//std::cout << "Mtcnn Detect Endup......";

	std::size_t dwFaceDetectCnt = vecFaceBoxes.size();

	if (dwFaceDetectCnt == 0)
	{
		pFaceRectArray[0] = NULL;
		pdwFaceCnt[0] = 0;

		return FACE_SNAPER_ERR_NONE;
	}

	int            dwMallocLen = (int)(sizeof(FACE_RST_TYPE)* dwFaceDetectCnt);
	PFACE_RST_TYPE pFaceRect = (PFACE_RST_TYPE)malloc(dwMallocLen);

	if (pFaceRect == NULL)
	{
		std::cout << "FeedOneFramToDetector, alloc memory failed: " << hHandle << std::endl;

		pFaceRectArray[0] = NULL;
		pdwFaceCnt[0] = 0;

		return FACE_SNAPER_ERR_NONE;
	}

#if 0

	for (std::size_t dwFaceIdx = 0; dwFaceIdx < dwFaceDetectCnt; dwFaceIdx++)
	{
		int dwFaceWidth = (vecFaceBoxes[dwFaceIdx].x2 - vecFaceBoxes[dwFaceIdx].x1);
		int dwFaceHeight = (vecFaceBoxes[dwFaceIdx].y2 - vecFaceBoxes[dwFaceIdx].y1);

		int dwMaxBorder = dwFaceWidth > dwFaceHeight ? dwFaceWidth : dwFaceHeight;
		int dwMaxRaidiu = dwMaxBorder >> 1;

		int dwFaceCentX = (vecFaceBoxes[dwFaceIdx].x2 + vecFaceBoxes[dwFaceIdx].x1)
			>> 1;

		int dwFaceCentY = (vecFaceBoxes[dwFaceIdx].y2 + vecFaceBoxes[dwFaceIdx].y1)
			>> 1;

		pFaceRect[dwFaceIdx].Rect.left = dwFaceCentX - dwMaxRaidiu;
		pFaceRect[dwFaceIdx].Rect.top = dwFaceCentY - dwMaxRaidiu;
		pFaceRect[dwFaceIdx].Rect.right = dwFaceCentX + dwMaxRaidiu;
		pFaceRect[dwFaceIdx].Rect.bottom = dwFaceCentY + dwMaxRaidiu;

		pFaceRect[dwFaceIdx].Pose = (IF_FacePose)0;
		pFaceRect[dwFaceIdx].Confidence = (float)(vecFaceBoxes[dwFaceIdx].score);
	}

#else

	for (std::size_t dwFaceIdx = 0; dwFaceIdx < dwFaceDetectCnt; dwFaceIdx++)
	{
		pFaceRect[dwFaceIdx].Rect.left = vecFaceBoxes[dwFaceIdx].x1;
		pFaceRect[dwFaceIdx].Rect.top = vecFaceBoxes[dwFaceIdx].y1;
		pFaceRect[dwFaceIdx].Rect.right = vecFaceBoxes[dwFaceIdx].x2;
		pFaceRect[dwFaceIdx].Rect.bottom = vecFaceBoxes[dwFaceIdx].y2;

		pFaceRect[dwFaceIdx].Pose = (IF_FacePose)0;
		pFaceRect[dwFaceIdx].Confidence = (float)(vecFaceBoxes[dwFaceIdx].score);
	}
#endif

	pFaceRectArray[0] = pFaceRect;
	pdwFaceCnt[0] = (unsigned int)dwFaceDetectCnt;

#endif

	return FACE_SNAPER_ERR_NONE;
}


void EagleMtcnnDetectManager::ReleaseDetFaceResult(PIF_FACERECT             pFaceRectArray,
	unsigned int             pdwFaceCnt)
{
	if (pFaceRectArray == NULL)
	{
		std::cout << "Try to release null result" << std::endl;
	}
	else
	{
		//cout << "EagleMtcnnDetectManager::ReleaseDetFaceResult start-end###" << std::endl;
		free(pFaceRectArray);

		pFaceRectArray = NULL;
	}
}


CascadeDetector::CascadeDetector(if_handle_t dwHandle)
{
    mSelfHandle   = dwHandle;

    mdwCascadeNum = 0;

    for(int dwCascadeIdx = 0; dwCascadeIdx < MAX_CASCADE_LEN; dwCascadeIdx++)
    {
        mahCascadeHandle[dwCascadeIdx] = 0;
        madwCascadeType[dwCascadeIdx]  = FACE_CASCADE_DET_TYPE_INVALID;
    }

	for (int i = 0; i < g_vecIntDetect.size(); i ++)
	{
		if (g_vecIntDetect[i] == E_IFaceRecType::IFACEREC_DETECTOR_EAGLE)
		{
			/*if_handle_t hNewVjCreated = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_DETECTOR_EAGLE, "model/eagle");
			mahCascadeHandle[mdwCascadeNum] = hNewVjCreated;
			madwCascadeType[mdwCascadeNum] = E_IFaceRecType::IFACEREC_DETECTOR_EAGLE;
			mdwCascadeNum += 0x1;*/
			if_handle_t hNewVjCreated = EagleMtcnnDetectManager::CreateIfDetectorHandle(0);
			mahCascadeHandle[mdwCascadeNum] = hNewVjCreated;
			madwCascadeType[mdwCascadeNum] = E_IFaceRecType::IFACEREC_DETECTOR_EAGLE;
			mdwCascadeNum += 0x1;
		}
		else if (g_vecIntDetect[i] == E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST)
		{
			if_handle_t hNewVjCreated = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST, "model/vjboost");
			mahCascadeHandle[mdwCascadeNum] = hNewVjCreated;
			madwCascadeType[mdwCascadeNum] = E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST;
			mdwCascadeNum += 0x1;
		}
		else if (g_vecIntDetect[i] == E_IFaceRecType::IFACEREC_DETECTOR_NICKLE)
		{
			if_handle_t hNewVjCreated = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_DETECTOR_NICKLE, "model/nickle");
			mahCascadeHandle[mdwCascadeNum] = hNewVjCreated;
			madwCascadeType[mdwCascadeNum] = E_IFaceRecType::IFACEREC_DETECTOR_NICKLE;
			mdwCascadeNum += 0x1;
		}
	}

    return;
}

CascadeDetector::~CascadeDetector()
{
    for(int dwDetectorIdx = 0; dwDetectorIdx < mdwCascadeNum; dwDetectorIdx++)
    {
        if(mahCascadeHandle[dwDetectorIdx] == 0)
        {
            std::cout << "Cascade, Try to release unexist detector: " << mSelfHandle;
            continue;
        }
	
        if(madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_INVALID)
        {
            std::cout << "Cascade, release wrong type detector: "     << mSelfHandle
                      << ", type: " << madwCascadeType[dwDetectorIdx] << std::endl;
        }
		else
		{
			//FaceRecSDKManager::DestoryIfHandle(mahCascadeHandle[dwDetectorIdx]);
			EagleMtcnnDetectManager::DestoryIfDetectoHandle(mahCascadeHandle[dwDetectorIdx]);
		}
    }
}

if_result_t CascadeDetector::CascadeFaceDetect(const Mat &       matLoad,
                                               const int         dwWidth,
                                               const int         dwHeight,
                                                     int               pdwRectType,
                                                     PFACE_RST_TYPE  * pFaceRectArray,
                                                     unsigned int    * pdwFaceCnt)
{
    if_result_t dwRet = FACE_SNAPER_ERR_NONE;
    
	dwRet = CascadeFaceDetectAct(matLoad,
                                 dwWidth,
                                 dwHeight,
                                 FACE_CASCADE_DETROUND_ORIG,
                                 pdwRectType,
                                 pFaceRectArray,
                                 pdwFaceCnt);

    if( (*pFaceRectArray != NULL)
      &&(*pdwFaceCnt     != 0))
    {
        return dwRet;
    }

    return FACE_SNAPER_ERR_NONE;
}

if_result_t CascadeDetector::CascadeFaceDetectAct(const Mat &       matLoad,
                                                  const int         dwWidth,
                                                  const int         dwHeight,
                                                        int         dwDetRound,
                                                        int               pdwRectType,
                                                        PFACE_RST_TYPE  * pFaceRectArray,
                                                        unsigned int    * pdwFaceCnt)
{
    int dwRet           = FACE_SNAPER_ERR_NONE;

	cv::Mat matGray;

    for(int dwDetectorIdx = 0; dwDetectorIdx < mdwCascadeNum; dwDetectorIdx++)
    {
        if(  (mahCascadeHandle[dwDetectorIdx] == 0)
           ||(madwCascadeType[dwDetectorIdx]  != pdwRectType))
        {
            continue;
        }

		if (madwCascadeType[dwDetectorIdx] == E_IFaceRecType::IFACEREC_DETECTOR_EAGLE || madwCascadeType[dwDetectorIdx] == E_IFaceRecType::IFACEREC_DETECTOR_NICKLE || madwCascadeType[dwDetectorIdx] == E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST)
		{
			/*dwRet = FaceRecSDKManager::FeedOneFramToDetector(mahCascadeHandle[dwDetectorIdx],
																	matLoad,
																	pFaceRectArray,
																	pdwFaceCnt);*/
			dwRet = EagleMtcnnDetectManager::FeedOneFramToDetector(mahCascadeHandle[dwDetectorIdx],
				matLoad,
				dwWidth,
				dwHeight,
				pFaceRectArray,
				pdwFaceCnt);
		}
        else
        {
            std::cout << "Cascade, CascadeFaceDetectAct wrong type detector: "     << mSelfHandle
                      <<", type: " << madwCascadeType[dwDetectorIdx]  << std::endl;
        }

        break;
    }

    return FACE_SNAPER_ERR_NONE;
}

if_handle_t CascadeDetectManager::CreateIfDetectorHandle(int   dwThreadCnt)
{
    if_handle_t hRetHandle = (if_handle_t)0;

    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    mHandleDetectSeq   = mHandleDetectSeq + 0xa;

    hRetHandle         = (if_handle_t)mHandleDetectSeq;

    SP_CascadeDetector   newDetector(new CascadeDetector(hRetHandle));

    mIfDetectorMap.insert(TCascadeDetectorMap::value_type(hRetHandle, newDetector));

    return hRetHandle;

}
    
void  CascadeDetectManager::DestoryIfDetectoHandle(if_handle_t  hHandle)
{
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    TCascadeDetectorMap::iterator findDetector = mIfDetectorMap.find(hHandle);

    if (findDetector == mIfDetectorMap.end())
    {
        std::cout << "CascadeDetectManager::DestoryIfDetectoHandle, unexist handle: " << hHandle << std::endl;
    }
    else
    {
        std::cout << "CascadeDetectManager::DestoryIfDetectoHandle, deleted: " << hHandle << std::endl;

        mIfDetectorMap.erase(findDetector);
    }
}

if_result_t CascadeDetectManager::FeedOneFramToDetector(if_handle_t       hHandle,
														const cv::Mat &       matLoad,
                                                        const int         dwWidth,
                                                        const int         dwHeight,
                                                        int				  pdwRectType,
														PFACE_RST_TYPE  * pFaceRectArray,
                                                        unsigned int    * pdwFaceCnt)
{
    if_result_t dwRet = FACE_SNAPER_ERR_NONE;

    boost::shared_lock<boost::shared_mutex> readLocker(mMapLocker);

	if (matLoad.data == NULL || pFaceRectArray == NULL || pdwFaceCnt == NULL)
    {
        std::cout << "FeedOneFramToDetector, wrong parameter: " << hHandle << std::endl;

        return FACE_SNAPER_ERR_NONE;
    }
    
    *pFaceRectArray = NULL;
    *pdwFaceCnt     = 0;

    TCascadeDetectorMap::iterator findDetector = mIfDetectorMap.find(hHandle);

    if (findDetector == mIfDetectorMap.end())
    {
        std::cout << "FeedOneFramToDetector, unexist handle: " << hHandle << std::endl;
    }
    else
    {
		dwRet = findDetector->second->CascadeFaceDetect(matLoad,
                                                        dwWidth,
                                                        dwHeight,
                                                        pdwRectType,
                                                        pFaceRectArray,
                                                        pdwFaceCnt);
    }

    return dwRet;
}

void  CascadeDetectManager::ReleaseDetFaceResult(int             dwRectType,
                                                 PIF_FACERECT    pFaceRectArray,
                                                 unsigned int    pdwFaceCnt)
{
    if(pFaceRectArray == NULL || pdwFaceCnt == 0)
    {
        std::cout << "Try to Release null face rect:" << (int64_t)pFaceRectArray
                  <<", or face cnt = : "              << pdwFaceCnt << std::endl;
        return;
    }

	if (dwRectType == E_IFaceRecType::IFACEREC_DETECTOR_EAGLE || dwRectType == E_IFaceRecType::IFACEREC_DETECTOR_NICKLE || dwRectType == E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST)
	{
		//FaceRecSDKManager::ReleaseDetFaceResult(pFaceRectArray, pdwFaceCnt);
		EagleMtcnnDetectManager::ReleaseDetFaceResult(pFaceRectArray, pdwFaceCnt);
	}
    else
    {
        std::cout << "Cascade, release wrong type detect result, type: " << dwRectType << std::endl;
    }   
}

FaceExtractor::FaceExtractor()
{
    for (int dwAlgIdx = 0; dwAlgIdx < ALG_NUM_MAX; dwAlgIdx++)
    {
        pFaceAlgExtraType[dwAlgIdx] = ALG_EXTRA_TYPE_UNKNOW;
        pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
    }
	
    dwAlgNums = 0;   
	
	if (g_mdwAlgVersion == ALG_EXTRA_TYPE_TUAN || g_mdwAlgVersion == ALG_EXTRA_TYPE_DEEPEYE)
    {
        //ALG_EXTRA_TYPE_TUAN
        pFaceAlgExtraType[dwAlgNums]  = ALG_EXTRA_TYPE_DEEPEYE;
		pFaceAlgExtraHandl[dwAlgNums] = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_EXTRACTOR_TUNA, "model/gluon");
        dwAlgNums += 0x1;
    }
	else if (g_mdwAlgVersion == ALG_EXTRA_TYPE_GLUON)
	{
		//ALG_EXTRA_TYPE_GLUON
		pFaceAlgExtraType[dwAlgNums] = ALG_EXTRA_TYPE_GLUON;
		pFaceAlgExtraHandl[dwAlgNums] = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_EXTRACTOR_GLUON, "model/gluon");
		dwAlgNums += 0x1;
	}
}

FaceExtractor::~FaceExtractor()
{
    for (int dwAlgIdx = 0; dwAlgIdx < dwAlgNums; dwAlgIdx++)
    {
        if (pFaceAlgExtraHandl[dwAlgIdx] == INVALID_HANDL)
        {        
            continue;
        }

        FaceRecSDKManager::DestoryIfHandle(pFaceAlgExtraHandl[dwAlgIdx]);
        pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
    }
}

int  FaceExtractor::ExtratFeatureFromImg(
                                                cv::Mat      &      rMatOrg,
												T_IFaceRecRect &    pFaceRect,
                                                float **            pFeature,
                                                int   *             pFeatureLen,
                                                int                 dwAlgType,
                                                TFeatureSeqIdInfo & rSeqIdInfo,
                                                int                 dwSwReqMode)
{
    if ((pFeature == NULL) || (pFeatureLen == NULL))
    {
        printf("FaceExtractor, ExtratFeatureFromImg, error Null param ");
        return FACE_SNAPER_ERR_NONE;
    }
    int dwAlgIdx = 0;
    float * pFeatureRet = NULL;
    int     dwFeatureLen = 0;   
    
    for (dwAlgIdx = 0; dwAlgIdx < dwAlgNums; dwAlgIdx++)
    {
        if ((pFaceAlgExtraType[dwAlgIdx] == dwAlgType) && 
            (pFaceAlgExtraHandl[dwAlgIdx] != INVALID_HANDL))
        {             
            float* pRetFeatureTmp = NULL;
            int    dwRetLen = 0;
            if (((dwAlgType == ALG_EXTRA_TYPE_DEEPEYE) || (dwAlgType == ALG_EXTRA_TYPE_TUAN)) && (pFaceAlgExtraHandl[dwAlgIdx] != INVALID_HANDL))
            {                
                cv::Mat  grayMat;
                cvtColor(rMatOrg, grayMat, CV_BGR2GRAY);
                dwRetLen = FaceRecSDKManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx],
                                                                    grayMat,
                                                                    pFaceRect,
                                                                    &pRetFeatureTmp);
                if ((dwRetLen != g_mdwAlgFeatureLen) || (pRetFeatureTmp == NULL))
                {
					printf("FaceExtractor, ExtratFeatureFromImg, Failed, FeatureLen: %d, RetLen: %d, AlgType: %d\n", g_mdwAlgFeatureLen, dwRetLen, dwAlgType);
                    if (pRetFeatureTmp != NULL)
                    {
                        FaceRecSDKManager::ReleaseFeautreResult(pRetFeatureTmp);
                        pRetFeatureTmp = NULL;
                    }                    
                    dwRetLen = 0;                    
                }

                pFeatureRet = pRetFeatureTmp;
                dwFeatureLen = dwRetLen;   
            }
			else if (dwAlgType == ALG_EXTRA_TYPE_GLUON)
			{
				// Gluon特征值flip归一化处理 20171115 pengjidong
				if (g_flipExtratFeature)
				{
					//正常提取一次
					float * pNomalFeature = NULL;
					int     pNomalFeatureLen = 0;
					pNomalFeatureLen = FaceRecSDKManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx], rMatOrg, pFaceRect, &pNomalFeature);

					std::cout << "FaceExtractor::ExtratFeatureFromImg::Gluon pNomalFeatureLen:" << pNomalFeatureLen << std::endl;

					if ((pNomalFeatureLen != g_mdwAlgFeatureLen) || (pNomalFeature == NULL))
					{
						std::cout << "FaceExtractor::ExtratFeatureFromImg::Gluon pNomalFeatureLen != g_mdwAlgFeatureLen" << g_mdwAlgFeatureLen << endl;
						if (pNomalFeature != NULL)
						{
							FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
							pNomalFeature = NULL;
						}
						pFeatureRet = pNomalFeature;
						dwFeatureLen = 0;
					}
					//flip 提取一次
					float * pFlipFeature = NULL;
					int     pFlipFeatureLen = 0;
					cv::Mat matFlip;

					cv::flip(rMatOrg, matFlip, 1);

					T_IFaceRecRect  pFaceRectFlip(pFaceRect);

					pFaceRectFlip.left = (rMatOrg.cols - pFaceRect.right) > 0 ? (rMatOrg.cols - pFaceRect.right) : 0;

					pFaceRectFlip.right = (rMatOrg.cols - pFaceRect.left) > 0 ? (rMatOrg.cols - pFaceRect.left) : 0;

					pFlipFeatureLen = FaceRecSDKManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx], matFlip, pFaceRectFlip, &pFlipFeature);


					if ((pFlipFeatureLen != g_mdwAlgFeatureLen) || (pFlipFeature == NULL))
					{
						if (pFlipFeature != NULL)
						{
							FaceRecSDKManager::ReleaseFeautreResult(pFlipFeature);
							pFlipFeature = NULL;
						}
						pFeatureRet = pFlipFeature;
						dwFeatureLen = 0;
					}

					//两种特征值合成
					float * pRetFeature = (float*)malloc(sizeof(float)*pNomalFeatureLen);

					if (pRetFeature == NULL)
					{
						FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
						FaceRecSDKManager::ReleaseFeautreResult(pFlipFeature);
						pFeatureRet = pNomalFeature;
						dwFeatureLen = 0;
					}

					for (int dwIndx = 0; dwIndx < pNomalFeatureLen; dwIndx++)
					{
						pRetFeature[dwIndx] = pNomalFeature[dwIndx] + pFlipFeature[dwIndx];
					}

					//归一化
					float dfNormaParam = 0.0;

					for (int dwFeatureIdx = 0; dwFeatureIdx < pNomalFeatureLen; dwFeatureIdx++)
					{
						dfNormaParam += pRetFeature[dwFeatureIdx] * pRetFeature[dwFeatureIdx];
					}

					dfNormaParam = sqrt(dfNormaParam);

					for (int dwFeatureIdx = 0; dwFeatureIdx < pNomalFeatureLen; dwFeatureIdx++)
					{
						pRetFeature[dwFeatureIdx] = pRetFeature[dwFeatureIdx] / dfNormaParam;
					}

					pFeatureRet = pRetFeature;
					dwFeatureLen = pNomalFeatureLen;

					FaceRecSDKManager::ReleaseFeautreResult(pNomalFeature);
					FaceRecSDKManager::ReleaseFeautreResult(pFlipFeature);
				}
				else
				{
					dwRetLen = FaceRecSDKManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx],
						rMatOrg,
						pFaceRect,
						&pRetFeatureTmp);
					if ((dwRetLen != g_mdwAlgFeatureLen) || (pRetFeatureTmp == NULL))
					{
						printf("FaceExtractor, ExtratFeatureFromImg, Failed, FeatureLen: %d, RetLen: %d, AlgType: %d\n", g_mdwAlgFeatureLen, dwRetLen, dwAlgType);

						if (pRetFeatureTmp != NULL)
						{
							FaceRecSDKManager::ReleaseFeautreResult(pRetFeatureTmp);
							pRetFeatureTmp = NULL;
						}
						dwRetLen = 0;
					}

					pFeatureRet = pRetFeatureTmp;
					dwFeatureLen = dwRetLen;
				}
			}
            else
            {
                printf("FaceExtractor, ExtratFeatureFromImg, invalid Handl:%p, , algType: \n", 
                       pFaceAlgExtraHandl[dwAlgIdx] , dwAlgType);
            }   
            break;
        }
    }

    if (dwAlgIdx == dwAlgNums)
    {
        printf("FaceExtractor, ExtratFeatureFromImg, Unkonw algType: %d\n" , dwAlgType);
    }
    
    *pFeature    = pFeatureRet;
    *pFeatureLen = dwFeatureLen;
    
    return FACE_SNAPER_ERR_NONE;
}

MutilAlgExtractor::MutilAlgExtractor(if_handle_t pHandl)
{
    mSelfHandle = pHandl;

    mdwTargsNum = 0;

    for (int dwTargsIdx = 0; dwTargsIdx < TARGS_NUM_MAX; dwTargsIdx++)
    {
        mTargsExtraHandl[dwTargsIdx].reset();
        mTargsExtraType[dwTargsIdx] = TARGS_EXTRA_TYPE_UNKNOW;
    }

    SP_FaceExtractor   newFaceExator = boost::make_shared<FaceExtractor>();
    SP_IfExtractorBase newExtor      = dynamic_pointer_cast<IfExtractorBase>(newFaceExator);
    mTargsExtraHandl[mdwTargsNum]    = newExtor;
    mTargsExtraType[mdwTargsNum]     = TARGS_EXTRA_TYPE_FACE;

    mdwTargsNum += 0x1;
}

MutilAlgExtractor::~MutilAlgExtractor()
{
    for (int dwTargsIdx = 0; dwTargsIdx < mdwTargsNum; dwTargsIdx++)
    {
        mTargsExtraHandl[dwTargsIdx].reset();
    }
}

int  MutilAlgExtractor::ExtratFeatureFromImage(cv::Mat   &         rMatGray,
											   T_IFaceRecRect &         pFaceRect,
                                               float    **         pFeature,
                                               int       *         pFeatureLen,
                                               int                 dwTargType,
                                               int                 dwAlgType,
                                               TFeatureSeqIdInfo & rSeqIdInfo,
                                               int                 dwSwReqMode)
{
    if (rMatGray.data == NULL || pFeature == NULL || pFeatureLen == NULL)
    {
        printf("MutilAlgExtractor, ExtratFeatureFromImage, wrong parameter: ");

        return FACE_SNAPER_ERR_NONE;
    }
    
    int dwRet = FACE_SNAPER_ERR_NONE;
    
    for (int dwTrgIdx = 0; dwTrgIdx < mdwTargsNum; dwTrgIdx++)
    {
        if (mTargsExtraType[dwTrgIdx] == TARGS_EXTRA_TYPE_UNKNOW)
        {
            continue;
        }
        if (dwTargType == mTargsExtraType[dwTrgIdx])
        {
            dwRet = mTargsExtraHandl[dwTrgIdx]->ExtratFeatureFromImg(rMatGray,
                                                                     pFaceRect, 
                                                                     pFeature, 
                                                                     pFeatureLen, 
                                                                     dwAlgType,
                                                                     rSeqIdInfo,
                                                                     dwSwReqMode);
            break;
        }
    }

    return dwRet;    
}

if_handle_t IfMutilAlgExtManger::CreateIfExtratorHandle(const string& strPath)
{
    if_handle_t hRetHandle = (if_handle_t)0;

    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    mHandleAlgSeq = mHandleAlgSeq + 0xa;

    hRetHandle = (if_handle_t)mHandleAlgSeq;

    SP_MutilAlgExtractor   newExtrator(new MutilAlgExtractor(hRetHandle));

    mIfAlgMap.insert(TMutilAlgMap::value_type(hRetHandle, newExtrator));

    return hRetHandle;
}

void IfMutilAlgExtManger::DestoryIfExtratorHandle(if_handle_t  hHandle)
{
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    TMutilAlgMap::iterator findExtrator = mIfAlgMap.find(hHandle);

    if (findExtrator == mIfAlgMap.end())
    {
        printf("IfMutilAlgExtManger, DestoryIfExtratorHandle, unexist handle: %p\n" , hHandle);
    }
    else
    {
        printf("IfMutilAlgExtManger, DestoryIfExtratorHandle, deleted: %p\n" , hHandle);
        mIfAlgMap.erase(findExtrator);
    }
}
   
int  IfMutilAlgExtManger::ExtratFeatureFromImage(if_handle_t         hHandle,
                                                 cv::Mat     &       rMatGray,
												 if_rect_t   &		 pFaceRect,
                                                 float **            pFeature, 
                                                 int                 dwTargType,
                                                 int                 dwAlgType,
                                                 TFeatureSeqIdInfo & rSeqIdInfo,
                                                 int                 dwSwReqMode)
{
    int dwFeatureLen = 0;
    
    boost::shared_lock<boost::shared_mutex> readLocker(mMapLocker);

    if (rMatGray.data == NULL || pFeature == NULL)
    {
        printf("IfMutilAlgExtManger, ExtratFeatureFromImage, wrong parameter: %p\n " , hHandle);
        return FACE_SNAPER_ERR_NONE;
    }
    
    TMutilAlgMap::iterator findExtarator = mIfAlgMap.find(hHandle);

    if (findExtarator == mIfAlgMap.end())
    {
        printf("IfMutilAlgExtManger, ExtratFeatureFromImage, unexist handle: %p\n" , hHandle);
    }
    else
    {
		T_IFaceRecRect tFaceRect;
		tFaceRect.left = pFaceRect.left;
		tFaceRect.top = pFaceRect.top;
		tFaceRect.right = pFaceRect.right;
		tFaceRect.bottom = pFaceRect.bottom;

        findExtarator->second->ExtratFeatureFromImage(rMatGray,
												      tFaceRect,
                                                      pFeature,
                                                     &dwFeatureLen,                                                    
                                                      dwTargType,
                                                      dwAlgType,
                                                      rSeqIdInfo,
                                                      dwSwReqMode);
    }

    return dwFeatureLen;
}

void IfMutilAlgExtManger::ReleaseFeautreResult(float *pFeature, int dwAlgType)
{
	if (dwAlgType != ALG_EXTRA_TYPE_UNKNOW)
	{
		FaceRecSDKManager::ReleaseFeautreResult(pFeature);
	}
}

IfMutilAlgCompare::IfMutilAlgCompare(if_handle_t handl)
{
    mMyHandl = handl;
    for (int dwIdx = 0; dwIdx < ALG_NUM_MAX; dwIdx++)
    {
        mAlgCmpHandl[dwIdx] = INVALID_HANDL;
        mAlgCmpType[dwIdx]  = ALG_EXTRA_TYPE_UNKNOW;
    }
    
    mdwCmpNums = 0;

	if (g_mdwAlgVersion == ALG_EXTRA_TYPE_ORG)
	{
#ifdef OLD_SDK_INCLUDED
		mAlgCmpHandl[mdwCmpNums] = if_create_verify_instance_with_prefix(g_strAlgModelPath.c_str());
		mAlgCmpType[mdwCmpNums]  = ALG_EXTRA_TYPE_ORG;

		mdwCmpNums += 0x1;
#endif
	}

	if (g_mdwAlgVersion == ALG_EXTRA_TYPE_TUAN || g_mdwAlgVersion == ALG_EXTRA_TYPE_DEEPEYE)
	{

		mAlgCmpHandl[mdwCmpNums] = (if_handle_t)0xdddddddd;
		mAlgCmpType[mdwCmpNums] = ALG_EXTRA_TYPE_DEEPEYE;

		mdwCmpNums += 0x1;

		mAlgCmpHandl[mdwCmpNums] = (if_handle_t)0xbbbbbbbb;
		mAlgCmpType[mdwCmpNums] = ALG_EXTRA_TYPE_TUAN;

		mdwCmpNums += 0x1;
	}
	else if (g_mdwAlgVersion == ALG_EXTRA_TYPE_GLUON)
	{
		mAlgCmpHandl[mdwCmpNums] = (if_handle_t)0xcccccccc;
		mAlgCmpType[mdwCmpNums] = ALG_EXTRA_TYPE_GLUON;

		mdwCmpNums += 0x1;
	}
	else if (g_mdwAlgVersion == ALG_EXTRA_TYPE_FUSION)
	{
		mAlgCmpHandl[mdwCmpNums] = (if_handle_t)0xeeeeeeee;
		mAlgCmpType[mdwCmpNums] = ALG_EXTRA_TYPE_FUSION;

		mdwCmpNums += 0x1;
	}
}

IfMutilAlgCompare::~IfMutilAlgCompare()
{
    #ifdef OLD_SDK_INCLUDED 
    for (int dwIdx = 0; dwIdx < ALG_NUM_MAX; dwIdx++)
    {        
        if (mAlgCmpType[dwIdx] == ALG_EXTRA_TYPE_ORG)
        {
            if_verify_release_instance(mAlgCmpHandl[dwIdx]);
        }
    }
    #endif
}

int IfMutilAlgCompare::IfFeatureCompare(const float     *pFeature0,
                                        int              dwLen0,
                                        const float     *pFeature1,
                                        int              dwLen1,
                                        float           *score,
                                        int              dwAlgType,
                                        int              dwTargType)
{
    if (pFeature0 == NULL || pFeature1 == NULL || score == NULL)
    {
        printf("IfMutilAlgCompare, IfFeatureCompare, wrong null parameter:\n");
        return FACE_SNAPER_ERR_NONE;
    }

	if ((dwAlgType == ALG_EXTRA_TYPE_DEEPEYE) || (dwAlgType == ALG_EXTRA_TYPE_TUAN))
	{
		FaceRecSDKManager::CompareImageFeature(E_IFaceRecType::IFACEREC_EXTRACTOR_TUNA, pFeature0, dwLen0, pFeature1, dwLen1, score);
	}
	else if (dwAlgType == ALG_EXTRA_TYPE_GLUON)
	{
		FaceRecSDKManager::CompareImageFeature(E_IFaceRecType::IFACEREC_EXTRACTOR_GLUON, pFeature0, dwLen0, pFeature1, dwLen1, score);
		//GluonFeatureCompare(pFeature0, dwLen0, pFeature1, dwLen1, score);
	}
	else
	{
		*score = (float)0;
		printf("IfFeatureCompare, don't support alg: %d\n", dwAlgType);
	}

    return FACE_SNAPER_ERR_NONE;
}

int IfMutilAlgCompare::GluonFeatureCompare(const float *   pfFeature0,
	int       dwFeatureLen0,
	const float *   pfFeature1,
	int       dwFeatureLen1,
	float *   pfScore)
{
	float fCmpScore = (float)0.0;

	if (dwFeatureLen0 != dwFeatureLen1)
	{
		*pfScore = (float)0.0;
		return 0;
	}


	for (int floatIdx = 0; floatIdx < dwFeatureLen0; floatIdx++)
	{
		fCmpScore = fCmpScore
			+ (pfFeature0[floatIdx] * pfFeature0[floatIdx])
			+ (pfFeature1[floatIdx] * pfFeature1[floatIdx])
			- ((pfFeature0[floatIdx] * pfFeature1[floatIdx]) * 4);
	}

	float fExpTemp = (float)(1 / (1 + exp(fCmpScore)));

#if 1
	if (fExpTemp <= 0.5)
	{
		fCmpScore = (float)((3.3342 * fExpTemp) - 0.7671);
	}
	else {
		fCmpScore = (float)((0.2    * fExpTemp) + 0.8);
	}


	*pfScore = fCmpScore;
#else
	*pfScore = fExpTemp;
#endif

	return 0;
}

if_handle_t IfMutilAlgCompareManger::CreateIfCmpHandle()
{
    if_handle_t hRetHandle = (if_handle_t)0;

    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    mHandleCmpSeq = mHandleCmpSeq + 0xa;

    hRetHandle = (if_handle_t)mHandleCmpSeq;

    SP_IfMutilAlgCompare   newCmptor(new IfMutilAlgCompare(hRetHandle));

    mIfCmpMap.insert(TMutilAlgCmpMap::value_type(hRetHandle, newCmptor));

    return hRetHandle;
}

void IfMutilAlgCompareManger::DestoryIfCmpHandle(if_handle_t  hHandle)
{
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    TMutilAlgCmpMap::iterator findExtrator = mIfCmpMap.find(hHandle);

    if (findExtrator == mIfCmpMap.end())
    {
        printf("IfMutilAlgCompareManger, DestoryIfCmpHandle, unexist handle: %p\n" , hHandle);
    }
    else
    {
        printf("IfMutilAlgCompareManger, DestoryIfCmpHandle, deleted: %p\n" , hHandle);
        mIfCmpMap.erase(findExtrator);
    }
}

int  IfMutilAlgCompareManger::IfFeatureCompare(if_handle_t hHandle,
	const float     *pFeature0,
	int              dwLen0,
	const float     *pFeature1,
	int              dwLen1,
	float           *score,
	int              dwAlgType,
	int              dwTargType)
{
    int dwRet = 0;
    boost::shared_lock<boost::shared_mutex> readLocker(mMapLocker);

    if (pFeature0 == NULL || pFeature1 == NULL || score == NULL)
    {
        printf("IfMutilAlgCompareManger, IfFeatureCompare, null parameter: %p\n" , hHandle);
        return FACE_SNAPER_ERR_NONE;
    }

    TMutilAlgCmpMap::iterator findCmptor = mIfCmpMap.find(hHandle);

    if (findCmptor == mIfCmpMap.end())
    {
        printf("IfMutilAlgCompareManger, IfFeatureCompare, unexist handle: %p\n" , hHandle);
    }
    else
    {
        dwRet = findCmptor->second->IfFeatureCompare(pFeature0,
                                                    dwLen0,
                                                    pFeature1,
                                                    dwLen1,
                                                    score,
                                                    dwAlgType,
                                                    dwTargType);
    }

    return dwRet;
}


