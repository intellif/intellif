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

#include "svm.h"

extern std::string g_strAlgModelPath;
extern int         g_mdwDetectMode;

extern int         g_mdwDetectHfProfile;
extern int         g_mdwDetectProfile;
extern int         g_mdwDetectRot;

extern int         g_mdwAlgVersion;

extern int         g_mdwIfCompFuncSel;

extern vector<int>    g_vecIntDetect;

boost::shared_mutex   IfDetectorManager::mMapLocker;
TIfDetectorMap        IfDetectorManager::mIfDetectorMap;
int                   IfDetectorManager::mHandleDetectSeq    = 0x3c3c3c3c;

boost::shared_mutex   EagleMtcnnDetectManager::mMtcnnMapLocker;
TMtcnnDetectorMap     EagleMtcnnDetectManager::mMtcnnDetectMap;

int                   EagleMtcnnDetectManager::mMtcnnHandleSeq = 0x12121212;

boost::shared_mutex   GluonFeatureExtManager::mGluonMapLocker;
TGluonExtratorMap     GluonFeatureExtManager::mGluonExtratorMap;

int                   GluonFeatureExtManager::mGluonExtractSeq = 0x34343434;

boost::shared_mutex   FusionFeatureExtManager::mFusionMapLocker;
TFusionExtratorMap    FusionFeatureExtManager::mFusionExtratorMap;

int                   FusionFeatureExtManager::mFusionExtractSeq = 0x43434343;

//#if FACE_EXTRAT_FUNCTION_TYPE == 1

boost::shared_mutex   IfFeatureExtManager::mMapLocker;
TIfExtratorMap        IfFeatureExtManager::mIfExtratorMap;
int                   IfFeatureExtManager::mHandleExtractSeq = 0x21212121;
//#endif

boost::shared_mutex   IfMutilAlgExtManger::mMapLocker;
TMutilAlgMap          IfMutilAlgExtManger::mIfAlgMap;

int                   IfMutilAlgExtManger::mHandleAlgSeq     = 0x76767676;

boost::shared_mutex   IfMutilAlgCompareManger::mMapLocker;
TMutilAlgCmpMap       IfMutilAlgCompareManger::mIfCmpMap;

int                   IfMutilAlgCompareManger::mHandleCmpSeq = 0x45454545;

boost::shared_mutex   CascadeDetectManager::mMapLocker;
TCascadeDetectorMap   CascadeDetectManager::mIfDetectorMap;

int                   CascadeDetectManager::mHandleDetectSeq = 0x67676767;

struct svm_model * g_pCompSvmHandl = NULL;
int                g_dwCompSvmPredictMode = COMP_SVM_PREDIECT_MODE_VALUE;

void  EagleMtcnnDetectManager::StaticInit(void)
{
	#ifndef _MSC_VER
	MtcnnMxnetHandle::MtcnnStaticInit();
	#endif
}

if_handle_t EagleMtcnnDetectManager::CreateIfDetectorHandle(int   dwThreadCnt)
{
	if_handle_t hRetHandle = (if_handle_t)0;

	boost::unique_lock<boost::shared_mutex> writeLocker(mMtcnnMapLocker);
	//cout << "EagleMtcnnDetectManager::CreateIfDetectorHandle::#mMtcnnHandleSeq=" << (if_handle_t)mMtcnnHandleSeq << std::endl;

	mMtcnnHandleSeq = mMtcnnHandleSeq + 0xa;

	//cout << "EagleMtcnnDetectManager::CreateIfDetectorHandle::mMtcnnHandleSeq=" << (if_handle_t)mMtcnnHandleSeq << std::endl;

	hRetHandle = (if_handle_t)mMtcnnHandleSeq;

	std::vector<float> vecFloatMtcnnThr;

	vecFloatMtcnnThr.push_back((float)0.75);
	vecFloatMtcnnThr.push_back((float)0.7);
	vecFloatMtcnnThr.push_back((float)0.97);

	string strMtcnnModelPath = g_strAlgModelPath
		+ "/"
		+ "mtcnn";

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

	//cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle hHandle=" << hHandle << std::endl;

	TMtcnnDetectorMap::iterator findDetector = mMtcnnDetectMap.find(hHandle);
	//cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle findDetector=" << findDetector << std::endl;
	if (findDetector == mMtcnnDetectMap.end())
	{
		std::cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle, unexist handle: " << hHandle << std::endl;
	}
	else
	{
		std::cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle, ##delete handle: " << hHandle << std::endl;
		mMtcnnDetectMap.erase(findDetector);
		//std::cout << "EagleMtcnnDetectManager::DestoryIfDetectoHandle, ##delete handle: end###" << std::endl;
	}
#endif
}


