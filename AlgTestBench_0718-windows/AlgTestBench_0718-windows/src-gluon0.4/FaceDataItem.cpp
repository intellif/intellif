/*
 * @file    FaceDataItem.cpp
 * @date    2015-06-24
 * @brief   the data item passed through while data goes through the face processing pipeline
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#include "FaceDataItem.h"

FaceDataItem::FaceDataItem(int        dwItemType,
                           int        dwImgType,
                           Mat &      rColorImg,
                           string     rImgPath,
                           int        dwImgIdx)
{
    mrColorImg    = rColorImg;

    mdwItemType   = dwItemType;
    msImgFilePath = rImgPath;
    mImgType      = dwImgType;

    mdwImgIdx     = dwImgIdx;
}

FaceDataItem::FaceDataItem(   int            dwItemType,
                              int            dwImgType,
                              unsigned int   dwImgSeq,
                              Mat &          rColorImg,       // the image data with color generate by opencv library
                              IF_FACERECT &  rtFaceInfo,
                              string &       rImgPath,
                              int            dwImgIdx)
{
    
    uDataItemInfo.tExtratInfo.tFaceInfo   = rtFaceInfo;
    uDataItemInfo.tExtratInfo.mdwImageSeq = dwImgSeq;

    #if 0
    FACE_SNAPER_LOG_E(  "face info str: rect left:" << uDataItemInfo.tExtratInfo.tFaceInfo.Rect.left \
                      <<", top:"                    << uDataItemInfo.tExtratInfo.tFaceInfo.Rect.top\
                      <<", right:"                  << uDataItemInfo.tExtratInfo.tFaceInfo.Rect.right\
                      <<", bottom:"                 << uDataItemInfo.tExtratInfo.tFaceInfo.Rect.bottom);
    #endif

    mdwItemType   = dwItemType;
    mrColorImg    = rColorImg;
    msImgFilePath = rImgPath;
    mImgType      = dwImgType;
    mdwImgIdx     = dwImgIdx;
}


FaceDataItem::FaceDataItem(   int            dwItemType,
                              int            dwImgType,
                              unsigned int   udwImgSeq,
                              Mat &          rColorImg,
                              string &       rImgPath,
                              int            dwImgIdx)
{
    uDataItemInfo.tStoreInfo.mdwImageSeq = udwImgSeq;

    mdwItemType   = dwItemType;
    mrColorImg    = rColorImg;
    msImgFilePath = rImgPath;
    mImgType      = dwImgType;
    mdwImgIdx     = dwImgIdx;
}


FaceDataItem::FaceDataItem(   int            dwItemType,
                              int            dwImgType,
                              Mat &          rColorImg,
                              PIF_FACERECT   pFaceRect,
                              int            dwFaceCount,
                              string &       rImgPath,
                              int            dwImgIdx)
{
    uDataItemInfo.tInqueInfo.mpFaceRect.mpFrontRect   = pFaceRect;
    uDataItemInfo.tInqueInfo.mdwFaceCount             = dwFaceCount;

    mdwItemType   = dwItemType;
    mrColorImg    = rColorImg;
    msImgFilePath = rImgPath;
    mImgType      = dwImgType;
    mdwImgIdx     = dwImgIdx;
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

int      FaceDataItem::getImgType(void)
{
    return mImgType;
}


int      FaceDataItem::getImgIdx(void)
{
    return mdwImgIdx;
}