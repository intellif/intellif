/*
 * @file    PreAndPostProcessor.cpp
 * @date    2015-06-24
 * @brief   Face Front Processing, which detect face from image data, and send image data to 
            face backend processor
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */


#include <iostream>
#include <fstream>
#include <vector>

#include "Performance.h"

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include "boost/filesystem.hpp"

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "curl/curl.h"
#include "mysql/mysql.h"

#include "IfsdkWrapper.h"
#include "IFaceRecType.h"

#include <iostream>
#include <streambuf>
#include <sstream>
#include <fstream>

using namespace std;
using namespace boost::filesystem;

extern string g_strConfigFilePath;
extern string g_strAlgModelPath;
extern int    g_mdwAlgFeatureLen;
extern int	  g_mdwMatOutputCnt;
extern int    g_mdwAlgVersion;
extern int    g_mdwRefreshMode;
extern int    g_flipExtratFeature;
extern vector<int> g_vecIntDetect;
extern FILE * pfResustFile;

FaceDataItem::FaceDataItem(int        dwItemType,
	int        dwImgType,
	Mat &      rColorImg,
	string     rImgPath,
	string     rImgTable,
	int        dwImgIdx)
{
	mrColorImg = rColorImg;
	mdwItemType = dwItemType;
	msImgFilePath = rImgPath;
	mImgType = dwImgType;
	mdwImgIdx = dwImgIdx;
	msImgTable = rImgTable;
}

FaceDataItem::FaceDataItem(int            dwItemType,
	int            dwImgType,
	unsigned int   dwImgSeq,
	Mat &          rColorImg,       // the image data with color generate by opencv library
	IF_FACERECT &  rtFaceInfo,
	string &       rImgPath,
	string &       rImgTable,
	int            dwImgIdx)
{

	uDataItemInfo.tExtratInfo.tFaceInfo = rtFaceInfo;
	uDataItemInfo.tExtratInfo.mdwImageSeq = dwImgSeq;
	mdwItemType = dwItemType;
	mrColorImg = rColorImg;
	msImgFilePath = rImgPath;
	mImgType = dwImgType;
	msImgTable = rImgTable;
	mdwImgIdx = dwImgIdx;
}

FaceDataItem::FaceDataItem(int            dwItemType,
	int            dwImgType,
	unsigned int   udwImgSeq,
	Mat &          rColorImg,
	string &       rImgPath,
	int            dwImgIdx)
{
	uDataItemInfo.tStoreInfo.mdwImageSeq = udwImgSeq;
	mdwItemType = dwItemType;
	mrColorImg = rColorImg;
	msImgFilePath = rImgPath;
	mImgType = dwImgType;
	mdwImgIdx = dwImgIdx;
}

FaceDataItem::FaceDataItem(int            dwItemType,
	int            dwImgType,
	Mat &          rColorImg,
	PIF_FACERECT   pFaceRect,
	int            dwFaceCount,
	string &       rImgPath,
	string &	   rImgTable,
	int            dwImgIdx)
{
	uDataItemInfo.tInqueInfo.mpFaceRect.mpFrontRect = pFaceRect;
	uDataItemInfo.tInqueInfo.mdwFaceCount = dwFaceCount;
	mdwItemType = dwItemType;
	mrColorImg = rColorImg;
	msImgFilePath = rImgPath;
	mImgType = dwImgType;
	mdwImgIdx = dwImgIdx;
	msImgTable = rImgPath;
}

FaceDataItem::~FaceDataItem()
{
}

UDataItemParam * FaceDataItem::getItemParam(void)
{
	return &uDataItemInfo;
}

int FaceDataItem::SetItemParam(const UDataItemParam & rItemParam)
{
	uDataItemInfo = rItemParam;
	return 0;
}

int FaceDataItem::getItemType(void)
{
	return mdwItemType;
}

Mat & FaceDataItem::getCvColorMat(void)
{
	return mrColorImg;
}

string & FaceDataItem::getImgPath(void)
{
	return msImgFilePath;
}

string & FaceDataItem::getImgTable(void)
{
	return msImgTable;
}

int      FaceDataItem::getImgType(void)
{
	return mImgType;
}

int      FaceDataItem::getImgIdx(void)
{
	return mdwImgIdx;
}

FaceRecoder::FaceRecoder(const string &      sOrgImgPath,
	int64_t      dwFaceCidId,
	const string &      sImgTable,
	const unsigned int  dwFaceSeq,
	float               fCmpScore)
{

	msOrgImgPath = sOrgImgPath;
	mdwFaceCidId = dwFaceCidId;
	msImgTable = sImgTable;
	mdwFaceSeq = dwFaceSeq;
	mfScore = fCmpScore;
}

FaceRecoder::~FaceRecoder()
{

}

//string &    getOrgImgPath(void);
unsigned int  FaceRecoder::getFaceSeq(void)
{
	return mdwFaceSeq;
}

string &      FaceRecoder::getImgPath(void)
{
	return msOrgImgPath;
}
string &      FaceRecoder::getImgTable(void)
{
	return msImgTable;
}

int64_t      FaceRecoder::getImgCidId(void)
{
	return mdwFaceCidId;
}

float         FaceRecoder::getCmpScore(void)
{
	return mfScore;
}


IplImage*     FaceRecoder::cvGetSubImage(IplImage *image, CvRect roi)
{
	IplImage *result;
	cvSetImageROI(image, roi);
	result = cvCreateImage(cvSize(roi.width, roi.height), image->depth, image->nChannels);
	cvCopy(image, result);
	cvResetImageROI(image);
	return result;
}

void  FaceRecoder::setCmpScore(float fScore)
{
	mfScore = fScore;
}


//三个节点中最大的
void HeapSort::shift(float array[], int i, int n)
{
	int left = i * 2 + 1;
	int right = left + 1;
	int largest = i;

	if (left < n && array[left] > array[largest])
	{
		largest = left;
	}

	if (right < n && array[right] > array[largest])
	{
		largest = right;
	}

	if (largest != i)
	{
		float temp = array[largest];
		array[largest] = array[i];
		array[i] = temp;
		shift(array, largest, n);
	}
}

//形成新堆
void HeapSort::heapSortInt(float array[], int n)
{
	for (int i = n / 2 - 1; i >= 0; i--)
	{
		shift(array, i, n);  //建立最大堆，将堆中最大的值交换到根节点
	}
	for (int i = n - 1; i > 0; i--)
	{
		float temp = array[i];  //将当前堆的根节点交换到堆尾的指定位置
		array[i] = array[0];
		array[0] = temp;
		shift(array, 0, i); //建立下一次的最大堆
	}
}


NameListItem::NameListItem(const string& rImgPath,
	const string& rImgTable,
	int64_t       dwFaceCidId,
	float *       pfFeature,
	int           dwFeatureLen)
{
	mdwFeatureLen = dwFeatureLen;
	msImgPath = rImgPath;
	mdwFaceCidId = dwFaceCidId;
	msImgTable = rImgTable;

	mdwRegressStatus = 0;

	for (int dwFeaIdx = 0; dwFeaIdx < dwFeatureLen; dwFeaIdx++)
	{
		mafFeature[dwFeaIdx] = pfFeature[dwFeaIdx];
	}

}

NameListItem::~NameListItem()
{

}

float *  NameListItem::getFeaturePtr(void)
{
	return mafFeature;
}

int      NameListItem::getFeatureLen(void)
{
	return mdwFeatureLen;
}

int64_t NameListItem::getFaceCidId(void)
{
	return mdwFaceCidId;
}

string & NameListItem::getImgPath(void)
{
	return msImgPath;
}

string & NameListItem::getImgTable(void)
{
	return msImgTable;
}

int      NameListItem::getStatus(void)
{
	return mdwRegressStatus;
}

int  NameListItem::OutputMatinfo2File(Performance * prPerformance, FILE * pfRstFile, int dwSaveResultPic, string strPathOut)
{
	boost::mutex::scoped_lock autolocker(mNameListLock);

	cout << "NameListItem::OutputMatinfo2File begin============================" << std::endl;

	path   matNameImgPath(msImgPath);

	string strNameImgName = matNameImgPath.filename().string();

	fprintf(pfRstFile, "%s;", strNameImgName.c_str());

	std::vector<float> acScoreRcd;
	std::vector<int>   acScoreIdx;

	for (FaceInfoMap::iterator faceLoop = mNewNameList.begin();
		faceLoop != mNewNameList.end();
		faceLoop++)
	{
		acScoreRcd.push_back(faceLoop->second->getCmpScore());
		acScoreIdx.push_back(faceLoop->first);;
	}

	//order the output
	for (int dwOutIdx = 0; dwOutIdx < g_mdwMatOutputCnt; dwOutIdx++)
	{
		if (dwOutIdx >= acScoreRcd.size())
		{
			break;
		}

		float maxScore = 0;
		int   outputIdx = 0;
		int   clrRcdIdx = 0;

		for (int dwRcdIdx = 0; dwRcdIdx < acScoreRcd.size(); dwRcdIdx++)
		{
			if (maxScore < acScoreRcd[dwRcdIdx])
			{
				maxScore = acScoreRcd[dwRcdIdx];
				outputIdx = acScoreIdx[dwRcdIdx];
				clrRcdIdx = dwRcdIdx;
			}
		}

		path   matPath(mNewNameList[outputIdx]->getImgPath());

		string strFileName = matPath.filename().string();

		int    dwScoreInt = (int)(maxScore * 100);

		acScoreRcd[clrRcdIdx] = 0;

		// 存储图片
#pragma region savepicture
		if (dwSaveResultPic)
		{
			if (prPerformance == NULL)
			{
				fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);
				std::cout << "Performance is NUll####" << std::endl;
				continue;
			}

			string inputpath = mNewNameList[outputIdx]->getImgPath();
			cout << "NameListItem::OutputMatinfo2File::save picture : orgpath = " << inputpath.c_str() << std::endl;
			path nameDirect(msImgPath);
			path tempPath(strPathOut);
			path orgImgPath = tempPath / "faceresult";
			path saveDirectFace = orgImgPath / basename(nameDirect);
			if (!exists(tempPath))
			{
				if (!create_directories(tempPath))
				{
					std::cout << "create folder failed:" << tempPath.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);
					continue;
				}
			}
			if (!exists(orgImgPath))
			{
				if (!create_directories(orgImgPath))
				{
					std::cout << "create folder failed:" << orgImgPath.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);
					continue;
				}
			}
			if (!exists(saveDirectFace))
			{
				if (!create_directories(saveDirectFace))
				{
					std::cout << "create folder failed:" << saveDirectFace.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);
					continue;
				}
			}

			cout << "table name :" << mNewNameList[outputIdx]->getImgTable().c_str() << "::cid:" << mNewNameList[outputIdx]->getImgCidId() << std::endl;
			string strBaseName = prPerformance->GetFaceXMFromMysql(mNewNameList[outputIdx]->getImgTable(), mNewNameList[outputIdx]->getImgCidId());
			if (strBaseName == "" || strBaseName.empty())
			{
				path srcImgPath(inputpath);
				strBaseName = basename(srcImgPath);
			}
			strFileName = strBaseName + ".jpg";
			cout << "strBaseName=" << strBaseName << std::endl;
			std::ostringstream ossFaceName;
			int index = dwOutIdx + 1;
			ossFaceName << index << "_" << mNewNameList[outputIdx]->getCmpScore() << "_" << strFileName;

			path dstFileFace = saveDirectFace / ossFaceName.str();
			cout << "save picture path =" << dstFileFace.string().c_str() << std::endl;
			//区分是mysql还是本地路径
			Mat matLoad;
			if ((inputpath.find("http:") < inputpath.length()))
			{
				string strDownLoad;
				bool bSuccess = prPerformance->decodeRemotePicCurl(inputpath, strDownLoad);
				std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
				matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);
			}
			else
			{
				matLoad = imread(inputpath);
			}

			if (matLoad.data == NULL)
			{
				printf("Load:%s, failed \n", inputpath.c_str());
				fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);
				continue;
			}

			IplImage saveImgFace = IplImage(matLoad);

			int adwJpgPara[3];
			adwJpgPara[0] = CV_IMWRITE_JPEG_QUALITY;
			adwJpgPara[1] = 100;
			adwJpgPara[2] = 0;

			cvSaveImage(dstFileFace.string().c_str(), &saveImgFace, adwJpgPara);
		}
#pragma endregion

		fprintf(pfRstFile, "%s,%d;", strFileName.c_str(), dwScoreInt);
	}

	fprintf(pfRstFile, "\n");

	//fclose(mpNamelistFileOutput);

	//mpNamelistFileOutput = NULL;

	return 0;
}

PartSortInfo  NameListItem::SortReslut()
{
	//order the output
	std::vector<float> acScoreRcd;
	std::vector<int>   acScoreIdx;

	for (FaceInfoMap::iterator faceLoop = mNewNameList.begin();
		faceLoop != mNewNameList.end();
		faceLoop++)
	{
		acScoreRcd.push_back(faceLoop->second->getCmpScore());
		acScoreIdx.push_back(faceLoop->first);;
	}

	float minScore = acScoreRcd[0];
	int   outputIdx = acScoreIdx[0];
	int   clrRcdIdx = 0;

	for (int dwRcdIdx = 1; dwRcdIdx < acScoreRcd.size(); dwRcdIdx++)
	{
		if (minScore > acScoreRcd[dwRcdIdx])
		{
			minScore = acScoreRcd[dwRcdIdx];
			outputIdx = acScoreIdx[dwRcdIdx];
		}
	}

	PartSortInfo item;
	item.score = minScore;
	item.dwId = outputIdx;
	return item;
}

void NameListItem::addMatchFace(boost::shared_ptr<FaceRecoder>& rFaceRcd, float fScore)
{
	boost::mutex::scoped_lock autolocker(mNameListLock);

	rFaceRcd->setCmpScore(fScore);

	if (mNewNameList.size() < g_mdwMatOutputCnt)
	{
		mNewNameList.insert(FaceInfoMap::value_type(rFaceRcd->getFaceSeq(), rFaceRcd));
	}
	else
	{
		//float fscore[MINT_OUTPUT_COUNT];
		//float *buffer = new float[sizeof(MINT_OUTPUT_COUNT)];
		//if (!acScoreRcd.empty())
		//{
		//	memcpy(buffer, &acScoreRcd[0], acScoreRcd.size()*sizeof(float));
		//}

		PartSortInfo item = SortReslut();
		if (fScore > item.score)
		{
			FaceInfoMap::iterator iter = mNewNameList.find(item.dwId);
			if (iter == mNewNameList.end())
			{
				printf("NameListItem::SortReslut unexist item.dwId: %d\n", item.dwId);
			}
			else
			{
				mNewNameList.erase(iter);
			}
			mNewNameList.insert(FaceInfoMap::value_type(rFaceRcd->getFaceSeq(), rFaceRcd));
		}

		//delete []buffer;
	}
}

//void NameListItem::SortMapByValue(FaceInfoMap& tMap, vector<pair<unsigned int, boost::shared_ptr<FaceRecoder>>>& tVector)
//{
//	for (FaceInfoMap::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
//	{
//		tVector.push_back(make_pair(curr->first, curr->second));
//	}
//	sort(tVector.begin(), tVector.end(), Cmp);
//}

//int NameListItem::Cmp(const pair<unsigned int, boost::shared_ptr<FaceRecoder>>& x, const pair<unsigned int, boost::shared_ptr<FaceRecoder>>& y)
//{
//	return x.second->getCmpScore() > y.second->getCmpScore();
//}

FeatureWrapperItem::FeatureWrapperItem(int dwAlgType, float * pfFeature, int dwFeatureLen, string mstrImgPath)
{
    mdwAlgType    = dwAlgType;
    mdwFeatureLen = dwFeatureLen;
    mpfFeaturePtr = pfFeature;
	msImgPath	  = mstrImgPath;
}

FeatureWrapperItem::~FeatureWrapperItem()
{
    if(mpfFeaturePtr != NULL)
    {
        free(mpfFeaturePtr);

        mpfFeaturePtr = NULL;
    }
}

IFaceHandlerFifo::IFaceHandlerFifo()
{
    std::cout << "IFaceHandlerFifo, +IFaceHandlerFifo()" << std::endl;

    //init the read ptr and write ptr to zero, to indicate that the que is 
    //empty
    mdwItemWPtr  = 0;      // work item read ptr
    mdwItemRPtr  = 0;      // work item write ptr

    for (int dwIdx = 0; dwIdx < FACE_HANDLER_FIFO_LEN; dwIdx++)
    {
        mIFaceHandlers[dwIdx] = NULL;
    }
}


IFaceHandlerFifo::~IFaceHandlerFifo()
{
    std::cout << "IFaceHandlerFifo, ~IFaceHandlerFifo()" << std::endl;
}


if_handle_t  IFaceHandlerFifo::GetIFaceHandler(void)
{
    unsigned int dwLeftItems;

    if_handle_t  hFaceHandlerRet = (if_handle_t)NULL;

    boost::mutex::scoped_lock autolocker(mItemFifoLocker);

fifo_empty_chk_loop:

    if(mdwItemWPtr >= mdwItemRPtr)
    {
        dwLeftItems = mdwItemWPtr - mdwItemRPtr; 
    }
    else
    {
        dwLeftItems = mdwItemWPtr
                    + FACE_HANDLER_FIFO_LEN
                    - mdwItemRPtr;
    }

    if(dwLeftItems == 0)
    {
        mFifoNotEmptyCond.wait(autolocker);

        goto fifo_empty_chk_loop;
    }
 
    hFaceHandlerRet =  mIFaceHandlers[mdwItemRPtr];

    //set to -1 to tell other that is handler is null
    mIFaceHandlers[mdwItemRPtr] = (if_handle_t)NULL;

    mdwItemRPtr = ((mdwItemRPtr + 0x1) & (FACE_HANDLER_FIFO_LEN - 1));

    return hFaceHandlerRet;
}


int          IFaceHandlerFifo::ReturnIFaceHandler(if_handle_t hReturn)
{
    unsigned int dwLeftSpace;

    boost::mutex::scoped_lock autoLocker(mItemFifoLocker);

    if(mdwItemWPtr >= mdwItemRPtr)
    {
        dwLeftSpace = mdwItemRPtr 
                    + FACE_HANDLER_FIFO_LEN
                    - mdwItemWPtr;
    }
    else
    {
        dwLeftSpace = mdwItemRPtr - mdwItemWPtr;
    }

    if(dwLeftSpace <= FACE_SNAPER_ITEM_PAD_NUM)
    {
        std::cout << "IFaceHandlerFifo,buffer overflow, drop the current data item" << std::endl;
        return FACE_SNAPER_ERR_OVER_ITEM_BUF;
    }

    if (mIFaceHandlers[mdwItemWPtr] != (if_handle_t)NULL)
    {
        std::cout << "IFaceHandlerFifo: handler override at pos: " << mdwItemWPtr << std::endl;
        return -1;
    }

    mIFaceHandlers[mdwItemWPtr] = hReturn;

    mdwItemWPtr = ((mdwItemWPtr + 0x1) & (FACE_HANDLER_FIFO_LEN - 1));
    //std::cout << "IFaceHandlerFifo,onDataReceived, wptr:" << mdwItemWPtr << std::endl;

    //wakup an thread my wait on the condition
    mFifoNotEmptyCond.notify_one();

    return 0;
}

