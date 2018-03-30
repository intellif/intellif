/*
 * @file     TestCaseCommon.h
 * @date     2015-06-24
 * @brief     the all class defination of data item pass through the face snaping pipeline
 * @author  zhong.bin (zhong.bin@intellif.com)
 *
 * Copyright (c) 2015, Shenzhen Intellifusion Technologies Co., Ltd.
 */

#ifndef __TEST_CASE_COMMON_H__
#define __TEST_CASE_COMMON_H__

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

#pragma warning(disable:4251)
#pragma warning(disable:4275)

#define FACE_SAVE_PADDING_V (40)
#define FACE_SAVE_PADDING_H (30)

typedef enum {

    IFACE_FUNCTION_CFG_DISABLE = 0x0,
    IFACE_FUNCTION_CFG_ENABLE = 0x1,
    
} IFaceFuncEnable;

typedef enum {

    COMPARE_FUNC_TYPE_CONV  = 0x0,
    COMPARE_FUNC_TYPE_SVM   = 0x1,

} ICompareFuncType;

typedef enum {
    COMP_SVM_PREDIECT_MODE_VALUE = 0x0,
    COMP_SVM_PREDIECT_MODE_PROBILITY = 0x1,
} SVM_PREDIECT_MODE_TYPE;

typedef enum {

    COMPARE_FUNC_ORDER_NORMAL  = 0x0,
    COMPARE_FUNC_ORDER_REVERSE = 0x1,

} ICompareFuncOrder;


typedef enum {
    FACE_SNAPER_ERR_NONE = 0x0,   // no error happened
    FACE_SNAPER_ERR_OVER_BACKEND  = -0x1,   // to many backend processor registered
    FACE_SNAPER_ERR_MISS_BACKEND  = -0x2,   // unregist but not found the backend processor
    FACE_SNAPER_ERR_OVER_TASKS    = -0x3,   // no task position founded when adding a new task
    FACE_SNAPER_ERR_WRONG_TYPE    = -0x4,   // start an invalid type task
    FACE_SNAPEr_ERR_NULL_OBJ      = -0x5,   // receive null object at callback function
    FACE_SNAPER_ERR_OVER_IPCS     = -0x6,   // start to many ipc tasks
    FACE_SNAPER_ERR_LOGIN_FAILED  = -0x7,   // login or logout to camera failed
    FACE_SNAPER_ERR_ALARM_FAILED  = -0x8,   // open or close alarm message failed
    FACE_SNAPER_ERR_REAL_PLAY     = -0x9,   // realplay open or realyplay closed failed
    FACE_SNAPER_ERR_SNAP_MODE     = -0xa,   // wrong face snap mode,when add and ipc camera task
    FACE_SNAPER_ERR_TASK_MISMATCH = -0xb,   // received a realplay data, but no relevent task
    FACE_SNAPER_ERR_FRAME_TYPE    = -0xc,   // wrong frame type from hik sdk
    FACE_SNAPER_ERR_DECODER_POS   = -0xe,   // have task postion, but the relevent decoder position is takened
    FACE_SNAPER_ERR_DECODER_LIS   = -0xf,   // register too many image listener to decoders
    FACE_SNAPER_ERR_DUMP_IMGLIS   = -0x10,  // dumplicate directly img listener for hiksnaper
    FACE_SNAPER_ERR_DUMP_UNREGIST = -0x11,  // twice ungister the img listerner for hiksnaper or,the img listerner is never registerd
    FACE_SNAPER_ERR_OVER_ITEM_BUF = -0x12,  // item buffer overflow
    FACE_SNAPER_ERR_ITEM_UNFREE   = -0x13,  // item buffer not free, but it's is saids can be store a new data item
    FACE_SNAPER_ERR_ITEM_TYPE     = -0x14,  // when doing the face front processing, received an wrong item type
    FACE_SNAPER_ERR_MISS_DECODER  = -0x15,  // a image received ,but no relevent Decoder
    FACE_SNAPER_ERR_MISS_TASK     = -0x16,  // teminate a task, but it is not exist
    FACE_SNAPER_ERR_NO_HWCARD     = -0x17,  // no hardware dec/enc card found on the computer
    FACE_SNAPER_ERR_BLAND_DECODER = -0x18,  // try to colse an invalid decoder, or an decoder that have not begin to streaming
    FACE_SNAPER_ERR_MISS_CHANNEL  = -0x19,  // try to use/free channel, but ,it is in idle state
    FACE_SNAPER_ERR_OVER_CHANNEL  = -0x1a,  // try to alloc too many hw channel
    FACE_SNAPER_ERR_SWDEC_CLOSE   = -0x1b,  //
    FACE_SNAPER_ERR_HWDEC_CLOSE   = -0x1c,  //
    FACE_SNAPER_ERR_OVER_RTSP     = -0x1d,  //
    FACE_SNAPER_ERR_MISS_RTSP     = -0x1e,  //
    FACE_SNAPER_ERR_RTSP_OPEN     = -0x1f,  // try to open an rtsp source, but faild
    FACE_SNAPER_ERR_RTSP_STREAM   = -0x20,  // try to find stream info in the rtsp streaming, but faild
    FACE_SNAPER_ERR_RTSP_VIDEO    = -0x21,  // try to find an video stream on rtsp, but faild
    FACE_SNAPER_ERR_FFMPEG_CODEC  = -0x22,  // try to find codec contex or codec, but faild
    FACE_SNAPER_ERR_FFMPEG_DATA   = -0x23,  // try to let ffmpeg decoder unknow data type or unimpletement type
    FACE_SNAPER_ERR_MISS_CB       = -0x24,  // begin to open stream while not register the proper callback
    FACE_SNAPER_ERR_DECODER_OPEN  = -0x25,  // open video decoder failed, ffmpeg
    FACE_SNAPER_ERR_PACKET_COPY   = -0x26,  // ffmpeg decoding, try to copy a AVPacket, but failed
} eFaceSnaperErrorCode;


