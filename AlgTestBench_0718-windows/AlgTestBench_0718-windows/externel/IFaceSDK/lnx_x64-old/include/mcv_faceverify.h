/*
 * @file   mcv_faceverify.h
 * @Author SenseTime Group Limited
 * @brief  Face verify SDK header for SenseTime C API.
 *
 * Copyright (c) 2014-2015, SenseTime Group Limited. All Rights Reserved.
 */

#ifndef VERIF_INTERFACE_COMMON_H
#define VERIF_INTERFACE_COMMON_H

#include "mcv_common.h"
#include "db_common.h"

//>> CONFIG_API_FACE_VERIF

/// @brief 初始化验证系统数据，返回验证系统实例句柄
MCV_SDK_API
mcv_handle_t
mcv_create_verify_instance();

/// @brief 初始化验证系统数据，返回验证系统实例句柄
MCV_SDK_API
mcv_handle_t
mcv_create_verify_instance_with_prefix(const char *prefix);

MCV_SDK_API
mcv_handle_t
mcv_duplicate_verify_instance_with_prefix(const char *prefix, mcv_handle_t old_instance);

/// @brief 提取人脸特征，返回特征数据长度，可以把返回数组存储起来一遍以后使用
/// @param verify_instance 验证系统句柄
/// @param bgra_image 彩色图像数据，大小为width*height*4字节，Linux上存储顺序与OpenCV一致(BGRA)
/// @param image_width 图像宽度
/// @param image_height 图像高度
/// @param face_rect Detector返回的人脸位置
/// @param p_feature 人脸特征数据指针，使用后使用mcv_verify_release_feature释放
MCV_SDK_API
int
mcv_extract_pca_feature(mcv_handle_t verify_instance, unsigned char *bgra_image,
		unsigned int image_width, unsigned int image_height,
		mcv_rect_t face_rect, float **p_feature);


/// @brief 人脸验证
/// @param verify_instance 验证系统句柄
/// @param feature1 feature2 mcv_extract_pca_feature取得的待验证的两张人脸特征数组
/// @param feature1_len feature2_len mcv_extract_pca_feature返回的特征长度
/// @param score输出人脸验证相似度，越大越相似
MCV_SDK_API
int
mcv_verify_pca_feature(mcv_handle_t verify_instance, const float *feature1, int feature1_len,
		const float *feature2, int feature2_len, float *score);

enum mcv_face_metric_type {
	MCV_FACE_METRIC_NONE,
	MCV_FACE_METRIC_MANUAL,
	MCV_FACE_METRIC_AUTO,
};

#define MCV_FACE_METRIC_FLAG_NORMALIZE_SCORE (1 << 0)
#define MCV_FACE_FEATURE_FLAG_GLASS      (1 << 0)
#define MCV_FACE_FEATURE_FLAG_GENDER     (1 << 1)

struct mcv_face_metric_options {
	enum mcv_face_metric_type type;  ///< 人脸验证模式，推荐使用 MCV_FACE_METRIC_AUTO，自动计算获取feature_flags的信息
	unsigned int flags;              ///< 验证参数设置，例如开启相似度得分归一化则flag = flag | MCV_FACE_METRIC_FLAG_NORMALIZE_SCORE;
	unsigned int feature_flags[2];   ///< 两张人脸feature属性的先验信息，有利于验证结果的修正，如果人脸验证模式选择了AUTO，则该组参数无效
};

/// @brief 高级人脸验证接口
/// @param verify_instance 验证系统句柄
/// @param feature1 feature2 mcv_extract_pca_feature取得的待验证的两张人脸特征数组
/// @param feature1_len feature2_len mcv_extract_pca_feature返回的特征长度
/// @param score 输出人脸验证相似度，越大越相似
/// @param opts 高级选项，控制归一化/验证模式/属性特征的设置
/// @param opt_size 高级选项结构体大小
MCV_SDK_API
int
mcv_verify_pca_feature_ex(mcv_handle_t verify_instance, const float *feature1, int feature1_len,
		const float *feature2, int feature2_len, float *score,
		struct mcv_face_metric_options *opts, unsigned int opt_size);

enum mcv_face_attr {
	MCV_FACE_ATTR_EYEGLASS = 0,
	MCV_FACE_ATTR_GENDER,
	MCV_FACE_ATTR_EMOTION,
	MCV_FACE_ATTR_SUNGLASS,
	MCV_FACE_ATTR_AGE,
	MCV_FACE_ATTR_COUNT
};

#define MCV_FACE_ATTR_GENDER_LENGTH 2
#define MCV_FACE_ATTR_GENDER_FEMALE 0
#define MCV_FACE_ATTR_GENDER_MALE   1

#define MCV_FACE_ATTR_EYEGLASS_LENGTH 2
#define MCV_FACE_ATTR_EYEGLASS_NO     0
#define MCV_FACE_ATTR_EYEGLASS_YES    1

