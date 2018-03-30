#ifndef __IFYOUTU_FACESDK_H__
#define __IFYOUTU_FACESDK_H__

#include <string>

#include "ifacesdk.h"

class YtSdkWrapperIniter {
	
public:
	 	static void      YtStaticInit(std::string strModelPath = "model");
};

class YtFeatureExtManager {

public:

	static if_handle_t CreateIfExtratorHandle(int           dwThreadCnt);

	static void        DestoryIfExtratorHandle(if_handle_t  hHandle);

	static int         ExtratFeatureFromImage(if_handle_t    hHandle,
		                                        void          *pRgbData,
		                                        int            dwWidth,
		                                        int            dwHeight,
		                                        if_rect_t     &rFaceRect,
		                                        float        **pFeature);

	static void        ReleaseFeautreResult(float        *  pFeature);

};


class YtDetectorManager {

public:

	static if_handle_t CreateIfDetectorHandle(int   dwThreadCnt);

	static void        DestoryIfDetectoHandle(if_handle_t  hHandle);

	static if_result_t FeedOneFramToDetector(if_handle_t       hHandle,
		                                       void	*            pRgbData,
		                                       const int         dwWidth,
		                                       const int         dwHeight,
		                                       PIF_FACERECT     *pFaceRectArray,
		                                       unsigned int     *pdwFaceCnt);

	static void        ReleaseDetFaceResult(PIF_FACERECT       pFaceRectArray,
		                                      unsigned int       pdwFaceCnt);

};


#endif
