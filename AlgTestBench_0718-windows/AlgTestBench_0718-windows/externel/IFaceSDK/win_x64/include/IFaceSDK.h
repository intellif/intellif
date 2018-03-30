/*
 * @file   ifacesdk.h
 * @release 2015-05-18
 * @brief  IFaceSDK C API
 *
 * Copyright (c) 2014-2015, Shenzhen Intellifusion Technologies Co., Ltd. All Rights Reserved.
 */

#ifndef IF_FACESDK_INTERFACE_H_
#define IF_FACESDK_INTERFACE_H_

#define SDK_EXPORTS

#ifdef _MSC_VER
#ifdef __cplusplus
#ifdef SDK_EXPORTS
#define IF_SDK_API  extern "C" __declspec(dllexport)
#else
#define IF_SDK_API extern "C" __declspec(dllimport) 
#endif
#else
#ifdef SDK_EXPORTS
#define IF_SDK_API __declspec(dllexport)
#else
#define IF_SDK_API __declspec(dllimport)
#endif
#endif
#else /* _MSC_VER */
#ifdef __cplusplus
#ifdef SDK_EXPORTS
#define IF_SDK_API  extern "C" __attribute__((visibility ("default")))
#else
#define IF_SDK_API extern "C"
#endif
#else
#ifdef SDK_EXPORTS
#define IF_SDK_API 
#else
#define IF_SDK_API __attribute__((visibility ("default")))
#endif
#endif
#endif

typedef void* if_handle_t;
typedef int   if_result_t;

typedef struct{
	int left;	///< 矩形最左边的坐标
	int top;	///< 矩形最上边的坐标
	int right;	///< 矩形最右边的坐标
	int bottom;	///< 矩形最下边的坐标
}if_rect_t;

typedef struct{
	float x;	///< 点的水平方向坐标，为浮点数
	float y;	///< 点的竖直方向坐标，为浮点数
}if_pointf_t;

typedef struct{
	int x;		///< 点的水平方向坐标，为整数
	int y;		///< 点的竖直方向坐标，为整数
}if_pointi_t;

#define IF_OK (0)
#define IF_E_INVALIDARG (-1)
#define IF_E_HANDLE (-2)
#define IF_E_OUTOFMEMORY (-3)
#define IF_E_FAIL (-4)
#define IF_E_DELNOTFOUND (-5)


//////////////////////////////////////////////////////////////////////////////////////

#define L2FEAT_LEN 180
#define PCAFEAT_LEN 180

struct if_db_item{
	int idx;
	int len;
	float rect[4];
	int pose;
	float feat[L2FEAT_LEN];
	float feat_pca[PCAFEAT_LEN];
};

typedef struct{
	struct if_db_item item;
	float score;
	float rank_score;
} if_face_search_result_t;


//////////////////////////////////////////////////////////////////////////////////////

/// @brief 表示不同的面部朝向，需要在人脸处理的相关函数中使用
enum IF_FacePose
{
	IF_StrictFrontal = 0, 		///< 严格正面
	IF_Frontal,			///< 正面
	IF_HalfLeft,			///< 略微向左
	IF_HalfRight,			///< 略微向右
	IF_FullLeft,			///< 向左
	IF_FullRight,			///< 向右
	IF_FrontalCW30,		///< 正面顺时针转约30度
	IF_LeftHalfProfileCW30,	///< 略微向左并顺时针转约30度
	IF_RightHalfProfileCW30,	///< 略微向右并顺时针约30度
	IF_LeftProfileCW30,		///< 向左并顺时针约30度
	IF_RightProfileCW30,		///< 向右并顺时针30度
	IF_FrontalCCW30,		///< 正面逆时针约30度
	IF_LeftHalfProfileCCW30,	///< 略微向左并逆时针约30度
	IF_RightHalfProfileCCW30,	///< 略微向右并逆时针约30度
	IF_LeftProfileCCW30,		///< 向左并逆时针约30度
	IF_RightProfileCCW30		///< 向右并逆时针约30度
};