//constructed function
//strPathCfg:Configuration file path IAlgTest.ini
Performance::Performance(string strPathCfg)
{
    //SP_IFaceHandlerFifo   newDetectHandleFifo(new IFaceHandlerFifo());
    //SP_IFaceHandlerFifo   newExtractHandleFifo(new IFaceHandlerFifo());
    SP_IFaceHandlerFifo   newMysqlHandleFifo(new IFaceHandlerFifo());
    //SP_IFaceHandlerFifo   newCompHandleFifo(new IFaceHandlerFifo());

    //mDetectHandleFifo   = newDetectHandleFifo;
    //mExtractHandleFifo  = newExtractHandleFifo;
    mMysqlHandleFifo    = newMysqlHandleFifo;
    //mCompareHandleFifo  = newCompHandleFifo;

    mStrCfgFile = strPathCfg;

	for (int dwThreadIdx = 0; dwThreadIdx < MAX_PERFORMANCE_THREADS; dwThreadIdx++)
    {
        madwSaveStopReq[dwThreadIdx] = 0;
        madwSaveStopRsp[dwThreadIdx] = 0;
		madwDetectSaveStopReq[dwThreadIdx] = 0;
		madwDetectSaveStopRsp[dwThreadIdx] = 0;
		madwExtractSaveStopReq[dwThreadIdx] = 0;
		madwExtractSaveStopRsp[dwThreadIdx] = 0;
		madwCompareStatus[dwThreadIdx] = 0;
		madwExtractStatus[dwThreadIdx] = 0;
		madwDetectStatus[dwThreadIdx] = 0;
    }

	mdwDetectHandleCnt = 0;
    mdwExtractHandleCnt = 0;
    mdwMysqlHandleCnt   = 0;
    mdwProcThreadCnt   = 0;

	mdwOldDetect    = 0;
	mdwNickleDetect	= 0;
	mdwVjboostDetect = 0;
	mdwMtcnnDetect = 1; //默认使用Eagle-Mtcnn检测器

	mdwNofaceProcMode = 0;
	mdwSaveDetectPic  = 0;
	mdwWriteToMysql = 0;
	mdwReadFromMysql = 0;
	mdwSaveMulFace = 0;
	mdwSaveResultPic = 0;
	mdwSaveFeatureBin = 0;
	mdwSearchStep = 600;
	mdwWriteInputToMysql = 0;
	mdwSingleThread = 0;

	mdwStartId = 0;
	mdwEndId = 0;
	mdwNowId = 0;
	mdwInputFaceSeq = 0;
	mdwQueryFaceSeq = 0;
	mdwOutputCount = 10;

	dwPendNameListCnt = 0;
	dwRcvdNameListCnt = 0;
	dwDetectPendingCnt = 0;
	dwDetectFinishedCnt = 0;
	dwExtractPendingCnt = 0;
	dwExtractFinishedCnt = 0;
	dwDetectItemRPtr = 0;      // work item read ptr
	dwDetectItemWPtr = 0;      // work item write ptr
	dwExtractItemRPtr = 0;      // work item read ptr
	dwExtractItemWPtr = 0;      // work item write ptr
	mdwFaceFeatureLen = 128;
	timeExtractAccTicks = 0;
	mdwExtractCountAcc = 0;
	mdwExtractCountOnce = 0;
	mdwExtractCountTimes = 0;
	mdwSaveResultToMysql = 0;

    mStrProxySrvFlag   = "ifsrc";
    mStrProxySrvAddr   = "127.0.0.1";
	mStrMysqlSrvAddr = "127.0.0.1";
	mdwMysqlSrvPort = 3306;
	mStrMysqlSrvUser = "root";
	mStrMysqlSrvPswd = "introcks1234";

    mfFatureThresHold  = (float)0.92;

    mdwAlgVersions    = 4;

    mStrFaceDbName     = "intellif_static";
    mStrFaceTables     = "t_cid_detail";

	mStrPathInput = "../img_input";
	mStrPathQuery = "../img_namelist";
	mStrPathOut = "../img_output";

	mStrModelPath = "model/gluon";

	//detect init
	mdwDetectType = E_IFaceRecType::IFACEREC_DETECTOR;
}

Performance::~Performance()
{
	/***
	std::cout << "Performance, ~Performance()" << std::endl;
	for (int dwHandleIdx = 0; dwHandleIdx < mdwDetectHandleCnt; dwHandleIdx++)
	{
		if_handle_t hDetectHandle = mDetectHandleFifo->GetIFaceHandler();

		if (hDetectHandle == NULL)
		{
			std::cout << "Can not destroy detect handle: " << std::endl;
			continue;
		}

		CascadeDetectManager::DestoryIfDetectoHandle(hDetectHandle);
	}
	***/
	for (int dwHandleIdx = 0; dwHandleIdx < mdwMysqlHandleCnt; dwHandleIdx++)
	{
		if_handle_t hMysqlHandle = mMysqlHandleFifo->GetIFaceHandler();

		if (hMysqlHandle == NULL)
		{
			std::cout << "Can not destroy mysql handle: " << std::endl;
			continue;
		}

		mysql_close((MYSQL *)hMysqlHandle);
	}
}

//load config file
int Performance::LoadConfigInformation(void)
{
    int dwRet = 0;

    mysql_library_init(0, NULL, NULL);
	char  cMysqlReconnFlag = 0x1;
    curl_global_init(CURL_GLOBAL_ALL);
	FaceRecSDKManager::Init(".");
    
    if(mStrCfgFile.c_str() == NULL)
    {
		std::cout << "LoadConfigInformation::Error: no config file cofiged:" << std::endl;
        return 0;
    }

    boost::shared_ptr<IniFile>  pIniFile(new IniFile());
    dwRet = pIniFile->openini(mStrCfgFile,IFACE_INI_PARAM_TYPE_NAME);
    if(dwRet != 0)
    {
        std::cout <<"LoadConfigInformation::Load ini file failed: " << mStrCfgFile.c_str() << std::endl;
        return -1;
    }
    mpIniFile = pIniFile;
    
	TryReadAndUpateCfgStr(mpIniFile, "global", "inputpath", mStrPathInput);
	TryReadAndUpateCfgStr(mpIniFile, "global", "querypath", mStrPathQuery);
	TryReadAndUpateCfgStr(mpIniFile, "global", "outpath",   mStrPathOut);
	if (!exists(path(mStrPathOut)))
	{
		if (!create_directories(path(mStrPathOut)))
		{
			std::cout << "LoadConfigInformation::Create folder failed:" << mStrPathOut.c_str()
				<< ", Please check the FS Permession settings" << std::endl;
		}
	}

	TryReadAndUpateCfgInt(mpIniFile, "schedule", "beginid",		mdwStartId);
	TryReadAndUpateCfgInt(mpIniFile, "schedule", "endupid",		mdwEndId);
	TryReadAndUpateCfgInt(mpIniFile, "schedule", "procsteps",	mdwSearchStep);

	mdwNowId = mdwStartId;

    TryReadAndUpateCfgStr(mpIniFile, "algparam",    "modelpath",		mStrModelPath);
    TryReadAndUpateCfgflt(mpIniFile, "algparam",    "threshold",		mfFatureThresHold);
	TryReadAndUpateCfgInt(mpIniFile, "algparam",    "versions",			mdwAlgVersions);
	TryReadAndUpateCfgInt(mpIniFile, "algparam",    "featurelenth",     mdwFaceFeatureLen);
	TryReadAndUpateCfgInt(mpIniFile, "algparam",    "flipfeature",      mdwFlipExtratFeature);

	g_mdwAlgVersion = mdwAlgVersions;
    g_strAlgModelPath  = mStrModelPath;
	g_mdwAlgFeatureLen = mdwFaceFeatureLen;
	g_flipExtratFeature = mdwFlipExtratFeature;

    TryReadAndUpateCfgInt(mpIniFile, "performance", "detectthreads",  mdwDetectHandleCnt);
    TryReadAndUpateCfgInt(mpIniFile, "performance", "extractthreads", mdwExtractHandleCnt);
    TryReadAndUpateCfgInt(mpIniFile, "performance", "mysqlthreads",   mdwMysqlHandleCnt);
    TryReadAndUpateCfgInt(mpIniFile, "performance", "procthreads",    mdwProcThreadCnt);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "saveresultpic",  mdwSaveResultPic);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "savefeaturebin", mdwSaveFeatureBin);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "savemulface",    mdwSaveMulFace);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "writetomysql",   mdwWriteToMysql);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "readfrommysql",  mdwReadFromMysql);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "outputcount",    mdwOutputCount);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "querytomysql",   mdwWriteInputToMysql);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "nofaceprocess",  mdwNofaceProcMode);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "saveDetectPic",  mdwSaveDetectPic);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "saveresulttomysql", mdwSaveResultToMysql);
	
	g_mdwMatOutputCnt = mdwOutputCount;

	if (mdwWriteToMysql || mdwWriteInputToMysql)
	{
		g_mdwRefreshMode = 1;
		mdwReadFromMysql = 0;//write default not read
	}
    
	//检测器
	TryReadAndUpateCfgInt(mpIniFile, "performance", "cascaded_det_oldalg", mdwOldDetect);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "cascaded_det_nickle", mdwNickleDetect);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "cascaded_det_vjboost", mdwVjboostDetect);
	TryReadAndUpateCfgInt(mpIniFile, "performance", "cascaded_det_mtcnn", mdwMtcnnDetect);
	if (mdwOldDetect)
	{
		g_vecIntDetect.push_back(E_IFaceRecType::IFACEREC_DETECTOR);
		mdwDetectType = E_IFaceRecType::IFACEREC_DETECTOR;
	}
	if (mdwNickleDetect)
	{
		g_vecIntDetect.push_back(E_IFaceRecType::IFACEREC_DETECTOR_NICKLE);
		mdwDetectType = E_IFaceRecType::IFACEREC_DETECTOR_NICKLE;
	}
	if (mdwVjboostDetect)
	{
		g_vecIntDetect.push_back(E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST);
		mdwDetectType = E_IFaceRecType::IFACEREC_DETECTOR_VJBOOST;
	}
	if (mdwMtcnnDetect)
	{
		g_vecIntDetect.push_back(E_IFaceRecType::IFACEREC_DETECTOR_EAGLE);
		mdwDetectType = E_IFaceRecType::IFACEREC_DETECTOR_EAGLE;
		EagleMtcnnDetectManager::StaticInit();
	}

    TryReadAndUpateCfgStr(mpIniFile, "database",    "addrip",      mStrMysqlSrvAddr);
    TryReadAndUpateCfgInt(mpIniFile, "database",    "port",        mdwMysqlSrvPort);
    TryReadAndUpateCfgStr(mpIniFile, "database",    "user",        mStrMysqlSrvUser);
    TryReadAndUpateCfgStr(mpIniFile, "database",    "password",    mStrMysqlSrvPswd);
    TryReadAndUpateCfgStr(mpIniFile, "database",    "dbname",      mStrFaceDbName);
    TryReadAndUpateCfgStr(mpIniFile, "database",    "tablename",   mStrFaceTables);

#pragma region mysqlhandle
    for(int dwHandleIdx = 0; dwHandleIdx < mdwMysqlHandleCnt; dwHandleIdx++)
    {
        MYSQL *  newMysqlHandle = mysql_init(NULL);

        if (newMysqlHandle == NULL)
        {
            std::cout << "LoadConfigInformation, init mysql handle is null" << std::endl;
            continue;
        }
		else
		{
			//init the options of mysql handler
			mysql_options(newMysqlHandle, MYSQL_SET_CHARSET_NAME, "utf8");
			mysql_options(newMysqlHandle, MYSQL_OPT_RECONNECT, &cMysqlReconnFlag);
		}

        if (mysql_real_connect(newMysqlHandle,
                               mStrMysqlSrvAddr.c_str(),
                               mStrMysqlSrvUser.c_str(),
                               mStrMysqlSrvPswd.c_str(),
                               "mysql",
                               mdwMysqlSrvPort,
                               0,
                               0) != NULL)
        {
            std::cout << "success connect, id:" << dwHandleIdx
                      << ", ip:"       << mStrMysqlSrvAddr
                      << ", usr-name:" << mStrMysqlSrvUser
                      << ", usr-pwd:"  << mStrMysqlSrvPswd
                      << ", db-name:"  << "mysql"
                      << ", db-port:"  << mdwMysqlSrvPort << std::endl;

        }
        else
        {
            std::cout << "failed connect, id:" << dwHandleIdx
                      << ", ip:"       << mStrMysqlSrvAddr
                      << ", usr-name:" << mStrMysqlSrvUser
                      << ", usr-pwd:"  << mStrMysqlSrvPswd
                      << ", db-name:"  << "mysql"
                      << ", db-port:"  << mdwMysqlSrvPort << std::endl;
        }

        mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)newMysqlHandle);
    }
#pragma endregion
    return 0;
}

//检测人脸特征值
int Performance::DetectAndExtractFace(const if_handle_t hDetectHandle, const if_handle_t hExtractHandle, const std::string &picturePath, float * & pfFeatureWrap)
{
	bool bLocalUrl = true;

	TFeatureSeqIdInfo tSeqIdInfo;
	tSeqIdInfo.dwSwSequence = 0;
	tSeqIdInfo.dwTrackId = 0;

	//区分是mysql还是本地路径
	Mat matLoad;
	if (picturePath.find("http:") < picturePath.length())
	{
		bLocalUrl = false;
		string strDownLoad;
		bool bSuccess = decodeRemotePicCurl(picturePath, strDownLoad);
		if (!bSuccess)
		{
			std::cout << "DetectAndExtractFace, download : " << picturePath.c_str() << ", failed\n" << std::endl;
			return -1;
		}
		std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
		matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);
	}
	else
	{
		matLoad = imread(picturePath);
	}

	if (matLoad.data == NULL)
	{
		std::cout << "DetectAndExtractFace::jpg load failed:  " << picturePath.c_str();
		return -1;
	}
#pragma region facedetect
	PIF_FACERECT   pface = NULL;
	unsigned int   count = 0;

	if (hDetectHandle == 0)
	{
		printf("DetectAndExtractFace::hDetectHandle invalid\n");
		return -1;
	}

	/*CascadeDetectManager::FeedOneFramToDetector(hDetectHandle,
		matLoad,
		matLoad.cols,
		matLoad.rows,
		mdwDetectType,
		&pface,
		&count);*/
	EagleMtcnnDetectManager::FeedOneFramToDetector(hDetectHandle,
		matLoad,
		matLoad.cols,
		matLoad.rows,
		&pface,
		&count);

	std::cout << "Performance::DetectAndExtractFace,face cnt:" << count << ",rect:" << pface[0].Rect.left << "," << pface[0].Rect.top << "," << pface[0].Rect.right << "," << pface[0].Rect.bottom << endl;

	FACE_RST_TYPE  tOneFaceRect;
	if (count == 0)
	{
#pragma region SaveInputPic
		if (mdwSaveDetectPic)
		{
			path  tempPath = path(mStrPathOut) / "noface";
			path  dstFileOrg = tempPath / path(picturePath).filename();
			if (!exists(tempPath))
			{
				if (!create_directories(tempPath))
				{
					std::cout << "LoadAndGerateQueryList::Create folder failed:" << tempPath.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					return -1;
				}
			}
			cout << "LoadAndGerateQueryList, org img path=" << picturePath.c_str() << "##and save noface path=" << dstFileOrg.string().c_str() << std::endl;

			tOneFaceRect.Rect.left = 0;
			tOneFaceRect.Rect.top = 0;
			tOneFaceRect.Rect.right = matLoad.cols;
			tOneFaceRect.Rect.bottom = matLoad.rows;
			SaveDetectFace2Disk(matLoad, tOneFaceRect.Rect, dstFileOrg.string());
		}
#pragma endregion
		if (mdwNofaceProcMode == 1)
		{
			count = 1;
			tOneFaceRect.Rect.left = 0;
			tOneFaceRect.Rect.top = 0;
			tOneFaceRect.Rect.right = matLoad.cols;
			tOneFaceRect.Rect.bottom = matLoad.rows;
		}
		else
		{
			printf("DetectAndExtractFace::%s, no face found\n", picturePath.c_str());
			return -1;
		}
	}
	else if (count == 1)
	{
		tOneFaceRect = pface[0];
		//CascadeDetectManager::ReleaseDetFaceResult(mdwDetectType, pface, count);
		EagleMtcnnDetectManager::ReleaseDetFaceResult(pface, count);
	}
#pragma region mulfaceprocee
	else //select the biggest
	{
		printf("DetectAndExtractFace, detect face count > 1 process start;\n");

		int dwMaxFaceSize = 0;
		int maxFaceIndex = 0;
		for (int dwFaceIdx = 0; dwFaceIdx < count; dwFaceIdx++)
		{
			if ((pface[dwFaceIdx].Rect.left < 0)
				|| (pface[dwFaceIdx].Rect.top  < 0)
				|| (pface[dwFaceIdx].Rect.right  > matLoad.cols)
				|| (pface[dwFaceIdx].Rect.bottom > matLoad.rows))
			{
				printf("DetectAndExtractFace::Detect face count > 1:roi retc out of range\t\t\t\t\n");
				//进行规避处理 pengjidong 20170808
				pface[dwFaceIdx].Rect.left = pface[dwFaceIdx].Rect.left < 0 ? 0 : pface[dwFaceIdx].Rect.left;
				pface[dwFaceIdx].Rect.top = pface[dwFaceIdx].Rect.top  < 0 ? 0 : pface[dwFaceIdx].Rect.top;
				pface[dwFaceIdx].Rect.right = (pface[dwFaceIdx].Rect.right > matLoad.cols) ? matLoad.cols : pface[dwFaceIdx].Rect.right;
				pface[dwFaceIdx].Rect.bottom = (pface[dwFaceIdx].Rect.bottom > matLoad.rows) ? matLoad.rows : pface[dwFaceIdx].Rect.bottom;
			}

			int lenth = pface[dwFaceIdx].Rect.right - pface[dwFaceIdx].Rect.left;
			int width = pface[dwFaceIdx].Rect.bottom - pface[dwFaceIdx].Rect.top;
			if (lenth > 0 && width > 0)
			{
				int faceSize = lenth * width;
				if (dwMaxFaceSize < faceSize)
				{
					dwMaxFaceSize = faceSize;
					maxFaceIndex = dwFaceIdx;
				}
			}
		} // end of for dwFaceIdx
		printf("DetectAndExtractFace, detect face count > 1 process end;index=%d!!!!\n", maxFaceIndex);
		tOneFaceRect = pface[maxFaceIndex];
		//CascadeDetectManager::ReleaseDetFaceResult(mdwDetectType, pface, count);
		EagleMtcnnDetectManager::ReleaseDetFaceResult(pface, count);
#pragma region saveorgface
		//保存原图
		if (mdwSaveMulFace)
		{
			cout << "######DetectAndExtractFace, mdwSaveMulFace = 1######" << std::endl;
			std::string strBgImgName = basename(path(picturePath));
			std::ostringstream ossImgName;
			ossImgName << strBgImgName
				<< "_" << tOneFaceRect.Rect.left
				<< "_" << tOneFaceRect.Rect.top
				<< "_" << (tOneFaceRect.Rect.right - tOneFaceRect.Rect.left)
				<< "_" << (tOneFaceRect.Rect.bottom - tOneFaceRect.Rect.top)
				<< ".jpg";
			path  tempPath(mStrPathOut);
			path  orgImgPath = tempPath / "mulface";
			path  saveDirectOrg = orgImgPath / strBgImgName;
			path  dstFileOrg = saveDirectOrg / ossImgName.str();
			cout << "DetectAndExtractFace, org img path=" << picturePath.c_str() << "##and save mulface path=" << dstFileOrg.string().c_str() << std::endl;
			SaveDetectFace2Disk(matLoad, tOneFaceRect.Rect, dstFileOrg.string());
		}
#pragma endregion
	}
	
	std::cout << "Performance::DetectAndExtractFace,save face_rect begin!!!:" << endl;
	// 保存人像框图片
	/*std::string strBgImgName = basename(path(picturePath));
	std::ostringstream ossImgName;
	ossImgName << strBgImgName
	<< "_" << tOneFaceRect.Rect.left
	<< "_" << tOneFaceRect.Rect.top
	<< "_" << (tOneFaceRect.Rect.right - tOneFaceRect.Rect.left)
	<< "_" << (tOneFaceRect.Rect.bottom - tOneFaceRect.Rect.top)
	<< ".jpg";
	path  tempPath(mStrPathOut);
	path  orgImgPath = tempPath / "face";
	path  saveDirectOrg = orgImgPath / strBgImgName;
	path  dstFileOrg = saveDirectOrg / ossImgName.str();
	SaveDetectFace2Disk(matLoad, tOneFaceRect.Rect, dstFileOrg.string());*/


