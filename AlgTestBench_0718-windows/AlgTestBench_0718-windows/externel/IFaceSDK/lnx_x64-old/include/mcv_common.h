/*
 * @file   mcv_common.h
 * @Author SenseTime Group Limited
 * @brief  Common header for SenseTime C API.
 *
 * Copyright (c) 2014-2015, SenseTime Group Limited. All Rights Reserved.
 */

#ifndef LIB_INTERFACE_COMMON_H
#define LIB_INTERFACE_COMMON_H

#ifdef _MSC_VER
#ifdef __cplusplus
#ifdef SDK_EXPORTS
#define MCV_SDK_API  extern "C" __declspec(dllexport)
#else
#define MCV_SDK_API extern "C" __declspec(dllimport) 
#endif
#else
#ifdef SDK_EXPORTS
#define MCV_SDK_API __declspec(dllexport)
#else
#define MCV_SDK_API __declspec(dllimport)
#endif
#endif
#else /* _MSC_VER */
#ifdef __cplusplus
#ifdef SDK_EXPORTS
#define MCV_SDK_API  extern "C" __attribute__((visibility ("default")))
#else
#define MCV_SDK_API extern "C"
#endif
#else
#ifdef SDK_EXPORTS
#define MCV_SDK_API 
#else
#define MCV_SDK_API __attribute__((visibility ("default")))
#endif
#endif
#endif

typedef void* mcv_handle_t;
typedef int   mcv_result_t;

typedef struct{
	int left;	///< 矩形最左边的坐标
	int top;	///< 矩形最上边的坐标
	int right;	///< 矩形最右边的坐标
	int bottom;	///< 矩形最下边的坐标
}mcv_rect_t;

typedef struct{
	float x;	///< 点的水平方向坐标，为浮点数
	float y;	///< 点的竖直方向坐标，为浮点数
}mcv_pointf_t;

typedef struct{
	int x;		///< 点的水平方向坐标，为整数
	int y;		///< 点的竖直方向坐标，为整数
}mcv_pointi_t;

#define MCV_OK (0)
#define MCV_E_INVALIDARG (-1)
#define MCV_E_HANDLE (-2)
#define MCV_E_OUTOFMEMORY (-3)
#define MCV_E_FAIL (-4)
#define MCV_E_DELNOTFOUND (-5)

#endif