typedef enum {
    FACE_ANGLE_LEVEL_0      = 0x0,
    FACE_ANGLE_LEVEL_1      = 0x1,
    FACE_ANGLE_LEVEL_2      = 0x2,
    FACE_ANGLE_LEVEL_3      = 0x3,
    FACE_ANGLE_LEVEL_4      = 0x4,
    FACE_ANGLE_LEVEL_5      = 0x5,
    FACE_ANGLE_LEVEL_6      = 0x6,
    FACE_ANGLE_LEVEL_7      = 0x7,
    FACE_ANGLE_LEVEL_UNKNOW = 0x8,
} EIFaceAngleLevel;


typedef struct {

    int64_t dwIdxInTable;
    
} TDbImportInfo;


typedef union {

    TDbImportInfo     tDataBaseInfo;
} UImgAccociateInfo;

typedef struct {

    int               mdwInfoType;

    UImgAccociateInfo uAccInfo;

} TAccociateInfo;

typedef struct _TFaceAttribute {

    float    fFaceQuality;
    int      dwFacePosture;
    
} TFaceAttribute;


extern int FeatureCompare(float *   pfFeature0, 
                          int       dwFeatureLen0,
                          float *   pfFeature1,
                          int       dwFeatureLen1,
                          float *   pfScore);


extern int IfFeatureCompare( float *   pfFeature0, 
                             int       dwFeatureLen0,
                             float *   pfFeature1,
                             int       dwFeatureLen1,
                             float *   pfScore);

#define FEATURE_COMPARE_FUNC_TYPE      (2)  // 0: old_sdk         1: old_feature function 2: new feature function
#define FACE_DETECT_FUNCTION_TYPE      (4)  // 0: old_sdk tracker 1: old sdk multiview    2: old sdk front         3: new sdk
#define INTELLIF_TRACK_DETECTOR_TYPE   (3)  // 0: old multiview   1: old front            2: new sdk
#define FACE_TRACK_FUNCTION_TYPE       (3)  // 0: old sdk tracker 1: old sdk front        2: blk 3: blk opted
#define FACE_EXTRAT_FUNCTION_TYPE      (2)  // 0: old sdk         1: new sdk