#pragma endregion
#pragma endregion
#pragma region faceextract
	//if_handle_t hExtractHandle = mExtractHandleFifo->GetIFaceHandler();

	if (hExtractHandle == 0)
	{
		printf("DetectAndExtractFace, Feather extrater, but verify handle is null\n");
		return -1;
	}

	float *     pfFeatureRst = NULL;
	int         flen = 0;

	/*flen = IfMutilAlgExtManger::ExtratFeatureFromImage(hExtractHandle,
		matLoad,
		tOneFaceRect.Rect,
		&pfFeatureRst,
		MutilAlgExtractor::TARGS_EXTRA_TYPE_FACE,
		mdwAlgVersions,
		tSeqIdInfo,
		0);*/
	T_IFaceRecRect tFaceRect;
	tFaceRect.left = tOneFaceRect.Rect.left;
	tFaceRect.top = tOneFaceRect.Rect.top;
	tFaceRect.right = tOneFaceRect.Rect.right;
	tFaceRect.bottom = tOneFaceRect.Rect.bottom;

	flen = FaceRecSDKManager::ExtratGluonFeature(hExtractHandle, matLoad, tFaceRect, &pfFeatureRst);

	if (pfFeatureRst == NULL || flen == 0)
	{
		std::cout << "DetectAndExtractFace, alg type: " << mdwAlgVersions << ",path:" << picturePath.c_str() << " ,extract failed" << std::endl;
		return -1;
	}
#pragma endregion

	pfFeatureWrap = (float *)malloc(flen * sizeof(float));

	if (pfFeatureWrap == NULL)
	{
		std::cout << "DetectAndExtractFace, Malloc Feature failed, path: " << picturePath.c_str() << std::endl;
		return -1;
	}

	memcpy(pfFeatureWrap, pfFeatureRst, flen * sizeof(float));

	if ((flen > 0) && (pfFeatureRst != NULL))
	{
		//IfMutilAlgExtManger::ReleaseFeautreResult(pfFeatureRst, mdwAlgVersions);
		FaceRecSDKManager::ReleaseFeautreResult(pfFeatureRst);
	}

	std::string strFileName = basename(picturePath);

#ifdef _MSC_VER
	std::string strFeaturePath = path(picturePath).parent_path().string() + "\\" + strFileName + ".bin";
#else
	std::string strFeaturePath = path(picturePath).parent_path().string() + "/" + strFileName + ".bin";
#endif

	cout << "feature bin path=" << strFeaturePath.c_str() << std::endl;

	boost::filesystem::path pathOneFeature(strFeaturePath);

#pragma region SaveQueryFeatures
	if (mdwSaveFeatureBin && bLocalUrl)
	{
		boost::filesystem::path pathSave2Dir = pathOneFeature.branch_path();
		if (!exists(pathSave2Dir))
		{
			try{
				if (!create_directories(pathSave2Dir))
				{
					std::cout << "DetectAndExtractFace::Create folder failed:" << pathSave2Dir.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					return -1;
				}
			}
			catch (...)
			{
				std::cout << "DetectAndExtractFace::Create folder failed:" << pathSave2Dir.string().c_str()
					<< ", Please check the FS Permession settings" << std::endl;
				return -1;
			}
		}

		std::ofstream fsFeatureSave(strFeaturePath, std::ios::out | std::ios::binary);
		if (!fsFeatureSave.is_open())
		{
			std::cout << "DetectAndExtractFace::Open write file failed: " << strFeaturePath;
		}
		else
		{
			int32_t dwSaveFileLen = flen * sizeof(float);
			fsFeatureSave.write((char *)pfFeatureWrap, dwSaveFileLen);
			fsFeatureSave.close();
		}
	}
#pragma endregion

	return flen;
}

int Performance::LoadAndGerateQueryList(void)
{
	if (mStrPathQuery.c_str() == NULL)
	{
		std::cout << "LoadAndGerateQueryList::Query dir path is null!!" << std::endl;
		return -1;
	}

	boost::filesystem::path pathInput(mStrPathQuery);
	if (!exists(pathInput))
	{
		std::cout << "LoadAndGerateQueryList::Query dir path not exists::" << mStrPathQuery.c_str() << std::endl;
		return -1;
	}

	if (!is_directory(pathInput))
	{
		printf("LoadAndGerateQueryList::%s not a directory\n", mStrPathQuery.c_str());
		return -1;
	}

	//if_handle_t hDetectHandle = CascadeDetectManager::CreateIfDetectorHandle(0);
	//if_handle_t hExtractHandle = IfMutilAlgExtManger::CreateIfExtratorHandle(mStrModelPath);
	//if_handle_t hDetectHandle = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_DETECTOR_EAGLE, "model/eagle");
	if_handle_t hDetectHandle = EagleMtcnnDetectManager::CreateIfDetectorHandle(0);
	if_handle_t hExtractHandle = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_EXTRACTOR_GLUON, "model/gluon");

	recursive_directory_iterator end_iter;

	for (recursive_directory_iterator iter(pathInput); iter != end_iter; iter++)
	{
		printf("LoadAndGerateQueryList::file_idx: %d, find: %s\t\t\t\t\n", mdwQueryFaceSeq, iter->path().string().c_str());
		fprintf(pfResustFile, "LoadAndGerateQueryList::file_idx: %d, find: %s\t\t\t\t\n", mdwQueryFaceSeq, iter->path().string().c_str());

		if (is_directory(*iter))
		{
			continue;
		}

		std::string strExtion = extension(*iter);
		boost::to_lower(strExtion);
		if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png")
		{
			continue;
		}

		std::string strFileName = basename(iter->path().string());

#ifdef _MSC_VER
		std::string strFeaturePath = mStrPathQuery + "\\" + strFileName + ".bin";
#else
		std::string strFeaturePath = mStrPathQuery + "/" + strFileName + ".bin";
#endif

		boost::filesystem::path pathOneFeature(strFeaturePath);

		if (exists(pathOneFeature))
		{
			std::ifstream fsFeatureIn(pathOneFeature.string(), std::ios::in | std::ios::binary);
			if (fsFeatureIn.is_open())
			{
				std::string   binFeatureIn((std::istreambuf_iterator<char>(fsFeatureIn)),
					std::istreambuf_iterator<char>());

				size_t dwMallocFeatureLen = binFeatureIn.size();

				if ((dwMallocFeatureLen & 0x3) != 0)
				{
					std::cout << "LoadAndGerateQueryList::Read Feature Len error, check it out" << std::endl;
					continue;
				}

				float * pfFeatureIn = (float *)malloc(dwMallocFeatureLen);

				if (pfFeatureIn == NULL)
				{
					std::cout << "LoadAndGerateQueryList::Malloc Feature failed, path: " << pathOneFeature.string().c_str() << std::endl;
					continue;
				}

				memcpy(pfFeatureIn, binFeatureIn.c_str(), dwMallocFeatureLen);

				addToNameList((pathOneFeature.stem().string() + ".jpg"), "", 0, mdwQueryFaceSeq, pfFeatureIn, (int)(dwMallocFeatureLen >> 2));
			}
			else
			{
				std::cout << "LoadAndGerateQueryList::Open feature file failed: " << pathOneFeature.string().c_str() << std::endl;
				continue;
			}
		}
		else
		{
			float * pfFeatureWrap = NULL;
			int flen = DetectAndExtractFace(hDetectHandle, hExtractHandle, iter->path().string(), pfFeatureWrap);

			printf("feature len: %d\n", flen);

			for (int dwFeatureIdx = 0; dwFeatureIdx < flen; dwFeatureIdx++)
			{
				if ((dwFeatureIdx & 0x7) == 7)
				{
					printf("%f,\n", pfFeatureWrap[dwFeatureIdx]);
				}
				else
				{
					printf("%f,", pfFeatureWrap[dwFeatureIdx]);
				}
			}

			if (flen <= 0 || pfFeatureWrap == NULL)
			{
				printf("LoadAndGerateQueryList::DetectAndExtractFace end,pfFeature=NULL,len=%d\n", flen);
				continue;
			}

			addToNameList(iter->path().string(), "", 0, mdwQueryFaceSeq, pfFeatureWrap, flen);
		}
		mdwQueryFaceSeq += 0x1;
	}

	//IfMutilAlgExtManger::DestoryIfExtratorHandle(hExtractHandle);
	//CascadeDetectManager::DestoryIfDetectoHandle(hDetectHandle);
	FaceRecSDKManager::DestoryIfHandle(hExtractHandle);
	//FaceRecSDKManager::DestoryIfHandle(hDetectHandle);
	EagleMtcnnDetectManager::DestoryIfDetectoHandle(hDetectHandle);

	return 0;
}

int Performance::LoadAndGerateInputList(SP_FaceUrlFeatureList & rSpUrlFeature)
{
	if (mStrPathInput.c_str() == NULL)
	{
		std::cout << "LoadAndGerateInputList::Input dir path is null!!" << std::endl;
		return -1;
	}

	boost::filesystem::path pathInput(mStrPathInput);
	if (!exists(pathInput))
	{
		std::cout << "LoadAndGerateInputList::Input dir path not exists::" << mStrPathInput.c_str() << std::endl;
		return -1;
	}

	if (!is_directory(pathInput))
	{
		printf("LoadAndGerateInputList::%s not a directory\n", mStrPathInput.c_str());
		return -1;
	}

	//if_handle_t hDetectHandle = CascadeDetectManager::CreateIfDetectorHandle(0);
	//if_handle_t hExtractHandle = IfMutilAlgExtManger::CreateIfExtratorHandle(mStrModelPath);
	//if_handle_t hDetectHandle = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_DETECTOR_EAGLE, "model/eagle");
	if_handle_t hDetectHandle = EagleMtcnnDetectManager::CreateIfDetectorHandle(0);
	if_handle_t hExtractHandle = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_EXTRACTOR_GLUON, "model/gluon");

	recursive_directory_iterator end_iter;

	for (recursive_directory_iterator iter(pathInput); iter != end_iter; iter++)
	{
		printf("LoadAndGerateInputList::file_idx: %d, find: %s\t\t\t\t\n", mdwInputFaceSeq, iter->path().string().c_str());
		fprintf(pfResustFile, "LoadAndGerateInputList::file_idx: %d, find: %s\t\t\t\t\n", mdwInputFaceSeq, iter->path().string().c_str());

		if (is_directory(*iter))
		{
			continue;
		}

		std::string strExtion = extension(*iter);
		boost::to_lower(strExtion);
		if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png")
		{
			continue;
		}

		std::string strFileName = basename(iter->path().string());

#ifdef _MSC_VER
		std::string strFeaturePath = mStrPathInput + "\\" + strFileName + ".bin";
#else
		std::string strFeaturePath = mStrPathInput + "/" + strFileName + ".bin";
#endif

		boost::filesystem::path pathOneFeature(strFeaturePath);

		if (exists(pathOneFeature))
		{
			std::ifstream fsFeatureIn(pathOneFeature.string(), std::ios::in | std::ios::binary);
			if (fsFeatureIn.is_open())
			{
				std::string   binFeatureIn((std::istreambuf_iterator<char>(fsFeatureIn)),
					std::istreambuf_iterator<char>());

				size_t dwMallocFeatureLen = binFeatureIn.size();

				if ((dwMallocFeatureLen & 0x3) != 0)
				{
					std::cout << "LoadAndGerateInputList::Read Feature Len error, check it out" << std::endl;
					continue;
				}

				float * pfFeatureIn = (float *)malloc(dwMallocFeatureLen);

				if (pfFeatureIn == NULL)
				{
					std::cout << "LoadAndGerateInputList::Malloc Feature failed, path: " << pathOneFeature.string().c_str() << std::endl;
					continue;
				}

				memcpy(pfFeatureIn, binFeatureIn.c_str(), dwMallocFeatureLen);

				SP_FeatureWrapperItem pNewFeature(new FeatureWrapperItem(mdwAlgVersions,
																			pfFeatureIn,
																			(int)(dwMallocFeatureLen >> 2),
																			(pathOneFeature.stem().string() + ".jpg")));

				
				SP_FaceUrlFeatureItem pNewUrlFeature(new FaceUrlFeatureItem());
				pNewUrlFeature->mSpFaceFeature = pNewFeature;
				pNewUrlFeature->mdwFaceId = 0;
				pNewUrlFeature->mStrFaceUrl = iter->path().string();
				pNewUrlFeature->mdwFaceCidId = 0;
				pNewUrlFeature->mdwFaceSeq = mdwInputFaceSeq;
				rSpUrlFeature->mListUrlFeature.push_back(pNewUrlFeature);
			}
			else
			{
				std::cout << "LoadAndGerateInputList::Open feature file failed: " << pathOneFeature.string().c_str() << std::endl;
				continue;
			}
		}
		else
		{
			float * pfFeatureWrap = NULL;
			int flen = DetectAndExtractFace(hDetectHandle, hExtractHandle, iter->path().string(), pfFeatureWrap);
			if (flen <= 0 || pfFeatureWrap == NULL)
			{
				printf("LoadAndGerateInputList::DetectAndExtractFace end,pfFeature=NULL,len=%d\n", flen);
				continue;
			}

			SP_FeatureWrapperItem pNewFeature(new FeatureWrapperItem(mdwAlgVersions, pfFeatureWrap, flen, iter->path().string()));
			SP_FaceUrlFeatureItem pNewUrlFeature(new FaceUrlFeatureItem());
			pNewUrlFeature->mSpFaceFeature = pNewFeature;
			pNewUrlFeature->mdwFaceId = 0;
			pNewUrlFeature->mStrFaceUrl = iter->path().string();
			pNewUrlFeature->mdwFaceCidId = 0;
			pNewUrlFeature->mdwFaceSeq = mdwInputFaceSeq;
			rSpUrlFeature->mListUrlFeature.push_back(pNewUrlFeature);
		}
		mdwInputFaceSeq += 0x1;
		printf("#### mdwFaceSeq=%d ####\n", mdwInputFaceSeq);
	}

	//IfMutilAlgExtManger::DestoryIfExtratorHandle(hExtractHandle);
	//CascadeDetectManager::DestoryIfDetectoHandle(hDetectHandle);
	FaceRecSDKManager::DestoryIfHandle(hExtractHandle);
	//FaceRecSDKManager::DestoryIfHandle(hDetectHandle);
	EagleMtcnnDetectManager::DestoryIfDetectoHandle(hDetectHandle);

	return 0;
}

void Performance::addToNameList(const string& rImgPath,
								const string& rImgTable,
								int64_t	   dwImgCidIdx,
								int           dwImgIdx,
								float *       pfFeature,
								int           dwFeatureLen)
{
	boost::mutex::scoped_lock autolocker(mStatuIniLock);

	boost::shared_ptr<NameListItem> newItem(new NameListItem(rImgPath, rImgTable, dwImgCidIdx, pfFeature, dwFeatureLen));

	mNameList.insert(NameListMap::value_type(dwImgIdx, newItem));
}

//performance loop
int Performance::DoPerformanceLoop(int dwThreadIdx)
{
	madwCompareStatus[dwThreadIdx] = 0;

	mCompareHanle[dwThreadIdx] = (if_handle_t)0xffffffff;

    while (1)
    {
		printf("DoPerformanceLoop start, dwThreadIdx=%d\n", dwThreadIdx);
		fprintf(pfResustFile, "DoPerformanceLoop start, dwThreadIdx=%d\n", dwThreadIdx);
        if (madwSaveStopReq[dwThreadIdx] != madwSaveStopRsp[dwThreadIdx])
        {
            madwSaveStopRsp[dwThreadIdx] += 0x1;
            printf("DoPerformanceLoop thread: %d, exist\n", dwThreadIdx);
			madwCompareStatus[dwThreadIdx] = 1;
            break;
        }

		if (mCompareHanle[dwThreadIdx] == 0)
		{
			printf("Performance::DoPerformanceLoop mCompareHanle is null!!! \n");
			continue;
		}
		
		int mdwScopeStart = 0;
		int mdwScopeEndup = 0;
		int dwScopeRet = GetOneIdScope(mdwScopeStart, mdwScopeEndup);
		fprintf(pfResustFile, "Performance::DoPerformanceLoop::dwThreadIdx=%d,startid=%d,endid=%d\n", dwThreadIdx, mdwScopeStart, mdwScopeEndup);
        if(mdwScopeEndup <= mdwStartId)
        {
            continue;
        }

        if(dwScopeRet != 0)
        {
			madwSaveStopRsp[dwThreadIdx] += 0x1;
			printf("DoPerformanceLoop, dwScopeRet != 0, idx: %d ,startid=%d,endid=%d\n", dwThreadIdx, mdwScopeStart, mdwScopeEndup);
			madwCompareStatus[dwThreadIdx] = 1;
            break;
        }

        SP_FaceUrlFeatureList rSpUrlFeature(new FaceUrlFeatureList());
//#pragma region WriteFeatureToMysql
//		if (mdwWriteToMysql)
//		{
//#pragma region writeinputtomysql
//			if (mdwWriteInputToMysql)
//			{
//				//将input图片写入数据库中 单线程
//				printf("DoPerformanceLoop write to mysql start, dwThreadIdx=%d\n", dwThreadIdx);
//				//boost::mutex::scoped_lock autoLocker(mTargetNameLock);
//				mTargetNameLock.lock();
//				if (mdwSingleThread)
//				{
//					printf("DoPerformanceLoop write to mysql end..,mdwSingleThread = 1, dwThreadIdx=%d\n", dwThreadIdx);
//					mTargetNameLock.unlock();
//					madwCompareStatus[dwThreadIdx] = 1;
//					break;
//				}
//				GetFaceUrlFromFolder(rSpUrlFeature);
//				WriteFaceFeature(rSpUrlFeature);
//				mdwSingleThread = 1;
//				mTargetNameLock.unlock();
//				printf("DoPerformanceLoop write to mysql end.., dwThreadIdx=%d\n", dwThreadIdx);
//				madwCompareStatus[dwThreadIdx] = 1;
//				break;
//			}
//#pragma endregion
//			else
//			{
//				//更新mysql的特征值
//				printf("GetFaceUrlFromMysql and WriteFaceFeature###start, dwThreadIdx = %d", dwThreadIdx);
//				GetFaceUrlFromMysql(mdwScopeStart, mdwScopeEndup, mdwAlgVersions, rSpUrlFeature);
//				WriteFaceFeature(rSpUrlFeature);
//				printf("GetFaceUrlFromMysql and WriteFaceFeature###end, dwThreadIdx = %d", dwThreadIdx);
//			}
//		}
//#pragma endregion
#pragma region ReadFeatureFromMysql
		//else if (mdwReadFromMysql)
		if (mdwReadFromMysql)
		{
			//std::cout << "######## Read feature From Mysql ########" << std::endl;
			GetFaceFeatureFromMysql(mdwScopeStart, mdwScopeEndup, mdwAlgVersions, rSpUrlFeature);
			ProcFaceUrlsAndFeature(rSpUrlFeature);
		}
		else
		{
			//比较本地input和namelist 单线程
			//boost::mutex::scoped_lock autoLocker(mTargetNameLock);
			mTargetNameLock.lock();
			if (mdwSingleThread)
			{
				printf("DoPerformanceLoop write to mysql end..,mdwSingleThread = 1, dwThreadIdx=%d\n", dwThreadIdx);
				mTargetNameLock.unlock();
				madwCompareStatus[dwThreadIdx] = 1;
				break;
			}
			LoadAndGerateInputList(rSpUrlFeature);
			ProcFaceUrlsAndFeature(rSpUrlFeature);
			mdwSingleThread = 1;
			mTargetNameLock.unlock();
			madwCompareStatus[dwThreadIdx] = 1;
			break;
		}
#pragma endregion

        printf("DoPerformanceLoop end,id is: %d to: %d,dwThreadIdx=%d\n", mdwScopeStart, mdwScopeEndup,dwThreadIdx);
		fprintf(pfResustFile, "DoPerformanceLoop end,id is: %d to: %d,dwThreadIdx=%d\n", mdwScopeStart, mdwScopeEndup, dwThreadIdx);
    }

	madwCompareStatus[dwThreadIdx] = 1;

    return 0;
}


