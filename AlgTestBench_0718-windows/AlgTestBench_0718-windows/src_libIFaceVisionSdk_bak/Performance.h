/*
 * @file     PreAndPostProcessor.h
 * @date     2015-06-24
 * @brief     the all class defination of data item pass through the face snaping pipeline
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif

#include "TestCaseCommon.h"

#include "boost/smart_ptr.hpp"

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

#include "boost/smart_ptr.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"
#include "boost/unordered_map.hpp"

#include "boost/filesystem.hpp"

#include "inifile.h"

#include <list>

using namespace inifile;
using namespace std;
using namespace cv;
using namespace boost::filesystem;

typedef struct {
	IplImage * pImageInfo;
} TFaceImageItem;


typedef struct {
	unsigned int   mdwImageSeq;
	IF_FACERECT    tFaceInfo;       // the face info array
} TFeatherExtraItem;

typedef struct {
	unsigned int   mdwImageSeq;
} TImageStoreItem;

typedef union {
	PIF_FACERECTID    mpTrackRect;
	PIF_FACERECT      mpFrontRect;
} UFaceRectPtr;

typedef struct {
	int            mdwInqueType;
	UFaceRectPtr   mpFaceRect;
	int            mdwFaceCount;
} TExtraInqueItem;

typedef union {
	TFaceImageItem    tImageInfo;
	TFeatherExtraItem tExtratInfo;
	TImageStoreItem   tStoreInfo;
	TExtraInqueItem   tInqueInfo;
} UDataItemParam;

class FaceDataItem {

public:
	enum {
		DATA_ITEM_TYPE_FACE_IMAGE = 0,
		DATA_ITEM_TYPE_VIDEO_FRAME = 1,
		DATA_ITEM_TYPE_FACE_EXTRAT = 2,
		DATA_ITEM_TYPE_FACE_STORE = 3,
		DATA_ITEM_TYPE_EXTRA_INQUE = 4,
	};

	typedef enum {
		FACE_INQUE_TYPE_DETECT = 0x0,
		FACE_INQUE_TYPE_TRACKER = 0x1,
	} E_FaceInqueType;

	FaceDataItem(int        dwItemType,
		int        dwImgType,
		Mat &      rColorImg,
		string     rImgPath,
		string     rImgTable,
		int        dwImgIdx);

	FaceDataItem(int            dwItemType,
		int            dwImgType,
		unsigned int   udwImgSeq,
		Mat &          rColorImg,       // the image data with color generate by opencv library
		IF_FACERECT &  pFaceInfo,
		string &       rImgPath,
		string &       rImgTable,
		int            dwImgIdx);

	FaceDataItem(int            dwItemType,
		int            dwImgType,
		unsigned int   udwImgSeq,
		Mat &          rColorImg,
		string &       rImgPath,
		int            dwImgIdx);      // the image data with color generate by opencv librar

	FaceDataItem(int            dwItemType,
		int            dwImgType,
		Mat &          rColorImg,
		PIF_FACERECT   pFaceRect,
		int            dwFaceCount,
		string &       rImgPath,
		string &	   rImgTable,
		int            dwImgIdx);

	virtual ~FaceDataItem();

	UDataItemParam *                  getItemParam(void);
	int                               SetItemParam(const UDataItemParam & rItemParam);
	int                               getItemType(void);
	Mat &                             getCvColorMat(void);
	string &                          getImgPath(void);
	string &						  getImgTable(void);
	int                               getImgType(void);
	int                               getImgIdx(void);

private:
	int              mdwItemType;
	UDataItemParam   uDataItemInfo;
	Mat              mrColorImg;       // the image data with color generate by opencv library
	string           msImgFilePath;
	int              mImgType;
	int              mdwImgIdx;
	string           msImgTable;
};

typedef boost::shared_ptr<FaceDataItem> SP_FaceDataItem;

class FaceRecoder {
public:
	FaceRecoder(const string &      sOrgImgPath,
		int64_t      dwFaceCidId,
		const string &      sImgTable,
		const unsigned int  dwFaceSeq,
		float               fCmpScore = (float)0.0);
	~FaceRecoder();

	//string &    getOrgImgPath(void);
	unsigned int  getFaceSeq(void);
	string &      getImgPath(void);
	int64_t       getImgCidId(void);
	string &      getImgTable(void);
	float         getCmpScore(void);
	void          setCmpScore(float fScore);

	//void          saveFace2Disk(path & sSave2FolderOrg, path & sSave2FolderFace);

private:
	string       msOrgImgPath;
	int64_t		 mdwFaceCidId;
	string       msImgTable;
	unsigned int mdwFaceSeq;
	IF_FACERECT  mFaceRect;

	float        mfScore;

	IplImage*    cvGetSubImage(IplImage *image, CvRect roi);
};

typedef boost::unordered_map<unsigned int, boost::shared_ptr<FaceRecoder>> FaceInfoMap;

typedef struct _part_sort_info {
	_part_sort_info(){
		score = (float)0.0;
		dwId = 0;
	}
	float           score;
	int				dwId;
} PartSortInfo;


class HeapSort
{
public:
	void shift(float array[], int i, int m);
	void heapSortInt(float array[], int n);
private:
};


class Performance;

class NameListItem {

public:
	NameListItem(const string& rImgPath,
		const string& rImgTable,
		int64_t       dwFaceCidId,
		float *       pfFeature,
		int           dwFeatureLen);

	~NameListItem();

	//static const int MINT_OUTPUT_COUNT = 100;
	float *  getFeaturePtr(void);
	int      getFeatureLen(void);
	string & getImgPath(void);
	int64_t  getFaceCidId(void);
	string & getImgTable(void);

	void     addMatchFace(boost::shared_ptr<FaceRecoder>& rFaceRcd, float fScore);

	int      getStatus(void);
	
	PartSortInfo SortReslut();

	//int      Cmp(const pair<unsigned int, boost::shared_ptr<FaceRecoder>>& x, const pair<unsigned int, boost::shared_ptr<FaceRecoder>>& y);

	//void	 SortMapByValue(FaceInfoMap& tMap, vector<pair<unsigned int, boost::shared_ptr<FaceRecoder>>>& tVector);

	int      OutputMatinfo2File(Performance * prPerformance, FILE * pfRstFile, int dwSaveResultPic, string strPathOut);

	//name list
	FaceInfoMap mNewNameList;

private:
	float   mafFeature[1024];
	int     mdwFeatureLen;
	int     mdwRegressStatus;
	string  msImgPath;
	int64_t mdwFaceCidId;
	string  msImgTable;

	boost::mutex                       mNameListLock;

	boost::mutex					   mInputListLock;

	//exclude list
	FaceInfoMap mExcludeList;
};

typedef boost::shared_ptr<NameListItem> SP_NameListItem;

typedef boost::unordered_map<int, SP_NameListItem> NameListMap;

typedef boost::unordered_map<int, SP_NameListItem> InputListMap;

typedef boost::shared_ptr<boost::thread> SP_BoostThread;

typedef struct _part_talbes_info {
	_part_talbes_info(){
		msFormerName = "";
		mdwCode = 0;
		mtStart = 0;
		mtEnd = 0;
		miStart = 0;
		miEnd = 0;
	}
	std::string     msFormerName;
	size_t          mdwCode;
	time_t          mtStart;
	time_t          mtEnd;
	int				miStart;
	int				miEnd;
} PartTablesInfo;

typedef std::vector<PartTablesInfo>  VecPartTablesInfo;

class StaticResultItem{

public:
	StaticResultItem() {}
	~StaticResultItem() {}
	int           mdwTop1;
	int			  mdwTop5;
	int			  mdwTop7;
	int			  mdwTop10;
	int           mdwSampleCount;
};

typedef boost::shared_ptr<StaticResultItem> SP_StaticResultItem;

class TestResultItem {

public:
	enum
	{
		TEST_RESULT_TYPE_STATIC = 0,
		TEST_RESULT_TYPE_DYNAMIC = 1,
		TEST_RESULT_TYPE_OTHER = 2,
	};

	TestResultItem() {}
	~TestResultItem() {}

	int64_t                  mdwResultId;
	int64_t                  mdwResultInfoId;
	int                      mdwResultStatus;
	int						 mdwResultType;
	string                   mStrResultUrl;
	string					 mStrXResultUrl;
	string					 mStrServerIp;
	int						 mdwSampleId;
	SP_StaticResultItem      mSpStaticResultItem;
};

typedef boost::shared_ptr<TestResultItem> SP_TestResultItem;

//Handler Manager
class IFaceHandlerFifo {

public:
	IFaceHandlerFifo();
	~IFaceHandlerFifo();

	if_handle_t  GetIFaceHandler(void);
	int          ReturnIFaceHandler(if_handle_t hReturn);

private:
	static   const int FACE_SNAPER_ITEM_PAD_NUM = 0x1;
	static   const int FACE_HANDLER_FIFO_LEN = 0x40;

	unsigned int       mdwItemWPtr;      // work item read ptr
	unsigned int       mdwItemRPtr;      // work item write ptr

	boost::mutex       mItemFifoLocker;

	boost::condition   mFifoNotEmptyCond;

	if_handle_t        mIFaceHandlers[FACE_HANDLER_FIFO_LEN];

};

typedef boost::shared_ptr<IFaceHandlerFifo> SP_IFaceHandlerFifo;

class FeatureWrapperItem {

public:

	FeatureWrapperItem(int dwAlgType, float * pfFeature, int dwFeatureLen, string mstrImgPath = "");

	~FeatureWrapperItem();

	int       mdwAlgType;
	int       mdwFeatureLen;
	float *   mpfFeaturePtr;

	string    msImgPath;

private:
	boost::mutex                       mNameListLock;
};

typedef boost::shared_ptr<FeatureWrapperItem> SP_FeatureWrapperItem;


class FaceUrlFeatureItem {

public:

	FaceUrlFeatureItem() {}
	~FaceUrlFeatureItem() {}

	int64_t                  mdwFaceId;

	int                      mdwFaceSeq;

	string                   mStrFaceUrl;

	int64_t                  mdwFaceCidId;

	string					 mStrFaceTable;

	SP_FeatureWrapperItem    mSpFaceFeature;

};

typedef boost::shared_ptr<FaceUrlFeatureItem> SP_FaceUrlFeatureItem;

typedef std::list<SP_FaceUrlFeatureItem>      TFaceUrlFeatureList;

class FaceUrlFeatureList {

public:

	FaceUrlFeatureList() {};
	~FaceUrlFeatureList(){};

	std::list<SP_FaceUrlFeatureItem> mListUrlFeature;

private:

};

typedef boost::shared_ptr<FaceUrlFeatureList> SP_FaceUrlFeatureList;

typedef std::list<SP_FeatureWrapperItem>      TFeatureWrapList;

class PerformanceItem {

public:
    
	PerformanceItem() {};
	~PerformanceItem(){};

    int64_t   mdwRepoCoSchId;

    string    mstrOrgData;
    time_t    mdwOrgUtcTick;

    std::list<SP_FeatureWrapperItem> mListFeatures;

private:
    
};

typedef boost::shared_ptr<PerformanceItem>					SP_PerformanceItem;
typedef boost::unordered_map<int64_t, SP_PerformanceItem>   TPerformanceItemMap;
typedef std::list<std::string>								TStringList;
typedef std::vector<int>									VecIntDetect;
typedef std::list<boost::filesystem::path>                  TFilePathList;

class Performance {

public:
	enum {
		FOLDER_TYPE_IMG_INPUT,
		FOLDER_TYPE_IMG_QUERY,
		FOLDER_TYPE_IMG_OUTPUT,
	};

	Performance(string strPathCfg);
    
	~Performance();

    int  LoadConfigInformation(void);
	int  LoadAndGerateQueryList(void);
	int  LoadAndGerateTargetList(void);
	int  DoPerformanceLoop(int dwThreadIdx);
	void  DoFaceDetectLoop(int dwThreadIdx);
	void  DoFaceExtractLoop(int dwThreadIdx);
    int  WaitingPorcessFinished(void);
	int  WaitingDetectPorcessFinished(void);
	int  WaitingExtractPorcessFinished(void);
	void TbbExtractPipelineStop(void);
	void TbbDetectPipelineStop(void);
	int  PrintCompareResult(void);
	int  Statistics(void);
	int  StartupProcessThreads(void);
	int  StartupDetectThreads(void);
	int  StartupExtractThreads(void);
	void RefreshFeature();
	string GetFaceXMFromMysql(const std::string &tableName, int photoid);
	bool   decodeRemotePicCurl(const std::string & rStrRemotUrl, string & strRevData);
	int    SaveDetectFace2Disk(cv::Mat matGray, if_rect_t tFaceRect, string strSavePath);

private:
	NameListMap mNameList;
    
    static const int MAX_PERFORMANCE_THREADS  = 0x40;
    static const int MAX_FEATURE_PADDED_LEN  = 1024;
	static const int FACE_SNAPER_MAX_ITEM_NUM = 0x200;
	static const int FACE_SNAPER_ITEM_PAD_NUM = 0x1;

    boost::shared_ptr<IniFile>  mpIniFile;
    
    string                      mStrCfgFile;

    //configuration fromo ini file
    string                      mStrPathInput;
    string                      mStrPathQuery;
    string                      mStrPathOut;

    string                      mStrProxySrvFlag;
    string                      mStrProxySrvAddr;

	int							mdwNofaceProcMode;
	int							mdwSaveDetectPic;
    int                         mdwSearchStep;
	int							mdwSaveMulFace;
	int							mdwSaveResultPic;
	int						    mdwSaveFeatureBin;
	int						    mdwWriteToMysql;
	int                         mdwReadFromMysql;
	int							mdwWriteInputToMysql;
	int							mdwStartId;
	int							mdwEndId;
	int						    mdwNowId;
	int                         mdwInputFaceSeq;
	int						    mdwQueryFaceSeq;
	int							mdwOutputCount;
	int							mdwFaceFeatureLen;
	int						    mdwFlipExtratFeature;
	int							mdwSaveResultToMysql;

    TStringList                 mListAlgVersions;
	TFilePathList               mQueryPathList;
	size_t                      dwPendNameListCnt;
	size_t                      dwRcvdNameListCnt;

	int							mdwAlgVersions;

    string                      mStrModelPath;
    float                       mfFatureThresHold;

    int                         mdwDetectHandleCnt;
    int                         mdwExtractHandleCnt;
    int                         mdwMysqlHandleCnt;
    int                         mdwProcThreadCnt;
	int							mdwSingleThread;

	int							mdwOldDetect;
	int							mdwNickleDetect;
	int							mdwVjboostDetect;
	int							mdwMtcnnDetect;

	string                      mStrFaceDbName;
	string                      mStrFaceTables;
    string                      mStrMysqlSrvAddr;
    int                         mdwMysqlSrvPort;
    string                      mStrMysqlSrvUser;
    string                      mStrMysqlSrvPswd;

	int							mdwCompareCountAcc;
	int							mdwCompareCountOnce;
	int							mdwCompareCountTimes;
	std::time_t					timeCompareAccTicks;

	int							mdwExtractCountAcc;
	int							mdwExtractCountOnce;
	int							mdwExtractCountTimes;
	std::time_t					timeExtractAccTicks;

	int                         madwSaveStopReq[MAX_PERFORMANCE_THREADS];
	int                         madwSaveStopRsp[MAX_PERFORMANCE_THREADS];
	int                         madwDetectSaveStopReq[MAX_PERFORMANCE_THREADS];
	int                         madwDetectSaveStopRsp[MAX_PERFORMANCE_THREADS];
	int                         madwExtractSaveStopReq[MAX_PERFORMANCE_THREADS];
	int                         madwExtractSaveStopRsp[MAX_PERFORMANCE_THREADS];
	int                         madwCompareStatus[MAX_PERFORMANCE_THREADS];
	int                         madwDetectStatus[MAX_PERFORMANCE_THREADS];
	int                         madwExtractStatus[MAX_PERFORMANCE_THREADS];
	SP_BoostThread              matProcThreads[MAX_PERFORMANCE_THREADS];
	SP_BoostThread              matDetectProcThreads[MAX_PERFORMANCE_THREADS];
	SP_BoostThread              matExtractProcThreads[MAX_PERFORMANCE_THREADS];

    SP_IFaceHandlerFifo         mDetectHandleFifo;
    SP_IFaceHandlerFifo         mExtractHandleFifo;
    SP_IFaceHandlerFifo         mMysqlHandleFifo;
    SP_IFaceHandlerFifo         mCompareHandleFifo;
	if_handle_t                 mCompareHanle[MAX_PERFORMANCE_THREADS];
	if_handle_t					mExtractHanle[MAX_PERFORMANCE_THREADS];
	if_handle_t					mDetectHanle[MAX_PERFORMANCE_THREADS];

	TPerformanceItemMap         mPerformItemMap;

    boost::mutex                mTargetNameLock;
	boost::mutex                mIdScopeLock;
    boost::mutex                mStatuIniLock;
	boost::mutex				mUpdateMysqlLock;
	boost::mutex				mFaceSeqLock;

	int    GetOneIdScope(int & dwGetStartId, int & dwGetEndupId);
	int    GetFaceFeatureFromMysql(int startid, int endid, int dwAlgType, SP_FaceUrlFeatureList & rSpUrlFeature);
	int    WriteFaceFeatureToMysql(SP_FaceUrlFeatureItem & rSpFaceUrlFeatuerItem);
	int    WriteFaceFeature(SP_FaceUrlFeatureList & rSpUrlFeature);
	int    GetFaceUrlFromMysql(int startid, int endid, int dwAlgType, SP_FaceUrlFeatureList & rSpUrlFeature);
	int    SelectRecordById(const std::string &tableName, int64_t  dwFaceId);
	int    UpdateFaceFeatuerToMysql(const std::string &tableName, boost::shared_ptr<FaceUrlFeatureItem>&  rSpFaceUrlFeatuerItem);
	int    InsertFaceFeatuerToMysql(const std::string &tableName, boost::shared_ptr<FaceUrlFeatureItem>&  rSpFaceUrlFeatuerItem);
	int    ProcFaceUrlsAndFeature(SP_FaceUrlFeatureList & rSpUrlFeature);
	int    LoadAndGerateInputList(SP_FaceUrlFeatureList & rSpUrlFeature);
	int    DetectAndExtractFace(const if_handle_t hDetectHandle, const if_handle_t hExtractHandle, const std::string &picturePath, float *  & pfFeatureWrap);
	int    GetFaceUrlFromFolder(SP_FaceUrlFeatureList & rSpUrlFeature);
	void   LoadAndGerateTargetLoop(int dwThreadIdx);
	int    PopOnePathFromList(boost::filesystem::path & rPathPush);
	int    WriteResultToMysql(SP_TestResultItem & rSpTestResultItem);
	int    WriteResultInfoToMysql(SP_TestResultItem & rSpTestResultItem);
	int    UpdateResultInfoToMysql(SP_TestResultItem & rSpTestResultItem);
	int    WriteStaticResultToMysql(SP_TestResultItem & rSpTestResultItem);
	
	//void   folderProcessLoop(string msFolderPath);
	void MysqlProcessLoop(void);

	//人脸检测多线程
	boost::mutex mDetectProcLocker;
	boost::mutex mDetectItemQueLocker;
	boost::condition mDetectQueNoEmptyCondition;
	boost::condition mDetectQueNoFullCondition;
	int           mdwDetectType;
	unsigned int  dwDetectItemWPtr;      // work item read ptr
	unsigned int  dwDetectItemRPtr;      // work item write ptr
	unsigned int dwDetectFinishedCnt;
	unsigned int dwDetectPendingCnt;
	boost::shared_ptr<FaceDataItem>  mDetectBufferItem[FACE_SNAPER_MAX_ITEM_NUM];
	void IncDetectFinishedCnt(void);
	unsigned int GetDetectFinishedCnt(void);
	unsigned int GetDetectPendingCnt(void);
	int DetectDataReceived(boost::shared_ptr<FaceDataItem>& prDataItem);
	boost::shared_ptr<FaceDataItem> FetchDetectDataItemIdx(int dwThreadIdx);
	int GetImageSequence(void);
	void DetectThreadWaitStop(void);

	//特征值提取多线程
	boost::mutex mExtractProcLocker;
	boost::mutex mExtractItemQueLocker;
	boost::condition mExtractQueNoEmptyCondition;
	boost::condition mExtractQueNoFullCondition;
	int           mdwExtractType;
	unsigned int  dwExtractItemWPtr;      // work item read ptr
	unsigned int  dwExtractItemRPtr;      // work item write ptr
	unsigned int dwExtractFinishedCnt;
	unsigned int dwExtractPendingCnt;
	boost::shared_ptr<FaceDataItem>  mExtractBufferItem[FACE_SNAPER_MAX_ITEM_NUM];
	void IncExtractFinishedCnt(void);
	unsigned int GetExtractFinishedCnt(void);
	unsigned int GetExtractPendingCnt(void);
	int ExtractDataReceived(boost::shared_ptr<FaceDataItem>& prDataItem);
	boost::shared_ptr<FaceDataItem> FetchExtractDataItemIdx(int dwThreadIdx);
	void ExtractThreadWaitStop(void);

	void addToNameList(const string& rImgPath,
						const string& rImgTable,
						int64_t	   dwImgCidIdx,
						int           dwImgIdx,
						float *       pfFeature,
						int           dwFeatureLen);

    int  TryReadAndUpateCfgStr(boost::shared_ptr<IniFile> & rInitFile,
                               const char * section, 
                               const char * key, 
                               string     & strCfg,
                               size_t       dwMaxLen = 0x100);
    
    int  TryReadAndUpateCfgInt(boost::shared_ptr<IniFile> & rInitFile,
                               const char * section, 
                               const char * key, 
                               int        & intCfg,
                               int          maxValue = 0x7fffffff);
    
    
    int  TryReadAndUpateCfgflt(boost::shared_ptr<IniFile> & rInitFile,
                               const char * section, 
                               const char * key, 
                               float      & fltCfg,
                               float        maxValue = (float)10000000.0);

    int  TrySetAndUpateCfgStr(boost::shared_ptr<IniFile> & rInitFile,
                              const char   * section, 
                              const char   * key, 
                              const string & value);

    int  TrySetAndUpateCfgInt(boost::shared_ptr<IniFile> & rInitFile,
                              const char   * section, 
                              const char   * key, 
                              const int    & value);

    int  TrySetAndUpateCfgflt(boost::shared_ptr<IniFile> & rInitFile,
                              const char    * section, 
                              const char    * key, 
                              const  float  & value);

    //utility function
    
    std::string get_remote_url(const std::string &rawString);
    
    bool        is_remote_url(const std::string &rawString);
    
    bool        is_route_url(const std::string &rRouteStr);
    
    time_t      time_string_to_time_t(const std::string& timeStr, int dwIsIso = 0);
    
    std::string time_t_to_iso_string(const time_t& tSec);

    static int  write2String(char *data, size_t size, size_t nmemb, string * writerData);

    IplImage*   cvGetSubImage(IplImage *image, CvRect roi);

};

typedef boost::shared_ptr<Performance> SP_Performance;
    
#endif