#if FACE_TRACK_FUNCTION_TYPE == 0 || FACE_TRACK_FUNCTION_TYPE == 2 || FACE_TRACK_FUNCTION_TYPE == 3
#define   PTRACK_RST_TYPE PIF_FACERECTID
#define   TRACK_RST_TYPE  IF_FACERECTID
#else
#define   PTRACK_RST_TYPE PIF_FACERECT
#define   TRACK_RST_TYPE  IF_FACERECT
#endif


#if FACE_DETECT_FUNCTION_TYPE == 0
#define   PFACE_RST_TYPE PIF_FACERECTID
#define   FACE_RST_TYPE  IF_FACERECTID
#else
#define   PFACE_RST_TYPE PIF_FACERECT
#define   FACE_RST_TYPE  IF_FACERECT
#endif


//for comapre optimization
#if  FEATURE_COMPARE_FUNC_TYPE  == 0

#define CREATE_FEATURE_COMPARE_HANDLE(path)                                if_create_verify_instance_with_prefix(path) 
#define DO_FEATURE_COMPARE_ACTION(ptr0, len0, ptr1, len1, rst_ptr) 		   if_verify_pca_feature(ptr0, len0, ptr1, len1, rst_ptr)
#define DESTORY_FEATURE_COMPARE_HANDLE(handle)                             if_verify_release_instance(handle)                              

#elif FEATURE_COMPARE_FUNC_TYPE == 1

#define CREATE_FEATURE_COMPARE_HANDLE(path)                                ((if_handle_t)0xffffffff) 
#define DO_FEATURE_COMPARE_ACTION(ptr0, len0, ptr1, len1, rst_ptr) 		   FeatureCompare(ptr0, len0, ptr1, len1, rst_ptr)
#define DESTORY_FEATURE_COMPARE_HANDLE(handle)                                  

#elif FEATURE_COMPARE_FUNC_TYPE == 2
#define CREATE_FEATURE_COMPARE_HANDLE(path)													IfMutilAlgCompareManger::CreateIfCmpHandle()
#define DO_FEATURE_COMPARE_ACTION(ptr0, len0, ptr1, len1, score, algtype, targtype) 		IfMutilAlgCompare::IfFeatureCompare(ptr0, len0, ptr1, len1, score, algtype, targtype)
#define DESTORY_FEATURE_COMPARE_HANDLE(handle)											    IfMutilAlgCompareManger::DestoryIfCmpHandle(handle)
#endif


#if FACE_TRACK_FUNCTION_TYPE == 0

#define CREATE_FACE_TRACKER_HANDLE(n)                                   if_facesdk_create_multi_face_tracker(n)
#define PERFORM_FACE_TRACK(handle, gray, color,cols, rows, pface, pcnt) if_facesdk_multi_face_tracker_feed_gray_frame(handle,   \
                                                                                                                gray.data,     \
                                                                                                                cols,     \
                                                                                                                rows,     \
                                                                                                                IF_FaceUp,\
                                                                                                                pface,    \
                                                                                                                pcnt)
#define RELEASE_FACE_TRACK_RESULT(pface, pcnt)                          if_facesdk_release_multitracker_result(pface, pcnt)
#define DESTROY_FACE_TRACKER_HANDLE(handle)                             if_facesdk_destroy_multi_face_tracker(handle)

#elif FACE_TRACK_FUNCTION_TYPE == 1

#define CREATE_FACE_TRACKER_HANDLE(n)                                   if_facesdk_create_frontal_detector_instance_from_resource(n)
#define PERFORM_FACE_TRACK(handle, gray, color,cols, rows, pface, pcnt) if_facesdk_frontal_detector(handle,   \
                                                                                                    gray.data,     \
                                                                                                    cols,     \
                                                                                                    rows,     \
                                                                                                    cols,     \
                                                                                                    pface,    \
                                                                                                    pcnt)
