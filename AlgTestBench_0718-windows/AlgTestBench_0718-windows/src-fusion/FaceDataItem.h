/*
 * @file     FaceDataItem.h
 * @date     2015-06-24
 * @brief     the all class defination of data item pass through the face snaping pipeline
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#ifndef __FACE_DATA_ITEM_H__
#define __FACE_DATA_ITEM_H__

//#include "TestCaseCommon.h"

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

#include "boost/smart_ptr.hpp"

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

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

#if 0

typedef struct {
    PIF_FACERECT   mpFaceRect;
    int            mdwFaceCount;
} TExtraInqueItem;

#else

typedef union {
    PIF_FACERECTID    mpTrackRect;
    PIF_FACERECT      mpFrontRect;
} UFaceRectPtr;

typedef struct {
    int            mdwInqueType;
    UFaceRectPtr   mpFaceRect;
    int            mdwFaceCount;
} TExtraInqueItem;

#endif

typedef union {
    TFaceImageItem    tImageInfo;
    TFeatherExtraItem tExtratInfo;
    TImageStoreItem   tStoreInfo;
    TExtraInqueItem   tInqueInfo;
} UDataItemParam;

class FaceDataItem {

public:

    enum {
        DATA_ITEM_TYPE_FACE_IMAGE    = 0,
        DATA_ITEM_TYPE_VIDEO_FRAME   = 1,
        DATA_ITEM_TYPE_FACE_EXTRAT   = 2,
        DATA_ITEM_TYPE_FACE_STORE    = 3,
        DATA_ITEM_TYPE_EXTRA_INQUE   = 4,
    };

    typedef enum {
        FACE_INQUE_TYPE_DETECT = 0x0,
        FACE_INQUE_TYPE_TRACKER = 0x1,
    } E_FaceInqueType;
    
    FaceDataItem( int        dwItemType,
                  int        dwImgType,
                  Mat &      rColorImg,
                  string     rImgPath,
                  int        dwImgIdx);

    FaceDataItem( int            dwItemType,
                  int            dwImgType,
                  unsigned int   udwImgSeq,
                  Mat &          rColorImg,       // the image data with color generate by opencv library
                  IF_FACERECT &  pFaceInfo,
                  string &       rImgPath,
                  int            dwImgIdx);

    FaceDataItem( int            dwItemType,
                  int            dwImgType,
                  unsigned int   udwImgSeq,
                  Mat &          rColorImg,
                  string &       rImgPath,
                  int            dwImgIdx);      // the image data with color generate by opencv librar

    FaceDataItem( int            dwItemType,
                  int            dwImgType,
                  Mat &          rColorImg,
                  PIF_FACERECT   pFaceRect,
                  int            dwFaceCount,
                  string &       rImgPath,
                  int            dwImgIdx);
    
    virtual ~FaceDataItem();

    UDataItemParam *                  getItemParam(void);
    int                               SetItemParam(const UDataItemParam & rItemParam);
    int                               getItemType(void);
    Mat &                             getCvColorMat(void);
    string &                          getImgPath(void);
    int                               getImgType(void);
    
    int                               getImgIdx(void);
    
private:
    
    int              mdwItemType;
    
    UDataItemParam   uDataItemInfo;

    Mat              mrColorImg;       // the image data with color generate by opencv library

    string           msImgFilePath;
    int              mImgType;
    int              mdwImgIdx;
};

typedef boost::shared_ptr<FaceDataItem> SP_FaceDataItem;
#endif