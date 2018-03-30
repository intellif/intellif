/*
* @file     FaceDataItem.h
* @date     2015-06-24
* @brief     the all class defination of data item pass through the face snaping pipeline
* @author  zhong.bin (zhong.bin@intellif.com)
*
* Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
*/

#ifndef __FACE_DATA_ITEM1_H__
#define __FACE_DATA_ITEM1_H__

#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif

#ifdef _MSC_VER
#include "IFaceSDK.h"
#else
#include "ifacesdk.h"
#endif

//#include "FaceDataItem.h"
#include "TestCaseCommon.h"
//#include "Performance.h"

#include "boost/smart_ptr.hpp"

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

#include "boost/smart_ptr.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"
#include "boost/unordered_map.hpp"

#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem;

class FaceRecoder {
public:
	FaceRecoder(const string &      sOrgImgPath,
		int64_t      dwFaceCidId,
		const string &      sImgTable,
		const unsigned int  dwFaceSeq,
		float               fCmpScore = (float)0.0);

	FaceRecoder(const string &      sOrgImgPath,
		const IF_FACERECT & rFaceRect,
		const unsigned int  dwFaceSeq,
		const float  *      pfFaceFeature,
		float               fCmpScore = (float)0.0);

	~FaceRecoder();

	//string &    getOrgImgPath(void);
	unsigned int  getFaceSeq(void);
	float *       getFeature(void);
	string &      getImgPath(void);
	int64_t       getImgCidId(void);
	string &      getImgTable(void);
	float         getCmpScore(void);
	void          setCmpScore(float fScore);

	void          saveFace2Disk(path & sSave2FolderOrg, path & sSave2FolderFace);

private:
	string       msOrgImgPath;
	int64_t		 mdwFaceCidId;
	string       msImgTable;
	unsigned int mdwFaceSeq;
	IF_FACERECT  mFaceRect;

	float        mfScore;

	float        afFeature[128];

	IplImage*    cvGetSubImage(IplImage *image, CvRect roi);
};

typedef boost::unordered_map<unsigned int, boost::shared_ptr<FaceRecoder> > FaceInfoMap;

class NameListItem {

public:
	NameListItem(const string& rImgPath,
		const string& rImgTable,
		int64_t       dwFaceCidId,
		float *       pfFeature,
		int           dwFeatureLen);

	~NameListItem();

	float *  getFeaturePtr(void);
	int      getFeatureLen(void);
	string & getImgPath(void);
	int64_t  getFaceCidId(void);
	string & getImgTable(void);

	void     addMatchFace(boost::shared_ptr<FaceRecoder>& rFaceRcd, float fScore);

	int      getStatus(void);

	int		 SortReslut();

	int      OutputMatinfo2File(FILE * pfRstFile, int dwSaveInputPic, string strPathOut);

	//name list
	FaceInfoMap mNewNameList;

private:
	float   mafFeature[128];
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
#endif