#define RELEASE_FACE_TRACK_RESULT(pface, pcnt)                          if_facesdk_release_frontal_result(pface, pcnt)
#define DESTROY_FACE_TRACKER_HANDLE(handle)                             if_facesdk_destroy_frontal_instance(handle)

#elif FACE_TRACK_FUNCTION_TYPE == 2

#define CREATE_FACE_TRACKER_HANDLE(n)                                   BLkTrackerManager::CreateBlkTrackerHandle(n)
#define PERFORM_FACE_TRACK(handle, gray, color,cols, rows, pface, pcnt) BLkTrackerManager::FeedFameDatToBlkTracker(handle,   \
                                                                                              gray,     \
                                                                                              color,    \
                                                                                              pface,    \
                                                                                              pcnt)
#define RELEASE_FACE_TRACK_RESULT(pface, pcnt)                          BLkTrackerManager::ReleaseBlkFaceResult(pface, pcnt)
#define DESTROY_FACE_TRACKER_HANDLE(handle)                             BLkTrackerManager::DestoryBlkTrackerHandle(handle)

#endif


#if  FACE_DETECT_FUNCTION_TYPE == 0

#define FACE_DETECT_RECT_TYPE                                             (0) //0, left point and width/height 1: center point radius

#define CREATE_FACE_DETECTOR_HANDLE(n)                                    if_facesdk_create_multi_face_tracker(n)
#define PERFORM_FACE_DETECT(handle, data, cols, rows, strid, pface, pcnt) if_facesdk_multi_face_tracker_feed_gray_frame(handle,   \
                                                                                                                        data,     \
                                                                                                                        cols,     \
                                                                                                                        rows,     \
                                                                                                                        IF_FaceUp,\
                                                                                                                        pface,    \
                                                                                                                        pcnt)
#define RELEASE_FACE_DETECT_RESULT(pface, pcnt)                           if_facesdk_release_multitracker_result(pface, pcnt)
#define DESTROY_FACE_DETECTOR_HANDLE(handle)                              if_facesdk_destroy_multi_face_tracker(handle)

#elif FACE_DETECT_FUNCTION_TYPE == 1

#define FACE_DETECT_RECT_TYPE                                             (0) //0, left point and width/height 1: center point radius


#define CREATE_FACE_DETECTOR_HANDLE(n)                                    if_facesdk_create_multiview_detector_instance_from_resource(0,n)
#define PERFORM_FACE_DETECT(handle, data, cols, rows, strid, pface, pcnt) if_facesdk_multiview_detector( handle,   \
                                                                                                         data,     \
                                                                                                         cols,     \
                                                                                                         rows,     \
                                                                                                         strid,    \
                                                                                                         pface,    \
                                                                                                         pcnt)
#define RELEASE_FACE_DETECT_RESULT(pface, pcnt)                            if_facesdk_release_multiview_result(pface, pcnt)
#define DESTROY_FACE_DETECTOR_HANDLE(handle)                               if_facesdk_destroy_multiview_instance(handle)

#elif FACE_DETECT_FUNCTION_TYPE == 2

#define FACE_DETECT_RECT_TYPE                                              (0) //0, left point and width/height 1: center point radius

#define CREATE_FACE_DETECTOR_HANDLE(n)                                     if_facesdk_create_frontal_detector_instance_from_resource(n)
#define PERFORM_FACE_DETECT(handle, data, cols, rows, strid,pface, pcnt)   if_facesdk_frontal_detector(handle,   \
                                                                                                       data,     \
                                                                                                       cols,     \
                                                                                                       rows,     \
                                                                                                       strid,    \
                                                                                                       pface,    \
                                                                                                       pcnt)
#define RELEASE_FACE_DETECT_RESULT(pface, pcnt)                            if_facesdk_release_frontal_result(pface, pcnt)
#define DESTROY_FACE_DETECTOR_HANDLE(handle)                               if_facesdk_destroy_frontal_instance(handle)

#elif FACE_DETECT_FUNCTION_TYPE == 3

#define FACE_DETECT_RECT_TYPE                                             (1) //0, left point and width/height 1: center point radiu


