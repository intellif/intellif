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
	int left;	///< ��������ߵ�����
	int top;	///< �������ϱߵ�����
	int right;	///< �������ұߵ�����
	int bottom;	///< �������±ߵ�����
}if_rect_t;

typedef struct{
	float x;	///< ���ˮƽ�������꣬Ϊ������
	float y;	///< �����ֱ�������꣬Ϊ������
}if_pointf_t;

typedef struct{
	int x;		///< ���ˮƽ�������꣬Ϊ����
	int y;		///< �����ֱ�������꣬Ϊ����
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

/// @brief ��ʾ��ͬ���沿������Ҫ�������������غ�����ʹ��
enum IF_FacePose
{
	IF_StrictFrontal = 0, 		///< �ϸ�����
	IF_Frontal,			///< ����
	IF_HalfLeft,			///< ��΢����
	IF_HalfRight,			///< ��΢����
	IF_FullLeft,			///< ����
	IF_FullRight,			///< ����
	IF_FrontalCW30,		///< ����˳ʱ��תԼ30��
	IF_LeftHalfProfileCW30,	///< ��΢����˳ʱ��תԼ30��
	IF_RightHalfProfileCW30,	///< ��΢���Ҳ�˳ʱ��Լ30��
	IF_LeftProfileCW30,		///< ����˳ʱ��Լ30��
	IF_RightProfileCW30,		///< ���Ҳ�˳ʱ��30��
	IF_FrontalCCW30,		///< ������ʱ��Լ30��
	IF_LeftHalfProfileCCW30,	///< ��΢������ʱ��Լ30��
	IF_RightHalfProfileCCW30,	///< ��΢���Ҳ���ʱ��Լ30��
	IF_LeftProfileCCW30,		///< ������ʱ��Լ30��
	IF_RightProfileCCW30		///< ���Ҳ���ʱ��Լ30��
};

typedef struct _IF_FACERECT
{
       if_rect_t Rect;		///< �����沿�ľ�������
       enum IF_FacePose Pose;	///< �沿�ĳ��򣬶�Ƕ��������᷵�ز�ͬ������������ֻ����Frontal
       float Confidence;	///< ���Ŷȣ���ע���Ƕ���������������ⷵ�ص����Ŷ���ʱ�����пɱ���
} IF_FACERECT, *PIF_FACERECT;

//>> CONFIG_API_LRALIGNMENTOR

/// @brief �ú������ڳ�ʼ���沿�ؼ����⹦������Ҫ������
/// @param npoint �沿�ؼ�����Ĺؼ�����������ѡ������� Readme �ļ���
IF_SDK_API
if_handle_t if_facesdk_create_LRAlignmentor_instance_from_resource(int npoint);

/// @brief ���øú��������沿�ؼ�����
/// @param lrAlignmentor_instance ����ɳ�ʼ�����沿�ؼ�����ʵ�����
/// @param gray_image ���ڼ��ĻҶ�ͼ 
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param face_rect �����沿�ؼ�����������ڵľ�������
/// @param face_pose �����沿�ؼ���������ĳ���
/// @param facial_points_count �沿�ؼ�����Ĺؼ��������������ʼ�����õ� npoint ��ͬ��
/// @param facial_points_array ���ڱ��������Ĺؼ������飨���û����䣩
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


/// @brief �����Ѿ���ɳ�ʼ�����沿�ؼ�����ʵ��
/// @param lrAlignmentor_instance ����ɳ�ʼ�����沿�ؼ�����ʵ�����
IF_SDK_API
void if_facesdk_destroy_LRAlignmentor_instance(
	if_handle_t lrAlignmentor_instance
);


/// @brief ����21��ؼ����������pose��Ϣ
/// @param facial_points_array �����������沿�ؼ�����Ϣ��Ŀǰֻ֧������21��
/// @param facial_points_count facial_points_array ���鳤�ȣ�������Ĺؼ�������
/// @param yaw ���ڱ���ˮƽת�ǵĵ�ַָ�루���û����䣩����ʵ���������������������NULL�򲻷��ظý��
/// @param pitch ���ڱ��温���ǵĵ�ַָ�루���û����䣩����ʵ�������ϸ��������������NULL�򲻷��ظý��
/// @param roll ���ڱ�����ת�ǵĵ�ַָ�루���û����䣩����ʵ���������������������NULL�򲻷��ظý��
/// @param eyedist ���ڱ������۾�ĵ�ַָ�루���û����䣩���������NULL�򲻷��ظý��
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

/// @brief ���ж�Ƕ�������⹦�ܳ�ʼ��
/// @param skip_below_thresh ѡ��true����ʶ����٣����ǿ��ܻ����©��⣻false���෴������ʹ��true
/// @param ncpu ���õĲ���CPU�������ڿ������̼߳���
IF_SDK_API
if_handle_t
if_facesdk_create_multiview_detector_instance_from_resource(int skip_below_thresh, int ncpu);

/// @brief ��ָ������ͼƬ���ж�Ƕ��������
/// @param detector_instance ����ɳ�ʼ���Ķ�Ƕ��������ʵ�����
/// @param gray_image ���ڼ��ĻҶ�ͼ��ÿ������ռ1�ֽ�
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param p_face_rects_array ָ�򱣴��⵽�����ľ������������ָ�룬������ڴ�ռ���Ҫ���û�����if_facesdk_release_multiview_result�����ͷ�
/// @param face_rects_count ָ�򱣴��⵽��������������
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

/// @brief �ͷŶ�Ƕ�������ⷵ�ؽ��ʱ����Ŀռ�
/// @param face_rects_array ������������
/// @param face_rects_count �����
IF_SDK_API
void
if_facesdk_release_multiview_result(
	PIF_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief �����ѳ�ʼ���Ķ�Ƕ��������ʵ��
/// @param detector_instance �����ٵĶ�Ƕ��������ľ��
IF_SDK_API
void
if_facesdk_destroy_multiview_instance(
	if_handle_t detector_instance
);

//>> CONFIG_API_SPIDER_FACE_DETECTOR

/// @brief ���ж�Ƕ�������⹦�ܳ�ʼ��
/// @param skip_below_thresh ѡ��true����ʶ����٣����ǿ��ܻ����©��⣻false���෴������ʹ��true
/// @param ncpu ���õĲ���CPU�������ڿ������̼߳���
IF_SDK_API
if_handle_t
if_facesdk_create_selftimer_face_detector_instance_from_resource(int ncpu);

/// @brief ��ָ������ͼƬ���ж�Ƕ��������
/// @param detector_instance ����ɳ�ʼ���Ķ�Ƕ��������ʵ�����
/// @param color_image ���ڼ�����ͨ����ͼ��ÿ������ռ4�ֽڣ�Android��iOS������ΪRGBA��ʽ������ƽ̨��ΪOpenCVĬ�ϵ�BGRA��ʽ
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param p_face_rects_array ָ�򱣴��⵽�����ľ������������ָ�룬������ڴ�ռ���Ҫ���û�����if_facesdk_release_multiview_result�����ͷ�
/// @param face_rects_count ָ�򱣴��⵽��������������
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

/// @brief �ͷŶ�Ƕ�������ⷵ�ؽ��ʱ����Ŀռ�
/// @param face_rects_array ������������
/// @param face_rects_count �����
IF_SDK_API
void
if_facesdk_release_selftimer_face_result(
	PIF_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief �����ѳ�ʼ���Ķ�Ƕ��������ʵ��
/// @param detector_instance �����ٵĶ�Ƕ��������ľ��
IF_SDK_API
void
if_facesdk_destroy_selftimer_face_detector_instance(
	if_handle_t detector_instance
);


//>> CONFIG_API_FRONTAL_DETECTOR

/// @brief ����������⹦�ܳ�ʼ��
/// @param ncpu ���õĲ���CPU�������ڿ������̼߳���
IF_SDK_API
if_handle_t
if_facesdk_create_frontal_detector_instance_from_resource(int ncpu);

/// @brief ��ָ������ͼƬ�����������
/// @param detector_instance ����ɳ�ʼ�����������ʵ�����
/// @param gray_image ���ڼ��ĻҶ�ͼ��ÿ������ռ1�ֽ�
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param p_face_rects_array ָ�򱣴��⵽�����ľ������������ָ�룬������ڴ�ռ���Ҫ���û�����if_facesdk_release_frontal_result�����ͷ�
/// @param face_rects_count ָ�򱣴��⵽��������������
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

/// @brief �ͷ�������ⷵ�ؽ��ʱ����Ŀռ�
/// @param face_rects_array ������������
/// @param face_rects_count �����
IF_SDK_API
if_result_t
if_facesdk_release_frontal_result(
	PIF_FACERECT face_rects_array,
	unsigned int face_rects_count
);

/// @brief �����ѳ�ʼ�����������ʵ��
/// @param detector_instance �����ٵ��������ʵ���ľ��
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
	IF_FaceUp = 0,			///< �������ϣ���������������
	IF_FaceLeft = 1,        ///< �������󣬼���������ʱ����ת��90��
	IF_FaceDown = 2,        ///< �������£�����������ת��180��
	IF_FaceRight = 3        ///< �������󣬼�������˳ʱ����ת��90��
};

typedef struct
{
	if_rect_t Rect;            ///< �����沿�ľ�������
	int ID;                     ///< faceID�����ڱ�ʾ��ʵʱ���������е���ͬ�����ڲ�ͬ֡��γ���
} IF_FACERECTID, *PIF_FACERECTID;

#define IF_MUTLITRACKER_ALIGN_POINTS 21

/// @brief ����ʵʱ�������ٹ��ܳ�ʼ��
/// @param ncpu ���õĲ���CPU�������ڿ������̼߳���
IF_SDK_API
if_handle_t
if_facesdk_create_multi_face_tracker(int ncpu);

/// @brief �����ѳ�ʼ����ʵʱ��������ʵ��
/// @param tracker_instance �����ٵ�ʵʱ��������ʵ���ľ��
IF_SDK_API
void
if_facesdk_destroy_multi_face_tracker(if_handle_t tracker_instance);

/// @brief ��������Ƶ֡����ʵʱ��������
/// @param tracker_instance ����ɳ�ʼ����ʵʱ��������ʵ�����
/// @param gray_image ������Ƶ֡�ĻҶ�ͼ��ÿ������ռ1�ֽ�
/// @param image_width  ������Ƶ֡�Ŀ�
/// @param image_height ������Ƶ֡�ĸ�
/// @param orientation ��Ƶ�������ķ�����������Ƶ֡��������Ϊ IF_FaceUp������������Ϊ IF_FaceLeft������Ҫ�Ƚ���Ƶ˳ʱ����ת90�Ⱥ��������ϣ�����������Ϊ IF_FaceDown������������Ϊ IF_FaceRight
/// @param p_face_rects_array ָ�򱣴��⵽�����ľ������������ָ�룬������ڴ�ռ���Ҫ���û�����if_facesdk_release_multitracker_result�����ͷ�
/// @param face_rects_count ָ�򱣴��⵽��������������
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

/// @brief ��������Ƶ֡�м�⵽�����������沿�ؼ�����
/// @param tracker_instance ����ɳ�ʼ����ʵʱ��������ʵ�����
/// @param face_rect ����ĵ���������δ��ת��
/// @param facial_points_array ���ڱ��������Ĺؼ������飬����Ϊ IF_MUTLITRACKER_ALIGN_POINTS �����û�����ռ䣩
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_align(
	if_handle_t tracker_instance,
	const IF_FACERECTID *face_rect,
	if_pointf_t *facial_points_array
);

/// @brief ����21��ؼ����������pose��Ϣ
/// @param facial_points_array �����������沿�ؼ�����Ϣ��Ŀǰֻ֧������21��
/// @param facial_points_count facial_points_array ���鳤�ȣ�������Ĺؼ�������
/// @param yaw ���ڱ���ˮƽת�ǵĵ�ַָ�루���û����䣩����ʵ���������������������NULL�򲻷��ظý��
/// @param pitch ���ڱ��温���ǵĵ�ַָ�루���û����䣩����ʵ�������ϸ��������������NULL�򲻷��ظý��
/// @param roll ���ڱ�����ת�ǵĵ�ַָ�루���û����䣩����ʵ���������������������NULL�򲻷��ظý��
/// @param eyedist ���ڱ������۾�ĵ�ַָ�루���û����䣩���������NULL�򲻷��ظý��
IF_SDK_API
if_result_t if_facesdk_get_pose(
	const if_pointf_t *facial_points_array,
	int facial_points_count,
	float *yaw,
	float *pitch,
	float *roll,
	float *eyedist
);

/// @brief ��ȡ������Ƶ֡ת�����ͼ��ָ�루�����ͷţ�
/// @param tracker_instance ����ɳ�ʼ����ʵʱ��������ʵ�����
/// @param out_image ���ڷ���ͼ�����ݵ�ָ�루�����������ͷŴ��ͼ��Ŀռ䣩
/// @param image_width  ָ�򱣴�ת����ͼƬ��ȵ�����
/// @param image_height ָ�򱣴�ת����ͼƬ�߶ȵ�����
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_get_preprocessed_image(
	if_handle_t tracker_instance,
	unsigned char **out_image,
	unsigned int *image_width, unsigned int *image_height
);

/// @brief ��ȡ�����ԭͼ������������
/// @param tracker_instance ����ɳ�ʼ����ʵʱ��������ʵ�����
/// @param face_rect ����ĵ���������ָ�룬��ת�󸲸�ԭ������
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_transform_rectangle(
	if_handle_t tracker_instance,
	IF_FACERECTID *face_rect
);

/// @brief ��ȡ�����ԭͼ���沿�ؼ�������
/// @param tracker_instance ����ɳ�ʼ����ʵʱ��������ʵ�����
/// @param face_rect ����ĵ��������꣬��ת�󸲸�ԭ������
IF_SDK_API
if_result_t
if_facesdk_multi_face_tracker_transform_point(
	if_handle_t tracker_instance,
	if_pointf_t *point
);


/// @brief �ͷ�ʵʱ�������ٷ��ؽ��ʱ����Ŀռ�
/// @param face_rects_array ������������
/// @param face_rects_count �����
IF_SDK_API
void
if_facesdk_release_multitracker_result(
	PIF_FACERECTID face_rects_array,
	unsigned int face_rects_count
);


/// @brief ��������ʶ����ʵ��
IF_SDK_API
if_handle_t
if_facesdk_create_attribute_detector();

IF_SDK_API
if_handle_t
if_facesdk_create_attribute_detector_with_path(const char *modelname);
/// @brief �г�����ʶ����֧�ֵ���������
/// @param attribute_instance �����ʵ��
/// @param out_list ����б�ָ������
/// @param max_list_len out_list����ĳ���
/// @param out_list_len ʵ�������֧�����Ը���
IF_SDK_API
if_result_t
if_facesdk_attribute_detector_list(
	if_handle_t attribute_instance,
	char *out_list[],
	unsigned int max_list_len,
	unsigned int *out_list_len
);

/// @brief ʹ������ʶ�������е�������ʶ��
/// @param attribute_instance �����ʵ��
/// @param attribute_name ������������
/// @param gray_image ���ڼ��ĻҶ�ͼ��ÿ������ռ1�ֽ�
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param facial_points_array �����������沿�ؼ�����Ϣ��Ŀǰֻ֧������21��
/// @param facial_points_count facial_points_array���鳤�ȣ�������Ĺؼ�������
/// @param result, ����1Ϊ����ΪTrue��0ΪFalse��-1�ǲ�֧�ִ�����
/// @param score �����жϷ���
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

/// @brief ʹ������ʶ����������������ʶ�����ܱȶ�ε��õ�����ʶ���
/// @param attribute_instance �����ʵ��
/// @param attribute_names ����������������
/// @param attribute_len attribute_names ���鳤��
/// @param gray_img ���ڼ��ĻҶ�ͼ��ÿ������ռ1�ֽ�
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param facial_points_array �����������沿�ؼ�����Ϣ��Ŀǰֻ֧������21��
/// @param facial_points_count facial_points_array ���鳤�ȣ�������Ĺؼ�������
/// @param results Ӧ���볤��Ϊattribute_len���ȵ����飬������������жϽ����0��1��
/// @param scores  Ӧ���볤��Ϊattribute_len���ȵ����飬�����жϷ���
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

/// @brief �ͷ�ʹ��if_facesdk_attribute_detector_list���ص������б�
/// @param attribute_list ���ͷ�����
/// @param attribute_len ʵ�����Ը���
IF_SDK_API
void
if_facesdk_release_attribute_list(
	char* attribute_list[],
	unsigned int attribute_len
);

/// @brief �ͷ����Լ����ʵ��
/// @param attribute_instance �����ʵ��
IF_SDK_API
void
if_facesdk_destroy_attribute_detector(if_handle_t attribute_instance);


/// @brief ������������ʵ��
IF_SDK_API
if_handle_t
if_facesdk_create_liveness_instance();

/// @brief �ͷŻ�������ʵ��
/// @param liveness_instance �����ʵ��
IF_SDK_API
void if_facesdk_destroy_liveness_instance(if_handle_t liveness_instance);

/// @brief �������������Ƶ֡���л�����
/// @param liveness_instance ����ɳ�ʼ���Ļ�����ʵ�����
/// @param gray_image ���ڽ��л�����ĻҶ�ͼ
/// @param image_width ���ڼ��ĻҶ�ͼ�Ŀ��
/// @param image_height ���ڼ��ĻҶ�ͼ�ĸ߶�
/// @param image_stride ���ڼ��ĻҶ�ͼ��ÿһ�еĿ�ȣ����ֽڼ��㣩
/// @param facial_points_array �����������沿�ؼ�����Ϣ��Ŀǰֻ֧������21��
/// @param facial_points_count facial_points_array ���鳤�ȣ�������Ĺؼ�������
/// @param score ָ�������÷�
/// @param state ָ�������������״̬������ö����Ʊ�ʾ�����Ƿ�գ�ۣ�state&1���Ƿ����죺state&2
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

/// @brief ���û�����������������ʧ�����仯ʱ
/// @param liveness_instance ����ɳ�ʼ���Ļ�����ʵ�����
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

/// @brief ��ʼ����֤ϵͳ���ݣ�������֤ϵͳʵ�����
IF_SDK_API
if_handle_t
if_create_verify_instance();

/// @brief ��ʼ����֤ϵͳ���ݣ�������֤ϵͳʵ�����
IF_SDK_API
if_handle_t
if_create_verify_instance_with_prefix(const char *prefix);

IF_SDK_API
if_handle_t
if_duplicate_verify_instance_with_prefix(const char *prefix, if_handle_t old_instance);

/// @brief ��ȡ���������������������ݳ��ȣ����԰ѷ�������洢����һ���Ժ�ʹ��
/// @param verify_instance ��֤ϵͳ���
/// @param bgra_image ��ɫͼ�����ݣ���СΪwidth*height*4�ֽڣ�Linux�ϴ洢˳����OpenCVһ��(BGRA)
/// @param image_width ͼ����
/// @param image_height ͼ��߶�
/// @param face_rect Detector���ص�����λ��
/// @param p_feature ������������ָ�룬ʹ�ú�ʹ��if_verify_release_feature�ͷ�
IF_SDK_API
int
if_extract_pca_feature(if_handle_t verify_instance, unsigned char *bgra_image,
		unsigned int image_width, unsigned int image_height,
		if_rect_t face_rect, float **p_feature);


/// @brief ������֤
/// @param verify_instance ��֤ϵͳ���
/// @param feature1 feature2 if_extract_pca_featureȡ�õĴ���֤������������������
/// @param feature1_len feature2_len if_extract_pca_feature���ص���������
/// @param score���������֤���ƶȣ�Խ��Խ����
IF_SDK_API
int
if_verify_pca_feature(if_handle_t verify_instance, const float *feature1, int feature1_len,
		const float *feature2, int feature2_len, float *score);

/// @brief �ͷ�if_extract_pca_feature���ص�����
IF_SDK_API
void
if_verify_release_feature(float *feature);

/// @brief �ͷ���֤ϵͳʵ��
/// @param verify_instance ��֤ϵͳ���
IF_SDK_API
void
if_verify_release_instance(if_handle_t verify_instance);

//>> CONFIG_API_FACE_SEARCH

/* Face search module */

// @brief ����һ���յ����ݿ�����
IF_SDK_API
if_handle_t
if_verify_search_create_index();

/// @brief ��ȡ������������
/// @param verify_instance ��֤ϵͳ���
/// @param bgra_image ��ɫͼ�����ݣ���СΪwidth*height*4�ֽڣ�Linux�ϴ洢˳����OpenCVһ��(BGRA)
/// @param image_width ͼ����
/// @param image_height ͼ��߶�
/// @param face_rect Detector���ص�����λ��
/// @param item ����������ݿ�������־ô洢����
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


/// @brief �����������ݿ�����
/// @param verify_instance ��֤ϵͳ���
/// @param items ����������
/// @param item_count items����
/// @param out_db_handle ������ݿ������ľ��
IF_SDK_API
if_result_t
if_verify_search_build_index(
	if_handle_t verify_instance,
	struct if_db_item *items,
	unsigned int item_count,
	if_handle_t *out_db_handle
	);

/// @brief �����������ݿ�
/// @param verify_inst ��֤ϵͳ���
/// @param index_handle �������
/// @param query ������������������
/// @param result ���ҽ�����
/// @param max_result_cnt �����������
/// @param result_cnt ʵ����������
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


/// @brief �������ݿ���������
/// @param index_handle �������
/// @param item ���������������������
/// @param idx �������ݿ�������ֵ

IF_SDK_API
if_result_t
if_verify_add_face(
	if_handle_t index_handle,
	const struct if_db_item* item,
	int *idx
);

/// @brief �������ݿ�ɾ������
/// @param index_handle �������
/// @param idx ��ɾ�������ݵ�����ֵ�������ʱһ�£�

IF_SDK_API
if_result_t
if_verify_delete_face(
	if_handle_t index_handle,
	int idx
);

/// @brief �������ݿⱣ��
/// @param index_handle �������
/// @param db_out ����ļ�

IF_SDK_API
if_result_t
if_verify_save_db(
	if_handle_t index_handle,
	char* db_out
);

/// @brief �������ݿ��ȡ
/// @param index_handle �������
/// @param db_in �����ļ�

IF_SDK_API
if_result_t
if_verify_load_db(
	if_handle_t index_handle,
	char* db_in
);


/// @brief �ͷ��������ݿ�����
/// @param index_handle �������
IF_SDK_API
void
if_verify_search_release_index(if_handle_t index_handle);


struct if_face_group {
	unsigned int count;         ///< ��������������
	int *indexes;               ///< ���ͼƬ����
	unsigned int group_flags;   ///< ��������
};

/// @brief ������������
/// @param verify_instance ��֤ϵͳ���
/// @param items �������������������������׵�ַ
/// @param item_count ������������������������С
/// @param grouping_threshold ����չ���ֿɴ����κ����֣����㷨��Ӱ��
/// @param groups ������������������Ϊ������������׵�ַ��������ڴ�ռ��ɺ����ڲ����䣬����Ҫ�û�����if_verify_release_face_grouping_result�ͷ�,���û���ʼ��Ϊ��
/// @param group_count ��������������������û���ʼ��Ϊ0
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

/// @brief �ͷ�������������������ʱ����Ŀռ�
/// @param groups ����ṹ������
/// @param group_count ���������
IF_SDK_API
void
if_verify_release_face_grouping_result(
	struct if_face_group *groups,
	unsigned int group_count
	);

//>> CONFIG_API_END__
#endif /* end of include guard: IF_SDK_INTERFACE_H_ */