int  Performance::TryReadAndUpateCfgStr(boost::shared_ptr<IniFile> & rInitFile,
                                             const char * section, 
                                             const char * key, 
                                             string     & strCfg,
                                             size_t       dwMaxLen)
{
    int dwRet = -1;
    
    if(section == NULL || key == NULL)
    {
        return dwRet;
    }

    if(!rInitFile->hasSection(section))
    {
        return dwRet;
    }

    
    if(!rInitFile->hasKey(section,key))
    {
        return dwRet;
    }

    strCfg = rInitFile->getStringValue(section, key, dwRet);

    if(dwMaxLen > 0 && strCfg.size() > dwMaxLen)
    {
        std::cout << "LoadConfiguration: cfg: [" << section << "]:"  
                  << key      << " =  "          << strCfg.c_str() 
                  << " too long, MaxLen: "       << dwMaxLen 
                  << ", change to NULL"          << std::endl;
    
        strCfg = "NULL";
    }
        
    return dwRet;
}

int  Performance::TryReadAndUpateCfgInt(boost::shared_ptr<IniFile> & rInitFile,
                                             const char * section, 
                                             const char * key, 
                                             int        & intCfg,
                                             int          maxValue)
{
    int dwRet = -1;
    
    if(section == NULL || key == NULL)
    {
        return dwRet;
    }

    if(!rInitFile->hasSection(section))
    {
        return dwRet;
    }

    
    if(!rInitFile->hasKey(section,key))
    {
        return dwRet;
    }
        
    intCfg = rInitFile->getIntValue(section, key, dwRet);

    if (intCfg > maxValue)
    {
        std::cout << "LoadConfiguration: cfg: ["  << section  << "]:"  
                  << key                          << " =  "   << intCfg 
                  << " too big, set to maxvalue:" << maxValue << std::endl;
        
        intCfg = maxValue;
    }
        
    return dwRet;
}


int  Performance::TryReadAndUpateCfgflt(boost::shared_ptr<IniFile> & rInitFile,
                                             const char * section, 
                                             const char * key, 
                                             float      & fltCfg,
                                             float        maxValue)
{
    int dwRet = -1;
    
    if(section == NULL || key == NULL)
    {
        return dwRet;
    }

    if(!rInitFile->hasSection(section))
    {
        return dwRet;
    }

    
    if(!rInitFile->hasKey(section,key))
    {
        return dwRet;
    }
        
    fltCfg = (float)rInitFile->getDoubleValue(section, key, dwRet);

    if (fltCfg > maxValue)
    {
        std::cout <<"LoadConfiguration: cfg: ["  << section  << "]:"
                  << key                         << " =  "   << fltCfg 
                  <<" too big, set to maxvalue:" << maxValue << std::endl;
    
        fltCfg = maxValue;
    }
        
    return dwRet;
}


int  Performance::TrySetAndUpateCfgStr(boost::shared_ptr<IniFile> & rInitFile,
                                            const char   * section, 
                                            const char   * key, 
                                            const string & value)
{
    int dwRet = -1;

    if (section == NULL || key == NULL)
    {
        return dwRet;
    }
    
    dwRet = rInitFile->setValue(section, key, value);
    
    return dwRet;
}

int  Performance::TrySetAndUpateCfgInt(boost::shared_ptr<IniFile> & rInitFile,
                                            const char * section, 
                                            const char * key, 
                                            const int  & value)
{
    std::ostringstream strValue;
    
    strValue << value;
    
    return TrySetAndUpateCfgStr(rInitFile, section, key, strValue.str());

}

int  Performance::TrySetAndUpateCfgflt(boost::shared_ptr<IniFile> & rInitFile,
                                            const char  * section, 
                                            const char  * key, 
                                            const float & value)
{
    std::ostringstream strValue;
    
    strValue << value;
    
    return TrySetAndUpateCfgStr(rInitFile, section, key, strValue.str());
}

//更新Mysql数据库特征值和version字段
//DB：intellif_static
//table:t_cid_detail/t_juzhu_detail
//Columns:id/face_feature/length(face_feature)/image_data/version/indexed
int Performance::WriteFaceFeatureToMysql(SP_FaceUrlFeatureItem & rSpFaceUrlFeatuerItem)
{
	string inputpath = rSpFaceUrlFeatuerItem->mStrFaceUrl;
	//printf("WriteFaceFeatureToMysql::idx=%d, url=%s\n", rSpFaceUrlFeatuerItem->mdwFaceId, inputpath.c_str());
	fprintf(pfResustFile, "WriteFaceFeatureToMysql::idx=%d, url=%s\n", rSpFaceUrlFeatuerItem->mdwFaceId, inputpath.c_str());

	if (rSpFaceUrlFeatuerItem->mdwFaceId == 0) // insert into mysql
	{
		InsertFaceFeatuerToMysql(rSpFaceUrlFeatuerItem->mStrFaceTable, rSpFaceUrlFeatuerItem);
		return 0;
	}
	UpdateFaceFeatuerToMysql(rSpFaceUrlFeatuerItem->mStrFaceTable, rSpFaceUrlFeatuerItem);
	return 0;
}

int Performance::WriteFaceFeature(SP_FaceUrlFeatureList & rSpUrlFeature)
{
	TFaceUrlFeatureList & rUrlFeatureList = rSpUrlFeature->mListUrlFeature;
	for (TFaceUrlFeatureList::iterator iterUrlFeature = rUrlFeatureList.begin();
										iterUrlFeature != rUrlFeatureList.end();
										iterUrlFeature++)
	{
		SP_FaceUrlFeatureItem & itemUrlFeature = *iterUrlFeature;
		std::string strExtion = extension(path(itemUrlFeature->mStrFaceUrl));
		boost::to_lower(strExtion);
		if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png" && strExtion != ".jpg?vip")
		{
			continue;
		}

		//区分是mysql还是本地路径
		Mat matLoad;
		if (itemUrlFeature->mStrFaceUrl.find("http:") < itemUrlFeature->mStrFaceUrl.length())
		{
			string strDownLoad;
			bool bSuccess = decodeRemotePicCurl(itemUrlFeature->mStrFaceUrl, strDownLoad);
			if (!bSuccess)
			{
				std::cout << "Performance::WriteFaceFeature, download : " << itemUrlFeature->mStrFaceUrl.c_str() << ", failed\n" << std::endl;
				return -1;
			}
			std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
			matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);
		}
		else
		{
			matLoad = imread(itemUrlFeature->mStrFaceUrl);
		}

		if (matLoad.data == NULL)
		{
			printf("Performance::WriteFaceFeature load:%s, failed\n", itemUrlFeature->mStrFaceUrl.c_str());
			continue;
		}

		printf("Performance::WriteFaceFeature input IDX:%d,%s,%s\n", itemUrlFeature->mdwFaceId, itemUrlFeature->mStrFaceTable.c_str(), itemUrlFeature->mStrFaceUrl.c_str());

		boost::shared_ptr<FaceDataItem> enqueItem(new FaceDataItem(FaceDataItem::DATA_ITEM_TYPE_FACE_IMAGE,
																	0,
																	matLoad,
																	itemUrlFeature->mStrFaceUrl,
																	itemUrlFeature->mStrFaceTable,
																	itemUrlFeature->mdwFaceId));

		DetectDataReceived(enqueItem);
	}
	return 0;
}

int  Performance::StartupProcessThreads(void)
{
    for (int dwThreadIdx = 0; dwThreadIdx < mdwProcThreadCnt; dwThreadIdx++)
    {    
		boost::thread * pSaveThread = new boost::thread(boost::bind(&Performance::DoPerformanceLoop,
                                                                     this,
                                                                     dwThreadIdx));

        //2015-07-15 detach to let boost recyle the thread resource when the thread exit
        pSaveThread->detach();
        
        boost::shared_ptr<boost::thread> saveThrPtr(pSaveThread);
        
        matProcThreads[dwThreadIdx] = saveThrPtr;
    }

    return 0;
}