#define CREATE_FACE_DETECTOR_HANDLE(n)                                     IfDetectorManager::CreateIfDetectorHandle(n)
#define PERFORM_FACE_DETECT(handle, data, cols, rows, strid,pface, pcnt)   IfDetectorManager::FeedOneFramToDetector(handle,   \
                                                                                                       data,     \
                                                                                                       cols,     \
                                                                                                       rows,     \
                                                                                                       pface,    \
                                                                                                       pcnt)
#define RELEASE_FACE_DETECT_RESULT(pface, pcnt)                            IfDetectorManager::ReleaseBlkFaceResult(pface, pcnt)
#define DESTROY_FACE_DETECTOR_HANDLE(handle)                               IfDetectorManager::DestoryIfDetectoHandle(handle)

#elif FACE_DETECT_FUNCTION_TYPE == 4

#define FACE_DETECT_RECT_TYPE                                              (0) //0, left point and width/height 1: center point radiu


#define CREATE_FACE_DETECTOR_HANDLE(n)									   CascadeDetectManager::CreateIfDetectorHandle(n)//CascadeDetectManager::CreateIfDetectorHandle(n)
#define PERFORM_FACE_DETECT(handle,pmat,cols,rows,strid,type,pface,pcnt)   CascadeDetectManager::FeedOneFramToDetector(handle,\
																											pmat,     \
																											cols,     \
																											rows,     \
																											type,     \
																											pface,    \
																											pcnt)
#define RELEASE_FACE_DETECT_RESULT(type, pface, pcnt)                      CascadeDetectManager::ReleaseDetFaceResult(type, pface, pcnt)
#define DESTROY_FACE_DETECTOR_HANDLE(handle)							   CascadeDetectManager::DestoryIfDetectoHandle(handle)

#endif


#if INTELLIF_TRACK_DETECTOR_TYPE == 0

#define INTELLIF_DETEC_HANDLE                                                 if_handle_t
#define TRACK_DETECT_RECT_TYPE                                                (0) //0, left point and width/height 1: center point radiu

#define CREATE_INTELLIF_DETECTOR_HANDLE(n)                                    if_facesdk_create_multiview_detector_instance_from_resource(0,n)
#define PERFORM_INTELLIF_DETECT(handle, data, cols, rows, strid, pface, pcnt) if_facesdk_multiview_detector( handle,   \
                                                                                                         data,     \
                                                                                                         cols,     \
                                                                                                         rows,     \
                                                                                                         strid,    \
                                                                                                         pface,    \
                                                                                                         pcnt)
#define RELEASE_INTELLIF_DETECT_RESULT(pface, pcnt)                            if_facesdk_release_multiview_result(pface, pcnt)
#define DESTROY_INTELLIF_DETECTOR_HANDLE(handle)                               if_facesdk_destroy_multiview_instance(handle)


#elif INTELLIF_TRACK_DETECTOR_TYPE == 1

#define INTELLIF_DETEC_HANDLE                                                  if_handle_t
#define TRACK_DETECT_RECT_TYPE                                                 (0) //0, left point and width/height 1: center point radiu

#define CREATE_INTELLIF_DETECTOR_HANDLE(n)                                     if_facesdk_create_frontal_detector_instance_from_resource(n)
#define PERFORM_INTELLIF_DETECT(handle, data, cols, rows, strid,pface, pcnt)   if_facesdk_frontal_detector(handle,   \
                                                                                                       data,     \
                                                                                                       cols,     \
                                                                                                       rows,     \
                                                                                                       strid,    \
                                                                                                       pface,    \
                                                                                                       pcnt)
#define RELEASE_INTELLIF_DETECT_RESULT(pface, pcnt)                            if_facesdk_release_frontal_result(pface, pcnt)
#define DESTROY_INTELLIF_DETECTOR_HANDLE(handle)                               if_facesdk_destroy_frontal_instance(handle)

#elif INTELLIF_TRACK_DETECTOR_TYPE == 2

#define INTELLIF_DETEC_HANDLE                                                  if_handle_t
#define TRACK_DETECT_RECT_TYPE                                                 (1) //0, left point and width/height 1: center point radiu


