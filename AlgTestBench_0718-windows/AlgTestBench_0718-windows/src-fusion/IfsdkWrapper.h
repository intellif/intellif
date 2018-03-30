/*
 * @file    IfsdkWrapper.h
 * @date    2016-07-07
 * @brief   define the micoros which may different between different os/toolchains, but use all by c/cpp files
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#ifndef __INTELLIF_FACESDK_WRAPPER_H__
#define __INTELLIF_FACESDK_WRAPPER_H__

#include "TestCaseCommon.h"

#include "FaceDetector.h"
//#include "FeaturesContainer.h"

#ifdef OLD_SDK_INCLUDED

#ifdef _MSC_VER
#include "IFaceSDK.h"
#else
#include "ifacesdk.h"
#endif

#endif

#include "boost/smart_ptr.hpp"
#include "boost/thread/thread.hpp"
#include "boost/unordered_map.hpp"

#include "mtcnn_handle.h"
#include "gluon_handle.h"
#include "Tuna17FeaturesContainer.h"

#define  ALG_NUM_MAX    (0x8)
#define  TARGS_NUM_MAX  (0x8)
#define  INVALID_HANDL  ((void *)-1)


typedef enum {
    ALG_EXTRA_TYPE_UNKNOW   = -1,
    ALG_EXTRA_TYPE_ORG      = 0x0,
    ALG_EXTRA_TYPE_TUAN     = 0x1,
    ALG_EXTRA_TYPE_RABBIT   = 0x2,
    ALG_EXTRA_TYPE_TORTOISE = 0x3,
	ALG_EXTRA_TYPE_GLUON    = 0x4,
    ALG_EXTRA_TYPE_DEEPEYE  = 0x5,
	ALG_EXTRA_TYPE_ORG_FLIP = 0x6,
	ALG_EXTRA_TYPE_FUSION   = 0x7,
} EAlgExtraType;


typedef struct {

    int64_t dwSwSequence;
    int64_t dwTrackId;

} TFeatureSeqIdInfo;

class FustionFeatureExter;

typedef boost::shared_ptr<MtcnnMxnetHandle>       SP_MtcnnMxnetHandle;
typedef boost::shared_ptr<GluonMxnetHandle>       SP_GluonMxnetHandle;
typedef boost::shared_ptr<Tuna17FeaturesContainer> SP_FeatureExter;
typedef boost::shared_ptr<FustionFeatureExter>    SP_FustionFeatureExter;

typedef boost::unordered_map<if_handle_t, SP_MtcnnMxnetHandle> TMtcnnDetectorMap;
typedef boost::unordered_map<if_handle_t, SP_GluonMxnetHandle> TGluonExtratorMap;
typedef boost::unordered_map<if_handle_t, SP_FustionFeatureExter> TFusionExtratorMap;

typedef boost::shared_ptr<FaceDetector>                    SP_FaceDetector;

typedef boost::unordered_map<if_handle_t, SP_FaceDetector> TIfDetectorMap;
typedef boost::unordered_map<if_handle_t, SP_FeatureExter> TIfExtratorMap;

class EagleMtcnnDetectManager {

public:
	static void        StaticInit(void);

	static if_handle_t CreateIfDetectorHandle(int   dwThreadCnt);

	static void        DestoryIfDetectoHandle(if_handle_t  hHandle);

	static if_result_t FeedOneFramToDetector(if_handle_t       hHandle,
		const Mat &       matRgbDat,
		const int         dwWidth,
		const int         dwHeight,
		PFACE_RST_TYPE  * pFaceRectArray,
		unsigned int    * pdwFaceCnt);

	static void        ReleaseDetFaceResult(PIF_FACERECT             pFaceRectArray,
		unsigned int             pdwFaceCnt);
private:

	static   boost::shared_mutex   mMtcnnMapLocker;
	static   TMtcnnDetectorMap     mMtcnnDetectMap;

	static   int                   mMtcnnHandleSeq;
};


class GluonFeatureExtManager {

public:

	static if_handle_t CreateIfExtratorHandle(int           dwThreadCnt);

	static void        DestoryIfExtratorHandle(if_handle_t  hHandle);

	static int         ExtratFeatureFromImage(if_handle_t      hHandle,
		cv::Mat      &   rMatRgb,
		if_rect_t    &   pFaceRect,
		float        **  pFeature);

	static void        ReleaseFeautreResult(float        *  pFeature);

	static void        IfFeatureExtMangerInit(void);

private:

	static   boost::shared_mutex   mGluonMapLocker;
	static   TGluonExtratorMap     mGluonExtratorMap;

	static   int                   mGluonExtractSeq;

};

class FustionFeatureExter {

public:

	FustionFeatureExter(if_handle_t hHandleVal, int dwThreadCnt);

	~FustionFeatureExter();

	int ExtratFeatureFromImage(cv::Mat      &   rMatRgb,
		if_rect_t    &   pFaceRect,
		float        **  pFeature);
private:

	if_handle_t  mhSelfHandle;
	if_handle_t  mhTunaHandle;
	if_handle_t  mhGluonHandle;

};

class FusionFeatureExtManager {

public:

	static if_handle_t CreateIfExtratorHandle(int           dwThreadCnt);

	static void        DestoryIfExtratorHandle(if_handle_t  hHandle);

	static int         ExtratFeatureFromImage(if_handle_t      hHandle,
		cv::Mat      &   rMatRgb,
		if_rect_t    &   pFaceRect,
		float        **  pFeature);

	static void        ReleaseFeautreResult(float        *  pFeature);

	static void        IfFeatureExtMangerInit(void);

private:

	static   boost::shared_mutex   mFusionMapLocker;
	static   TFusionExtratorMap    mFusionExtratorMap;

	static   int                   mFusionExtractSeq;

};

class IfDetectorManager {

public:

    typedef enum {

        IF_PARAMETER_MODE_FALSE = 0x0,
        IF_PARAMETER_MODE_TRUE  = 0x1,

    } EIfDetectorMode;

    
    static if_handle_t CreateIfDetectorHandle(int   dwThreadCnt);
    
    static void        DestoryIfDetectoHandle(if_handle_t  hHandle);

    static if_result_t FeedOneFramToDetector(if_handle_t       hHandle,
                                             unsigned char   * pcGrayDat,
                                             const int         dwWidth,
                                             const int         dwHeight,                     
                                             PFACE_RST_TYPE  * pFaceRectArray,
                                             unsigned int    * pdwFaceCnt);

    static void        ReleaseBlkFaceResult(PIF_FACERECT             pFaceRectArray,
                                            unsigned int             pdwFaceCnt);

private:

    static   boost::shared_mutex   mMapLocker;
    static   TIfDetectorMap        mIfDetectorMap;

    static   int                   mHandleDetectSeq;

}; 
//#endif

#if FACE_EXTRAT_FUNCTION_TYPE == 1

//class IfFeatureExtManager {
//
//public:
//    
//    static if_handle_t CreateIfExtratorHandle(int           dwThreadCnt);
//    
//    static void        DestoryIfExtratorHandle(if_handle_t  hHandle);
//
//    static int         ExtratFeatureFromImage(  if_handle_t      hHandle,
//                                                cv::Mat      &   rMatGray,                     
//                                                if_rect_t    &   pFaceRect,
//                                                float        **  pFeature);
//
//    static void        ReleaseFeautreResult(float        *  pFeature);
//
//private:
//
//    static   boost::shared_mutex   mMapLocker;
//    static   TIfExtratorMap        mIfExtratorMap;
//
//    static   int                   mHandleExtractSeq;
//
//};
#endif

class CascadeDetector {

public:

	typedef enum {
		FACE_CASCADE_DET_TYPE_INVALID	= -0x1,
		FACE_CASCADE_DET_TYPE_ORG		= 0x0,
		FACE_CASCADE_DET_TYPE_NICKLE	= 0x1,
		FACE_CASCADE_DET_TYPE_VJBOOST	= 0x2,
		FACE_CASCADE_DET_TYPE_MTCNN		= 0x4,
	} EFaceCascadeDetTypes;


	typedef enum {
		FACE_CASCADE_DETROUND_ORIG = 0x0,
		FACE_CASCADE_DETROUND_FLIP = 0x1,
		FACE_CASCADE_DETROUND_RESIZE = 0x2,
	} EFaceCascadeRound;

	CascadeDetector(if_handle_t dwHandle);
	~CascadeDetector();

	if_result_t CascadeFaceDetect(const Mat &       matLoad,
		const int         dwWidth,
		const int         dwHeight,
		int               pdwRectType,
		PFACE_RST_TYPE  * pFaceRectArray,
		unsigned int    * pdwFaceCnt);

	void  CascadeResultRelease(int            dwRectType,
		PIF_FACERECT   pFaceRectArray,
		unsigned int   pdwFaceCnt);

private:

	typedef enum {
		FACE_TRANS_DET_TYPE_NONE = 0x0,
		FACE_TRANS_DET_TYPE_FLIP = 0x1,
		FACE_TRANS_DET_TYPE_RESIZE = 0x2,
	} EFaceTransDetTypes;

	typedef enum {
		FACE_RESIZE_TYPE_NONE = 0x0,
		FACE_RESIZE_TYPE_ZOOMIN = 0x1,
		FACE_RESIZE_TYPE_ZOOMOUT = 0x2,
	} EFaceDetectResizeType;

	static   const int  MAX_CASCADE_LEN = 0x8;

	if_handle_t         mSelfHandle;

	int                 mdwCascadeNum;

	if_handle_t         mahCascadeHandle[MAX_CASCADE_LEN];
	int                 madwCascadeType[MAX_CASCADE_LEN];

	if_result_t CascadeFaceDetectAct(const Mat &       matLoad,
		const int         dwWidth,
		const int         dwHeight,
		int         dwDetRound,
		int               pdwRectType,
		PFACE_RST_TYPE  * pFaceRectArray,
		unsigned int    * pdwFaceCnt);

};


typedef boost::shared_ptr<CascadeDetector>                    SP_CascadeDetector;
typedef boost::unordered_map<if_handle_t, SP_CascadeDetector> TCascadeDetectorMap;

class CascadeDetectManager {

public:

	static if_handle_t CreateIfDetectorHandle(int   dwThreadCnt);

	static void        DestoryIfDetectoHandle(if_handle_t  hHandle);

	static if_result_t FeedOneFramToDetector(if_handle_t       hHandle,
		const Mat &       matLoad,
		const int         dwWidth,
		const int         dwHeight,
		int				  pdwRectType,
		PFACE_RST_TYPE  * pFaceRectArray,
		unsigned int    * pdwFaceCnt);

	static void        ReleaseDetFaceResult(int                      dwRectType,
		PIF_FACERECT             pFaceRectArray,
		unsigned int             pdwFaceCnt);

private:

	static   boost::shared_mutex   mMapLocker;
	static   TCascadeDetectorMap   mIfDetectorMap;

	static   int                   mHandleDetectSeq;

};

class IfFeatureExtManager {

public:

	static if_handle_t CreateIfExtratorHandle(int           dwThreadCnt);

	static void        DestoryIfExtratorHandle(if_handle_t  hHandle);

	static int         ExtratFeatureFromImage(if_handle_t      hHandle,
		cv::Mat      &   rMatGray,
		if_rect_t    &   pFaceRect,
		float        **  pFeature);

	static void        ReleaseFeautreResult(float        *  pFeature);

	static void        IfFeatureExtMangerInit(void);

private:

	static   boost::shared_mutex   mMapLocker;
	static   TIfExtratorMap        mIfExtratorMap;

	static   int                   mHandleExtractSeq;

};


class IfExtractorBase {
public:
	virtual      ~IfExtractorBase(){}
	virtual int  ExtratFeatureFromImg(cv::Mat    &        rMatGray,
		if_rect_t  &        pFaceRect,
		float **            pFeature,
		int   *             pFeatureLen,
		int                 dwAlgType,
		TFeatureSeqIdInfo & rSeqIdInfo,
		int                 dwSwReqMode) = 0;
protected:

};

typedef boost::shared_ptr<IfExtractorBase>  SP_IfExtractorBase;


class FaceExtractor : public IfExtractorBase
{
public:

	typedef enum {
		ALG_UNKNOW_FEATURE_LEN = 0,
		ALG_ORG_FEATURE_LEN = 181,
		ALG_ORG_FLIP_FEATURE_LEN = 256,
		ALG_TUNA_FEATURE_LEN = 128,
		ALG_DEEPEYE_FEATURE_LEN = 128,
		ALG_GLUON_FEATURE_LEN = 128,
		ALG_FUSION_FEATURE_LEN = 256,
	} EAlgExtraLen;

	FaceExtractor();
	~FaceExtractor();

	int  ExtratFeatureFromImg(cv::Mat      &      rMatGray,
		if_rect_t    &      pFaceRect,
		float **            pFeature,
		int   *             pFeatureLen,
		int                 dwAlgType,
		TFeatureSeqIdInfo & rSeqIdInfo,
		int                 dwSwReqMode);
	void        dump();
private:
	if_handle_t         pFaceAlgExtraHandl[ALG_NUM_MAX];
	int                 pFaceAlgExtraType[ALG_NUM_MAX];
	int                 dwAlgNums;
};


typedef boost::shared_ptr<FaceExtractor> SP_FaceExtractor;


class MutilAlgExtractor
{
public:
	typedef enum {
		TARGS_EXTRA_TYPE_UNKNOW = -1,
		TARGS_EXTRA_TYPE_FACE = 0x0,
		TARGS_EXTRA_TYPE_CLOTH = 0x1,
	} ETargsExtraType;

	MutilAlgExtractor(if_handle_t pHandl);
	~MutilAlgExtractor();

	int         ExtratFeatureFromImage(cv::Mat      &      rMatGray,
		if_rect_t    &      pFaceRect,
		float **            pFeature,
		int   *             pFeatureLen,
		int                 dwTargType,
		int                 dwAlgType,
		TFeatureSeqIdInfo & rSeqIdInfo,
		int                 dwSwReqMode);
private:

	SP_IfExtractorBase   mTargsExtraHandl[TARGS_NUM_MAX];
	int                  mTargsExtraType[TARGS_NUM_MAX];
	if_handle_t          mSelfHandle;
	int                  mdwTargsNum;
};

typedef boost::shared_ptr<MutilAlgExtractor>                    SP_MutilAlgExtractor;
typedef boost::unordered_map<if_handle_t, SP_MutilAlgExtractor> TMutilAlgMap;


class IfMutilAlgExtManger
{
public:
	static if_handle_t CreateIfExtratorHandle(const string& strPath);

	static void        DestoryIfExtratorHandle(if_handle_t  hHandle);

	static int         ExtratFeatureFromImage(if_handle_t         hHandle,
		cv::Mat      &      rMatGray,
		if_rect_t    &      pFaceRect,
		float **            pFeature,
		int                 dwTargType,
		int                 dwAlgType,
		TFeatureSeqIdInfo & rSeqIdInfo,
		int                 dwSwReqMode);

	static void        ReleaseFeautreResult(float        *pFeature, int dwAlgType);

	static void        dump();

private:

	static   boost::shared_mutex   mMapLocker;
	static   TMutilAlgMap          mIfAlgMap;

	static   int                   mHandleAlgSeq;
};

class IfMutilAlgCompare
{
public:
	IfMutilAlgCompare(if_handle_t handl);
	~IfMutilAlgCompare();

	static int IfFeatureCompare(
								const float     *pFeature0,
								int              dwLen0,
								const float     *pFeature1,
								int              dwLen1,
								float           *score,
								int              dwAlgType,
								int              dwTargType = 0);

private:
	if_handle_t         mAlgCmpHandl[ALG_NUM_MAX];
	int                 mAlgCmpType[ALG_NUM_MAX];
	if_handle_t         mMyHandl;
	int                 mdwCmpNums;

	static int AntFeatureCompare(const float *   pfFeature0,
		int       dwFeatureLen0,
		const float *   pfFeature1,
		int       dwFeatureLen1,
		float *   pfScore);

	static int GluonFeatureCompare(const float *   pfFeature0,
		int       dwFeatureLen0,
		const float *   pfFeature1,
		int       dwFeatureLen1,
		float *   pfScore);

	static int FusionFeatureCompare(const float *   pfFeature0,
		int       dwFeatureLen0,
		const float *   pfFeature1,
		int       dwFeatureLen1,
		float *   pfScore);

	static int TunaFeatureCompareConv(const float *   pfFeature0,
		int       dwFeatureLen0,
		const float *   pfFeature1,
		int       dwFeatureLen1,
		float *   pfScore);

	static int TunaFeatureCompareSvm(const float *   pfFeature0,
		int             dwFeatureLen0,
		const float *   pfFeature1,
		int             dwFeatureLen1,
		float *         pfScore);

	static int TunaFeatureCompare(const float *   pfFeature0,
		int       dwFeatureLen0,
		const float *   pfFeature1,
		int       dwFeatureLen1,
		float *   pfScore);
};

typedef boost::shared_ptr<IfMutilAlgCompare>                    SP_IfMutilAlgCompare;
typedef boost::unordered_map<if_handle_t, SP_IfMutilAlgCompare> TMutilAlgCmpMap;

class IfMutilAlgCompareManger {
public:
	static if_handle_t CreateIfCmpHandle();

	static void        DestoryIfCmpHandle(if_handle_t  hHandle);

	static int         IfFeatureCompare(if_handle_t      hHandle,
		const float     *pFeature0,
		int              dwLen0,
		const float     *pFeature1,
		int              dwLen1,
		float           *score,
		int              dwAlgType,
		int              dwTargType = 0);

private:

	static   boost::shared_mutex   mMapLocker;
	static   TMutilAlgCmpMap       mIfCmpMap;
	static   int                   mHandleCmpSeq;
};
#endif


