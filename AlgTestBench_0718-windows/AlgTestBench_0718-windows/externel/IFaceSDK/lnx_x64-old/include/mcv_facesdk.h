/*
 * @file   mcv_facesdk.h
 * @Author SenseTime Group Limited
 * @brief  FaceSDK header for SenseTime C API.
 *
 * Copyright (c) 2014-2015, SenseTime Group Limited. All Rights Reserved.
 */

#ifndef MCV_FACESDK_INTERFACE_H_
#define MCV_FACESDK_INTERFACE_H_

#include "mcv_common.h"

/// @brief 表示不同的面部朝向，需要在人脸处理的相关函数中使用
enum MCV_FacePose
{
	MCV_StrictFrontal = 0, 		///< 严格正面
	MCV_Frontal,			///< 正面
	MCV_HalfLeft,			///< 略微向左
	MCV_HalfRight,			///< 略微向右
	MCV_FullLeft,			///< 向左
	MCV_FullRight,			///< 向右
	MCV_FrontalCW30,		///< 正面顺时针转约30度
	MCV_LeftHalfProfileCW30,	///< 略微向左并顺时针转约30度
	MCV_RightHalfProfileCW30,	///< 略微向右并顺时针约30度
	MCV_LeftProfileCW30,		///< 向左并顺时针约30度
	MCV_RightProfileCW30,		///< 向右并顺时针30度
	MCV_FrontalCCW30,		///< 正面逆时针约30度
	MCV_LeftHalfProfileCCW30,	///< 略微向左并逆时针约30度
	MCV_RightHalfProfileCCW30,	///< 略微向右并逆时针约30度
	MCV_LeftProfileCCW30,		///< 向左并逆时针约30度
	MCV_RightProfileCCW30		///< 向右并逆时针约30度
};

typedef struct _MCV_FACERECT
{
       mcv_rect_t Rect;		///< 代表面部的矩形区域
       enum MCV_FacePose Pose;	///< 面部的朝向，多角度人脸检测会返回不同结果，正脸检测只返回Frontal
       float Confidence;	///< 置信度，请注意多角度人脸检测和正脸检测返回的置信度暂时不具有可比性
} MCV_FACERECT, *PMCV_FACERECT;

//>> CONFIG_API_LRALIGNMENTOR

/// @brief 该函数用于初始化面部关键点检测功能所需要的数据
/// @param npoint 面部关键点检测的关键点数量（可选参数详见 Readme 文件）
MCV_SDK_API
mcv_handle_t mcv_facesdk_create_LRAlignmentor_instance_from_resource(int npoint);

/// @brief 调用该函数进行面部关键点检测
/// @param lrAlignmentor_instance 已完成初始化的面部关键点检测实例句柄
/// @param gray_image 用于检测的灰度图 
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param face_rect 进行面部关键点检测的脸所在的矩形区域
/// @param face_pose 进行面部关键点检测的脸的朝向
/// @param facial_points_count 面部关键点检测的关键点数量（需与初始化所用的 npoint 相同）
/// @param facial_points_array 用于保存检测结果的关键点数组（由用户分配）
MCV_SDK_API
mcv_result_t mcv_facesdk_LRAlign(
	mcv_handle_t lrAlignmentor_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	mcv_rect_t face_rect,
	unsigned int face_pose,
	unsigned int facial_points_count,
	mcv_pointf_t* facial_points_array
);


/// @brief 销毁已经完成初始化的面部关键点检测实例
/// @param lrAlignmentor_instance 已完成初始化的面部关键点检测实例句柄
MCV_SDK_API
void mcv_facesdk_destroy_LRAlignmentor_instance(
	mcv_handle_t lrAlignmentor_instance
);


/// @brief 根据21点关键点计算人脸pose信息
/// @param facial_points_array 输入人脸的面部关键点信息，目前只支持输入21点
/// @param facial_points_count facial_points_array 数组长度，即输入的关键点数量
/// @param yaw 用于保存水平转角的地址指针（由用户分配），真实度量的左负右正，如果输入NULL则不返回该结果
/// @param pitch 用于保存俯仰角的地址指针（由用户分配），真实度量的上负下正，如果输入NULL则不返回该结果
/// @param roll 用于保存旋转角的地址指针（由用户分配），真实度量的左负右正，如果输入NULL则不返回该结果
/// @param eyedist 用于保存两眼距的地址指针（由用户分配），如果输入NULL则不返回该结果
MCV_SDK_API
mcv_result_t mcv_facesdk_get_pose(
	const mcv_pointf_t *facial_points_array,
	int facial_points_count,
	float *yaw,
	float *pitch,
	float *roll,
	float *eyedist
);