typedef struct _IF_FACERECT
{
       if_rect_t Rect;		///< 代表面部的矩形区域
       enum IF_FacePose Pose;	///< 面部的朝向，多角度人脸检测会返回不同结果，正脸检测只返回Frontal
       float Confidence;	///< 置信度，请注意多角度人脸检测和正脸检测返回的置信度暂时不具有可比性
} IF_FACERECT, *PIF_FACERECT;

//>> CONFIG_API_LRALIGNMENTOR

/// @brief 该函数用于初始化面部关键点检测功能所需要的数据
/// @param npoint 面部关键点检测的关键点数量（可选参数详见 Readme 文件）
IF_SDK_API
if_handle_t if_facesdk_create_LRAlignmentor_instance_from_resource(int npoint);

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
IF_SDK_API
if_result_t if_facesdk_LRAlign(
	if_handle_t lrAlignmentor_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	if_rect_t face_rect,
	unsigned int face_pose,
	unsigned int facial_points_count,
	if_pointf_t* facial_points_array
);


/// @brief 销毁已经完成初始化的面部关键点检测实例
/// @param lrAlignmentor_instance 已完成初始化的面部关键点检测实例句柄
IF_SDK_API
void if_facesdk_destroy_LRAlignmentor_instance(
	if_handle_t lrAlignmentor_instance
);


