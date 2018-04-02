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
      ������: Init
      ��;: ��ʼ��SDK
      �������: sLicensePath - License·��
      �������: ��
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult Init(const string & sLicensePath);

    /**********************************************************
      ������: Create
      ��;: �½��㷨���
      �������: eType - �㷨����
                sModelPath - ģ��·��
      �������: hHandle - �㷨���
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult Create(E_IFaceRecType eType, const string & sModelPath, IFaceRecHandle & hHandle);

    /**********************************************************
      ������: Destroy
      ��;: ɾ���㷨���
      �������: hHandle - ��ɾ���ľ��
      �������: ��
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult Destroy(IFaceRecHandle hHandle);

    /**********************************************************
      ������: FaceDetect
      ��;: �沿���
      �������: hHandle - �㷨���
                pRgbDat - BGRͼ������
                dwWidth - ͼ����
                dwHeight - ͼ��߶�
      �������: dFaceCnt - ��⵽��������
                pFaceRectArray - ��⵽������������
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult FaceDetect(IFaceRecHandle   hHandle,
                                               void           * pRgbDat,
                                               const int        dwWidth,
                                               const int        dwHeight,
                                               int            & dFaceCnt,
                                               T_IFaceRecRect **pFaceRectArray);

    /**********************************************************
      ������: FeatureExtract
      ��;: ������ȡ
      �������: hHandle - �㷨���
                pRgbDat - BGRͼ������
                dwWidth - ͼ����
                dwHeight - ͼ��߶�
                rFaceRect - ��Ҫ��ȡ��������
      �������: pFeature - ��ȡ����������ֵ����
                ��ȡ����������ֵ����
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult FeatureExtract(IFaceRecHandle   hHandle,
                                                   void           * pRgbData,                   
                                                   const int        dwWidth,
                                                   const int        dwHeight,
                                                   T_IFaceRecRect & tFaceRect,
                                                   int            & dFeatureLen,
                                                   float        * & pFeature);
    
    /**********************************************************
      ������: FeatureCompare
      ��;: ����ֵ�ȶ�
      �������: eType - ����ֵ��ȡ�㷨����
                pfFeature0 - ��һ������ֵ����
                dFeatureLen0 - ��һ������ֵ����
                pfFeature1 - �ڶ�������ֵ����
                dFeatureLen1 - �ڶ�������ֵ����
                fScore - ����ֵ�ȶԽ�� 0~1
      �������: ��
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult FeatureCompare(E_IFaceRecType   eType, 
                                                   const float    * pfFeature0,
                                                   unsigned int     dFeatureLen0,
                                                   const float    * pfFeature1,
                                                   unsigned int     dFeatureLen1,
                                                   float          & fScore);

    /**********************************************************
      ������: PosePredict
      ��;: �����Ƕ�����
      �������: hHandle - �㷨���
                pRgbDat - BGRͼ������
                dwWidth - ͼ����
                dwHeight - ͼ��߶�
                tFaceRect - ��Ҫ������������
      �������: dFacePose - �����Ƕ�����
                fConfidence - ���������Ŷ�
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult PosePredict(IFaceRecHandle   hHandle,
                                                void           * pRgbDat,                                        
                                                int              dwWidth,
                                                int              dwHeight,
                                                T_IFaceRecRect & tFaceRect,
                                                int            & dFacePose,
                                                float          & fConfidence);

    /**********************************************************
      ������: GenderPredict
      ��;: �����Ա�����
      �������: hHandle - �㷨���
                pRgbDat - BGRͼ������
                dwWidth - ͼ����
                dwHeight - ͼ��߶�
                tFaceRect - ��Ҫ������������
      �������: dGender - �Ա�0δ֪��1���ԣ�2Ů��
                fConfidence - ���������Ŷ�
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult GenderPredict(IFaceRecHandle  hHandle,
                                                  void          * pRgbDat,                                        
                                                  int             dwWidth,
                                                  int             dwHeight,
                                                  T_IFaceRecRect& tFaceRect,
                                                  int           & dGender,
                                                  float         & fConfidence);

    /**********************************************************
      ������: LocatePredict
      ��;: ������λ����
      �������: hHandle - �㷨���
                pRgbDat - BGRͼ������
                dwWidth - ͼ����
                dwHeight - ͼ��߶�
                tFaceRect - ��Ҫ������������
      �������: dLMCnt - ��λ�����
                ptLandMark - ��λ����������
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult LocatePredict(IFaceRecHandle       hHandle,
                                                  void               * pRgbDat,                                        
                                                  int                  dwWidth,
                                                  int                  dwHeight,
                                                  T_IFaceRecRect     & tFaceRect,
                                                  int                & dLMCnt,
                                                  T_IFaceRecPoint2f* & ptLandMark);

    /**********************************************************
      ������: ReleaseExtractResult
      ��;: �ͷ�������ȡ���
      �������: pFeature - ������ȡ���ָ��
      �������: ��
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult ReleaseExtractResult(float * pFeature);

    /**********************************************************
      ������: ReleaseDetectResult
      ��;: �ͷż����
      �������: pFaceRectArray - �����ָ��
      �������: ��
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult ReleaseDetectResult(T_IFaceRecRect * pFaceRectArray);

    /**********************************************************
      ������: ReleaseLocateResult
      ��;: �ͷŶ�λ���
      �������: ptLandMark - ��λ���ָ��
      �������: ��
      ����ֵ: 0-�ɹ�������-�����
    **********************************************************/
    IFACEREC_SDK_API IFaceRecResult ReleaseLocateResult(T_IFaceRecPoint2f * ptLandMark);
};

#endif
