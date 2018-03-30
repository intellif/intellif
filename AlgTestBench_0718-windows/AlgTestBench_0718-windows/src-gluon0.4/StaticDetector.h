#ifndef _STATICDETECTOR_H_
#define _STATICDETECTOR_H_

#include "FaceDetect.h"

// determine the memory size required to load the static detector
int StaticDetectorMemsize(int views);

// determine the memory size required to load the static skin lut
int StaticSkinMemsize();
// determine the memory size required to load the static stage statistic struct
int StaticStageStatsMemsize(int views);

// load the static detector into a pre-allocated memory buffer
DETECTOR*	LoadStaticDetector(const UINT8* buffer, int bufferSize, int views);

// copy a detector into a pre-allocated buffer
DETECTOR*	Copy(const DETECTOR &src, const UINT8* buffer, int bufferSize);
// copy a cascade into a pre-allocated buffer
int Copy(const CASCADE &src, CASCADE &dest, const UINT8* buffer, int bufferSize);
// load the static skin detector into a pre-allocated memory buffer
SKINLUT*	   LoadStaticSkin(const UINT8* buffer, int bufferSize, pix_fmt fmt);															
// load the static stage statistics into a pre-allocated memory buffer
STAGE_STATS* LoadStaticStageStats(const UINT8* buffer, int buffersize, DETECTOR *parent);


#endif //_STATICDETECTOR_H_