/// @brief 根据21点关键点计算人脸pose信息
/// @param facial_points_array 输入人脸的面部关键点信息，目前只支持输入21点
/// @param facial_points_count facial_points_array 数组长度，即输入的关键点数量
/// @param yaw 用于保存水平转角的地址指针（由用户分配），真实度量的左负右正，如果输入NULL则不返回该结果
/// @param pitch 用于保存俯仰角的地址指针（由用户分配），真实度量的上负下正，如果输入NULL则不返回该结果
/// @param roll 用于保存旋转角的地址指针（由用户分配），真实度量的左负右正，如果输入NULL则不返回该结果
/// @param eyedist 用于保存两眼距的地址指针（由用户分配），如果输入NULL则不返回该结果
IF_SDK_API
if_result_t if_facesdk_get_pose(
	const if_pointf_t *facial_points_array,
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
IF_SDK_API
if_handle_t
if_facesdk_create_multiview_detector_instance_from_resource(int skip_below_thresh, int ncpu);

/// @brief 对指定输入图片进行多角度人脸检测
/// @param detector_instance 已完成初始化的多角度人脸检测实例句柄
/// @param gray_image 用于检测的灰度图，每个像素占1字节
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用if_facesdk_release_multiview_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
IF_SDK_API
if_result_t
if_facesdk_multiview_detector(
	if_handle_t detector_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	PIF_FACERECT* p_face_rects_array,
	unsigned int* face_rects_count
);

/// @brief 释放多角度人脸检测返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
IF_SDK_API
void
if_facesdk_release_multiview_result(
	PIF_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief 销毁已初始化的多角度人脸检测实例
/// @param detector_instance 待销毁的多角度人脸检测的句柄
IF_SDK_API
void
if_facesdk_destroy_multiview_instance(
	if_handle_t detector_instance
);

//>> CONFIG_API_SPIDER_FACE_DETECTOR

/// @brief 进行多角度人脸检测功能初始化
/// @param skip_below_thresh 选择true则误识别减少，但是可能会出现漏检测；false则相反，建议使用true
/// @param ncpu 可用的并行CPU数，用于开启多线程加速
IF_SDK_API
if_handle_t
if_facesdk_create_selftimer_face_detector_instance_from_resource(int ncpu);

/// @brief 对指定输入图片进行多角度人脸检测
/// @param detector_instance 已完成初始化的多角度人脸检测实例句柄
/// @param color_image 用于检测的四通道彩图，每个像素占4字节，Android和iOS下输入为RGBA格式，其他平台均为OpenCV默认的BGRA格式
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用if_facesdk_release_multiview_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
IF_SDK_API
if_result_t
if_facesdk_selftimer_face_detector(
	if_handle_t detector_instance,
	const unsigned char* color_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	PIF_FACERECT* p_face_rects_array,
	unsigned int* face_rects_count
);

/// @brief 释放多角度人脸检测返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
IF_SDK_API
void
if_facesdk_release_selftimer_face_result(
	PIF_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief 销毁已初始化的多角度人脸检测实例
/// @param detector_instance 待销毁的多角度人脸检测的句柄
IF_SDK_API
void
if_facesdk_destroy_selftimer_face_detector_instance(
	if_handle_t detector_instance
);


//>> CONFIG_API_FRONTAL_DETECTOR

/// @brief 进行正脸检测功能初始化
/// @param ncpu 可用的并行CPU数，用于开启多线程加速
IF_SDK_API
if_handle_t
if_facesdk_create_frontal_detector_instance_from_resource(int ncpu);

/// @brief 对指定输入图片进行正脸检测
/// @param detector_instance 已完成初始化的正脸检测实例句柄
/// @param gray_image 用于检测的灰度图，每个像素占1字节
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用if_facesdk_release_frontal_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
IF_SDK_API
if_result_t
if_facesdk_frontal_detector(
	if_handle_t detector_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	PIF_FACERECT* p_face_rects_array,
	unsigned int* face_rects_count
);

/// @brief 释放正脸检测返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
IF_SDK_API
if_result_t
if_facesdk_release_frontal_result(
	PIF_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief 销毁已初始化的正脸检测实例
/// @param detector_instance 待销毁的正脸检测实例的句柄
IF_SDK_API
void
if_facesdk_destroy_frontal_instance(
	if_handle_t detector_instance
);

//>> CONFIG_API_END__

//////////////////////////////////////////////////////////////////////////////////////

//>> CONFIG_API_FACE_TRACK

enum IF_FaceOrientation
{
	IF_FaceUp = 0,			///< 人脸向上，即人脸朝向正常
	IF_FaceLeft = 1,        ///< 人脸向左，即人脸被逆时针旋转了90度
	IF_FaceDown = 2,        ///< 人脸向下，即人脸被旋转了180度
	IF_FaceRight = 3        ///< 人脸向左，即人脸被顺时针旋转了90度
};

typedef struct
{
	if_rect_t Rect;            ///< 代表面部的矩形区域
	int ID;                     ///< faceID，用于表示在实时人脸跟踪中的相同人脸在不同帧多次出现
} IF_FACERECTID, *PIF_FACERECTID;

#define IF_MUTLITRACKER_ALIGN_POINTS 21

/// @brief 进行实时人脸跟踪功能初始化
/// @param ncpu 可用的并行CPU数，用于开启多线程加速
IF_SDK_API
if_handle_t
if_facesdk_create_multi_face_tracker(int ncpu);

/// @brief 销毁已初始化的实时人脸跟踪实例
/// @param tracker_instance 待销毁的实时人脸跟踪实例的句柄
IF_SDK_API
void
if_facesdk_destroy_multi_face_tracker(if_handle_t tracker_instance);

/// @brief 对连续视频帧进行实时人脸跟踪
/// @param tracker_instance 已完成初始化的实时人脸跟踪实例句柄
/// @param gray_image 输入视频帧的灰度图，每个像素占1字节
/// @param image_width  输入视频帧的宽
/// @param image_height 输入视频帧的高
/// @param orientation 视频中人脸的方向，以输入视频帧人脸朝上为 IF_FaceUp，以人脸朝左为 IF_FaceLeft，即需要先将视频顺时针旋转90度后人脸朝上，以人脸朝下为 IF_FaceDown，以人脸朝右为 IF_FaceRight
/// @param p_face_rects_array 指向保存检测到人脸的矩形区域数组的指针，数组的内存空间需要由用户调用if_facesdk_release_multitracker_result函数释放
/// @param face_rects_count 指向保存检测到人脸数量的整数
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_feed_gray_frame(
	if_handle_t tracker_instance,
	const unsigned char *gray_image,
	unsigned int image_width,
	unsigned int image_height,
	enum IF_FaceOrientation orientation,
	PIF_FACERECTID* p_face_rects_array,
	unsigned int* face_rects_count
	);

/// @brief 对连续视频帧中检测到的人脸进行面部关键点检测
/// @param tracker_instance 已完成初始化的实时人脸跟踪实例句柄
/// @param face_rect 输入的单个人脸框（未旋转）
/// @param facial_points_array 用于保存检测结果的关键点数组，点数为 IF_MUTLITRACKER_ALIGN_POINTS （由用户分配空间）
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_align(
	if_handle_t tracker_instance,
	const IF_FACERECTID *face_rect,
	if_pointf_t *facial_points_array
);

/// @brief 根据21点关键点计算人脸pose信息
/// @param facial_points_array 输入人脸的面部关键点信息，目前只支持输入21点
/// @param facial_points_count facial_points_array 数组长度，即输入的关键点数量
/// @param yaw 用于保存水平转角的地址指针（由用户分配），真实度量的左负右正，如果输入NULL则不返回该结果
/// @param pitch 用于保存俯仰角的地址指针（由用户分配），真实度量的上负下正，如果输入NULL则不返回该结果
/// @param roll 用于保存旋转角的地址指针（由用户分配），真实度量的左负右正，如果输入NULL则不返回该结果
/// @param eyedist 用于保存两眼距的地址指针（由用户分配），如果输入NULL则不返回该结果
IF_SDK_API
if_result_t if_facesdk_get_pose(
	const if_pointf_t *facial_points_array,
	int facial_points_count,
	float *yaw,
	float *pitch,
	float *roll,
	float *eyedist
);

/// @brief 获取连续视频帧转正后的图像指针（不可释放）
/// @param tracker_instance 已完成初始化的实时人脸跟踪实例句柄
/// @param out_image 用于返回图像内容的指针（无需声明或释放存放图像的空间）
/// @param image_width  指向保存转正后图片宽度的整数
/// @param image_height 指向保存转正后图片高度的整数
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_get_preprocessed_image(
	if_handle_t tracker_instance,
	unsigned char **out_image,
	unsigned int *image_width, unsigned int *image_height
);

/// @brief 获取相对于原图的人脸框坐标
/// @param tracker_instance 已完成初始化的实时人脸跟踪实例句柄
/// @param face_rect 输入的单个人脸框指针，旋转后覆盖原人脸框
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_transform_rectangle(
	if_handle_t tracker_instance,
	IF_FACERECTID *face_rect
);

/// @brief 获取相对于原图的面部关键点坐标
/// @param tracker_instance 已完成初始化的实时人脸跟踪实例句柄
/// @param face_rect 输入的单个点坐标，旋转后覆盖原点坐标
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_transform_point(
	if_handle_t tracker_instance,
	if_pointf_t *point
);


/// @brief 释放实时人脸跟踪返回结果时分配的空间
/// @param face_rects_array 保存结果的数组
/// @param face_rects_count 结果数
IF_SDK_API
void
if_facesdk_release_multitracker_result(
	PIF_FACERECTID face_rects_array,
	unsigned int face_rects_count
);


/// @brief 创建属性识别器实例
IF_SDK_API
if_handle_t
if_facesdk_create_attribute_detector();

IF_SDK_API
if_handle_t
if_facesdk_create_attribute_detector_with_path(const char *modelname);
/// @brief 列出属性识别器支持的所有属性
/// @param attribute_instance 检测器实例
/// @param out_list 输出列表指针数组
/// @param max_list_len out_list数组的长度
/// @param out_list_len 实际输出的支持属性个数
IF_SDK_API
if_result_t
if_facesdk_attribute_detector_list(
	if_handle_t attribute_instance,
	char *out_list[],
	unsigned int max_list_len,
	unsigned int *out_list_len
);

/// @brief 使用属性识别器进行单个属性识别
/// @param attribute_instance 检测器实例
/// @param attribute_name 待检测的属性名
/// @param gray_image 用于检测的灰度图，每个像素占1字节
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param facial_points_array 输入人脸的面部关键点信息，目前只支持输入21点
/// @param facial_points_count facial_points_array数组长度，即输入的关键点数量
/// @param result, 返回1为属性为True，0为False，-1是不支持此属性
/// @param score 返回判断分数
IF_SDK_API
if_result_t
if_facesdk_attribute_detector(
	if_handle_t attribute_instance,
	const char *attribute_name,
	const unsigned char *gray_img,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	const if_pointf_t *facial_points_array,
	int facial_points_count,
	int *result,
	float *score
);

/// @brief 使用属性识别器进行批量属性识别，性能比多次调用单属性识别好
/// @param attribute_instance 检测器实例
/// @param attribute_names 待检测的属性名数组
/// @param attribute_len attribute_names 数组长度
/// @param gray_img 用于检测的灰度图，每个像素占1字节
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param facial_points_array 输入人脸的面部关键点信息，目前只支持输入21点
/// @param facial_points_count facial_points_array 数组长度，即输入的关键点数量
/// @param results 应输入长度为attribute_len长度的数组，输出各个属性判断结果（0或1）
/// @param scores  应输入长度为attribute_len长度的数组，返回判断分数
IF_SDK_API
if_result_t
if_facesdk_attribute_detector_batch(
	if_handle_t attribute_instance,
	const char* const *attribute_names,
	unsigned int attribute_len,
	const unsigned char *gray_img,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	const if_pointf_t *facial_points_array,
	int facial_points_count,
	int *result,
	float *scores
);

/// @brief 释放使用if_facesdk_attribute_detector_list返回的属性列表
/// @param attribute_list 待释放数组
/// @param attribute_len 实际属性个数
IF_SDK_API
void
if_facesdk_release_attribute_list(
	char* attribute_list[],
	unsigned int attribute_len
);

/// @brief 释放属性检测器实例
/// @param attribute_instance 检测器实例
IF_SDK_API
void
if_facesdk_destroy_attribute_detector(if_handle_t attribute_instance);


/// @brief 创建活体检测器实例
IF_SDK_API
if_handle_t
if_facesdk_create_liveness_instance();

/// @brief 释放活体检测器实例
/// @param liveness_instance 检测器实例
IF_SDK_API
void if_facesdk_destroy_liveness_instance(if_handle_t liveness_instance);

/// @brief 对连续输入的视频帧进行活体检测
/// @param liveness_instance 已完成初始化的活体检测实例句柄
/// @param gray_image 用于进行活体检测的灰度图
/// @param image_width 用于检测的灰度图的宽度
/// @param image_height 用于检测的灰度图的高度
/// @param image_stride 用于检测的灰度图中每一行的跨度（以字节计算）
/// @param facial_points_array 输入人脸的面部关键点信息，目前只支持输入21点
/// @param facial_points_count facial_points_array 数组长度，即输入的关键点数量
/// @param score 指向活体检测得分
/// @param state 指向活体检测器各项状态结果（用二进制表示），是否眨眼：state&1，是否张嘴：state&2
#define IF_LIVENESS_STATE_EYESOPEN	(1 << 0)
#define IF_LIVENESS_STATE_MOUTHOPEN	(1 << 1)
IF_SDK_API
if_result_t
if_facesdk_liveness(
	if_handle_t liveness_instance,
	const unsigned char* gray_image,
	unsigned int image_width,
	unsigned int image_height,
	unsigned int image_stride,
	if_pointf_t *facial_points_array,
	unsigned int facial_points_count,
	float *score,
	unsigned int *state
);

/// @brief 重置活体检测器，当人脸消失或发生变化时
/// @param liveness_instance 已完成初始化的活体检测实例句柄
IF_SDK_API
void if_facesdk_liveness_reset(if_handle_t liveness_instance);


enum IF_SCORE_CATEGORY {
	IF_WRONG_POS = 0,
	IF_BLUR,
	IF_OVER_EXPOSURE,
	IF_UNDER_EXPOSURE,
	IF_POSITIVE,
	IF_SCORE_CATEGORY_CNT
};

typedef struct _IF_QUALITY_SCORE
{
	float score;
	enum IF_SCORE_CATEGORY category;
} IF_QUALITY_SCORE, *PIF_QUALITY_SCORE;


IF_SDK_API
if_handle_t if_facesdk_create_face_evaluator_instance_from_resource();

IF_SDK_API
if_result_t if_facesdk_face_evaluator_evaluate(
	if_handle_t evaluator_instance,
	unsigned char *data,
	unsigned int width,
	unsigned int height,
	if_pointf_t *pose,
	unsigned int num_points,
	PIF_QUALITY_SCORE p_quality_score);

IF_SDK_API
void if_facesdk_destroy_face_evaluator_instance(if_handle_t evaluator_instance);

//>> CONFIG_API_END__


//////////////////////////////////////////////////////////////////////////////////////

//>> CONFIG_API_FACE_VERIF

/// @brief 初始化验证系统数据，返回验证系统实例句柄
IF_SDK_API
if_handle_t
if_create_verify_instance();

/// @brief 初始化验证系统数据，返回验证系统实例句柄
IF_SDK_API
if_handle_t
if_create_verify_instance_with_prefix(const char *prefix);

IF_SDK_API
if_handle_t
if_duplicate_verify_instance_with_prefix(const char *prefix, if_handle_t old_instance);

/// @brief 提取人脸特征，返回特征数据长度，可以把返回数组存储起来一遍以后使用
/// @param verify_instance 验证系统句柄
/// @param bgra_image 彩色图像数据，大小为width*height*4字节，Linux上存储顺序与OpenCV一致(BGRA)
/// @param image_width 图像宽度
/// @param image_height 图像高度
/// @param face_rect Detector返回的人脸位置
/// @param p_feature 人脸特征数据指针，使用后使用if_verify_release_feature释放
IF_SDK_API
int
if_extract_pca_feature(if_handle_t verify_instance, unsigned char *bgra_image,
		unsigned int image_width, unsigned int image_height,
		if_rect_t face_rect, float **p_feature);


/// @brief 人脸验证
/// @param verify_instance 验证系统句柄
/// @param feature1 feature2 if_extract_pca_feature取得的待验证的两张人脸特征数组
/// @param feature1_len feature2_len if_extract_pca_feature返回的特征长度
/// @param score输出人脸验证相似度，越大越相似
IF_SDK_API
int
if_verify_pca_feature(if_handle_t verify_instance, const float *feature1, int feature1_len,
		const float *feature2, int feature2_len, float *score);

/// @brief 释放if_extract_pca_feature返回的特征
IF_SDK_API
void
if_verify_release_feature(float *feature);

/// @brief 释放验证系统实例
/// @param verify_instance 验证系统句柄
IF_SDK_API
void
if_verify_release_instance(if_handle_t verify_instance);

//>> CONFIG_API_FACE_SEARCH

/* Face search module */

// @brief 创建一个空的数据库索引
IF_SDK_API
if_handle_t
if_verify_search_create_index();

/// @brief 提取人脸搜索特征
/// @param verify_instance 验证系统句柄
/// @param bgra_image 彩色图像数据，大小为width*height*4字节，Linux上存储顺序与OpenCV一致(BGRA)
/// @param image_width 图像宽度
/// @param image_height 图像高度
/// @param face_rect Detector返回的人脸位置
/// @param item 输出搜索数据库项，可做持久存储备用
IF_SDK_API
if_result_t
if_verify_search_get_feature(
	if_handle_t verify_instance,
	unsigned char *bgra_image,
	unsigned int image_width,
	unsigned int image_height,
	if_rect_t face_rect,
	struct if_db_item *item
	);


/// @brief 建立人脸数据库索引
/// @param verify_instance 验证系统句柄
/// @param items 搜索项数组
/// @param item_count items长度
/// @param out_db_handle 输出数据库索引的句柄
IF_SDK_API
if_result_t
if_verify_search_build_index(
	if_handle_t verify_instance,
	struct if_db_item *items,
	unsigned int item_count,
	if_handle_t *out_db_handle
	);

/// @brief 搜索人脸数据库
/// @param verify_inst 验证系统句柄
/// @param index_handle 索引句柄
/// @param query 待查人脸特征描述符
/// @param result 查找结果输出
/// @param max_result_cnt 最大输出结果数
/// @param result_cnt 实际输出结果数
IF_SDK_API
if_result_t
if_verify_search_face(
	if_handle_t verify_inst,
	if_handle_t index_handle,
	const struct if_db_item *query,
	if_face_search_result_t *result,
	unsigned int max_result_cnt,
	unsigned int *result_cnt
);


/// @brief 人脸数据库增加数据
/// @param index_handle 索引句柄
/// @param item 待加入的人脸特征描述符
/// @param idx 加入数据库后的索引值

IF_SDK_API
if_result_t
if_verify_add_face(
	if_handle_t index_handle,
	const struct if_db_item* item,
	int *idx
);

/// @brief 人脸数据库删除数据
/// @param index_handle 索引句柄
/// @param idx 待删除的数据的索引值（与加入时一致）

IF_SDK_API
if_result_t
if_verify_delete_face(
	if_handle_t index_handle,
	int idx
);

/// @brief 人脸数据库保存
/// @param index_handle 索引句柄
/// @param db_out 输出文件

IF_SDK_API
if_result_t
if_verify_save_db(
	if_handle_t index_handle,
	char* db_out
);

/// @brief 人脸数据库读取
/// @param index_handle 索引句柄
/// @param db_in 读入文件

IF_SDK_API
if_result_t
if_verify_load_db(
	if_handle_t index_handle,
	char* db_in
);


/// @brief 释放人脸数据库索引
/// @param index_handle 索引句柄
IF_SDK_API
void
if_verify_search_release_index(if_handle_t index_handle);


struct if_face_group {
	unsigned int count;         ///< 人脸聚类结果个数
	int *indexes;               ///< 输出图片索引
	unsigned int group_flags;   ///< 保留参数
};

/// @brief 批量人脸分组
/// @param verify_instance 验证系统句柄
/// @param items 待分组的人脸特征数据数组的首地址
/// @param item_count 待分组的人脸特征数据数组大小
/// @param grouping_threshold 待拓展，现可传入任何数字，对算法无影响
/// @param groups 输出保存分组结果，参数为分组结果数组的首地址，数组的内存空间由函数内部分配，但需要用户调用if_verify_release_face_grouping_result释放,需用户初始化为空
/// @param group_count 输出保存分组的组数，需用户初始化为0
IF_SDK_API
if_result_t
if_verify_face_grouping(
	if_handle_t verify_instance,
	struct if_db_item **items,
	unsigned int item_count,
	float grouping_threshold,
	struct if_face_group **groups,
	unsigned int *group_count
	);

/// @brief 释放批量人脸分组结果返回时分配的空间
/// @param groups 保存结构的数组
/// @param group_count 分组的组数
IF_SDK_API
void
if_verify_release_face_grouping_result(
	struct if_face_group *groups,
	unsigned int group_count
	);

//>> CONFIG_API_END__
#endif /* end of include guard: IF_SDK_INTERFACE_H_ */

