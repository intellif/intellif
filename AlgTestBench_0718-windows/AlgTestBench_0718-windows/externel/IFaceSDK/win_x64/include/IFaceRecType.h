#ifndef IFACERECTYPE_H
#define IFACERECTYPE_H

#include <string>

using namespace std;

typedef int IFaceRecHandle;
typedef int IFaceRecResult;

typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} T_IFaceRecRect;

typedef enum 
{
    IFACEREC_EXTRACTOR_TUNA     = 0x0,
    IFACEREC_EXTRACTOR_GLUON    = 0x1,
    IFACEREC_EXTRACTOR          = 0x1,
	IFACEREC_DETECTOR_VJBOOST   = 0x10,
    IFACEREC_DETECTOR_NICKLE    = 0x11,
    IFACEREC_DETECTOR_EAGLE     = 0x12,
    IFACEREC_DETECTOR           = 0x12,
    IFACEREC_PREDICTOR_POSE     = 0x20,
    IFACEREC_PREDICTOR_GENDER   = 0x21,
    IFACEREC_PREDICTOR_LOCATE   = 0x22,
    IFACEREC_TYPE_END           = 0x22,
    IFACEREC_INVALID            = -1
} E_IFaceRecType;

/*typedef enum 
{
    FEATURE_EXT_RECT_TYPE_BORDER = 0x0,  // top, left,bottom and right
    FEATURE_EXT_RECT_TYPE_CYCLE  = 0x1,  // center point radiu, not support
    FEATURE_EXT_RECT_TYPE_AROUND = 0X2,  // top, left,width and height
} EFeatureExtRectType;*/

enum E_IFaceRecPose{    
	IFACEREC_STRICTFRONTAL  = 0,
	IFACEREC_FRONTAL        = 1,
	IFACEREC_HALFLEFT       = 2,
	IFACEREC_HALFRIGHT      = 3,
	IFACEREC_FULLLEFT       = 4,
	IFACEREC_FULLRIGHT      = 5,
	IFACEREC_FRONTAL_CW30   = 6,
	IFACEREC_HALFLEFT_CW30  = 7,
	IFACEREC_HALFRIGHT_CW30 = 8,
	IFACEREC_FULLLEFT_CW30  = 9,  
	IFACEREC_FULLRIGHT_CW30 = 10,
	IFACEREC_FRONTAL_ACW30  = 11,
	IFACEREC_HALFLEFT_ACW30 = 12,
	IFACEREC_HALFRIGHT_ACW30= 13,
	IFACEREC_FULLLEFT_ACW30 = 14,
	IFACEREC_FULLRIGHT_ACW30= 15
};

typedef struct
{
    int dPitchType; // 以x轴为中心,脸部上下俯仰类型
    int dYawType;   // 以y轴为中心,脸部左右旋转类型
    int dRollType;  // 以中心点为中心，x-y平面旋转类型
} T_IFaceRecPose;

typedef struct
{
    float fx;
    float fy;
} T_IFaceRecPoint2f;

enum
{
    IFACEREC_RESULT_OK                    = 0,
    //初始化错误
    IFACEREC_RESULT_ERR_INIT_FAILED       = 0x01, //初始化失败
    //句柄创建错误
    IFACEREC_RESULT_ERR_TYPE_UNSUPPORTED  = 0x10, //算法类型不支持
    IFACEREC_RESULT_ERR_CREATE_HANDLE     = 0x11, //创建算法句柄失败
    //调用流程错误
    IFACEREC_RESULT_ERR_UNEXIST_HANDLE    = 0x20, //算法句柄不存在
    IFACEREC_RESULT_ERR_DB_ALREADY_EXIST  = 0x21, //数据库重复创建
    IFACEREC_RESULT_ERR_DB_NOT_EXIST      = 0x22, //数据库不存在
    //数据库错误
    IFACEREC_RESULT_ERR_DB_CREATE         = 0x30,
    IFACEREC_RESULT_ERR_DB_INSERT         = 0x31,
    IFACEREC_RESULT_ERR_DB_UPDATE         = 0x32,
    IFACEREC_RESULT_ERR_DB_DELETE         = 0x33,
    IFACEREC_RESULT_ERR_DB_SELECT         = 0x34,
    
    //算法错误
    
    IFACEREC_RESULT_ERR                   = -1,   //未定义的错误
};



#endif

