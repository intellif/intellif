/*  
   Description: Face Detector
   Authors:     Yongqiang.Mou(mou.yongqiang@intellif.com), Dihong.Tian(tian.dihong@intellif.com)

   Do not redistribute without permission 
   For any questions please contact Yongqiang.Mou or Dihong.Tian
   Copyright(C) 2016, Shenzhen Intellifusion Technologies Co., Ltd., all rights reserved.
*/

#ifndef __H_H_LIB_FACE_DETECTOR_H_H__
#define __H_H_LIB_FACE_DETECTOR_H_H__
#include <cstdio>
#include <exception>
#include <vector>
using namespace std;

#ifdef WIN32
#ifndef STATIC_LIB
#ifdef FACEDETECTOR_EXPORTS
#define FACE_DETECT_LIB_API __declspec(dllexport) 
#else
#define FACE_DETECT_LIB_API __declspec(dllimport) 
#endif
#else
#define FACE_DETECT_LIB_API
#endif
#else
#define FACE_DETECT_LIB_API
#endif

typedef int INT32;

struct FapRect
{
	INT32 left;
	INT32 top;
	INT32 right;
	INT32 bottom;       // left, top, right and bottom co-ordinates of a rectangle.
	FapRect() {left=top=right=bottom=0;};
	FapRect(INT32 l,INT32 t,INT32 r,INT32 b)
	{
		if (l>r || t>b)
		{
			//throw std::exception::exception();
			throw std::exception();
		}
		left    = l;
		top     = t;
		right   = r;
		bottom  = b;
	};
};


struct Face
{
	FapRect rect;
	INT32 centerX;
	INT32 centerY;
	INT32 radius;
	INT32 inPlaneRot;
	INT32 horzOopRot;
	INT32 vertOopRot;
	INT32 confidence;
	INT32 rotationState;
};
typedef std::vector<Face> FaceArray;


class FACE_DETECT_LIB_API FaceDetector
{
public:
	FaceDetector(bool bMode = true);
	~FaceDetector();
	int Init(bool bHalfProfile = true, bool bProfile = false, bool bRot = true);
	int DetectFaces(unsigned char* ucImgData, const int nImgWidth, const int nImgHeight, FaceArray& faceArray);
    void Release();
private:
	void* m_fdp;
	bool m_bVideoMode;
};
#endif