#define CREATE_INTELLIF_DETECTOR_HANDLE(n)                                     IfDetectorManager::CreateIfDetectorHandle(n)
#define PERFORM_INTELLIF_DETECT(handle, data, cols, rows, strid,pface, pcnt)   IfDetectorManager::FeedOneFramToDetector(handle,   \
                                                                                                       data,     \
                                                                                                       cols,     \
                                                                                                       rows,     \
                                                                                                       pface,    \
                                                                                                       pcnt)
#define RELEASE_INTELLIF_DETECT_RESULT(pface, pcnt)                            IfDetectorManager::ReleaseBlkFaceResult(pface, pcnt)
#define DESTROY_INTELLIF_DETECTOR_HANDLE(handle)                               IfDetectorManager::DestoryIfDetectoHandle(handle)
#endif



#if FACE_EXTRAT_FUNCTION_TYPE == 0

#define FACE_FEATURE_LEN                                                   (181)
#define FEATURE_EXTRA_MAT_CVT                                              CV_BGR2BGRA
#define FEATURE_EXTRA_MAT_TYPE                                             (0)

#define CREATE_FEATURE_EXTRAT_HANDLE(n,path)                               if_create_verify_instance_with_prefix(path)
#define PERFORM_FEATURE_EXTRAT(handle, imgdata,cols, rows, face, feature)  if_extract_pca_feature(handle,        \
                                                                                                  imgdata.data,  \
                                                                                                  cols,     \
                                                                                                  rows,     \
                                                                                                  face,     \
                                                                                                  feature)
                                                                                              
#define RELEASE_FEATURE_EXTRAT_RESULT(feature)                             if_verify_release_feature(feature)
#define DESTROY_FEATURE_EXTRAT_HANDLE(handle)                              if_verify_release_instance(handle)


#elif FACE_EXTRAT_FUNCTION_TYPE == 1

#define FACE_FEATURE_LEN                                                   (128)  //pengjidong 20170808
#define FEATURE_EXTRA_MAT_CVT                                              CV_BGR2GRAY
#define FEATURE_EXTRA_MAT_TYPE                                             (1)

#define CREATE_FEATURE_EXTRAT_HANDLE(n,path)                               IfFeatureExtManager::CreateIfExtratorHandle(n)
#define PERFORM_FEATURE_EXTRAT(handle, imgdata,cols, rows, face, feature)  IfFeatureExtManager::ExtratFeatureFromImage(handle, \
                                                                                                                       imgdata,\
                                                                                                                       face,   \
                                                                                                                       feature)
                                                                                              
#define RELEASE_FEATURE_EXTRAT_RESULT(feature)                             IfFeatureExtManager::ReleaseFeautreResult(feature)
#define DESTROY_FEATURE_EXTRAT_HANDLE(handle)                              IfFeatureExtManager::DestoryIfExtratorHandle(handle)

#elif FACE_EXTRAT_FUNCTION_TYPE == 2

#define FACE_FEATURE_LEN                                                   (128)
#define FEATURE_EXTRA_MAT_CVT                                              CV_BGR2BGRA
#define FEATURE_EXTRA_MAT_TYPE                                             (0)

#define CREATE_FEATURE_EXTRAT_HANDLE(n,strpath)                                          IfMutilAlgExtManger::CreateIfExtratorHandle(strpath)
#define PERFORM_FEATURE_EXTRAT(handle,imgdata,cols,rows,face,feature,targs,alg,seq,mode) IfMutilAlgExtManger::ExtratFeatureFromImage(handle,  \
																																	imgdata, \
																																	face, \
																																	feature, \
																																	targs, \
																																	alg, \
																																	seq, \
																																	mode)

#define RELEASE_FEATURE_EXTRAT_RESULT(feature, alg)                                     IfMutilAlgExtManger::ReleaseFeautreResult(feature, alg)
#define DESTROY_FEATURE_EXTRAT_HANDLE(handle)                                           IfMutilAlgExtManger::DestoryIfExtratorHandle(handle)

#endif



#endif