if_result_t EagleMtcnnDetectManager::FeedOneFramToDetector(if_handle_t       hHandle,
	const Mat &       matRgbDat,
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

if_handle_t  GluonFeatureExtManager::CreateIfExtratorHandle(int  dwThreadCnt)
{
	boost::unique_lock<boost::shared_mutex> writeLocker(mGluonMapLocker);

	mGluonExtractSeq = mGluonExtractSeq + 0xa;

	if_handle_t hRetHandle = (if_handle_t)mGluonExtractSeq;

	string strGluonModelPath = g_strAlgModelPath
		+ "/"
		+ "gluon04";

	SP_GluonMxnetHandle newHandle(new GluonMxnetHandle(strGluonModelPath,
		112,
		96,
		1,
		-1));


	mGluonExtratorMap.insert(TGluonExtratorMap::value_type(hRetHandle, newHandle));

	return hRetHandle;
}

void  GluonFeatureExtManager::DestoryIfExtratorHandle(if_handle_t  hHandle)
{
	//cout << "GluonFeatureExtManager::DestoryIfExtratorHandle start;;" << std::endl;
	boost::unique_lock<boost::shared_mutex> writeLocker(mGluonMapLocker);

	TGluonExtratorMap::iterator findExtrator = mGluonExtratorMap.find(hHandle);

	if (findExtrator == mGluonExtratorMap.end())
	{
		std::cout << "GluonFeatureExtManager::DestoryIfExtratorHandle, unexist handle: " << hHandle << std::endl;
	}
	else
	{
		std::cout << "GluonFeatureExtManager::DestoryIfExtratorHandle, deleted: " << hHandle << std::endl;

		mGluonExtratorMap.erase(findExtrator);
	}
}

int  GluonFeatureExtManager::ExtratFeatureFromImage(if_handle_t      hHandle,
	cv::Mat      &   rMatRgb,
	if_rect_t    &   rFaceRect,
	float        **  pFeature)
{
	boost::shared_lock<boost::shared_mutex> readLocker(mGluonMapLocker);

	int dwFeatureLen = 0;

	if (rMatRgb.data == NULL || pFeature == NULL)
	{
		std::cout << "ExtratFeatureFromImage, wrong parameter: " << hHandle << std::endl;

		return dwFeatureLen;
	}

	if (rMatRgb.channels() != 3)
	{
		printf("ExtratFeatureFromImage, Mat channel missMatch: %d \n", rMatRgb.channels());

		pFeature[0] = NULL;

		return dwFeatureLen;
	}

	TGluonExtratorMap::iterator findExtrator = mGluonExtratorMap.find(hHandle);

	if (findExtrator == mGluonExtratorMap.end())
	{
		std::cout << "ExtratFeatureFromImage, unexist handle: " << hHandle << std::endl;

		pFeature[0] = NULL;

		return dwFeatureLen;
	}

	std::vector<float>  vecFeatureExt;

#if 0
	int dwFaceCropX = rFaceRect.left < 0 ? 0 : rFaceRect.left;
	int dwFaceCropY = rFaceRect.top  < 0 ? 0 : rFaceRect.top;

	int dwFaceCropW = (rFaceRect.right > rMatRgb.cols)
		? (rMatRgb.cols - dwFaceCropX)
		: (rFaceRect.right - dwFaceCropX);

	int dwFaceCropH = (rFaceRect.bottom > rMatRgb.rows)
		? (rMatRgb.rows - dwFaceCropY)
		: (rFaceRect.bottom - dwFaceCropY);


	CvRect oRoiCroped = cvRect(dwFaceCropX, dwFaceCropY, dwFaceCropW, dwFaceCropH);

	Mat    matCroped;

	try{
		rMatRgb(oRoiCroped).copyTo(matCroped);
	}
	catch (...)
	{
		std::cout << "rect error: face x1:" << rFaceRect.left << ", y1:" << rFaceRect.top << ", x2:" << rFaceRect.right << ", y2:" << rFaceRect.bottom << std::endl;
		std::cout << "rect error: crop x:" << dwFaceCropX << ", y:" << dwFaceCropY << ", width:" << dwFaceCropW << ", height:" << dwFaceCropH << ", mat.cols:" << rMatRgb.cols << ", mat.rows:" << rMatRgb.rows << std::endl;

		pFeature[0] = NULL;

		return dwFeatureLen;
	}

	dwFeatureLen = findExtrator->second->Feature(matCroped, vecFeatureExt);

#else
	int dwFaceWidth = (rFaceRect.right - rFaceRect.left);
	int dwFaceHeight = (rFaceRect.bottom - rFaceRect.top);

	int dwMaxBorder = dwFaceWidth > dwFaceHeight ? dwFaceWidth : dwFaceHeight;
	int dwMaxRaidiu = dwMaxBorder >> 1;

	int dwFaceCentX = (rFaceRect.right + rFaceRect.left) >> 1;

	int dwFaceCentY = (rFaceRect.bottom + rFaceRect.top) >> 1;

	int dwCenterAdj = (dwFaceWidth > dwFaceHeight)
		? ((dwFaceWidth - dwFaceHeight) >> 1)
		: ((dwFaceHeight - dwFaceWidth) >> 1);

	int dwLeft = dwFaceCentX - dwMaxRaidiu;
	int dwTop = dwFaceCentY - dwMaxRaidiu;
	int dwRight = dwFaceCentX + dwMaxRaidiu;
	int dwBottom = dwFaceCentY + dwMaxRaidiu;

	if (dwLeft < 0)
	{
		dwLeft -= dwCenterAdj;
		dwRight -= dwCenterAdj;
	}
	else if (dwTop < 0)
	{
		dwTop -= dwCenterAdj;
		dwBottom -= dwCenterAdj;
	}
	else if (dwRight > rMatRgb.cols)
	{
		dwLeft += dwCenterAdj;
		dwRight += dwCenterAdj;
	}
	else if (dwBottom > rMatRgb.rows)
	{
		dwTop += dwCenterAdj;
		dwBottom += dwCenterAdj;
	}

	dwFeatureLen = findExtrator->second->Feature(rMatRgb, dwLeft, dwTop, dwRight, dwBottom, vecFeatureExt);
#endif

	if (dwFeatureLen == 0 || vecFeatureExt.size() == 0)
	{
		std::cout << "ExtratFeatureFromImage, failed to extrace feature: " << hHandle << std::endl;

		pFeature[0] = NULL;
		dwFeatureLen = 0;

		return dwFeatureLen;
	}

	int     dwMallocLen = (int)(sizeof(float)* dwFeatureLen);
	float * pFaceFeature = (float *)malloc(dwMallocLen);

	if (pFaceFeature == NULL)
	{
		std::cout << "ExtratFeatureFromImage, failed to alloc memory: " << hHandle << std::endl;

		pFeature[0] = NULL;
		dwFeatureLen = 0;
	}
	else
	{
		//std::cout << "Feature len:" << dwFeatureLen << std::endl;

		for (int dwFeatureIdx = 0; dwFeatureIdx < dwFeatureLen; dwFeatureIdx++)
		{
			pFaceFeature[dwFeatureIdx] = vecFeatureExt[dwFeatureIdx];

#if 0
			if ((dwFeatureIdx & 0x7) == 7)
			{
				std::cout << vecFeatureExt[dwFeatureIdx] << "," << std::endl;
			}
			else
			{
				std::cout << vecFeatureExt[dwFeatureIdx] << ",";
			}
#endif
		}

		pFeature[0] = pFaceFeature;
	}

	return dwFeatureLen;
}

void  GluonFeatureExtManager::ReleaseFeautreResult(float        *  pFeature)
{
	if (pFeature == NULL)
	{
		printf("Try to release null result");
	}
	else
	{
		free(pFeature);

		pFeature = NULL;
	}
}

void  GluonFeatureExtManager::IfFeatureExtMangerInit(void)
{

}

FustionFeatureExter::FustionFeatureExter(if_handle_t hHandleVal, int dwThreadCnt)
{
	mhSelfHandle = hHandleVal;

	mhTunaHandle = IfFeatureExtManager::CreateIfExtratorHandle(dwThreadCnt);
	mhGluonHandle = GluonFeatureExtManager::CreateIfExtratorHandle(dwThreadCnt);
}


FustionFeatureExter::~FustionFeatureExter()
{
	if (mhTunaHandle != 0)
	{
		IfFeatureExtManager::DestoryIfExtratorHandle(mhTunaHandle);
	}

	if (mhGluonHandle != 0)
	{
		GluonFeatureExtManager::DestoryIfExtratorHandle(mhGluonHandle);
	}
}


int FustionFeatureExter::ExtratFeatureFromImage(cv::Mat      &   rMatRgb,
	if_rect_t    &   pFaceRect,
	float        **  pFeature)
{
	if (mhTunaHandle == 0 || mhGluonHandle == 0)
	{
		std::cout << "Fusion Handle is null, Tuna: " << mhTunaHandle << ", Gluon: " << mhGluonHandle << std::endl;

		pFeature[0] = NULL;

		return 0;
	}

	cv::Mat mtGray;

	cv::cvtColor(rMatRgb, mtGray, CV_BGR2GRAY);

	float *       pfTunaFeatherRst;

	int dwTunaLen = IfFeatureExtManager::ExtratFeatureFromImage(mhTunaHandle,
		mtGray,
		pFaceRect,
		&pfTunaFeatherRst);

	if (dwTunaLen == 0 || pfTunaFeatherRst == NULL)
	{
		pFeature[0] = NULL;

		return 0;
	}

	// do tuna normize
	float dfNormaParam = 0.0;

	for (int dwFeatureIdx = 0; dwFeatureIdx < dwTunaLen; dwFeatureIdx++)
	{
		dfNormaParam += pfTunaFeatherRst[dwFeatureIdx] * pfTunaFeatherRst[dwFeatureIdx];
	}

	dfNormaParam = sqrt(dfNormaParam);

	for (int dwFeatureIdx = 0; dwFeatureIdx < dwTunaLen; dwFeatureIdx++)
	{
		pfTunaFeatherRst[dwFeatureIdx] = pfTunaFeatherRst[dwFeatureIdx] / dfNormaParam;
	}

	//gluon Feature Extract

	float *       pfGluonFeatherRst;

	int dwGluonLen = GluonFeatureExtManager::ExtratFeatureFromImage(mhGluonHandle,
		rMatRgb,
		pFaceRect,
		&pfGluonFeatherRst);

	if (dwGluonLen == 0 || pfGluonFeatherRst == NULL)
	{
		IfFeatureExtManager::ReleaseFeautreResult(pfTunaFeatherRst);
		pFeature[0] = NULL;

		return 0;
	}

	int     dwFeatureLen = 0;

	int     dwMallocLen = (int)(sizeof(float)* (dwTunaLen + dwGluonLen));
	float * pFaceFeature = (float *)malloc(dwMallocLen);

	if (pFaceFeature == NULL)
	{
		std::cout << "ExtratFeatureFromImage, failed to alloc memory: " << std::endl;

		pFeature[0] = NULL;
		dwFeatureLen = 0;
	}
	else
	{
		for (int dwFeatureIdx = 0; dwFeatureIdx < dwTunaLen; dwFeatureIdx++)
		{
			pFaceFeature[dwFeatureIdx] = pfTunaFeatherRst[dwFeatureIdx];
		}

		for (int dwFeatureIdx = 0; dwFeatureIdx < dwGluonLen; dwFeatureIdx++)
		{
			pFaceFeature[dwTunaLen + dwFeatureIdx] = pfGluonFeatherRst[dwFeatureIdx];
		}


		pFeature[0] = pFaceFeature;
		dwFeatureLen = dwTunaLen + dwGluonLen;
	}

	IfFeatureExtManager::ReleaseFeautreResult(pfTunaFeatherRst);
	GluonFeatureExtManager::ReleaseFeautreResult(pfGluonFeatherRst);


	return dwFeatureLen;
}

void FusionFeatureExtManager::IfFeatureExtMangerInit(void)
{
	Tuna17FeaturesContainer::LibStaticInit();
}

if_handle_t FusionFeatureExtManager::CreateIfExtratorHandle(int           dwThreadCnt)
{
	boost::unique_lock<boost::shared_mutex> writeLocker(mFusionMapLocker);

	mFusionExtractSeq = mFusionExtractSeq + 0xa;

	if_handle_t hRetHandle = (if_handle_t)mFusionExtractSeq;

	SP_FustionFeatureExter newFusionFeatureExter(new FustionFeatureExter(hRetHandle, dwThreadCnt));

	mFusionExtratorMap.insert(TFusionExtratorMap::value_type(hRetHandle, newFusionFeatureExter));

	return hRetHandle;
}


void FusionFeatureExtManager::DestoryIfExtratorHandle(if_handle_t  hHandle)
{
	boost::unique_lock<boost::shared_mutex> writeLocker(mFusionMapLocker);

	TFusionExtratorMap::iterator findExtrator = mFusionExtratorMap.find(hHandle);

	if (findExtrator == mFusionExtratorMap.end())
	{
		printf("Fusion Destory ExtratorHandle, unexist handle: %d \n", hHandle);
	}
	else
	{
		printf("Fusion Destory ExtratorHandle, deleted: %d \n", hHandle);

		//findExtrator->second->Release();

		mFusionExtratorMap.erase(findExtrator);
	}
}


int FusionFeatureExtManager::ExtratFeatureFromImage(if_handle_t      hHandle,
	cv::Mat      &   rMatRgb,
	if_rect_t    &   pFaceRect,
	float        **  pFeature)
{
	boost::shared_lock<boost::shared_mutex> readLocker(mFusionMapLocker);

	int dwFeatureLen = 0;

	if (rMatRgb.data == NULL || pFeature == NULL)
	{
		printf("ExtratFeatureFromImage, wrong parameter: %d \n", hHandle);

		return dwFeatureLen;
	}

	TFusionExtratorMap::iterator findExtrator = mFusionExtratorMap.find(hHandle);

	if (findExtrator == mFusionExtratorMap.end())
	{
		printf("FusionFeatureExtManager try  ext, unexist handle: %d \n", hHandle);

		pFeature[0] = NULL;

		return 0;
	}
	else
	{
		//printf("FusionFeatureExtManager, ext with: %d \n", hHandle);

		int dwFeatureLen = findExtrator->second->ExtratFeatureFromImage(rMatRgb,
			pFaceRect,
			pFeature);


		return dwFeatureLen;
	}

}


void FusionFeatureExtManager::ReleaseFeautreResult(float        *  pFeature)
{
	if (pFeature != NULL)
	{
		free(pFeature);
	}
}

if_handle_t IfDetectorManager::CreateIfDetectorHandle(int   dwThreadCnt)
{
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    mHandleDetectSeq       = mHandleDetectSeq + 0xa;

    if_handle_t hRetHandle = (if_handle_t)mHandleDetectSeq;

    bool bDetectMode  = (g_mdwDetectMode == IF_PARAMETER_MODE_FALSE) ? false : true;
                      
    SP_FaceDetector newHandle(new FaceDetector(bDetectMode));

    bool bHalfProfile = (g_mdwDetectHfProfile == IF_PARAMETER_MODE_FALSE) ? false : true;
    bool bProfile     = (g_mdwDetectProfile == IF_PARAMETER_MODE_FALSE) ? false : true;
    bool bRot         = (g_mdwDetectRot == IF_PARAMETER_MODE_FALSE) ? false : true;

    newHandle->Init(bHalfProfile, bProfile, bRot);
   
    mIfDetectorMap.insert(TIfDetectorMap::value_type(hRetHandle, newHandle));

    return hRetHandle;
}
    
void  IfDetectorManager::DestoryIfDetectoHandle(if_handle_t  hHandle)
{
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    TIfDetectorMap::iterator findDetector = mIfDetectorMap.find(hHandle);

    if (findDetector == mIfDetectorMap.end())
    {
        printf("IfDetectorManager::DestoryBlkTrackerHandle, unexist handle %p\n", hHandle);
    }
    else
    {
        printf("IfDetectorManager::DestoryBlkTrackerHandle, deleted: %p\n", hHandle);

        //findDetector->second->Release();

        mIfDetectorMap.erase(findDetector);
    }
}

if_result_t IfDetectorManager::FeedOneFramToDetector(      if_handle_t      hHandle,
                                                           unsigned char  * pcGrayDat,
                                                     const int              dwWidth,
                                                     const int              dwHeight,                     
                                                           PFACE_RST_TYPE * pFaceRectArray,
                                                           unsigned int   * pdwFaceCnt)
{
    boost::shared_lock<boost::shared_mutex> readLocker(mMapLocker);

    if(pcGrayDat == NULL || pFaceRectArray == NULL || pdwFaceCnt == NULL)
    {
        printf("FeedOneFramToDetector, wrong parameter: %p\n", hHandle);

        return FACE_SNAPER_ERR_NONE;
    }

    TIfDetectorMap::iterator findDetector = mIfDetectorMap.find(hHandle);

    if (findDetector == mIfDetectorMap.end())
    {
        printf("FeedOneFramToDetector, unexist handle: %p \n" ,hHandle);

        pFaceRectArray[0] = NULL;
        pdwFaceCnt[0]     = 0;
    }
    else
    {
        FaceArray tFaceDetected;
    
        findDetector->second->DetectFaces(pcGrayDat,
                                          dwWidth,
                                          dwHeight,
                                          tFaceDetected);

        std::size_t dwFaceDetectCnt = tFaceDetected.size();

        if(dwFaceDetectCnt == 0)
        {
            pFaceRectArray[0] = NULL;
            pdwFaceCnt[0]     = 0;
        }
        else
        {
            int            dwMallocLen = (int)(sizeof(FACE_RST_TYPE) * dwFaceDetectCnt);
            PFACE_RST_TYPE pFaceRect   = (PFACE_RST_TYPE)malloc(dwMallocLen);

            if(pFaceRect == NULL)
            {
                pFaceRectArray[0] = NULL;
                pdwFaceCnt[0]     = 0;
            }
            else
            {
                for(std::size_t dwFaceIdx = 0; dwFaceIdx < dwFaceDetectCnt; dwFaceIdx++)
                {
                    pFaceRect[dwFaceIdx].Rect.left   = tFaceDetected[dwFaceIdx].centerX;
                    pFaceRect[dwFaceIdx].Rect.top    = tFaceDetected[dwFaceIdx].centerY;
                    pFaceRect[dwFaceIdx].Rect.right  = tFaceDetected[dwFaceIdx].radius;
                    pFaceRect[dwFaceIdx].Rect.bottom = tFaceDetected[dwFaceIdx].radius;
                    pFaceRect[dwFaceIdx].Pose        = (IF_FacePose)0;
                    pFaceRect[dwFaceIdx].Confidence  = (float)(tFaceDetected[dwFaceIdx].confidence * 1.0);
                }

                pFaceRectArray[0] = pFaceRect;
                pdwFaceCnt[0]     = (unsigned int)dwFaceDetectCnt;
            }
        }
    }

    return FACE_SNAPER_ERR_NONE;
}

void  IfDetectorManager::ReleaseBlkFaceResult(PIF_FACERECT             pFaceRectArray,
                                              unsigned int             pdwFaceCnt)
{
    if(pFaceRectArray == NULL)
    {
        printf("Try to release null result");
    }
    else
    {
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
		if (g_vecIntDetect[i] == FACE_CASCADE_DET_TYPE_MTCNN)
		{
			if_handle_t hNewVjCreated = EagleMtcnnDetectManager::CreateIfDetectorHandle(0);
			mahCascadeHandle[mdwCascadeNum] = hNewVjCreated;
			madwCascadeType[mdwCascadeNum] = FACE_CASCADE_DET_TYPE_MTCNN;
			mdwCascadeNum += 0x1;
		}
		else if (g_vecIntDetect[i] == FACE_CASCADE_DET_TYPE_VJBOOST)
		{
			if_handle_t hNewVjCreated = IfDetectorManager::CreateIfDetectorHandle(1);
			mahCascadeHandle[mdwCascadeNum] = hNewVjCreated;
			madwCascadeType[mdwCascadeNum] = FACE_CASCADE_DET_TYPE_VJBOOST;
			mdwCascadeNum += 0x1;
		}
		else
		{
#ifdef OLD_SDK_INCLUDED		
			if_handle_t hNewFrontCreated = if_facesdk_create_frontal_detector_instance_from_resource(1);
			madwCascadeType[mdwCascadeNum] = FACE_CASCADE_DET_TYPE_ORG;
			mahCascadeHandle[mdwCascadeNum] = hNewFrontCreated;
			mdwCascadeNum += 0x1;
#endif
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

		if (madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_ORG)
        {
            #ifdef OLD_SDK_INCLUDED
            if_facesdk_destroy_frontal_instance(mahCascadeHandle[dwDetectorIdx]);
            #else
            std::cout << "try to release oldsdk handle,but is not enabled" << std::endl;
            #endif
        }
		else if (madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_VJBOOST)
        {
            IfDetectorManager::DestoryIfDetectoHandle(mahCascadeHandle[dwDetectorIdx]);
        }
		else if (madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_MTCNN)
		{
			EagleMtcnnDetectManager::DestoryIfDetectoHandle(mahCascadeHandle[dwDetectorIdx]);
		}
        else if(madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_INVALID)
        {
            std::cout << "Cascade, release wrong type detector: "     << mSelfHandle
                      << ", type: " << madwCascadeType[dwDetectorIdx] << std::endl;
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

		if (madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_ORG)
        {
#ifdef OLD_SDK_INCLUDED
			cvtColor(matLoad, matGray, CV_BGR2GRAY);
            dwRet = if_facesdk_frontal_detector(mahCascadeHandle[dwDetectorIdx],
										matGray.data,
                                        dwWidth,
                                        dwHeight,
                                        dwWidth,
                                        pFaceRectArray,
                                        pdwFaceCnt);
#endif
        }
		else if (madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_VJBOOST)
        {
			cvtColor(matLoad, matGray, CV_BGR2GRAY);
            dwRet = IfDetectorManager::FeedOneFramToDetector(mahCascadeHandle[dwDetectorIdx],
												             matGray.data,
                                                             dwWidth,
                                                             dwHeight,
                                                             pFaceRectArray,
                                                             pdwFaceCnt);

            if(  (*pFaceRectArray != NULL)
               &&(*pdwFaceCnt     >  0))
            {
                PFACE_RST_TYPE pFaceRect = *pFaceRectArray;
                unsigned  int  dwFaceCnt = *pdwFaceCnt;

                for(unsigned int dwRectIdx = 0; dwRectIdx < dwFaceCnt; dwRectIdx++)
                {
                    cycleRect2BoderRect(pFaceRect[dwRectIdx].Rect);
                }
            }
        }
		else if (madwCascadeType[dwDetectorIdx] == FACE_CASCADE_DET_TYPE_MTCNN)
		{
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

void  CascadeDetector::CascadeResultRelease(int            dwRectType,
                                            PIF_FACERECT   pFaceRectArray,
                                            unsigned int   pdwFaceCnt)
{
    return;
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
	//std::cout << "CascadeDetectManager::DestoryIfDetectoHandle start: " << hHandle << std::endl;
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
														const Mat &       matLoad,
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

    if(dwRectType == CascadeDetector::FACE_CASCADE_DET_TYPE_ORG)
    {
#ifdef OLD_SDK_INCLUDED
        if_facesdk_release_frontal_result(pFaceRectArray,pdwFaceCnt);
#else
        std::cout << "old sdk not enabled, may be some wrong happened" << std::endl;
#endif
    }
	else if (dwRectType == CascadeDetector::FACE_CASCADE_DET_TYPE_VJBOOST)
    {
        IfDetectorManager::ReleaseBlkFaceResult(pFaceRectArray, pdwFaceCnt);
    }
	else if (dwRectType == CascadeDetector::FACE_CASCADE_DET_TYPE_MTCNN)
	{
		EagleMtcnnDetectManager::ReleaseDetFaceResult(pFaceRectArray, pdwFaceCnt);
	}
    else
    {
        std::cout << "Cascade, release wrong type detect result, type: " << dwRectType << std::endl;
    }   
}

if_handle_t  IfFeatureExtManager::CreateIfExtratorHandle(int  dwThreadCnt)
{
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    mHandleExtractSeq      = mHandleExtractSeq + 0xa;

    if_handle_t hRetHandle = (if_handle_t)mHandleExtractSeq;

#ifdef _MSC_VER
	string strModelPath = g_strAlgModelPath + "\\tuna17";
#else
	string strModelPath = g_strAlgModelPath + "/tuna17";
#endif

	SP_FeatureExter newHandle(new Tuna17FeaturesContainer(strModelPath,
													-1,
													1));

    int dwInitRet = newHandle->Init();

    if(dwInitRet == 0)
    {
        printf("CreateIfExtratorHandle, load model failed: %p \n", hRetHandle);
    }
   
    mIfExtratorMap.insert(TIfExtratorMap::value_type(hRetHandle, newHandle));

    return hRetHandle;
}

void  IfFeatureExtManager::DestoryIfExtratorHandle(if_handle_t  hHandle)
{
	//cout << "IfFeatureExtManager::DestoryIfExtratorHandle start$##" << std::endl;
    boost::unique_lock<boost::shared_mutex> writeLocker(mMapLocker);

    TIfExtratorMap::iterator findExtrator = mIfExtratorMap.find(hHandle);

    if (findExtrator == mIfExtratorMap.end())
    {
        printf("IfFeatureExtManager::DestoryIfExtratorHandle, unexist handle: %p \n", hHandle);
    }
    else
    {
        printf("IfFeatureExtManager::DestoryIfExtratorHandle, deleted: %p \n", hHandle);

        //findExtrator->second->Release();

        mIfExtratorMap.erase(findExtrator);
    }
}



int  IfFeatureExtManager::ExtratFeatureFromImage(if_handle_t      hHandle,
                                                 cv::Mat      &   rMatGray,                     
                                                 if_rect_t    &   rFaceRect,
                                                 float        **  pFeature)
{
    boost::shared_lock<boost::shared_mutex> readLocker(mMapLocker);

    int dwFeatureLen = 0;

    if(rMatGray.data == NULL || pFeature == NULL)
    {
        printf("ExtratFeatureFromImage, wrong parameter: %p \n", hHandle);

        return dwFeatureLen;
    }

    if(rMatGray.channels() > 1)
    {
        printf("ExtratFeatureFromImage, Mat channel missMatch: %d \n", rMatGray.channels());

        pFeature[0] = NULL;
        
        return dwFeatureLen;
    }

    TIfExtratorMap::iterator findExtrator = mIfExtratorMap.find(hHandle);

    if (findExtrator == mIfExtratorMap.end())
    {
        printf("ExtratFeatureFromImage, unexist handle: %p \n", hHandle);

        pFeature[0]     = NULL;
    }
    else
    {
        cv::Mat  matFeature;
        //cv::Rect rectFace(rFaceRect.left, rFaceRect.top, rFaceRect.right, rFaceRect.bottom);

		int dwRectWidth = rFaceRect.right - rFaceRect.left;
		int dwRectHeight = rFaceRect.bottom - rFaceRect.top;

		int      dwRadius = (dwRectWidth > dwRectHeight) ? (dwRectWidth / 2) : (dwRectHeight / 2);
		cv::Rect rectFace = cv::Rect(rFaceRect.left + dwRectWidth / 2, rFaceRect.top + dwRectHeight / 2, dwRadius, dwRadius);
    
        findExtrator->second->ComputingDescriptor(rMatGray,
                                                  rectFace,
                                                  matFeature);

        dwFeatureLen = matFeature.cols;

        if(dwFeatureLen == 0 || matFeature.data == NULL)
        {
            pFeature[0] = NULL;
        }
        else
        {
            int     dwMallocLen  = (int)(sizeof(float) * matFeature.cols);
            float * pFaceFeature = (float *)malloc(dwMallocLen);

            if(pFaceFeature == NULL)
            {
                pFeature[0] = NULL;
            }
            else
            {
                float * pFeatureSrc = (float *)matFeature.data;
            
                for(int dwFeatureIdx = 0; dwFeatureIdx < dwFeatureLen; dwFeatureIdx++)
                {
                    pFaceFeature[dwFeatureIdx] = pFeatureSrc[dwFeatureIdx];
                }

                pFeature[0] = pFaceFeature;
            }
        }
    }

    return dwFeatureLen;
}

void  IfFeatureExtManager::ReleaseFeautreResult(float        *  pFeature)
{
    if(pFeature == NULL)
    {
        printf("Try to release null result");
    }
    else
    {
        free(pFeature);

        pFeature = NULL;
    }
}

void  IfFeatureExtManager::IfFeatureExtMangerInit(void)
{
	//FeaturesContainer::LibStaticInit();
	Tuna17FeaturesContainer::LibStaticInit();
}

FaceExtractor::FaceExtractor()
{
    for (int dwAlgIdx = 0; dwAlgIdx < ALG_NUM_MAX; dwAlgIdx++)
    {
        pFaceAlgExtraType[dwAlgIdx] = ALG_EXTRA_TYPE_UNKNOW;
        pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
    }
	
    dwAlgNums = 0;   

    if(g_mdwAlgVersion == ALG_EXTRA_TYPE_ORG)
    {
        #ifdef OLD_SDK_INCLUDED
        //org
        pFaceAlgExtraType[dwAlgNums]  = ALG_EXTRA_TYPE_ORG;
        pFaceAlgExtraHandl[dwAlgNums] = if_create_verify_instance_with_prefix(
                                        g_strAlgModelPath.c_str());
        dwAlgNums += 0x1;    
        //org flip
        //pFaceAlgExtraType[dwAlgNums]  = ALG_EXTRA_TYPE_ORG_FLIP;
        //pFaceAlgExtraHandl[dwAlgNums] = IfMultiFeatureExtManager::CreateIfExtratorHandle(
        //                                IFaceConfiguration::msIfaceData.c_str());
        
        //dwAlgNums += 0x1;
        #endif
    }
	
	if (g_mdwAlgVersion == ALG_EXTRA_TYPE_TUAN || g_mdwAlgVersion == ALG_EXTRA_TYPE_DEEPEYE)
    {
        //ALG_EXTRA_TYPE_TUAN
        pFaceAlgExtraType[dwAlgNums]  = ALG_EXTRA_TYPE_DEEPEYE;
        pFaceAlgExtraHandl[dwAlgNums] = IfFeatureExtManager::CreateIfExtratorHandle(1);
        dwAlgNums += 0x1;

		pFaceAlgExtraType[dwAlgNums] = ALG_EXTRA_TYPE_TUAN;
		pFaceAlgExtraHandl[dwAlgNums] = IfFeatureExtManager::CreateIfExtratorHandle(1);
		dwAlgNums += 0x1;
    }
	else if (g_mdwAlgVersion == ALG_EXTRA_TYPE_GLUON)
	{
		//ALG_EXTRA_TYPE_GLUON
		pFaceAlgExtraType[dwAlgNums] = ALG_EXTRA_TYPE_GLUON;
		pFaceAlgExtraHandl[dwAlgNums] = GluonFeatureExtManager::CreateIfExtratorHandle(1);
		dwAlgNums += 0x1;
	}
	else if (g_mdwAlgVersion == ALG_EXTRA_TYPE_FUSION)
	{
		//ALG_EXTRA_TYPE_GLUON
		pFaceAlgExtraType[dwAlgNums] = ALG_EXTRA_TYPE_FUSION;
		pFaceAlgExtraHandl[dwAlgNums] = FusionFeatureExtManager::CreateIfExtratorHandle(1);
		dwAlgNums += 0x1;
	}
}

FaceExtractor::~FaceExtractor()
{
	//cout << "FaceExtractor::~FaceExtractor#####" << std::endl;
    for (int dwAlgIdx = 0; dwAlgIdx < dwAlgNums; dwAlgIdx++)
    {
        if (pFaceAlgExtraHandl[dwAlgIdx] == INVALID_HANDL)
        {        
            continue;
        }

        if (pFaceAlgExtraType[dwAlgIdx] == ALG_EXTRA_TYPE_ORG)
        {
            #ifdef OLD_SDK_INCLUDED
            if_verify_release_instance(pFaceAlgExtraHandl[dwAlgIdx]);
            pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
            #endif
        }
		else if ((pFaceAlgExtraType[dwAlgIdx] == ALG_EXTRA_TYPE_DEEPEYE) || (pFaceAlgExtraType[dwAlgIdx] == ALG_EXTRA_TYPE_TUAN))
        {
            IfFeatureExtManager::DestoryIfExtratorHandle(pFaceAlgExtraHandl[dwAlgIdx]);
            pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
        }  
		else if (pFaceAlgExtraType[dwAlgIdx] == ALG_EXTRA_TYPE_GLUON)
		{
			cout << "FaceExtractor::~FaceExtractor=ALG_EXTRA_TYPE_GLUON=ALG_EXTRA_TYPE_GLUON#####" << std::endl;
			GluonFeatureExtManager::DestoryIfExtratorHandle(pFaceAlgExtraHandl[dwAlgIdx]);
			pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
		}
		else if (pFaceAlgExtraType[dwAlgIdx] == ALG_EXTRA_TYPE_FUSION)
		{
			cout << "FaceExtractor::~FaceExtractor=ALG_EXTRA_TYPE_FUSION=ALG_EXTRA_TYPE_FUSION#####" << std::endl;
			FusionFeatureExtManager::DestoryIfExtratorHandle(pFaceAlgExtraHandl[dwAlgIdx]);
			pFaceAlgExtraHandl[dwAlgIdx] = INVALID_HANDL;
		}
    }
	//cout << "FaceExtractor::~FaceExtractor end...#####" << std::endl;
}

int  FaceExtractor::ExtratFeatureFromImg(
                                                cv::Mat      &      rMatOrg,
                                                if_rect_t    &      pFaceRect,
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
		//printf("pFaceAlgExtraType:%d,pFaceAlgExtraHandl:%p\n", pFaceAlgExtraType[dwAlgIdx], pFaceAlgExtraHandl[dwAlgIdx]);
        if ((pFaceAlgExtraType[dwAlgIdx] == dwAlgType) && 
            (pFaceAlgExtraHandl[dwAlgIdx] != INVALID_HANDL))
        {             
            float* pRetFeatureTmp = NULL;
            int    dwRetLen = 0;
            if (dwAlgType == ALG_EXTRA_TYPE_ORG)
            {       
                #ifdef OLD_SDK_INCLUDED         
                Mat  colorMat;                
                cvtColor(rMatOrg, colorMat, CV_BGR2BGRA);
                dwRetLen = if_extract_pca_feature(pFaceAlgExtraHandl[dwAlgIdx],
                                                        colorMat.data,
                                                        colorMat.cols,
                                                        colorMat.rows,
                                                        pFaceRect,
                                                        &pRetFeatureTmp
                                                        );
                if ((dwRetLen != ALG_ORG_FEATURE_LEN) || (pRetFeatureTmp == NULL))
                {
                    printf("FaceExtractor, ExtratFeatureFromImg, Failed, algType: %d\n" , dwAlgType);
                    
                    if (pRetFeatureTmp != NULL)
                    {
                        if_verify_release_feature(pRetFeatureTmp);
                        pRetFeatureTmp = NULL;
                    }

                    dwRetLen = 0;
                }

                pFeatureRet = pRetFeatureTmp;
                dwFeatureLen = dwRetLen;
                #endif
            }
			else if (((dwAlgType == ALG_EXTRA_TYPE_DEEPEYE) || (dwAlgType == ALG_EXTRA_TYPE_TUAN)) &&
                     (pFaceAlgExtraHandl[dwAlgIdx] != INVALID_HANDL))
            {                
                Mat  grayMat;
                cvtColor(rMatOrg, grayMat, CV_BGR2GRAY);
                dwRetLen = IfFeatureExtManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx],
                                                                    grayMat,
                                                                    pFaceRect,
                                                                    &pRetFeatureTmp);
                if ((dwRetLen != ALG_TUNA_FEATURE_LEN) || (pRetFeatureTmp == NULL))
                {
                    printf("FaceExtractor, ExtratFeatureFromImg, Failed, algType: %d\n" , dwAlgType);

                    if (pRetFeatureTmp != NULL)
                    {
                        IfFeatureExtManager::ReleaseFeautreResult(pRetFeatureTmp);
                        pRetFeatureTmp = NULL;
                    }                    
                    dwRetLen = 0;                    
                }

                pFeatureRet = pRetFeatureTmp;
                dwFeatureLen = dwRetLen;   
            }
			else if (dwAlgType == ALG_EXTRA_TYPE_GLUON)
			{
				dwRetLen = GluonFeatureExtManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx],
																rMatOrg,
																pFaceRect,
																&pRetFeatureTmp);
				if ((dwRetLen != ALG_GLUON_FEATURE_LEN) || (pRetFeatureTmp == NULL))
				{
					printf("FaceExtractor, ExtratFeatureFromImg, Failed, algType: %d\n", dwAlgType);

					if (pRetFeatureTmp != NULL)
					{
						GluonFeatureExtManager::ReleaseFeautreResult(pRetFeatureTmp);
						pRetFeatureTmp = NULL;
					}
					dwRetLen = 0;
				}

				pFeatureRet = pRetFeatureTmp;
				dwFeatureLen = dwRetLen;
				//printf("FaceExtractor, ExtratFeatureFromImg, dwAlgType: %d, feature len:%d\n ", dwAlgType, dwRetLen);
			}
			else if (dwAlgType == ALG_EXTRA_TYPE_FUSION)
			{
				dwRetLen = FusionFeatureExtManager::ExtratFeatureFromImage(pFaceAlgExtraHandl[dwAlgIdx],
					rMatOrg,
					pFaceRect,
					&pRetFeatureTmp);
				if ((dwRetLen != ALG_FUSION_FEATURE_LEN) || (pRetFeatureTmp == NULL))
				{
					printf("FaceExtractor, ExtratFeatureFromImg, Failed, algType: %d\n", dwAlgType);

					if (pRetFeatureTmp != NULL)
					{
						FusionFeatureExtManager::ReleaseFeautreResult(pRetFeatureTmp);
						pRetFeatureTmp = NULL;
					}
					dwRetLen = 0;
				}

				pFeatureRet = pRetFeatureTmp;
				dwFeatureLen = dwRetLen;
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

void   FaceExtractor::dump()
{
    
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
	cout << "MutilAlgExtractor::~MutilAlgExtractor start::#"<< std::endl;
    for (int dwTargsIdx = 0; dwTargsIdx < mdwTargsNum; dwTargsIdx++)
    {
        mTargsExtraHandl[dwTargsIdx].reset();
    }
	//cout << "MutilAlgExtractor::~MutilAlgExtractor end::#" << std::endl;
}

int  MutilAlgExtractor::ExtratFeatureFromImage(cv::Mat   &         rMatGray,
                                               if_rect_t &         pFaceRect,
                                               float    **         pFeature,
                                               int       *         pFeatureLen,
                                               int                 dwTargType,
                                               int                 dwAlgType,
                                               TFeatureSeqIdInfo & rSeqIdInfo,
                                               int                 dwSwReqMode)
{
	//cout << "MutilAlgExtractor::ExtratFeatureFromImage start.mdwTargsNum=#####" << mdwTargsNum << "::dwTargType=" << dwTargType << std::endl;
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
			//cout << "MutilAlgExtractor::ExtratFeatureFromImage::ExtratFeatureFromImg##" << std::endl;
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
	//cout << "IfMutilAlgExtManger::DestoryIfExtratorHandle start##" << std::endl;
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
                                                 if_rect_t   &       pFaceRect,
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
        findExtarator->second->ExtratFeatureFromImage(rMatGray,
                                                      pFaceRect,
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
	//cout << "IfMutilAlgExtManger::ReleaseFeautreResult start::##" << std::endl;
    if (dwAlgType == ALG_EXTRA_TYPE_ORG)
    {
        #ifdef OLD_SDK_INCLUDED
        if_verify_release_feature(pFeature);
        #endif
    }
	else if ((dwAlgType == ALG_EXTRA_TYPE_DEEPEYE) || (dwAlgType == ALG_EXTRA_TYPE_TUAN))
    {
        IfFeatureExtManager::ReleaseFeautreResult(pFeature);
    }
	else if (dwAlgType == ALG_EXTRA_TYPE_GLUON)
	{
		//std::cout << "GluonFeatureExtManager::ReleaseFeautreResult" << std::endl;
		GluonFeatureExtManager::ReleaseFeautreResult(pFeature);
		//std::cout << "GluonFeatureExtManager::ReleaseFeautreResult end" << std::endl;
	}
	else if (dwAlgType == ALG_EXTRA_TYPE_FUSION)
	{
		FusionFeatureExtManager::ReleaseFeautreResult(pFeature);
	}
}

void    IfMutilAlgExtManger::dump()
{
    
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

    if (dwAlgType == ALG_EXTRA_TYPE_ORG)
    {
        AntFeatureCompare(pFeature0, dwLen0, pFeature1, dwLen1, score);
    }
	else if ((dwAlgType == ALG_EXTRA_TYPE_DEEPEYE) || (dwAlgType == ALG_EXTRA_TYPE_TUAN))
    {
        TunaFeatureCompare(pFeature0, dwLen0, pFeature1, dwLen1, score);
    }
	else if (dwAlgType == ALG_EXTRA_TYPE_GLUON)
	{
		GluonFeatureCompare(pFeature0, dwLen0, pFeature1, dwLen1, score);
	}
	else if (dwAlgType == ALG_EXTRA_TYPE_FUSION)
	{
		FusionFeatureCompare(pFeature0, dwLen0, pFeature1, dwLen1, score);
	}
    else if (dwAlgType == ALG_EXTRA_TYPE_RABBIT)
    {
        *score = (float)0;
        printf("IfFeatureCompare, don't support alg: %d\n" , dwAlgType);
    }
    else if (dwAlgType == ALG_EXTRA_TYPE_TORTOISE)
    {
        *score = (float)0;
        printf("IfFeatureCompare, don't support alg: %d\n" , dwAlgType);
    }
    else if (dwAlgType == ALG_EXTRA_TYPE_ORG_FLIP)
    {
		*score = (float)0;
		printf("IfFeatureCompare, don't support alg: %d\n", dwAlgType);
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

int IfMutilAlgCompare::FusionFeatureCompare(const float *   pfFeature0,
	int       dwFeatureLen0,
	const float *   pfFeature1,
	int       dwFeatureLen1,
	float *   pfScore)
{

	float fCmpScore = (float)0.0;

	int dwMinLen = dwFeatureLen0 > dwFeatureLen1 ? dwFeatureLen1 : dwFeatureLen0;

	//if(dwFeatureLen0 != dwFeatureLen1)
	//{
	//    *pfScore = (float)0.0;
	//    
	//    return 0;
	//}

	double   dotMulCross = 0.0;
	//double dotFeature0 = 0.0;
	//double dotFeature1 = 0.0;

	for (int dwFeatureIdx = 0; dwFeatureIdx < dwMinLen; dwFeatureIdx++)
	{
		dotMulCross += (pfFeature0[dwFeatureIdx] * pfFeature1[dwFeatureIdx]);
		//dotFeature0 += (pfFeature0[dwFeatureIdx] * pfFeature0[dwFeatureIdx]);
		//dotFeature1 += (pfFeature1[dwFeatureIdx] * pfFeature1[dwFeatureIdx]);
	}

	//if (dotFeature0 == 0 || dotFeature1 == 0)
	//{
	//    *pfScore = (float)0.0;
	//
	//    return 0;
	//}

	//double squreDot0 = sqrt(dotFeature0);
	//double squreDot1 = sqrt(dotFeature1);

	float  fResult = (float)(dotMulCross / 2);

	*pfScore = fResult;

	return 0;
}

int IfMutilAlgCompare::AntFeatureCompare(const float *   pfFeature0, 
                      int       dwFeatureLen0,
                      const float *   pfFeature1,
                      int       dwFeatureLen1,
                      float *   pfScore)
{
    float fCmpScore  = (float)0.0;

    if(dwFeatureLen0 != dwFeatureLen1)
    {
        *pfScore = (float)0.0;
        return 0;
    }
    
    for(int floatIdx = 0; floatIdx < dwFeatureLen0; floatIdx++)
    {
        fCmpScore = fCmpScore
                  + ( pfFeature0[floatIdx] * pfFeature0[floatIdx])
                  + ( pfFeature1[floatIdx] * pfFeature1[floatIdx])
                  - ((pfFeature0[floatIdx] * pfFeature1[floatIdx]) * 4 );
    }
    
    float fExpTemp = (float) (1 / (1 + exp(fCmpScore)));

    #if 1
    if(fExpTemp <= 0.5) 
    {
        fCmpScore = (float)((3.3342 * fExpTemp) - 0.7671);
    } else {
        fCmpScore = (float)((0.2    * fExpTemp) + 0.8);
    }
    

    *pfScore = fCmpScore;
    #else
    *pfScore = fExpTemp;
    #endif
    
    return 0;
}


int IfMutilAlgCompare::TunaFeatureCompareConv(const float *   pfFeature0, 
                                 int       dwFeatureLen0,
                           const float *   pfFeature1,
                                 int       dwFeatureLen1,
                                 float *   pfScore)
{
    float fCmpScore  = (float)0.0;

    int dwMinLen = dwFeatureLen0 > dwFeatureLen1 ? dwFeatureLen1 : dwFeatureLen0;

    //if(dwFeatureLen0 != dwFeatureLen1)
    //{
    //    *pfScore = (float)0.0;
    //    
    //    return 0;
    //}

    double dotMulCross = 0.0;
    double dotFeature0 = 0.0;
    double dotFeature1 = 0.0;

    for (int dwFeatureIdx = 0; dwFeatureIdx < dwMinLen; dwFeatureIdx++)
    {
        dotMulCross += (pfFeature0[dwFeatureIdx] * pfFeature1[dwFeatureIdx]);
        dotFeature0 += (pfFeature0[dwFeatureIdx] * pfFeature0[dwFeatureIdx]);
        dotFeature1 += (pfFeature1[dwFeatureIdx] * pfFeature1[dwFeatureIdx]);
    }

    if (dotFeature0 == 0 || dotFeature1 == 0)
    {
        *pfScore = (float)0.0;

        return 0;
    }

    double squreDot0 = sqrt(dotFeature0);
    double squreDot1 = sqrt(dotFeature1);

    float  fResult   = (float)(dotMulCross / (squreDot0 * squreDot1));

    //if(  (g_mfVerifyScoreThr > (float)0.0)
    //   &&(g_mfVerifyScoreThr < (float)1.0))
    //{
    //    if(fResult <= g_mfVerifyScoreThr)
    //    {
    //        fResult   = ((float)0.92) * fResult / g_mfVerifyScoreThr;
    //    }
    //    else
    //    {
    //        float sfDivX = ((float)1.0) - g_mfVerifyScoreThr;
    //        float sfDivY = (float)0.08;
    //        
    //        fResult  = ((float)0.92) 
    //                 + ((sfDivY * (fResult - g_mfVerifyScoreThr))  / sfDivX);
    //    }
    //}
    
    *pfScore = (float)fResult;

    return 0;
}


int IfMutilAlgCompare::TunaFeatureCompareSvm(const float *   pfFeature0, 
                          int             dwFeatureLen0,
                          const float *   pfFeature1,
                          int             dwFeatureLen1,
                          float *         pfScore)
{
    float fCmpScore  = (float)0.0;

    if(g_pCompSvmHandl == NULL)
    {
        *pfScore = fCmpScore;
    
        printf("TunaFeatureCompareSvm, Svm Model have not been inited: ");

        return 0;
    }

    svm_node * pArraySvmNode = (svm_node *)malloc(sizeof(svm_node)* (dwFeatureLen0 + 1));

    if(pArraySvmNode == NULL)
    {
        *pfScore = fCmpScore;
    
        printf("TunaFeatureCompareSvm, Svm node malloc failed: ");

        return 0;
    }

    #if 0
    double dfNormaParam0 = 0.0;
    double dfNormaParam1 = 0.0;

    for (int dwFeatureIdx = 0; dwFeatureIdx < dwFeatureLen0; dwFeatureIdx++)
    {
        dfNormaParam0 += pfFeature0[dwFeatureIdx] * pfFeature0[dwFeatureIdx];
        dfNormaParam1 += pfFeature1[dwFeatureIdx] * pfFeature1[dwFeatureIdx];
    }

    dfNormaParam0 = sqrt(dfNormaParam0);
    dfNormaParam1 = sqrt(dfNormaParam1);

    for(int dwSvmNodeIdx = 0; dwSvmNodeIdx < dwFeatureLen0; dwSvmNodeIdx++)
    {
        pArraySvmNode[dwSvmNodeIdx].index = dwSvmNodeIdx + 1;
        pArraySvmNode[dwSvmNodeIdx].value = (pfFeature0[dwSvmNodeIdx] / dfNormaParam0)
                                          - (pfFeature1[dwSvmNodeIdx] / dfNormaParam1);
    }
    #else
    Mat  rMatFeature0(1, dwFeatureLen0, CV_32FC1, const_cast<float*>(pfFeature0));
    Mat  rMatFeature1(1, dwFeatureLen1, CV_32FC1, const_cast<float*>(pfFeature1));

    cv::Mat rMatDiff = (rMatFeature0 / cv::norm(rMatFeature0))
                     - (rMatFeature1 / cv::norm(rMatFeature1));

    float * pFeatureDiff = (float *)rMatDiff.data;

    for (int dwSvmNodeIdx = 0; dwSvmNodeIdx < dwFeatureLen0; dwSvmNodeIdx++)
    {
        pArraySvmNode[dwSvmNodeIdx].index = dwSvmNodeIdx + 1;
        pArraySvmNode[dwSvmNodeIdx].value = pFeatureDiff[dwSvmNodeIdx];
    }
    #endif

    pArraySvmNode[dwFeatureLen0].index = -1;
    pArraySvmNode[dwFeatureLen0].value = 0;

    double dfCompareLabel = 0.0;
    double dfCompareProbility[2];

    if(g_dwCompSvmPredictMode == COMP_SVM_PREDIECT_MODE_PROBILITY)
    {
        dfCompareLabel = svm_predict_probability(g_pCompSvmHandl, pArraySvmNode, dfCompareProbility);
        
        if ((int)dfCompareLabel == 1)
        {
            *pfScore = (float)dfCompareProbility[1];
        }
        else
        {
            *pfScore = (float)dfCompareProbility[0];
        }
    }
    else
    {
        dfCompareLabel = svm_predict_values(g_pCompSvmHandl, pArraySvmNode, dfCompareProbility);
        
        float dwReturnVal = ((float)dfCompareProbility[0] + (float)10.0)
                          / ((float)20.0);
        *pfScore = (float)dfCompareProbility[0];


    }

    free(pArraySvmNode);
    
    return 0;
}

int IfMutilAlgCompare::TunaFeatureCompare(const float *   pfFeature0, 
                                          int       dwFeatureLen0,
                                          const float *   pfFeature1,
                                          int       dwFeatureLen1,
                                          float *   pfScore)
{
    if( pfScore == NULL )
    {        
        return 0;
    }

    float fCmpScore  = (float)0.0;

    if( (pfFeature0 == 0) || (pfFeature1 == 0) /*|| (dwFeatureLen0 != dwFeatureLen1)*/)
    {
        *pfScore = fCmpScore;
        return 0;
    }

    int dwRet = 0;

    if( g_mdwIfCompFuncSel == COMPARE_FUNC_TYPE_CONV )
    {
        dwRet = TunaFeatureCompareConv(pfFeature0, 
                                     dwFeatureLen0,
                                     pfFeature1,
                                     dwFeatureLen1,
                                     pfScore);
    }
    else if( g_mdwIfCompFuncSel == COMPARE_FUNC_TYPE_SVM )
    {
        dwRet = TunaFeatureCompareSvm( pfFeature0, 
                                     dwFeatureLen0,
                                     pfFeature1,
                                     dwFeatureLen1,
                                     pfScore);
    }
    else
    {
        printf("TunaFeatureCompare, wrong function type: %d\n" , g_mdwIfCompFuncSel);
    }
    
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