#define MCV_FACE_ATTR_EMOTION_LENGTH  7
#define MCV_FACE_ATTR_EMOTION_ANGRY      0
#define MCV_FACE_ATTR_EMOTION_CALM       1
#define MCV_FACE_ATTR_EMOTION_CONFUSED   2
#define MCV_FACE_ATTR_EMOTION_DISGUST    3
#define MCV_FACE_ATTR_EMOTION_HAPPY      4
#define MCV_FACE_ATTR_EMOTION_SAD        5
#define MCV_FACE_ATTR_EMOTION_SURPRISED  6

#define MCV_FACE_ATTR_SUNGLASS_LENGTH 2
#define MCV_FACE_ATTR_SUNGLASS_NO     0
#define MCV_FACE_ATTR_SUNGLASS_YES    1

#define MCV_FACE_ATTR_AGE_LENGTH      1


/// @brief 释放mcv_extract_pca_feature返回的特征
MCV_SDK_API
void
mcv_verify_release_feature(float *feature);

/// @brief 释放验证系统实例
/// @param verify_instance 验证系统句柄
MCV_SDK_API
void
mcv_verify_release_instance(mcv_handle_t verify_instance);

//>> CONFIG_API_FACE_SEARCH

/* Face search module */

// @brief 创建一个空的数据库索引
MCV_SDK_API
mcv_handle_t
mcv_verify_search_create_index();



/// @brief 提取人脸搜索特征
/// @param verify_instance 验证系统句柄
/// @param bgra_image 彩色图像数据，大小为width*height*4字节，Linux上存储顺序与OpenCV一致(BGRA)
/// @param width 图像宽度
/// @param height 图像高度
/// @param face_rect Detector返回的人脸位置
/// @param item 输出搜索数据库项，可做持久存储备用
MCV_SDK_API
mcv_result_t
mcv_verify_search_get_feature(mcv_handle_t verify_instance, unsigned char *bgra_image,
		unsigned int image_width, unsigned int image_height,
		mcv_rect_t face_rect,  struct db_item *item);

struct mcv_face_group {
	unsigned int count;
	int *indexes;
	int is_messy_group;
};

/// @brief 批量人脸分组
MCV_SDK_API
mcv_result_t
mcv_verify_face_grouping(mcv_handle_t verify_instance, struct db_item **items,
		unsigned int item_count, float grouping_threshold,
		struct mcv_face_group **groups,
		unsigned int *group_count
		);

MCV_SDK_API
void
mcv_verify_release_face_grouping_result(struct mcv_face_group *groups, unsigned int group_count);


/// @brief 建立人脸数据库索引
/// @param verify_instance 验证系统句柄
/// @param items 搜索项数组
/// @param item_count items长度
/// @param out_index 索引句柄
MCV_SDK_API
mcv_result_t
mcv_verify_search_build_index(mcv_handle_t verify_instance,
	struct db_item *items, unsigned int item_count, mcv_handle_t *out_db_handle);

/// @brief 搜索人脸数据库
/// @param verify_instance 验证系统句柄
/// @param index 索引句柄
/// @param query 待查人脸特征描述符
/// @param result 查找结果输出
/// @param max_result_count 最大输出结果数
/// @param result_count 实际输出结果数
MCV_SDK_API
mcv_result_t
mcv_verify_search_face(mcv_handle_t verify_inst, mcv_handle_t index_handle,
		const struct db_item *query, 
		mcv_face_search_result_t *result, 
		unsigned int max_result_cnt,
		unsigned int *result_cnt);


/// @brief 人脸数据库增加数据
/// @param index 索引句柄
/// @param item 待加入的人脸特征描述符
/// @param idx 加入数据库后的索引值

MCV_SDK_API
mcv_result_t
mcv_verify_add_face(mcv_handle_t index_handle, const struct db_item* item, int *idx);

/// @brief 人脸数据库删除数据
/// @param index 索引句柄
/// @param idx 待删除的数据的索引值（与加入时一致）

MCV_SDK_API
mcv_result_t 
mcv_verify_delete_face(mcv_handle_t index_handle, int idx);

/// @brief 人脸数据库保存
/// @param index 索引句柄
/// @param db_out 输出文件

MCV_SDK_API
mcv_result_t
mcv_verify_save_db(mcv_handle_t index_handle,char* db_out);

/// @brief 人脸数据库读取
/// @param index 索引句柄
/// @param db_in 读入文件

MCV_SDK_API
mcv_result_t
mcv_verify_load_db(mcv_handle_t index_handle,char* db_in);


/// @brief 释放人脸数据库索引
/// @param index_handle 索引句柄
MCV_SDK_API
void
mcv_verify_search_release_index(mcv_handle_t index_handle);

//>> CONFIG_API_END__

#endif