int  Performance::WaitingPorcessFinished(void)
{
    while(1)
    {
        int dwUnFinished = 0;
    
        for (int dwThreadIdx = 0; dwThreadIdx < mdwProcThreadCnt; dwThreadIdx++)
        {    
			if (madwCompareStatus[dwThreadIdx] == 0)
            {
                dwUnFinished += 0x1;
            }
        }

        if(dwUnFinished == 0)
        {
            break;
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }

    return 0;
}

// get next startid and endid
int  Performance::GetOneIdScope(int & dwGetStartId, int & dwGetEndupId)
{
	boost::mutex::scoped_lock autoLocker(mIdScopeLock);
	
	dwGetStartId = mdwNowId;
	dwGetEndupId = mdwNowId + mdwSearchStep;

	if (dwGetEndupId > mdwEndId)
	{
		dwGetEndupId = mdwEndId;
	}

	if (dwGetStartId < mdwStartId)
	{
		dwGetStartId = mdwStartId;
	}
	
	if ((dwGetStartId > mdwEndId)
		|| (dwGetEndupId > mdwEndId))
	{
		return -1;
	}
   
    mdwNowId += 1;
	mdwNowId += mdwSearchStep;
	
	return 0;
}

time_t Performance::time_string_to_time_t(const std::string& timeStr, int dwIsIso)
{
    time_t ttRet = 0;

    if(dwIsIso == 0)
    {
        boost::posix_time::ptime pt(boost::posix_time::time_from_string(timeStr));
        tm tmTmp = boost::posix_time::to_tm(pt);
        ttRet = mktime(&tmTmp);
    }
    else
    {
        boost::posix_time::ptime pt(boost::posix_time::from_iso_string(timeStr));
        tm tmTmp = boost::posix_time::to_tm(pt);
        ttRet = mktime(&tmTmp);
    }

    return ttRet;
}

std::string Performance::time_t_to_iso_string(const time_t& tSec)
{
    string  imgOfTime;

    if (tSec == 0)
    {
        boost::posix_time::ptime     nowTime(boost::date_time::second_clock<boost::posix_time::ptime>::local_time());
        imgOfTime = to_iso_string(nowTime);
    }
    else
    {
        struct std::tm tmValLocal;
#ifdef _MSC_VER
        localtime_s(&tmValLocal, &tSec);
#else
        localtime_r(&tSec, &tmValLocal);
#endif
        boost::posix_time::ptime      rcdTime(boost::posix_time::ptime_from_tm(tmValLocal));

        imgOfTime = to_iso_string(rcdTime);
    }
    return imgOfTime;
}

//从数据库读取特征值
//DB：intellif_static
//table:t_cid_detail/t_juzhu_detail
//Columns:id/face_feature/length(face_feature)/image_data/version/indexed
int Performance::GetFaceFeatureFromMysql(int startid, int endid, int dwAlgType, SP_FaceUrlFeatureList & rSpUrlFeature)
{
	int dwRet = 0;

	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	std::cout << "Performance,GetFaceFeatureFromMysql, begin id: " << startid
		<< ", end id: " << endid << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "init mysql handle is null" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if ((rSpUrlFeature.get() == NULL)
		|| (startid < 0)
		|| (endid <= 0)
		|| (startid > endid))
	{
		std::cout << "Performance,GetFaceFeatureFromMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	VecPartTablesInfo vPartTables;
	std::vector<std::string> vecTableInfo;

	boost::split(vecTableInfo, mStrFaceTables, boost::is_any_of(","));

	if (vecTableInfo.size() < 1)
	{
		printf("Table name error, check it out: %s \n", mStrFaceTables.c_str());
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	for (int i = 0; i < vecTableInfo.size(); i++)
	{
		PartTablesInfo item;
		item.miEnd = endid;
		item.miStart = startid;
		item.msFormerName = vecTableInfo[i];
		vPartTables.push_back(item);
	}

	for (VecPartTablesInfo::iterator tLoop = vPartTables.begin();
		tLoop != vPartTables.end();
		tLoop++)
	{
		MYSQL_STMT *stmt;
		MYSQL_BIND  params[6];

		std::ostringstream osQuery;
		std::ostringstream osTableName;

		if (startid > 0 && endid >= startid)
		{
			osQuery << "SELECT id,image_data,face_feature,length(face_feature),version,from_cid_id FROM "
				<< mStrFaceDbName << "." << tLoop->msFormerName
				<< " WHERE id >= " << startid
				<< " AND   id <= " << endid
				<< " AND version = " << dwAlgType;
		}
		else
		{
			osQuery << "SELECT id,image_data,face_feature,length(face_feature),version from_cid_id FROM "
				<< mStrFaceDbName << "." << tLoop->msFormerName
				<< " WHERE version = " << dwAlgType;
		}

		std::cout << "sql: " << osQuery.str() << std::endl;
		fprintf(pfResustFile, "Performance::GetFaceFeatureFromMysql::sql=%s\n", osQuery.str().c_str());

		stmt = mysql_stmt_init(pDbHandl);

		if (stmt == NULL)
		{
			std::cout << "GetFaceFeatureFromMysql, mysql_stmt_init == NULL" << std::endl;
			continue;
		}

		if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
		{
			std::cout << "GetFaceFeatureFromMysql mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		memset(params, 0, sizeof(params));

		float    pfGetFeatureData[MAX_FEATURE_PADDED_LEN];
		char     acGetFaceUrlPath[0x100];
		int64_t  dwGetFaceId;
		int      dwFeatureLen = 0;
		int      dwFeatureVersion;
		int64_t  dwGetFromCidId;

		params[0].buffer_type = MYSQL_TYPE_LONGLONG;
		params[0].buffer = &dwGetFaceId;
		params[0].buffer_length = sizeof(dwGetFaceId);

		params[1].buffer_type = MYSQL_TYPE_STRING;
		params[1].buffer = acGetFaceUrlPath;
		params[1].buffer_length = 0x100;

		params[2].buffer_type = MYSQL_TYPE_BLOB;
		params[2].buffer = pfGetFeatureData;
		params[2].buffer_length = sizeof(float)* MAX_FEATURE_PADDED_LEN;

		params[3].buffer_type = MYSQL_TYPE_LONG;
		params[3].buffer = &dwFeatureLen;
		params[3].buffer_length = sizeof(dwFeatureLen);

		params[4].buffer_type = MYSQL_TYPE_LONG;
		params[4].buffer = &dwFeatureVersion;
		params[4].buffer_length = sizeof(dwFeatureVersion);

		params[5].buffer_type = MYSQL_TYPE_LONGLONG;
		params[5].buffer = &dwGetFromCidId;
		params[5].buffer_length = sizeof(dwGetFromCidId);

		if (0 != mysql_stmt_bind_result(stmt, params))
		{
			std::cout << "GetFaceFeatureFromMysql mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		if (0 != mysql_stmt_execute(stmt))
		{
			std::cout << "GetFaceFeatureFromMysql mysql_stmt_execute failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		if (0 != mysql_stmt_store_result(stmt))
		{
			std::cout << "GetFaceFeatureFromMysql mysql_stmt_store_result failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		int dwRows = (int)mysql_stmt_num_rows(stmt);

		std::cout << "result rows: " << dwRows << std::endl;
		fprintf(pfResustFile, "Performance::GetFaceFeatureFromMysql::sql result rows=%d\n", dwRows);

		while (mysql_stmt_fetch(stmt) == 0)
		{
			int dwRealFeatureLen = dwFeatureLen / sizeof(float);
			float * pFeatureGet = (float *)malloc(sizeof(float)* dwRealFeatureLen);

			if (pFeatureGet == NULL)
			{
				std::cout << "memory alloc failed in GetFaceFeatureFromMysql" << std::endl;
				continue;
			}

			SP_FeatureWrapperItem pNewFeaturePtr(new FeatureWrapperItem(dwFeatureVersion,
																		pFeatureGet,
																		dwRealFeatureLen));

			for (int dwFloatIdx = 0; dwFloatIdx < dwRealFeatureLen; dwFloatIdx++)
			{
				pFeatureGet[dwFloatIdx] = pfGetFeatureData[dwFloatIdx];
			}

			SP_FaceUrlFeatureItem pNewUrlFeature(new FaceUrlFeatureItem());

			pNewUrlFeature->mSpFaceFeature = pNewFeaturePtr;
			pNewUrlFeature->mdwFaceId = dwGetFaceId;
			pNewUrlFeature->mStrFaceUrl = acGetFaceUrlPath;
			pNewUrlFeature->mdwFaceCidId = dwGetFromCidId;
			pNewUrlFeature->mdwFaceSeq = mdwInputFaceSeq;
			pNewUrlFeature->mStrFaceTable = tLoop->msFormerName;
			rSpUrlFeature->mListUrlFeature.push_back(pNewUrlFeature);
			boost::mutex::scoped_lock autoLocker(mFaceSeqLock);
			mdwInputFaceSeq += 0x1;
			printf("#### mdwFaceSeq=%d ####\n", mdwInputFaceSeq);
		}

		mysql_stmt_free_result(stmt);

		mysql_stmt_close(stmt);

	}

	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

//从数据库读取url地址，根据id分布式读取
//DB：intellif_static
//table:t_cid_detail/t_juzhu_detail
//Columns:id/face_feature/length(face_feature)/image_data/version/indexed
int Performance::GetFaceUrlFromMysql(int startid, int endid, int dwAlgType, SP_FaceUrlFeatureList & rSpUrlFeature)
{
	int dwRet = 0;

	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	//std::cout << "Performance,GetFaceUrlFromMysql, begin id: " << startid << ", end id: " << endid << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "init mysql handle is null" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if ((rSpUrlFeature.get() == NULL)
		|| (startid < 0)
		|| (endid < 0)
		|| (startid > endid))
	{
		std::cout << "Performance,GetFaceUrlFromMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	VecPartTablesInfo vPartTables;
	std::vector<std::string> vecTableInfo;

	boost::split(vecTableInfo, mStrFaceTables, boost::is_any_of(","));

	if (vecTableInfo.size() < 1)
	{
		printf("Performance,GetFaceUrlFromMysql Table name error, check it out: %s \n", mStrFaceTables.c_str());
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	for (int i = 0; i < vecTableInfo.size(); i++)
	{
		PartTablesInfo item;
		item.miEnd = endid;
		item.miStart = startid;
		item.msFormerName = vecTableInfo[i];
		vPartTables.push_back(item);
	}

	for (VecPartTablesInfo::iterator tLoop = vPartTables.begin();
		tLoop != vPartTables.end();
		tLoop++)
	{
		MYSQL_STMT *stmt;
		MYSQL_BIND  params[3];

		std::ostringstream osQuery;
		std::ostringstream osTableName;

		if (startid > 0 && endid >= startid)
		{
			osQuery << "SELECT id,image_data,version FROM "
				<< mStrFaceDbName << "." << tLoop->msFormerName
				<< " WHERE id >= " << startid
				<< " AND   id <= " << endid
				<< " AND   version != " << dwAlgType;
		}
		else
		{
			osQuery << "SELECT id,image_data,version FROM "
				<< mStrFaceDbName << "." << tLoop->msFormerName
				<< " WHERE version != " << dwAlgType;
		}

		std::cout << "sql: " << osQuery.str() << std::endl;

		stmt = mysql_stmt_init(pDbHandl);

		if (stmt == NULL)
		{
			std::cout << "Performance,GetFaceUrlFromMysql, stmt == NULL" << std::endl;
			continue;
		}

		if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
		{
			std::cout << "Performance,GetFaceUrlFromMysql mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		memset(params, 0, sizeof(params));

		char     acGetFaceUrlPath[0x100];
		int64_t  dwGetFaceId;
		int      dwFeatureVersion;

		params[0].buffer_type = MYSQL_TYPE_LONGLONG;
		params[0].buffer = &dwGetFaceId;
		params[0].buffer_length = sizeof(dwGetFaceId);

		params[1].buffer_type = MYSQL_TYPE_STRING;
		params[1].buffer = acGetFaceUrlPath;
		params[1].buffer_length = 0x100;;

		params[2].buffer_type = MYSQL_TYPE_LONG;
		params[2].buffer = &dwFeatureVersion;
		params[2].buffer_length = sizeof(dwFeatureVersion);

		if (0 != mysql_stmt_bind_result(stmt, params))
		{
			std::cout << "Performance,GetFaceUrlFromMysql mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		if (0 != mysql_stmt_execute(stmt))
		{
			std::cout << "Performance,GetFaceUrlFromMysql mysql_stmt_execute failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		if (0 != mysql_stmt_store_result(stmt))
		{
			std::cout << "mysql_stmt_store_result failed: " << osQuery.str() << std::endl;
			mysql_stmt_close(stmt);
			continue;
		}

		int dwRows = (int)mysql_stmt_num_rows(stmt);

		std::cout << "result rows: " << dwRows << std::endl;

		while (mysql_stmt_fetch(stmt) == 0)
		{
			SP_FaceUrlFeatureItem pNewUrlFeature(new FaceUrlFeatureItem());
			SP_FeatureWrapperItem pNewFeaturePtr(new FeatureWrapperItem(dwAlgType, NULL, 0));
			pNewFeaturePtr->mpfFeaturePtr = NULL;
			pNewUrlFeature->mSpFaceFeature = pNewFeaturePtr;
			pNewUrlFeature->mdwFaceId = dwGetFaceId;
			pNewUrlFeature->mStrFaceUrl = acGetFaceUrlPath;
			pNewUrlFeature->mdwFaceCidId = 0;
			pNewUrlFeature->mStrFaceTable = tLoop->msFormerName;
			pNewUrlFeature->mdwFaceSeq = 0;
			rSpUrlFeature->mListUrlFeature.push_back(pNewUrlFeature);
		}

		mysql_stmt_free_result(stmt);

		mysql_stmt_close(stmt);

	}

	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

//从数据库读取姓名
//DB：intellif_static
//table:t_cid_info/t_juzhu_info
//Columns:id/xm/photo
string Performance::GetFaceXMFromMysql(const std::string &tableName, int photoid)
{
	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	std::cout << "Performance,GetFaceXMFromMysql, photoid id: " << photoid
		<< ",tableName: " << tableName << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "GetFaceXMFromMysql,init mysql handle is null" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	if (photoid <= 0)
	{
		std::cout << "GetFaceXMFromMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	string name = "";

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[1];

	std::ostringstream osQuery;
	std::ostringstream osTableName;

	osQuery << "SELECT xm FROM "
		<< mStrFaceDbName << "." << tableName
		<< " WHERE id = " << photoid;

	std::cout << "sql: " << osQuery.str() << std::endl;

	stmt = mysql_stmt_init(pDbHandl);

	if (stmt == NULL)
	{
		std::cout << "GetFaceXMFromMysql, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "GetFaceXMFromMysql, mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	memset(params, 0, sizeof(params));

	char     acGetFaceName[0x20];

	params[0].buffer_type = MYSQL_TYPE_STRING;
	params[0].buffer = acGetFaceName;
	params[0].buffer_length = 0x20;

	if (0 != mysql_stmt_bind_result(stmt, params))
	{
		std::cout << "GetFaceXMFromMysql,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "GetFaceXMFromMysql,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	if (0 != mysql_stmt_store_result(stmt))
	{
		std::cout << "GetFaceXMFromMysql,mysql_stmt_store_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}

	int dwRows = (int)mysql_stmt_num_rows(stmt);

	std::cout << "GetFaceXMFromMysql,result rows: " << dwRows << std::endl;
	if (dwRows == 0)
	{
		mysql_stmt_free_result(stmt);
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return "";
	}
	while (mysql_stmt_fetch(stmt) == 0)
	{
		name = acGetFaceName;
	}

	mysql_stmt_free_result(stmt);

	mysql_stmt_close(stmt);

	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
	
	return name;
}

//更新Mysql数据库特征值和version字段
//DB：intellif_static
//table:t_cid_detail/t_juzhu_detail
//Columns:id/face_feature/length(face_feature)/image_data/version/indexed
int Performance::UpdateFaceFeatuerToMysql(const std::string &tableName, boost::shared_ptr<FaceUrlFeatureItem>&  rSpFaceUrlFeatuerItem)
{
	int dwRet = 0;
	
	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	//std::cout << "Performance,UpdateFaceFeatuerToMysql, tableName:" << tableName << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "UpdateFaceFeatuerToMysql,init mysql handle is null." << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (tableName.empty() || rSpFaceUrlFeatuerItem.get() == NULL)
	{
		std::cout << "UpdateFaceFeatuerToMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[2];

	std::ostringstream osQuery;
	std::ostringstream osTableName;

	osQuery << "UPDATE " << mStrFaceDbName << "." << tableName
		<< " SET face_feature = ? , version = ? WHERE id=" << rSpFaceUrlFeatuerItem->mdwFaceId;

	//std::cout << "sql: " << osQuery.str() << std::endl;

	stmt = mysql_stmt_init(pDbHandl);

	if (stmt == NULL)
	{
		std::cout << "UpdateFaceFeatuerToMysql, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "UpdateFaceFeatuerToMysql,mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	memset(params, 0, sizeof(params));

	params[0].buffer_type = MYSQL_TYPE_BLOB;
	params[0].buffer = rSpFaceUrlFeatuerItem->mSpFaceFeature->mpfFeaturePtr;
	params[0].buffer_length = sizeof(float)* rSpFaceUrlFeatuerItem->mSpFaceFeature->mdwFeatureLen;

	params[1].buffer_type = MYSQL_TYPE_LONG;
	params[1].buffer = &rSpFaceUrlFeatuerItem->mSpFaceFeature->mdwAlgType;
	params[1].buffer_length = sizeof(rSpFaceUrlFeatuerItem->mSpFaceFeature->mdwAlgType);

	if (0 != mysql_stmt_bind_param(stmt, params))
	{
		std::cout << "UpdateFaceFeatuerToMysql,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "UpdateFaceFeatuerToMysql,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	mysql_stmt_free_result(stmt);

	mysql_stmt_close(stmt);

	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

//插入Mysql数据库特征值和version字段
//DB：intellif_static
//table:t_cid_detail/t_juzhu_detail
//Columns:id/face_feature/length(face_feature)/image_data/version/indexed
int Performance::InsertFaceFeatuerToMysql(const std::string &tableName, boost::shared_ptr<FaceUrlFeatureItem>&  rSpFaceUrlFeatuerItem)
{
	int dwRet = 0;

	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	std::cout << "Performance,InsertFaceFeatuerToMysql, pDbHandl:" << pDbHandl << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "InsertFaceFeatuerToMysql,init mysql handle is null." << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (tableName.empty() || rSpFaceUrlFeatuerItem.get() == NULL || rSpFaceUrlFeatuerItem->mdwFaceId != 0)
	{
		std::cout << "InsertFaceFeatuerToMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[3];

	std::ostringstream osQuery;
	std::ostringstream osTableName;

	osQuery << "INSERT INTO " << mStrFaceDbName << "." << tableName
		<< " (face_feature,version,image_data,created,updated,from_cid_id,from_image_id,indexed) "
		<< " VALUES ( ?,?,?,\'1999-09-09 09:09:09\',\'1999-09-09 09:09:09\',0,0,1 )";

	std::cout << "sql: " << osQuery.str() << std::endl;

	stmt = mysql_stmt_init(pDbHandl);

	if (stmt == NULL)
	{
		std::cout << "InsertFaceFeatuerToMysql, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "InsertFaceFeatuerToMysql,mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	memset(params, 0, sizeof(params));

	params[0].buffer_type = MYSQL_TYPE_BLOB;
	params[0].buffer = rSpFaceUrlFeatuerItem->mSpFaceFeature->mpfFeaturePtr;
	params[0].buffer_length = sizeof(float)* rSpFaceUrlFeatuerItem->mSpFaceFeature->mdwFeatureLen;

	params[1].buffer_type = MYSQL_TYPE_LONG;
	params[1].buffer = &rSpFaceUrlFeatuerItem->mSpFaceFeature->mdwAlgType;
	params[1].buffer_length = sizeof(rSpFaceUrlFeatuerItem->mSpFaceFeature->mdwAlgType);

	params[2].buffer_type = MYSQL_TYPE_STRING;
	params[2].buffer = (void *)(rSpFaceUrlFeatuerItem->mStrFaceUrl.c_str());
	params[2].buffer_length = (unsigned long)rSpFaceUrlFeatuerItem->mStrFaceUrl.length();

	if (0 != mysql_stmt_bind_param(stmt, params))
	{
		std::cout << "InsertFaceFeatuerToMysql,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "InsertFaceFeatuerToMysql,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	mysql_stmt_free_result(stmt);

	mysql_stmt_close(stmt);

	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

//根据id号查询记录
int Performance::SelectRecordById(const std::string &tableName, int64_t  dwFaceId)
{
	int dwRet = 0;

	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	//std::cout << "Performance,SelectRecordById, tableName:" << tableName << ",dwFaceId:" << dwFaceId << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "SelectRecordById,init mysql handle is null." << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (tableName.empty() || dwFaceId <= 0)
	{
		std::cout << "SelectRecordById, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[1];

	std::ostringstream osQuery;
	std::ostringstream osTableName;

	osQuery << "SELECT id FROM "
		<< mStrFaceDbName << "." << tableName
		<< " WHERE id = " << dwFaceId;

	//std::cout << "sql: " << osQuery.str() << std::endl;

	stmt = mysql_stmt_init(pDbHandl);

	if (stmt == NULL)
	{
		std::cout << "SelectRecordById, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "SelectRecordById,mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	memset(params, 0, sizeof(params));

	int64_t  dwGetFaceId;

	params[0].buffer_type = MYSQL_TYPE_LONGLONG;
	params[0].buffer = &dwGetFaceId;
	params[0].buffer_length = sizeof(dwGetFaceId);

	if (0 != mysql_stmt_bind_result(stmt, params))
	{
		std::cout << "SelectRecordById,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "SelectRecordById,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	if (0 != mysql_stmt_store_result(stmt))
	{
		std::cout << "SelectRecordById,mysql_stmt_store_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	int dwRows = (int)mysql_stmt_num_rows(stmt);

	//std::cout << "result rows: " << dwRows << std::endl;

	if (dwRows != 1)
	{
		std::cout << "SelectRecordById,mysql_stmt_num_rows not 1 rows: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	mysql_stmt_free_result(stmt);

	mysql_stmt_close(stmt);

	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

//compare process
int Performance::ProcFaceUrlsAndFeature(SP_FaceUrlFeatureList & rSpUrlFeature)
{
	TFaceUrlFeatureList & rUrlFeatureList = rSpUrlFeature->mListUrlFeature;

	for (NameListMap::iterator listLoop = mNameList.begin();
								listLoop != mNameList.end();
								listLoop++)
	{
		for (TFaceUrlFeatureList::iterator iterUrlFeature = rUrlFeatureList.begin();
			iterUrlFeature != rUrlFeatureList.end();
			iterUrlFeature++)
		{
			SP_FaceUrlFeatureItem & itemUrlFeature = *iterUrlFeature;

			//itemUrlFeature::input;   itemFeature::namelist(query)
			int dwMinFeatureLen = listLoop->second->getFeatureLen() < itemUrlFeature->mSpFaceFeature->mdwFeatureLen
				? listLoop->second->getFeatureLen()
				: itemUrlFeature->mSpFaceFeature->mdwFeatureLen;
				
			boost::shared_ptr<FaceRecoder> addFaceInfo(new FaceRecoder(itemUrlFeature->mStrFaceUrl, 
																		itemUrlFeature->mdwFaceCidId, 
																		itemUrlFeature->mStrFaceTable, 
																		itemUrlFeature->mdwFaceSeq));

			float fCmpScore = (float)0.0;

			IfMutilAlgCompare::IfFeatureCompare(itemUrlFeature->mSpFaceFeature->mpfFeaturePtr,
				dwMinFeatureLen,
				listLoop->second->getFeaturePtr(),
				dwMinFeatureLen,
				&fCmpScore,
				itemUrlFeature->mSpFaceFeature->mdwAlgType,
				itemUrlFeature->mSpFaceFeature->mdwAlgType);

			if (fCmpScore < mfFatureThresHold)
			{
				continue;
			}

			listLoop->second->addMatchFace(addFaceInfo, fCmpScore);
		}
	}

	return 0;
}

//write compare result score to file
int Performance::PrintCompareResult(void)
{
	path resultPath = path(mStrPathOut) / "search_result.txt";

	if (!exists(path(mStrPathOut)))
	{
		create_directories(path(mStrPathOut));
	}

	FILE * mpFileOutput = fopen(resultPath.string().c_str(), "w");

	if (mpFileOutput == NULL)
	{
		printf("%s, open failed\n");
		return -1;
	}

	cout << "mNamelist CNT = " << mdwQueryFaceSeq << std::endl;

	for (int dwNameListIdx = 0; dwNameListIdx < mdwQueryFaceSeq; dwNameListIdx++)
	{
		NameListMap::iterator findNameList = mNameList.find(dwNameListIdx);

		if (findNameList == mNameList.end())
		{
			continue;
		}
		cout << "Result::Namelist path=" << findNameList->second->getImgPath().c_str() << ",count=" << findNameList->second->mNewNameList.size() << std::endl;
		//DWORD starttime = GetTickCount();
		findNameList->second->OutputMatinfo2File(this, mpFileOutput, mdwSaveResultPic, mStrPathOut);
		//DWORD endtime = GetTickCount();
		//cout << "The run time is:" << (endtime - starttime) << "ms!" << endl;//输出运行时间
	}
	fclose(mpFileOutput);

	mpFileOutput = NULL;

	return 0;
}

//count result and write to static_result
int Performance::Statistics(void)
{
	path resultPath = path(mStrPathOut) / "search_result.txt";

	if (!exists(resultPath))
	{
		cout << "search_result.txt file not exist." << std::endl;
		return -1;
	}

	string stringLine;
	ifstream infile;
	vector<string> tempLine;
	string tempName;
	int top1 = 0, top2 = 0, top3 = 0, top5 = 0, top7 = 0, top10 = 0, top20 = 0, top = 0;

	infile.open(resultPath.string().c_str());
	while (!infile.eof())
	{
		getline(infile, stringLine);
		if (stringLine.empty())
		{
			printf("stringLine is Null.\n");
			continue;
		}
		bool bFind = false;
		boost::char_separator<char> sep(";");
		boost::split(tempLine, stringLine, boost::is_any_of(";"));
		string namelist = tempLine[0];
		fprintf(pfResustFile, "######%s 90 len = %d#####\n", namelist.c_str(), tempLine.size());
		string nameliststem;
		if (namelist.rfind("_") == string::npos)
		{
			nameliststem = namelist;
		}
		else
		{
			nameliststem = namelist.substr(0, namelist.rfind("_")) + ".jpg";
		}
		for (int i = 1; i < tempLine.size(); i++)
		{
			//printf("templine=%s\n",tempLine[i].c_str());
			//string tempname = tempLine[i].substr(0, tempLine[i].rfind("_"));
			vector<string> tempname;
			boost::split(tempname, tempLine[i], boost::is_any_of(","));
			if (!tempname[0].empty() && nameliststem == tempname[0])
			{
				//printf("option::= %d\n", i);
				bFind = true;
				if (i == 1)
				{
					top1++;
					top2++;
					top3++;
					top5++;
					top7++;
					top10++;
					top20++;
				}
				else if (i == 2)
				{
					top2++;
					top3++;
					top5++;
					top7++;
					top10++;
					top20++;
				}
				else if (i == 3)
				{
					top3++;
					top5++;
					top7++;
					top10++;
					top20++;
				}
				else if (i <= 5)
				{
					top5++;
					top7++;
					top10++;
					top20++;
				}
				else if (i <= 7)
				{
					top7++;
					top10++;
					top20++;
				}
				else if (i <= 10)
				{
					top10++;
					top20++;
				}
				else if (i <= 20)
				{
					top20++;
				}
			}
			else
			{
				//printf("%s not include %s\n", namelist.c_str(), tempname[0].c_str());
				top++;
			}
			if (bFind)
			{
				break;
			}
		}
		if (!bFind)
		{
			//fprintf(pfResustFile, "Top20 no find file:name=%s\n", namelist.c_str());
		}
	}
	infile.close();
	path staticResultPath = path(mStrPathOut) / "search_result_static.txt";
	FILE * mpFileOutput = fopen(staticResultPath.string().c_str(), "w");

	if (mpFileOutput == NULL)
	{
		printf("%s, open failed\n");

		return 0;
	}

	cout << "NamelistCnt=" << mdwQueryFaceSeq << std::endl;

	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top1", top1, ((float)top1 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top2", top2, ((float)top2 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top3", top3, ((float)top3 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top5", top5, ((float)top5 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top7", top7, ((float)top7 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top10", top10, ((float)top10 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d,result:%.2f%%\n", "top20", top20, ((float)top20 / mdwQueryFaceSeq) * 100);
	fprintf(mpFileOutput, "%s:%d\n", "Query Count:", mdwQueryFaceSeq);

	fclose(mpFileOutput);

	mpFileOutput = NULL;
	
	if (mdwSaveResultToMysql)
	{
		//结果写入数据库中
		SP_TestResultItem spTestResultItem(new TestResultItem());
		SP_StaticResultItem spStaticResultItem(new StaticResultItem());
		spStaticResultItem->mdwSampleCount = mdwQueryFaceSeq;
		spStaticResultItem->mdwTop1 = top1;
		spStaticResultItem->mdwTop5 = top5;
		spStaticResultItem->mdwTop7 = top7;
		spStaticResultItem->mdwTop10 = top10;

		spTestResultItem->mdwResultStatus = 1;
		spTestResultItem->mdwResultId = 1;
		spTestResultItem->mdwSampleId = 1;
		spTestResultItem->mSpStaticResultItem = spStaticResultItem;
		spTestResultItem->mStrResultUrl = staticResultPath.string().c_str();
		spTestResultItem->mStrXResultUrl = resultPath.string().c_str();
		spTestResultItem->mdwResultType = TestResultItem::TEST_RESULT_TYPE_STATIC;

		WriteResultToMysql(spTestResultItem);
	}

	return 0;
}

std::string Performance::get_remote_url(const std::string &rawString)
{
    if (is_remote_url(rawString))
    {
        return rawString;
    }
    else if (is_route_url(rawString))
    {
        if (rawString[0] == '/')
        {
            return std::string(mStrProxySrvAddr + rawString);
        }
        else
        {
            return std::string(mStrProxySrvAddr + "/" + rawString);
        }
    }
    return "";
}

bool Performance::is_remote_url(const std::string &rawString)
{
    const std::string strPrefix_http  = "http://";
    const std::string strPrefix_https = "https://";

    bool result = (rawString.find(strPrefix_http)  != std::string::npos)
               || (rawString.find(strPrefix_https) != std::string::npos);

    return result;
}

bool Performance::is_route_url(const std::string &rRouteStr)
{
    bool bRet = false;
    if ((mStrProxySrvFlag != "NULL") &&
        (mStrProxySrvAddr != "NULL") && 
        (rRouteStr.length() > 0))
    {        
        
        std::vector<std::string>      vecRemotUrlInfo;
        boost::split(vecRemotUrlInfo, rRouteStr, boost::is_any_of("/\\"));
        
        int dwIndex = 0;
        
        while (dwIndex < vecRemotUrlInfo.size())
        {
            if (vecRemotUrlInfo[dwIndex] == "" || 
                vecRemotUrlInfo[dwIndex] == "/")
            {
                dwIndex++;
                continue;
            }
            if (vecRemotUrlInfo[dwIndex] == mStrProxySrvFlag)
            {
                bRet = true;
            }
            else if (vecRemotUrlInfo[dwIndex] == "share_sync")
            {
                bRet = true;
            }

            break;
        }        
    }

    return bRet;
}

bool Performance::decodeRemotePicCurl(const std::string & rStrRemotUrl, string & strRevData)
{
    CURL * pCurlHand = curl_easy_init();

    if (pCurlHand == NULL)
    {
        std::cout << "DecodeRemotFileCurl, failed to init curl handle" << std::endl;

        return false;
    }

    curl_easy_setopt(pCurlHand, CURLOPT_URL, rStrRemotUrl.c_str());
    curl_easy_setopt(pCurlHand, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(pCurlHand, CURLOPT_WRITEDATA, &strRevData);
	curl_easy_setopt(pCurlHand, CURLOPT_WRITEFUNCTION, Performance::write2String);
    curl_easy_setopt(pCurlHand, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(pCurlHand, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(pCurlHand, CURLOPT_TIMEOUT, 60);

    CURLcode dwCurlRet = curl_easy_perform(pCurlHand);

    if (dwCurlRet != CURLE_OK)
    {
        std::cout << "Download file faild, rStrRemotUrl: " << rStrRemotUrl << ", " << curl_easy_strerror(dwCurlRet) << std::endl;

        curl_easy_cleanup(pCurlHand);

        return false;
    }

    curl_easy_cleanup(pCurlHand);

    return true;
}

int Performance::write2String(char *data, size_t size, size_t nmemb, string * writerData)
{
    size_t sizes = (size * nmemb);

    if (writerData == NULL)
    {
        return 0;
    }

    writerData->append(data, sizes);

    return (int)sizes;
}

IplImage*     Performance::cvGetSubImage(IplImage *image, CvRect roi)
{
    IplImage *result;
    
    cvSetImageROI(image, roi);
    
    result = cvCreateImage(cvSize(roi.width, 
                                  roi.height), 
                                  image->depth, 
                                  image->nChannels);
    cvCopy(image, result);
    
    cvResetImageROI(image);

    return result;
}

int  Performance::GetFaceUrlFromFolder(SP_FaceUrlFeatureList & rSpUrlFeature)
{
#pragma region inputtoMYSQL
	path procPath(mStrPathInput);

	if (!exists(procPath))
	{
		printf("Performance::GetFaceUrlFromFolder::%s not exit\n", mStrPathInput.c_str());
		return -1;
	}

	if (!is_directory(procPath))
	{
		printf("Performance::GetFaceUrlFromFolder::%s not a directory\n");
		return -1;
	}

	recursive_directory_iterator end_iter;

	for (recursive_directory_iterator iter(procPath); iter != end_iter; iter++)
	{
		if (is_directory(*iter))
		{
			continue;
		}

		std::string strExtion = extension(*iter);

		boost::to_lower(strExtion);

		if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png")
		{
			continue;
		}

		SP_FaceUrlFeatureItem pNewUrlFeature(new FaceUrlFeatureItem());
		SP_FeatureWrapperItem pNewFeaturePtr(new FeatureWrapperItem(mdwAlgVersions, NULL, 0));
		pNewFeaturePtr->mpfFeaturePtr = NULL;
		pNewUrlFeature->mSpFaceFeature = pNewFeaturePtr;
		pNewUrlFeature->mdwFaceId = 0;
		pNewUrlFeature->mStrFaceUrl = iter->path().string();
		pNewUrlFeature->mdwFaceCidId = 0;
		pNewUrlFeature->mStrFaceTable = "t_cid_detail";
		pNewUrlFeature->mdwFaceSeq = 0;
		rSpUrlFeature->mListUrlFeature.push_back(pNewUrlFeature);
	}

	return 0;
#pragma endregion
}

int  Performance::SaveDetectFace2Disk(cv::Mat matGray, if_rect_t tFaceRect, string strSavePath)
{
	if (!exists(path(strSavePath).parent_path()))
	{
		if (!create_directories(path(strSavePath).parent_path()))
		{
			std::cout << "create folder failed:" << path(strSavePath).parent_path().string().c_str()
				<< ", Please check the FS Permession settings" << std::endl;
			return -1;
		}
	}
	if (!exists(path(strSavePath).parent_path().parent_path()))
	{
		if (!create_directories(path(strSavePath).parent_path().parent_path()))
		{
			std::cout << "create folder failed:" << path(strSavePath).parent_path().parent_path().string().c_str()
				<< ", Please check the FS Permession settings" << std::endl;
			return -1;
		}
	}

    int retLeft   = (tFaceRect.left  > 0) 
                  ? (tFaceRect.left) 
                  : 0;
    int retRight  = (tFaceRect.right < (matGray.cols)) 
                  ? (tFaceRect.right)
                  : (matGray.cols);

    int retTop    = (tFaceRect.top  > 0) 
                  ? (tFaceRect.top) 
                  : 0;
    int retBottom = (tFaceRect.bottom < (matGray.rows)) 
                  ? (tFaceRect.bottom)
                  : (matGray.rows);

	CvRect     oRoiImg = cvRect(retLeft,
		retTop,
		retRight - retLeft,
		retBottom - retTop);

    try {
    #ifdef _MSC_VER
		IplImage saveIpl = IplImage(matGray);
		IplImage * pRoiImg = cvGetSubImage(&saveIpl, oRoiImg);
		int adwJpgPara[3];
		adwJpgPara[0] = CV_IMWRITE_JPEG_QUALITY;
		adwJpgPara[1] = 100;
		adwJpgPara[2] = 0;
		cvSaveImage(strSavePath.c_str(), pRoiImg, adwJpgPara);
		cvReleaseImage(&pRoiImg);
    #else
		//Mat    roiMat(rImgMat, Rect(retLeft,retTop,retRight,retBottom));
		Mat      roiMat(matGray, oRoiImg);
		imwrite(strSavePath, roiMat);
    #endif
    }
    catch (...)
    {
        return -1;
    }
    return 0;
}

//检测人脸特征值
//int Performance::DetectAndExtractFace(const std::string &picturePath, int dwFolderType, string strTableName)
//{
//	bool bLocalUrl = true;
//	int dwImgIdx = 0;
//
//	//区分是mysql还是本地路径
//	Mat matLoad;
//	if (picturePath.find("http:") < picturePath.length())
//	{
//		bLocalUrl = false;
//		string strDownLoad;
//		bool bSuccess = decodeRemotePicCurl(picturePath, strDownLoad);
//		if (!bSuccess)
//		{
//			std::cout << "WriteFaceFeatureToMysql, download : " << picturePath.c_str() << ", failed\n" << std::endl;
//			return -1;
//		}
//		std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
//		matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);
//	}
//	else
//	{
//		matLoad = imread(picturePath);
//	}
//
//	if (matLoad.data == NULL)
//	{
//		std::cout << "DetectAndExtractFace::jpg load failed:  " << picturePath.c_str();
//		return -1;
//	}
//
//	boost::shared_ptr<FaceDataItem> enqueItem(new FaceDataItem(FaceDataItem::DATA_ITEM_TYPE_FACE_IMAGE,
//		dwFolderType,
//		matLoad,
//		picturePath,
//		dwImgIdx));
//
//	dwImgIdx += 0x1;
//
//	DetectDataReceived(enqueItem);
//
//	return 0;
//}

//void Performance::folderProcessLoop(string msFolderPath)
//{
//	path procPath(msFolderPath);
//
//	if (!exists(procPath))
//	{
//		printf("folderProcessLoop::%s not exit\n", msFolderPath.c_str());
//		return;
//	}
//
//	if (!is_directory(procPath))
//	{
//		printf("folderProcessLoop::%s not a directory\n");
//		return;
//	}
//	
//	int dwImgIdx = 0;
//
//	recursive_directory_iterator end_iter;
//
//	for (recursive_directory_iterator iter(procPath); iter != end_iter; iter++)
//	{
//		printf("file_idx: %d, find: %s\t\t\t\t\n", dwImgIdx, iter->path().string().c_str());
//
//		if (is_directory(*iter))
//		{
//			continue;
//		}
//
//		std::string strExtion = extension(*iter);
//
//		boost::to_lower(strExtion);
//
//		if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png")
//		{
//			continue;
//		}
//
//		string picturePath = iter->path().string().c_str();
//		int mdwFolderType = 0;
//		//区分是mysql还是本地路径
//		Mat matLoad;
//		if (picturePath.find("http:") < picturePath.length())
//		{
//			string strDownLoad;
//			bool bSuccess = decodeRemotePicCurl(picturePath, strDownLoad);
//			if (!bSuccess)
//			{
//				std::cout << "WriteFaceFeatureToMysql, download : " << picturePath.c_str() << ", failed\n" << std::endl;
//				return;
//			}
//			std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
//			matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);
//		}
//		else
//		{
//			matLoad = imread(picturePath);
//		}
//
//		if (matLoad.data == NULL)
//		{
//			printf("load:%s, failed\n", iter->path().string().c_str());
//
//			continue;
//		}
//
//		boost::shared_ptr<FaceDataItem> enqueItem(new FaceDataItem(FaceDataItem::DATA_ITEM_TYPE_FACE_IMAGE,
//																	mdwFolderType,
//																	matLoad,
//																	iter->path().string(),
//																	dwImgIdx));
//
//		dwImgIdx += 0x1;
//
//		DetectDataReceived(enqueItem);
//	}
//
//}

void Performance::RefreshFeature()
{
	StartupDetectThreads();

	StartupExtractThreads();

	MysqlProcessLoop();

	WaitingDetectPorcessFinished();

	WaitingExtractPorcessFinished();

	TbbDetectPipelineStop();

	TbbExtractPipelineStop();
}

void Performance::MysqlProcessLoop(void)
{
	if (mdwWriteToMysql && mdwStartId != 0 && mdwEndId != 0)
	{
		SP_FaceUrlFeatureList rSpUrlFeature(new FaceUrlFeatureList());
		GetFaceUrlFromMysql(mdwStartId, mdwEndId, mdwAlgVersions, rSpUrlFeature);

		TFaceUrlFeatureList & rUrlFeatureList = rSpUrlFeature->mListUrlFeature;
		for (TFaceUrlFeatureList::iterator iterUrlFeature = rUrlFeatureList.begin();
			iterUrlFeature != rUrlFeatureList.end();
			iterUrlFeature++)
		{
			SP_FaceUrlFeatureItem & itemUrlFeature = *iterUrlFeature;
			std::string strExtion = extension(path(itemUrlFeature->mStrFaceUrl));
			boost::to_lower(strExtion);
			if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png" && strExtion != ".jpg?vip")
			{
				continue;
			}
			string strDownLoad;
			bool bSuccess = decodeRemotePicCurl(itemUrlFeature->mStrFaceUrl, strDownLoad);
			std::vector<uchar> data(strDownLoad.begin(), strDownLoad.end());
			Mat matLoad = imdecode(data, CV_LOAD_IMAGE_COLOR);

			if (matLoad.data == NULL)
			{
				printf("load:%s, failed\n", itemUrlFeature->mStrFaceUrl.c_str());
				continue;
			}

			boost::shared_ptr<FaceDataItem> enqueItem(new FaceDataItem(FaceDataItem::DATA_ITEM_TYPE_FACE_IMAGE,
				0,
				matLoad,
				itemUrlFeature->mStrFaceUrl,
				itemUrlFeature->mStrFaceTable,
				itemUrlFeature->mdwFaceId));

			//printf("Performance::mysqlProcessLoop input IDX:%d\n", itemUrlFeature->mdwFaceId);
			DetectDataReceived(enqueItem);
		}
	}
	else
	{
		//将input图片写入数据库中
#pragma region inputtoMYSQL
		path procPath(mStrPathInput);

		if (!exists(procPath))
		{
			printf("%s not exit\n", mStrPathInput.c_str());
			return;
		}

		if (!is_directory(procPath))
		{
			printf("%s not a directory\n");
			return;
		}

		recursive_directory_iterator end_iter;

		for (recursive_directory_iterator iter(procPath); iter != end_iter; iter++)
		{
			printf("file_idx: %d, find: %s\t\t\t\t\n", mdwInputFaceSeq, iter->path().string().c_str());

			if (is_directory(*iter))
			{
				continue;
			}

			std::string strExtion = extension(*iter);

			boost::to_lower(strExtion);

			if (strExtion != ".jpg" && strExtion != ".bmp" && strExtion != ".png")
			{
				continue;
			}

			Mat matLoad = imread(iter->path().string());

			if (matLoad.data == NULL)
			{
				printf("load:%s, failed\n", iter->path().string().c_str());

				continue;
			}

			boost::shared_ptr<FaceDataItem> enqueItem(new FaceDataItem(FaceDataItem::DATA_ITEM_TYPE_FACE_IMAGE,
				0,
				matLoad,
				iter->path().string(),
				"t_cid_detail",
				0));

			mdwInputFaceSeq += 0x1;
			printf("Performance::mysqlProcessLoop input IDX:%d\n", mdwInputFaceSeq);
			DetectDataReceived(enqueItem);
		}
#pragma endregion
	}
}

int  Performance::StartupDetectThreads(void)
{
	for (int dwThreadIdx = 0; dwThreadIdx < mdwDetectHandleCnt; dwThreadIdx++)
	{
		boost::thread * pSaveThread = new boost::thread(boost::bind(&Performance::DoFaceDetectLoop,
			this,
			dwThreadIdx));

		//2015-07-15 detach to let boost recyle the thread resource when the thread exit
		pSaveThread->detach();

		boost::shared_ptr<boost::thread> saveThrPtr(pSaveThread);

		matDetectProcThreads[dwThreadIdx] = saveThrPtr;
	}

	return 0;
}

int  Performance::WaitingDetectPorcessFinished(void)
{
	while (1)
	{
		unsigned int dwPendCnt = GetDetectPendingCnt();
		unsigned int dwFinishCnt = GetDetectFinishedCnt();

		printf("Performance::WaitingDetectPorcessFinished:: pendcnt=%d, finishcnt=%d\n", dwPendCnt, dwFinishCnt);

		if (dwFinishCnt < dwPendCnt)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
		else
		{
			break;
		}
	}

	return 0;
}

int  Performance::DetectDataReceived(boost::shared_ptr<FaceDataItem>& prDataItem)
{
	unsigned int dwLeftSpace;

	boost::mutex::scoped_lock autoLocker(mDetectItemQueLocker);

	while (1)
	{
		if (dwDetectItemWPtr >= dwDetectItemRPtr)
		{
			dwLeftSpace = dwDetectItemRPtr + FACE_SNAPER_MAX_ITEM_NUM - dwDetectItemWPtr;
		}
		else
		{
			dwLeftSpace = dwDetectItemRPtr - dwDetectItemWPtr;
		}

		if (dwLeftSpace <= FACE_SNAPER_ITEM_PAD_NUM)
		{
			mDetectQueNoFullCondition.wait(autoLocker);//full,wait no full
		}
		else
		{
			break;
		}
	}

	if (mDetectBufferItem[dwDetectItemWPtr].get() != NULL)
	{
		printf("DetectDataReceived::the buffer positon not release, check it out\n");
		return FACE_SNAPER_ERR_ITEM_UNFREE;
	}

	mDetectBufferItem[dwDetectItemWPtr] = prDataItem;
	dwDetectItemWPtr = ((dwDetectItemWPtr + 0x1) & (FACE_SNAPER_MAX_ITEM_NUM - 1));
	dwDetectPendingCnt += 0x1;
	//printf("DetectDataReceived:: received detect pic cnt:%d\n", dwDetectPendingCnt);
	//wakup an thread my wait on the condition
	mDetectQueNoEmptyCondition.notify_one();

	return FACE_SNAPER_ERR_NONE;
}

boost::shared_ptr<FaceDataItem>  Performance::FetchDetectDataItemIdx(int dwThreadIdx)
{
	unsigned int dwLeftItems;
	boost::shared_ptr<FaceDataItem> pItemRet;
	boost::mutex::scoped_lock autolocker(mDetectItemQueLocker);
	if (dwDetectItemWPtr >= dwDetectItemRPtr)
	{
		dwLeftItems = dwDetectItemWPtr - dwDetectItemRPtr;
	}
	else
	{
		dwLeftItems = dwDetectItemWPtr + FACE_SNAPER_MAX_ITEM_NUM - dwDetectItemRPtr;
	}

	if (dwLeftItems == 0)
	{
		mDetectQueNoEmptyCondition.wait(autolocker);//empty, wait no empty
		return pItemRet;
	}

	pItemRet = mDetectBufferItem[dwDetectItemRPtr];
	mDetectBufferItem[dwDetectItemRPtr].reset();
	dwDetectItemRPtr = ((dwDetectItemRPtr + 0x1) & (FACE_SNAPER_MAX_ITEM_NUM - 1));
	mDetectQueNoFullCondition.notify_one();

	printf("##get Performance::FetchDetectDataItemIdx finished::##\n");

	return pItemRet;
}

void Performance::DoFaceDetectLoop(int dwThreadIdx)
{
	madwDetectStatus[dwThreadIdx] = 0;

	//if_handle_t    hDetectHandle = mDetectHandleFifo->GetIFaceHandler();
	//mDetectHanle[dwThreadIdx] = CascadeDetectManager::CreateIfDetectorHandle(0);
	//mDetectHanle[dwThreadIdx] = EagleMtcnnDetectManager::CreateIfDetectorHandle(0);
	mDetectHanle[dwThreadIdx] = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_DETECTOR_EAGLE, "model/eagle");

	printf("Performance::DoFaceDetectLoop,detect threads start::%d ### mDetectHanle[dwThreadIdx]:%d\n", dwThreadIdx, mDetectHanle[dwThreadIdx]);

	while (1)
	{
		if (madwDetectSaveStopReq[dwThreadIdx] != madwDetectSaveStopRsp[dwThreadIdx])
		{
			madwDetectSaveStopRsp[dwThreadIdx] += 0x1;
			break;
		}

		boost::shared_ptr<FaceDataItem> getedDataItem = FetchDetectDataItemIdx(dwThreadIdx);
		if (getedDataItem.get() == NULL)
		{
			//no data invalid, check it again......
			continue;
		}

		string picturePath = getedDataItem->getImgPath();
		UDataItemParam * puItemPara = getedDataItem->getItemParam();
		int              dwItemType = getedDataItem->getItemType();
		Mat              matLoad = getedDataItem->getCvColorMat();

		PIF_FACERECT   pface = NULL;
		unsigned int   count = 0;

		if (mDetectHanle[dwThreadIdx] == 0)
		{
			printf("DetectAndExtractFace::hDetectHandle invalid\n");
			IncDetectFinishedCnt();
			continue;
		}
#pragma region facedetect
		/*CascadeDetectManager::FeedOneFramToDetector(mDetectHanle[dwThreadIdx],
													matLoad,
													matLoad.cols,
													matLoad.rows,
													mdwDetectType,
													&pface,
													&count);*/
		/*EagleMtcnnDetectManager::FeedOneFramToDetector(mDetectHanle[dwThreadIdx],
			matLoad,
			matLoad.cols,
			matLoad.rows,
			&pface,
			&count);*/
		printf("###start performance::FaceRecSDKManager::FeedOneFramToDetector:::::###:\n");
		FaceRecSDKManager::FeedOneFramToDetector(mDetectHanle[dwThreadIdx],
			matLoad,
			&pface,
			&count);

		printf("###end performance::FaceRecSDKManager::FeedOneFramToDetector:::::###::count:%d\n", count);

		unsigned int dwFreeCnt = count;

		FACE_RST_TYPE  tOneFaceRect;
#pragma region nofaceprocess
		if (count == 0)
		{
#pragma region SaveInputPic
			if (mdwSaveDetectPic)
			{
				path  tempPath = path(mStrPathOut) / "noface";
				path  dstFileOrg = tempPath / path(picturePath).filename();
				//cout << "DoFaceDetectLoop, org img path=" << picturePath.c_str() << "##and save noface path=" << dstFileOrg.string().c_str() << std::endl;
				if (!exists(tempPath))
				{
					if (!create_directories(tempPath))
					{
						std::cout << "DoFaceDetectLoop::Create folder failed:" << tempPath.string().c_str()
							<< ", Please check the FS Permession settings" << std::endl;
						IncDetectFinishedCnt();
						continue;
					}
				}
				
				tOneFaceRect.Rect.left = 0;
				tOneFaceRect.Rect.top = 0;
				tOneFaceRect.Rect.right = matLoad.cols;
				tOneFaceRect.Rect.bottom = matLoad.rows;
				SaveDetectFace2Disk(matLoad, tOneFaceRect.Rect, dstFileOrg.string());
			}
#pragma endregion
			if (mdwNofaceProcMode == 1)
			{
				cout << "mdwNofaceProcMode, right=" << matLoad.cols << "##and bottom=" << matLoad.rows << std::endl;
				count = 1;
				tOneFaceRect.Rect.left = 0;
				tOneFaceRect.Rect.top = 0;
				tOneFaceRect.Rect.right = matLoad.cols;
				tOneFaceRect.Rect.bottom = matLoad.rows;
			}
			else
			{
				printf("DoFaceDetectLoop::%s, no face found\n", picturePath.c_str());
				IncDetectFinishedCnt();
				continue;
			}
		}
#pragma endregion
		else if (count == 1)
		{
			tOneFaceRect = pface[0];
		}
#pragma region mulfaceprocess
		else //select the biggest
		{
			//printf("DoFaceDetectLoop, detect face count > 1 process start;\n");
			int dwMaxFaceSize = 0;
			int maxFaceIndex = 0;
			for (int dwFaceIdx = 0; dwFaceIdx < count; dwFaceIdx++)
			{
				if ((pface[dwFaceIdx].Rect.left < 0)
					|| (pface[dwFaceIdx].Rect.top  < 0)
					|| (pface[dwFaceIdx].Rect.right  > matLoad.cols)
					|| (pface[dwFaceIdx].Rect.bottom > matLoad.rows))
				{
					printf("DoFaceDetectLoop::Detect face count > 1:roi retc out of range\t\t\t\t\n");
					//进行规避处理 pengjidong 20170808
					pface[dwFaceIdx].Rect.left = pface[dwFaceIdx].Rect.left < 0 ? 0 : pface[dwFaceIdx].Rect.left;
					pface[dwFaceIdx].Rect.top = pface[dwFaceIdx].Rect.top  < 0 ? 0 : pface[dwFaceIdx].Rect.top;
					pface[dwFaceIdx].Rect.right = (pface[dwFaceIdx].Rect.right > matLoad.cols) ? matLoad.cols : pface[dwFaceIdx].Rect.right;
					pface[dwFaceIdx].Rect.bottom = (pface[dwFaceIdx].Rect.bottom > matLoad.rows) ? matLoad.rows : pface[dwFaceIdx].Rect.bottom;
				}

				int lenth = pface[dwFaceIdx].Rect.right - pface[dwFaceIdx].Rect.left;
				int width = pface[dwFaceIdx].Rect.bottom - pface[dwFaceIdx].Rect.top;
				if (lenth > 0 && width > 0)
				{
					int faceSize = lenth * width;
					if (dwMaxFaceSize < faceSize)
					{
						dwMaxFaceSize = faceSize;
						maxFaceIndex = dwFaceIdx;
					}
				}
			} // end of for dwFaceIdx
			//printf("DoFaceDetectLoop, detect face count > 1 process end;index=%d!!!!\n", maxFaceIndex);
			tOneFaceRect = pface[maxFaceIndex];
#pragma region saveorgface
			//保存原图
			if (mdwSaveMulFace)
			{
				//cout << "######DoFaceDetectLoop, mdwSaveMulFace = 1######" << std::endl;
				std::string strBgImgName = basename(path(picturePath));
				std::ostringstream ossImgName;
				ossImgName << strBgImgName
					<< "_" << tOneFaceRect.Rect.left
					<< "_" << tOneFaceRect.Rect.top
					<< "_" << (tOneFaceRect.Rect.right - tOneFaceRect.Rect.left)
					<< "_" << (tOneFaceRect.Rect.bottom - tOneFaceRect.Rect.top)
					<< ".jpg";
				path  tempPath(mStrPathOut);
				path  orgImgPath = tempPath / "mulface";
				path  saveDirectOrg = orgImgPath / strBgImgName;
				path  dstFileOrg = saveDirectOrg / ossImgName.str();
				//cout << "DoFaceDetectLoop, org img path=" << picturePath.c_str() << "##and save mulface path=" << dstFileOrg.string().c_str() << std::endl;
				SaveDetectFace2Disk(matLoad, tOneFaceRect.Rect, dstFileOrg.string());
			}
#pragma endregion
		}
#pragma endregion
#pragma endregion

		int udwAllocedSeq = GetImageSequence();

		boost::shared_ptr<FaceDataItem> enqueItem(new FaceDataItem((int)FaceDataItem::DATA_ITEM_TYPE_FACE_EXTRAT,
			getedDataItem->getImgType(),
			udwAllocedSeq,
			matLoad,
			tOneFaceRect,
			getedDataItem->getImgPath(),
			getedDataItem->getImgTable(),
			getedDataItem->getImgIdx()));

		ExtractDataReceived(enqueItem);

		if (dwFreeCnt > 0)
		{
			//CascadeDetectManager::ReleaseDetFaceResult(mdwDetectType, pface, dwFreeCnt);
			//EagleMtcnnDetectManager::ReleaseDetFaceResult(pface, dwFreeCnt);
			FaceRecSDKManager::ReleaseDetFaceResult(pface, dwFreeCnt);
		}

		IncDetectFinishedCnt();
	} //end of while

	if (mDetectHanle[dwThreadIdx] != 0)
	{
		//CascadeDetectManager::DestoryIfDetectoHandle(mDetectHanle[dwThreadIdx]);
		//EagleMtcnnDetectManager::DestoryIfDetectoHandle(mDetectHanle[dwThreadIdx]);
		FaceRecSDKManager::DestoryIfHandle(mDetectHanle[dwThreadIdx]);
	}

	//mDetectHandleFifo->ReturnIFaceHandler(hDetectHandle);
	madwDetectStatus[dwThreadIdx] = 1;
}

int Performance::GetImageSequence(void)
{
	int udwSeq;
	boost::mutex::scoped_lock autolocker(mFaceSeqLock);
	udwSeq = mdwInputFaceSeq;
	mdwInputFaceSeq += 0x1;
	return udwSeq;
}

void Performance::IncDetectFinishedCnt(void)
{
	boost::mutex::scoped_lock autolocker(mDetectProcLocker);
	dwDetectFinishedCnt += 0x1;
	printf("IncDetectFinishedCnt,detect pic cnt:%d\n", dwDetectFinishedCnt);
}

unsigned int Performance::GetDetectFinishedCnt(void)
{
	boost::mutex::scoped_lock autolocker(mDetectProcLocker);
	return dwDetectFinishedCnt;
}

unsigned int Performance::GetDetectPendingCnt(void)
{
	boost::mutex::scoped_lock autolocker(mDetectProcLocker);
	return dwDetectPendingCnt;
}

void Performance::TbbDetectPipelineStop(void)
{
	//send the stop request to thread
	for (int dwThreadIdx = 0; dwThreadIdx < mdwDetectHandleCnt; dwThreadIdx++)
	{
		madwDetectSaveStopReq[dwThreadIdx] += 0x1;
	}

	//wait the thread reponse the stop request
	for (int dwThreadIdx = 0; dwThreadIdx < mdwDetectHandleCnt; dwThreadIdx++)
	{
		//need to notify to makesure the thread wakup form sleep to decide weather to 
		//exit.
		mDetectQueNoEmptyCondition.notify_all();
	}

	DetectThreadWaitStop();

	//send the thread exit count
	//now we just create some work threads instead of TBB PipeLine;
	for (int dwThreadIdx = 0; dwThreadIdx < mdwDetectHandleCnt; dwThreadIdx++)
	{
		if (matDetectProcThreads[dwThreadIdx].get() == NULL)
		{
			continue;
		}

		matDetectProcThreads[dwThreadIdx]->join();
	}
}

void Performance::DetectThreadWaitStop(void)
{
	int dwThreadPending;    //the thread count pending to exit

	while (1)                //wait untile all thread exited
	{
		dwThreadPending = 0;

		for (int dwThreadIdx = 0; dwThreadIdx < mdwDetectHandleCnt; dwThreadIdx++)
		{
			if (madwDetectSaveStopReq[dwThreadIdx] != madwDetectSaveStopRsp[dwThreadIdx])
			{
				dwThreadPending += 0x1;
			}
		}

		if (dwThreadPending == 0)
		{
			break;
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	} //end of while(1)
}


int  Performance::StartupExtractThreads(void)
{
	for (int dwThreadIdx = 0; dwThreadIdx < mdwExtractHandleCnt; dwThreadIdx++)
	{
		boost::thread * pSaveThread = new boost::thread(boost::bind(&Performance::DoFaceExtractLoop,
			this,
			dwThreadIdx));

		//2015-07-15 detach to let boost recyle the thread resource when the thread exit
		pSaveThread->detach();

		boost::shared_ptr<boost::thread> saveThrPtr(pSaveThread);

		matExtractProcThreads[dwThreadIdx] = saveThrPtr;
	}

	return 0;
}

int  Performance::WaitingExtractPorcessFinished(void)
{
	while (1)
	{
		unsigned int dwPendCnt = GetExtractPendingCnt();
		unsigned int dwFinishCnt = GetExtractFinishedCnt();

		printf("Performance::WaitingExtractPorcessFinished:: pendcnt=%d, finishcnt=%d\n", dwPendCnt, dwFinishCnt);

		if (dwFinishCnt < dwPendCnt)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
		else
		{
			break;
		}
	}

	return 0;
}

int Performance::ExtractDataReceived(boost::shared_ptr<FaceDataItem>& prDataItem)
{
	unsigned int dwLeftSpace;

	boost::mutex::scoped_lock autoLocker(mExtractItemQueLocker);

	while (1)
	{
		if (dwExtractItemWPtr >= dwExtractItemRPtr)
		{
			dwLeftSpace = dwExtractItemRPtr + FACE_SNAPER_MAX_ITEM_NUM - dwExtractItemWPtr;
		}
		else
		{
			dwLeftSpace = dwExtractItemRPtr - dwExtractItemWPtr;
		}

		if (dwLeftSpace <= FACE_SNAPER_ITEM_PAD_NUM)
		{
			mExtractQueNoFullCondition.wait(autoLocker);//full,wait no full
		}
		else
		{
			break;
		}
	}

	if (mExtractBufferItem[dwExtractItemWPtr].get() != NULL)
	{
		printf("ExtractDataReceived::the buffer positon not release, check it out\n");
		return FACE_SNAPER_ERR_ITEM_UNFREE;
	}

	mExtractBufferItem[dwExtractItemWPtr] = prDataItem;
	dwExtractItemWPtr = ((dwExtractItemWPtr + 0x1) & (FACE_SNAPER_MAX_ITEM_NUM - 1));
	dwExtractPendingCnt += 0x1;
	//wakup an thread my wait on the condition
	mExtractQueNoEmptyCondition.notify_one();

	return FACE_SNAPER_ERR_NONE;
}

boost::shared_ptr<FaceDataItem>  Performance::FetchExtractDataItemIdx(int dwThreadIdx)
{
	unsigned int dwLeftItems;
	boost::shared_ptr<FaceDataItem> pItemRet;
	boost::mutex::scoped_lock autolocker(mExtractItemQueLocker);
	if (dwExtractItemWPtr >= dwExtractItemRPtr)
	{
		dwLeftItems = dwExtractItemWPtr - dwExtractItemRPtr;
	}
	else
	{
		dwLeftItems = dwExtractItemWPtr + FACE_SNAPER_MAX_ITEM_NUM - dwExtractItemRPtr;
	}

	if (dwLeftItems == 0)
	{
		mExtractQueNoEmptyCondition.wait(autolocker);//empty, wait no empty
		return pItemRet;
	}

	mdwExtractCountAcc += 0x1;
	mdwExtractCountOnce += 0x1;

	std::time_t timeAccTicksTmp = time(NULL);

	if (timeExtractAccTicks == 0)
	{
		timeExtractAccTicks = timeAccTicksTmp;
		mdwExtractCountOnce = 0;
	}
	else if (timeExtractAccTicks != 0 && timeExtractAccTicks != timeAccTicksTmp)
	{
		mdwExtractCountTimes += 0x1;

		printf("time: %d, extrace cnt per second: %f, avg: %f \n", (long long)timeAccTicksTmp, ((float)mdwExtractCountOnce) / (timeAccTicksTmp - timeExtractAccTicks), ((float)mdwExtractCountAcc) / mdwExtractCountTimes);
		fprintf(pfResustFile, "time: %d, extrace cnt per second: %f, avg: %f \n", (long long)timeAccTicksTmp, ((float)mdwExtractCountOnce) / (timeAccTicksTmp - timeExtractAccTicks), ((float)mdwExtractCountAcc) / mdwExtractCountTimes);

		timeExtractAccTicks = timeAccTicksTmp;
		mdwExtractCountOnce = 0;
	}

	pItemRet = mExtractBufferItem[dwExtractItemRPtr];
	mExtractBufferItem[dwExtractItemRPtr].reset();
	dwExtractItemRPtr = ((dwExtractItemRPtr + 0x1) & (FACE_SNAPER_MAX_ITEM_NUM - 1));
	mExtractQueNoFullCondition.notify_one();

	return pItemRet;
}

void Performance::DoFaceExtractLoop(int dwThreadIdx)
{
	madwExtractStatus[dwThreadIdx] = 0;

	//if_handle_t    hExtractHandle = mExtractHandleFifo->GetIFaceHandler();
	//mExtractHanle[dwThreadIdx] = IfMutilAlgExtManger::CreateIfExtratorHandle(mStrModelPath);
	time_t now_time;
	now_time = time(NULL);
	printf("Performance::DoFaceExtractLoop,Extract Feature threads start::%d,%d\n", dwThreadIdx, now_time);
	mExtractHanle[dwThreadIdx] = FaceRecSDKManager::CreateIfSDKHandle(E_IFaceRecType::IFACEREC_EXTRACTOR_GLUON, "model/gluon");
	now_time = time(NULL);
	printf("Performance::DoFaceExtractLoop,Extract Feature threads end::%d,%d\n", dwThreadIdx, now_time);
	fprintf(pfResustFile, "Performance::DoFaceExtractLoop,Extract Feature threads start::%d\n", dwThreadIdx);

	while (1)
	{
		if (madwExtractSaveStopReq[dwThreadIdx] != madwExtractSaveStopRsp[dwThreadIdx])
		{
			madwExtractSaveStopRsp[dwThreadIdx] += 0x1;
			printf("madwExtractSaveStopReq[%d] !=  ###\n", dwThreadIdx);
			break;
		}

		boost::shared_ptr<FaceDataItem> getedDataItem = FetchExtractDataItemIdx(dwThreadIdx);
		if (getedDataItem.get() == NULL)
		{
			//no data invalid, check it again......
			continue;
		}

		//printf("####Performance::DoFaceExtractLoop:: get a FetchExtractDataItemIdx::dwThreadIdx=%d#####\n", dwThreadIdx);

		string picturePath = getedDataItem->getImgPath();
		int dwItemType = getedDataItem->getItemType();
		UDataItemParam * puItemPara = getedDataItem->getItemParam();

		if (dwItemType != FaceDataItem::DATA_ITEM_TYPE_FACE_EXTRAT)
		{
			printf("DoFaceExtractLoop::Feather extrater, but wrong param type\n");
			IncExtractFinishedCnt();
			continue;
		}

		if (mExtractHanle[dwThreadIdx] == 0)
		{
			printf("DoFaceExtractLoop::hExtractHandle invalid\n");
			IncExtractFinishedCnt();
			continue;
		}

		// Face detection and feature extraction
		IF_FACERECT & tOneFaceRect = puItemPara->tExtratInfo.tFaceInfo;
		Mat &         matLoad = getedDataItem->getCvColorMat();

		//printf("ExtratFeatureFromImage::tOneFaceRect,left=%d,right=%d,top=%d,bottom=%d\n", tOneFaceRect.Rect.left, tOneFaceRect.Rect.right, tOneFaceRect.Rect.top, tOneFaceRect.Rect.bottom);

		TFeatureSeqIdInfo tSeqIdInfo;
		tSeqIdInfo.dwSwSequence = 0;
		tSeqIdInfo.dwTrackId = 0;

#pragma region faceextract
		float *     pfFeatureRst = NULL;
		int         flen = 0;

		T_IFaceRecRect tFaceRect;
		tFaceRect.left = tOneFaceRect.Rect.left;
		tFaceRect.top = tOneFaceRect.Rect.top;
		tFaceRect.right = tOneFaceRect.Rect.right;
		tFaceRect.bottom = tOneFaceRect.Rect.bottom;

		//printf("ExtratGluonFeature::%d,%d,%d,%d\n", tFaceRect.left, tFaceRect.top, tFaceRect.right, tFaceRect.bottom);

		/*flen = IfMutilAlgExtManger::ExtratFeatureFromImage(mExtractHanle[dwThreadIdx],
															matLoad,
															tOneFaceRect.Rect,
															&pfFeatureRst,
															MutilAlgExtractor::TARGS_EXTRA_TYPE_FACE,
															mdwAlgVersions,
															tSeqIdInfo,
															0);*/
		flen = FaceRecSDKManager::ExtratGluonFeature(mExtractHanle[dwThreadIdx], matLoad, tFaceRect, &pfFeatureRst);

		if (pfFeatureRst == NULL || flen == 0)
		{
			std::cout << "DoFaceExtractLoop, alg type: " << mdwAlgVersions << ",path:" << picturePath.c_str() << " ,extract failed" << std::endl;
			continue;
		}
#pragma endregion

		std::string strFileName = basename(picturePath);
#ifdef _MSC_VER
		std::string strFeaturePath = path(picturePath).parent_path().string() + "\\" + strFileName + ".bin";
#else
		std::string strFeaturePath = path(picturePath).parent_path().string() + "/" + strFileName + ".bin";
#endif
		//cout << "feature bin path=" << strFeaturePath.c_str() << std::endl;
		boost::filesystem::path pathOneFeature(strFeaturePath);
#pragma region SaveQueryFeatures
		//save local url
		if (mdwSaveFeatureBin && (picturePath.find("http:") >= picturePath.length()))
		{
			fprintf(pfResustFile, "DoFaceExtractLoop::mdwSaveFeatureBin::path=%s\n", strFeaturePath.c_str());
			boost::filesystem::path pathSave2Dir = pathOneFeature.branch_path();
			if (!exists(pathSave2Dir))
			{
				try{
					if (!create_directories(pathSave2Dir))
					{
						std::cout << "DoFaceExtractLoop::Create folder failed:" << pathSave2Dir.string().c_str()
							<< ", Please check the FS Permession settings" << std::endl;
						IncExtractFinishedCnt();
						continue;
					}
				}
				catch (...)
				{
					std::cout << "DoFaceExtractLoop::Create folder failed:" << pathSave2Dir.string().c_str()
						<< ", Please check the FS Permession settings" << std::endl;
					IncExtractFinishedCnt();
					continue;
				}
			}

			std::ofstream fsFeatureSave(strFeaturePath, std::ios::out | std::ios::binary);
			if (!fsFeatureSave.is_open())
			{
				std::cout << "DoFaceExtractLoop::Open write file failed: " << strFeaturePath;
				fprintf(pfResustFile, "DoFaceExtractLoop::Open write file failed:\n");
			}
			else
			{
				int32_t dwSaveFileLen = flen * sizeof(float);
				fsFeatureSave.write((char *)pfFeatureRst, dwSaveFileLen);
				fsFeatureSave.close();
			}
		}
#pragma endregion
#pragma region saveFeatureToMysql
		if (mdwWriteToMysql || mdwWriteInputToMysql)
		{
			//cout << "Save Feature to MYSQL:path=" << getedDataItem->getImgPath().c_str() << std::endl;
			fprintf(pfResustFile, "Save Feature to MYSQL:path=%s\n", getedDataItem->getImgPath().c_str());
			//将特征值写入mysql数据库
			SP_FaceUrlFeatureItem pNewUrlFeature(new FaceUrlFeatureItem());
			SP_FeatureWrapperItem pNewFeaturePtr(new FeatureWrapperItem(mdwAlgVersions, pfFeatureRst, flen));
			pNewUrlFeature->mSpFaceFeature = pNewFeaturePtr;
			pNewUrlFeature->mdwFaceCidId = 0;
			pNewUrlFeature->mdwFaceId = getedDataItem->getImgIdx();
			pNewUrlFeature->mStrFaceUrl = getedDataItem->getImgPath();
			pNewUrlFeature->mStrFaceTable = getedDataItem->getImgTable();
			pNewUrlFeature->mdwFaceSeq = 0;
			int result = WriteFaceFeatureToMysql(pNewUrlFeature);
			//printf("write to mysql result=%d,threadidx=%d\n",result,dwThreadIdx);
			//if ((flen > 0) && (pfFeatureRst != NULL))
			//{
			//	printf("ExtratFeatureFromImage::ReleaseFeautreResult,dwThreadIdx=%d\n", dwThreadIdx);
			//	IfMutilAlgExtManger::ReleaseFeautreResult(pfFeatureRst, mdwAlgVersions);
			//}
			//printf("ExtratFeatureFromImage::Write to mysql end,dwThreadIdx=%d\n", dwThreadIdx);
			IncExtractFinishedCnt();
			//printf("ExtratFeatureFromImage::Write to mysql end2121,finish=%d, dwThreadIdx=%d\n", dwExtractFinishedCnt, dwThreadIdx);
			continue;
		}
#pragma endregion
		if ((flen > 0) && (pfFeatureRst != NULL))
		{
			//IfMutilAlgExtManger::ReleaseFeautreResult(pfFeatureRst, mdwAlgVersions);
			FaceRecSDKManager::ReleaseFeautreResult(pfFeatureRst);
		}

		IncExtractFinishedCnt();
	} //end of while

	//printf("ExtratFeatureFromImage::DestoryIfExtratorHandle,dwThreadIdx=%d\n", dwThreadIdx);

	if (mExtractHanle[dwThreadIdx] != 0)
	{
		//IfMutilAlgExtManger::DestoryIfExtratorHandle(mExtractHanle[dwThreadIdx]);
		FaceRecSDKManager::DestoryIfHandle(mExtractHanle[dwThreadIdx]);
	}

	//mExtractHandleFifo->ReturnIFaceHandler(hExtractHandle);
	madwExtractStatus[dwThreadIdx] = 1;
}

void Performance::IncExtractFinishedCnt(void)
{
	boost::mutex::scoped_lock autolocker(mExtractProcLocker);
	dwExtractFinishedCnt += 0x1;
	printf("IncExtractFinishedCnt, extract feature cnt:%d\n", dwExtractFinishedCnt);
}

unsigned int Performance::GetExtractFinishedCnt(void)
{
	boost::mutex::scoped_lock autolocker(mExtractProcLocker);
	return dwExtractFinishedCnt;
}

unsigned int Performance::GetExtractPendingCnt(void)
{
	boost::mutex::scoped_lock autolocker(mExtractProcLocker);
	return dwExtractPendingCnt;
}

void Performance::TbbExtractPipelineStop(void)
{
	//send the stop request to thread
	for (int dwThreadIdx = 0; dwThreadIdx < mdwExtractHandleCnt; dwThreadIdx++)
	{
		madwExtractSaveStopReq[dwThreadIdx] += 0x1;
	}

	//wait the thread reponse the stop request
	for (int dwThreadIdx = 0; dwThreadIdx < mdwExtractHandleCnt; dwThreadIdx++)
	{
		//need to notify to makesure the thread wakup form sleep to decide weather to 
		//exit.
		mExtractQueNoEmptyCondition.notify_all();
	}
	ExtractThreadWaitStop();
	//send the thread exit count
	//now we just create some work threads instead of TBB PipeLine;
	for (int dwThreadIdx = 0; dwThreadIdx < mdwExtractHandleCnt; dwThreadIdx++)
	{
		if (matExtractProcThreads[dwThreadIdx].get() == NULL)
		{
			continue;
		}
		matExtractProcThreads[dwThreadIdx]->join();
	}
}

void Performance::ExtractThreadWaitStop(void)
{
	int dwThreadPending;    //the thread count pending to exit

	while (1)                //wait untile all thread exited
	{
		dwThreadPending = 0;
		for (int dwThreadIdx = 0; dwThreadIdx < mdwExtractHandleCnt; dwThreadIdx++)
		{
			if (madwExtractSaveStopReq[dwThreadIdx] != madwExtractSaveStopRsp[dwThreadIdx])
			{
				dwThreadPending += 0x1;
			}
		}
		if (dwThreadPending == 0)
		{
			break;
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	} //end of while(1)
}

int Performance::WriteResultToMysql(SP_TestResultItem & rSpTestResultItem)
{
	// 根据结果类型写入不同的详细结果数据表中
	int resultid = 0;
	switch (rSpTestResultItem->mdwResultType)
	{
	case 0:
		resultid = WriteStaticResultToMysql(rSpTestResultItem);
		break;
	case 1:
		break;
	default:
		break;
	}

	//写入t_result_info中
	rSpTestResultItem->mdwResultId = resultid;
	WriteResultInfoToMysql(rSpTestResultItem);

	return 0;
}

int Performance::WriteResultInfoToMysql(SP_TestResultItem & rSpTestResultItem)
{
	int dwRet = 0;
	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();
	std::cout << "Performance,WriteResultInfoToMysql, pDbHandl:" << pDbHandl << std::endl;
	if (pDbHandl == NULL)
	{
		std::cout << "WriteResultInfoToMysql,init mysql handle is null." << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (rSpTestResultItem.get() == NULL)
	{
		std::cout << "WriteResultInfoToMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[6];
	std::ostringstream osQuery;
	std::ostringstream osTableName;
	osQuery << "INSERT INTO " << "intellif_test.t_result_info"
		<< " (sampleid,resultid,status,type,created,updated) "
		<< " VALUES ( ?,?,?,?,?,? )";
	std::cout << "sql: " << osQuery.str() << std::endl;
	stmt = mysql_stmt_init(pDbHandl);
	if (stmt == NULL)
	{
		std::cout << "WriteResultInfoToMysql, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "WriteResultInfoToMysql,mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	memset(params, 0, sizeof(params));
	params[0].buffer_type = MYSQL_TYPE_LONG;
	params[0].buffer = &rSpTestResultItem->mdwSampleId;
	params[0].buffer_length = sizeof(rSpTestResultItem->mdwSampleId);

	params[1].buffer_type = MYSQL_TYPE_LONG;
	params[1].buffer = &rSpTestResultItem->mdwResultId;
	params[1].buffer_length = sizeof(rSpTestResultItem->mdwResultId);

	params[2].buffer_type = MYSQL_TYPE_LONG;
	params[2].buffer = &rSpTestResultItem->mdwResultStatus;
	params[2].buffer_length = sizeof(rSpTestResultItem->mdwResultStatus);

	params[3].buffer_type = MYSQL_TYPE_LONG;
	params[3].buffer = &rSpTestResultItem->mdwResultType;
	params[3].buffer_length = sizeof(rSpTestResultItem->mdwResultType);

	std::cout << "WriteResultInfoToMysql,mysql_stmt_bind_param: " << rSpTestResultItem->mdwSampleId << "," << rSpTestResultItem->mdwResultId << "," << rSpTestResultItem->mdwResultStatus << "," << rSpTestResultItem->mdwResultType << std::endl;

	string strNowTime;
	boost::posix_time::ptime   nowTime(boost::posix_time::second_clock::local_time());
	strNowTime = to_iso_string(nowTime);
	/*
	else
	{
	struct std::tm tmValLocal;
	#ifdef _MSC_VER
	localtime_s(&tmValLocal, &rAlarmInfo.tTimeStamp);
	#else
	localtime_r(&rAlarmInfo.tTimeStamp, &tmValLocal);
	#endif
	ptime        rcdTime(ptime_from_tm(tmValLocal));
	alarmOfTime = to_iso_string(rcdTime);
	}
	*/
	params[4].buffer_type = MYSQL_TYPE_STRING;
	params[4].buffer = (void *)strNowTime.c_str();
	params[4].buffer_length = (unsigned long)strNowTime.length();
	params[5].buffer_type = MYSQL_TYPE_STRING;
	params[5].buffer = (void *)strNowTime.c_str();
	params[5].buffer_length = (unsigned long)strNowTime.length();

	if (0 != mysql_stmt_bind_param(stmt, params))
	{
		std::cout << "WriteResultInfoToMysql,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "WriteResultInfoToMysql,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

int Performance::UpdateResultInfoToMysql(SP_TestResultItem & rSpTestResultItem)
{
	int dwRet = 0;
	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();
	std::cout << "Performance,UpdateResultInfoToMysql, pDbHandl:" << pDbHandl << std::endl;
	if (pDbHandl == NULL)
	{
		std::cout << "UpdateResultInfoToMysql,init mysql handle is null." << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (rSpTestResultItem.get() == NULL)
	{
		std::cout << "UpdateResultInfoToMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[3];
	std::ostringstream osQuery;
	std::ostringstream osTableName;
	osQuery << "UPDATE " << "intellif_test.t_result_info"
		<< " SET resultid = ? , status = ? ,updated = ? WHERE id=" << rSpTestResultItem->mdwResultInfoId;

	std::cout << "sql: " << osQuery.str() << std::endl;

	stmt = mysql_stmt_init(pDbHandl);
	if (stmt == NULL)
	{
		std::cout << "UpdateResultInfoToMysql, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "UpdateResultInfoToMysql,mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	memset(params, 0, sizeof(params));
	params[0].buffer_type = MYSQL_TYPE_LONG;
	params[0].buffer = &rSpTestResultItem->mdwResultId;
	params[0].buffer_length = sizeof(rSpTestResultItem->mdwResultId);

	params[1].buffer_type = MYSQL_TYPE_LONG;
	params[1].buffer = &rSpTestResultItem->mdwResultStatus;
	params[1].buffer_length = sizeof(rSpTestResultItem->mdwResultStatus);

	std::cout << "UpdateResultInfoToMysql,mysql_stmt_bind_param: " << rSpTestResultItem->mdwResultId << "," << rSpTestResultItem->mdwResultStatus << std::endl;

	string strNowTime;
	boost::posix_time::ptime   nowTime(boost::posix_time::second_clock::local_time());
	strNowTime = to_iso_string(nowTime);
	params[2].buffer_type = MYSQL_TYPE_STRING;
	params[2].buffer = (void *)strNowTime.c_str();
	params[2].buffer_length = (unsigned long)strNowTime.length();

	if (0 != mysql_stmt_bind_param(stmt, params))
	{
		std::cout << "UpdateResultInfoToMysql,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "UpdateResultInfoToMysql,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

int Performance::WriteStaticResultToMysql(SP_TestResultItem & rSpTestResultItem)
{
	int dwRet = 0;
	MYSQL * pDbHandl = (MYSQL *)mMysqlHandleFifo->GetIFaceHandler();

	std::cout << "Performance,WriteStaticResultToMysql, pDbHandl:" << pDbHandl << std::endl;

	if (pDbHandl == NULL)
	{
		std::cout << "WriteStaticResultToMysql,init mysql handle is null." << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (rSpTestResultItem.get() == NULL)
	{
		std::cout << "WriteStaticResultToMysql, param is error" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}

	MYSQL_STMT *stmt;
	MYSQL_BIND  params[7];
	std::ostringstream osQuery;
	std::ostringstream osTableName;
	osQuery << "INSERT INTO " << "intellif_test.t_result_detail"
		<< " (top1,top5,top7,top10,xpath,temppath,count) "
		<< " VALUES ( ?,?,?,?,?,?,? )";

	std::cout << "sql: " << osQuery.str() << std::endl;

	stmt = mysql_stmt_init(pDbHandl);
	if (stmt == NULL)
	{
		std::cout << "WriteStaticResultToMysql, stmt == NULL" << std::endl;
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (0 != mysql_stmt_prepare(stmt, osQuery.str().c_str(), (unsigned long)osQuery.str().length()))
	{
		std::cout << "WriteStaticResultToMysql,mysql_stmt_prepare failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	memset(params, 0, sizeof(params));
	params[0].buffer_type = MYSQL_TYPE_LONG;
	params[0].buffer = &rSpTestResultItem->mSpStaticResultItem->mdwTop1;
	params[0].buffer_length = sizeof(rSpTestResultItem->mSpStaticResultItem->mdwTop1);

	params[1].buffer_type = MYSQL_TYPE_LONG;
	params[1].buffer = &rSpTestResultItem->mSpStaticResultItem->mdwTop5;
	params[1].buffer_length = sizeof(rSpTestResultItem->mSpStaticResultItem->mdwTop5);

	params[2].buffer_type = MYSQL_TYPE_LONG;
	params[2].buffer = &rSpTestResultItem->mSpStaticResultItem->mdwTop7;
	params[2].buffer_length = sizeof(rSpTestResultItem->mSpStaticResultItem->mdwTop7);

	params[3].buffer_type = MYSQL_TYPE_LONG;
	params[3].buffer = &rSpTestResultItem->mSpStaticResultItem->mdwTop10;
	params[3].buffer_length = sizeof(rSpTestResultItem->mSpStaticResultItem->mdwTop10);

	params[4].buffer_type = MYSQL_TYPE_STRING;
	params[4].buffer = (void *)(rSpTestResultItem->mStrResultUrl.c_str());
	params[4].buffer_length = (unsigned long)rSpTestResultItem->mStrResultUrl.length();

	params[5].buffer_type = MYSQL_TYPE_STRING;
	params[5].buffer = (void *)(rSpTestResultItem->mStrXResultUrl.c_str());
	params[5].buffer_length = (unsigned long)rSpTestResultItem->mStrXResultUrl.length();

	params[6].buffer_type = MYSQL_TYPE_LONG;
	params[6].buffer = &rSpTestResultItem->mSpStaticResultItem->mdwSampleCount;
	params[6].buffer_length = sizeof(rSpTestResultItem->mSpStaticResultItem->mdwSampleCount);

	if (0 != mysql_stmt_bind_param(stmt, params))
	{
		std::cout << "WriteStaticResultToMysql,mysql_stmt_bind_result failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	if (0 != mysql_stmt_execute(stmt))
	{
		std::cout << "WriteStaticResultToMysql,mysql_stmt_execute failed: " << osQuery.str() << std::endl;
		mysql_stmt_close(stmt);
		mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);
		return -1;
	}
	dwRet = mysql_insert_id(pDbHandl);
	std::cout << "WriteStaticResultToMysql,mysql_stmt_execute success, id: " << dwRet << std::endl;
	mysql_stmt_free_result(stmt);
	mysql_stmt_close(stmt);
	mMysqlHandleFifo->ReturnIFaceHandler((if_handle_t)pDbHandl);

	return dwRet;
}