//>> CONFIG_API_MULTIDETECTOR

/// @brief 进行多角度人脸检测功能初始化
/// @param skip_below_thresh 选择true则误识别减少，但是可能会出现漏检测；false则相反，建议使用true
/// @param ncpu 可用的并行CPU数，用于开启多线程加速
MCV_SDK_API
mcv_handle_t
mcv_facesdk_create_multiview_detector_instance_from_resource(int skip_below_thresh, int ncpu);

/// @brief 对指定输入图片进行多角度人脸检测
/// @param detector_instance 已完成初始化的多角度人脸检测实例句柄
/// @param gray_image 用于检测的灰度图，每个像素占1字节
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用mcv_facesdk_release_multiview_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
MCV_SDK_API
mcv_result_t
mcv_facesdk_multiview_detector(
	mcv_handle_t detector_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	PMCV_FACERECT* p_face_rects_array,
	unsigned int* face_rects_count
);

/// @brief 释放多角度人脸检测返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
MCV_SDK_API
void
mcv_facesdk_release_multiview_result(
	PMCV_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief 销毁已初始化的多角度人脸检测实例
/// @param detector_instance 待销毁的多角度人脸检测的句柄
MCV_SDK_API
void
mcv_facesdk_destroy_multiview_instance(
	mcv_handle_t detector_instance
);

//>> CONFIG_API_SPIDER_FACE_DETECTOR

/// @brief 进行多角度人脸检测功能初始化
/// @param skip_below_thresh 选择true则误识别减少，但是可能会出现漏检测；false则相反，建议使用true
/// @param ncpu 可用的并行CPU数，用于开启多线程加速
MCV_SDK_API
mcv_handle_t
mcv_facesdk_create_selftimer_face_detector_instance_from_resource(int ncpu);

/// @brief 对指定输入图片进行多角度人脸检测
/// @param detector_instance 已完成初始化的多角度人脸检测实例句柄
/// @param color_image 用于检测的四通道彩图，每个像素占4字节，Android和iOS下输入为RGBA格式，其他平台均为OpenCV默认的BGRA格式
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用mcv_facesdk_release_multiview_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
MCV_SDK_API
mcv_result_t
mcv_facesdk_selftimer_face_detector(
	mcv_handle_t detector_instance,
	const unsigned char* color_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	PMCV_FACERECT* p_face_rects_array,
	unsigned int* face_rects_count
);

/// @brief 释放多角度人脸检测返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
MCV_SDK_API
void
mcv_facesdk_release_selftimer_face_result(
	PMCV_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief 销毁已初始化的多角度人脸检测实例
/// @param detector_instance 待销毁的多角度人脸检测的句柄
MCV_SDK_API
void
mcv_facesdk_destroy_selftimer_face_detector_instance(
	mcv_handle_t detector_instance
);


//>> CONFIG_API_FRONTAL_DETECTOR

/// @brief 进行正脸检测功能初始化
/// @param ncpu 可用的并行CPU数，用于开启多线程加速
MCV_SDK_API
mcv_handle_t
mcv_facesdk_create_frontal_detector_instance_from_resource(int ncpu);

/// @brief 对指定输入图片进行正脸检测
/// @param detector_instance 已完成初始化的正脸检测实例句柄
/// @param gray_image 用于检测的灰度图，每个像素占1字节
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用mcv_facesdk_release_frontal_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
MCV_SDK_API
mcv_result_t
mcv_facesdk_frontal_detector(
	mcv_handle_t detector_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	PMCV_FACERECT* p_face_rects_array,
	unsigned int* face_rects_count
);

/// @brief 释放正脸检测返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
MCV_SDK_API
mcv_result_t
mcv_facesdk_release_frontal_result(
	PMCV_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief 销毁已初始化的正脸检测实例
/// @param detector_instance 待销毁的正脸检测实例的句柄
MCV_SDK_API
void
mcv_facesdk_destroy_frontal_instance(
	mcv_handle_t detector_instance
);

//>> CONFIG_API_END__

#endif /* end of include guard: MCV_SDK_INTERFACE_H_ */

