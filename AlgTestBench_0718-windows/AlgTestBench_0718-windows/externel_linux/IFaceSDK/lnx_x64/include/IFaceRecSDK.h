/*
 * @file    IfsdkWrapper.h
 * @date    2016-07-07
 * @brief   define the micoros which may different between different os/toolchains, but use all by c/cpp files
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#ifndef IFACERECSDK_H
#define IFACERECSDK_H

#include <string>
#include "IFaceRecType.h"

#ifdef _WIN32
#define IFACEREC_SDK_API __declspec(dllexport)
#elif __APPLE__
#define IFACEREC_SDK_API
#elif __ANDROID__
#define IFACEREC_SDK_API __attribute ((visibility("default")))
#elif __linux__
#define IFACEREC_SDK_API __attribute ((visibility("default")))
#endif


namespace IFaceRecSDK
{
    /**********************************************************
      函数名: Init
      用途: 初始化SDK
      输入参数: sLicensePath - License路径
      输出参数: 无
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult Init(const string & sLicensePath);

    /**********************************************************
      函数名: Create
      用途: 新建算法句柄
      输入参数: eType - 算法类型
                sModelPath - 模型路径
      输出参数: hHandle - 算法句柄
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult Create(E_IFaceRecType eType, const string & sModelPath, IFaceRecHandle & hHandle);

    /**********************************************************
      函数名: Destroy
      用途: 删除算法句柄
      输入参数: hHandle - 待删除的句柄
      输出参数: 无
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult Destroy(IFaceRecHandle hHandle);

    /**********************************************************
      函数名: FaceDetect
      用途: 面部检测
      输入参数: hHandle - 算法句柄
                pRgbDat - BGR图像数据
                dwWidth - 图像宽度
                dwHeight - 图像高度
      输出参数: dFaceCnt - 检测到的人脸数
                pFaceRectArray - 检测到的人脸框数组
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult FaceDetect(IFaceRecHandle   hHandle,
                                               void           * pRgbDat,
                                               const int        dwWidth,
                                               const int        dwHeight,
                                               int            & dFaceCnt,
                                               T_IFaceRecRect **pFaceRectArray);

    /**********************************************************
      函数名: FeatureExtract
      用途: 特征提取
      输入参数: hHandle - 算法句柄
                pRgbDat - BGR图像数据
                dwWidth - 图像宽度
                dwHeight - 图像高度
                rFaceRect - 需要提取的人脸框
      输出参数: pFeature - 提取的人脸特征值数组
                提取的人脸特征值长度
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult FeatureExtract(IFaceRecHandle   hHandle,
                                                   void           * pRgbData,                   
                                                   const int        dwWidth,
                                                   const int        dwHeight,
                                                   T_IFaceRecRect & tFaceRect,
                                                   int            & dFeatureLen,
                                                   float        * & pFeature);
    
    /**********************************************************
      函数名: FeatureCompare
      用途: 特征值比对
      输入参数: eType - 特征值提取算法类型
                pfFeature0 - 第一个特征值数组
                dFeatureLen0 - 第一个特征值长度
                pfFeature1 - 第二个特征值数组
                dFeatureLen1 - 第二个特征值长度
                fScore - 特征值比对结果 0~1
      输出参数: 无
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult FeatureCompare(E_IFaceRecType   eType, 
                                                   const float    * pfFeature0,
                                                   unsigned int     dFeatureLen0,
                                                   const float    * pfFeature1,
                                                   unsigned int     dFeatureLen1,
                                                   float          & fScore);

    /**********************************************************
      函数名: PosePredict
      用途: 人脸角度评估
      输入参数: hHandle - 算法句柄
                pRgbDat - BGR图像数据
                dwWidth - 图像宽度
                dwHeight - 图像高度
                tFaceRect - 需要评估的人脸框
      输出参数: dFacePose - 人脸角度类型
                fConfidence - 评估的置信度
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult PosePredict(IFaceRecHandle   hHandle,
                                                void           * pRgbDat,                                        
                                                int              dwWidth,
                                                int              dwHeight,
                                                T_IFaceRecRect & tFaceRect,
                                                int            & dFacePose,
                                                float          & fConfidence);

    /**********************************************************
      函数名: GenderPredict
      用途: 人脸性别评估
      输入参数: hHandle - 算法句柄
                pRgbDat - BGR图像数据
                dwWidth - 图像宽度
                dwHeight - 图像高度
                tFaceRect - 需要评估的人脸框
      输出参数: dGender - 性别，0未知，1男性，2女性
                fConfidence - 评估的置信度
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult GenderPredict(IFaceRecHandle  hHandle,
                                                  void          * pRgbDat,                                        
                                                  int             dwWidth,
                                                  int             dwHeight,
                                                  T_IFaceRecRect& tFaceRect,
                                                  int           & dGender,
                                                  float         & fConfidence);

    /**********************************************************
      函数名: LocatePredict
      用途: 人脸定位评估
      输入参数: hHandle - 算法句柄
                pRgbDat - BGR图像数据
                dwWidth - 图像宽度
                dwHeight - 图像高度
                tFaceRect - 需要评估的人脸框
      输出参数: dLMCnt - 定位点个数
                ptLandMark - 定位点坐标数组
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult LocatePredict(IFaceRecHandle       hHandle,
                                                  void               * pRgbDat,                                        
                                                  int                  dwWidth,
                                                  int                  dwHeight,
                                                  T_IFaceRecRect     & tFaceRect,
                                                  int                & dLMCnt,
                                                  T_IFaceRecPoint2f* & ptLandMark);

    /**********************************************************
      函数名: ReleaseExtractResult
      用途: 释放特征提取结果
      输入参数: pFeature - 特征提取结果指针
      输出参数: 无
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult ReleaseExtractResult(float * pFeature);

    /**********************************************************
      函数名: ReleaseDetectResult
      用途: 释放检测结果
      输入参数: pFaceRectArray - 检测结果指针
      输出参数: 无
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult ReleaseDetectResult(T_IFaceRecRect * pFaceRectArray);

    /**********************************************************
      函数名: ReleaseLocateResult
      用途: 释放定位结果
      输入参数: ptLandMark - 定位结果指针
      输出参数: 无
      返回值: 0-成功，其他-错误号
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult ReleaseLocateResult(T_IFaceRecPoint2f * ptLandMark);
};

#endif
