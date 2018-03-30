#ifndef FACE_DETECT_LIB
#define FACE_DETECT_LIB

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifndef DETECTOR_STATIC_LIB
#ifdef WIN32
#include <windows.h>

#ifdef FACE_DETECT_LIB_EXPORTS
#define LINKAGE        extern "C"
#define FACE_DETECT_LIB_API __declspec(dllexport)
#else
#define LINKAGE
#define FACE_DETECT_LIB_API __declspec(dllimport)
#endif
#else
#define LINKAGE
#define FACE_DETECT_LIB_API 
#endif
#else
#ifdef WIN32
#include <windows.h>
#endif
#define LINKAGE
#define FACE_DETECT_LIB_API 
#endif

typedef void *FDP;			// this is used to pass instances of the face detector back and forth

// these are the return values of all the function calls
#define FD_OK							0x0000
#define FD_ERR							0x0001
#define FD_WARNING						0x0002
#define FD_OUT_OF_MEMORY				0x0004|FD_ERR
#define FD_UNINITIALIZED				0x0008|FD_ERR
#define FD_INCONS_OPTS					0x0010|FD_ERR
#define FD_NOIMAGE						0x0020|FD_ERR
#define FD_FACE_INDEX_OUT_OF_RANGE		0x0040|FD_ERR
#define FD_IMAGE_ERR					0x0080|FD_ERR
#define FD_UNSUPPORTED_IMAGE			0x0100|FD_ERR

// these are to flag the pixel order / type
#define _UNKNOWN_PIX_	0x00
#define _GRAY_PIX_		0x01
#define _YCC_PIX_		0x02
#define _RGB_PIX_		0x03
#define _BGR_PIX_		0x04
#define _LAB_PIX_		0x05

LINKAGE  FACE_DETECT_LIB_API int initFaceDetector(FDP *fd_out);					// [OUT] initialized FDP

LINKAGE  FACE_DETECT_LIB_API int freeFaceDetector(FDP fd);							// [IN] initialized FDP

LINKAGE  FACE_DETECT_LIB_API int	setFaceDetectorOptions(FDP fd,					// [IN] initialized FDP
														   int	use_skin,			// [IN] boolean flag to activate skin detection
														   int rot_30,				// [IN] boolean flag to activate 30 degree rotations around active 90 degree axes
														   int rot_90,				// [IN] boolean flag to activate all 4 90 degree axes
														   int no_portraits,		// [IN] boolean flag to disable portrait detection
														   int no_half_profiles,	// [IN] boolean flag to disable half-profile detection (currently does nothing)
														   int no_profiles,		// [IN] boolean flab to disable full profile detection
														   int	min_certainty,		// [IN] int value 0-100, 0 == native certainty (faster), 1->100 min->max certainty, native is about 96
														   int	min_face_dim);		// [IN] search for faces >= to this face size (>=24)

// this is the new API call which allows more flexibility in setting the options
LINKAGE  FACE_DETECT_LIB_API int	setFaceDetectorOptionsEx(FDP fd,				// [IN] initialized FDP
															 int use_skin,			// [IN] boolean flag to activate skin detection
															 int use_rot_30,		// [IN] boolean flag to activate 30 degree rotations around active 90 degree axes
															 int do_rot_0,			// [IN] boolean flag to activate 0 degree in plane rotations
															 int do_rot_90,			// [IN] boolean flag to activate 90 degree in plane rotations
															 int do_rot_180,		// [IN] boolean flag to activate 180 degree in plane rotations
															 int do_rot_270,		// [IN] boolean flag to activate 270 degree in plane rotations
															 int no_portraits,		// [IN] boolean flag to disable portrait detection
															 int no_half_profiles,	// [IN] boolean flag to disable half-profile detection (currently does nothing)
															 int no_profiles,		// [IN] boolean flab to disable full profile detection
															 int min_certainty,		// [IN] int value 0-100, 0 == native certainty (faster), 1->100 min->max certainty, native is about 96
															 int min_face_dim);		// [IN] search for faces >= to this face size (>=24)

// The input image data should be an 8-bit array organized in top-down row-wise format, where each pixel is represented by bpp consecutive bytes
// with pixel order / colorspace given by the pixType. Acceptable pixTypes are _GRAY_PIX_, _YCC_PIX_, _RGB_PIX_,_BGR_PIX_. The pix array is 
// converted internally to 3 byte YCC - processing associated with the image handling can be minimized by supplying an image either in this format
// or in 1 byte Y format.
//
LINKAGE  FACE_DETECT_LIB_API int runFaceDetector(FDP fd,								// [IN] initialized FDP
												 unsigned char* pix,							// [IN] pointer to the image data (should be recast as UINT8*)
												 int pixlen,						// [IN] length of pix in bytes
												 int bpp,							// [IN] bytes per pixel
												 int pixType,						// [IN] _GRAY_PIX_ (=0x01); _YCC_PIX_ (=0x02); _RGB_PIX_ (=0x03); _BGR_PIX_ (=0x04)
												 int width,							// [IN] width of the input image
												 int height);						// [IN] height of the input image

LINKAGE  FACE_DETECT_LIB_API int getFaceCount(FDP fd,									// [IN] initialized FDP
											  int *num_faces);						// [OUT] number of faces detected in last runFaceDetector call

LINKAGE  FACE_DETECT_LIB_API int getFace(FDP fd,										// [IN] initialized FDP
										 int face_index,								// [IN] index of face in list of detected faces (0<=face_index<num_faces)
										 int upscale_perc,							// [IN] number >= 100 giving percentage to upscale face dimensions
										 int *xpos,									// [OUT] horizontal centre of face in pixels
										 int *ypos,									// [OUT] vertical centre of face in pixels
										 int *radius,									// [OUT] radius of face in pixels
										 int *in_plane_rotation,					// [OUT] (in degrees), number between 0 & 359
										 int *horiz_out_of_plane_rotation,		// [OUT] (in degrees), number between -90 and 90
										 int *vert_out_of_plane_rotation,		// [OUT] (in degrees), number between -90 and 90
										 int *confidence);							// [OUT] confidence this is a face, between 0 & 255

// this is for bottom-up images (Win32 DIBs)
LINKAGE  FACE_DETECT_LIB_API int getInvertedFace(FDP fd,										// [IN] initialized FDP
												 int face_index,								// [IN] index of face in list of detected faces (0<=face_index<num_faces)
												 int upscale_perc,							// [IN] number >= 100 giving percentage to upscale face dimensions
												 int *xpos,									// [OUT] horizontal centre of face in pixels
												 int *ypos,									// [OUT] vertical centre of face in pixels
												 int *radius,									// [OUT] radius of face in pixels
												 int *in_plane_rotation,					// [OUT] (in degrees), number between 0 & 359
												 int *horiz_out_of_plane_rotation,		// [OUT] (in degrees), number between -90 and 90
												 int *vert_out_of_plane_rotation,		// [OUT] (in degrees), number between -90 and 90
												 int *confidence);							// [OUT] confidence this is a face, between 0 & 255


LINKAGE  FACE_DETECT_LIB_API int initVideoFaceDetector(FDP *fd_out);							// [OUT] initialized video FDP

LINKAGE  FACE_DETECT_LIB_API int	setVideoFaceDetectorOptions(FDP fd,							// [IN] initialized video FDP
																int	use_skin,				// [IN] boolean flag to activate skin detection
																int	use_rot_30,				// [IN] boolean flag to activate 30 degree rotations around active 90 degree axes
																int	use_rot_90,				// [IN] boolean flag to activate all 4 90 degree axes
																int	no_portraits,			// [IN] boolean flag to disable portrait detection
																int	no_half_profiles,		// [IN] boolean flag to disable half-profile detection (currently does nothing)
																int	no_profiles,			// [IN] boolean flab to disable full profile detection
																int	min_certainty,			// [IN] int value 0-100, 0 == native certainty (faster), 1->100 min->max certainty, native is about 96
																int	min_face_dim,			// [IN] search for faces >= to this face size (>=24)
																int	video_frame_stagger);// [IN] video stagger level (0=none; 1=2x2; 2=4x4; 3=6x6)

// this is the new API call which allows more flexibility in setting the options
LINKAGE  FACE_DETECT_LIB_API int	setVideoFaceDetectorOptionsEx(FDP fd,				// [IN] initialized FDP
																  int use_skin,			// [IN] boolean flag to activate skin detection
																  int use_rot_30,		// [IN] boolean flag to activate 30 degree rotations around active 90 degree axes
																  int do_rot_0,			// [IN] boolean flag to activate 0 degree in plane rotations
																  int do_rot_90,			// [IN] boolean flag to activate 90 degree in plane rotations
																  int do_rot_180,		// [IN] boolean flag to activate 180 degree in plane rotations
																  int do_rot_270,		// [IN] boolean flag to activate 270 degree in plane rotations
																  int no_portraits,		// [IN] boolean flag to disable portrait detection
																  int no_half_profiles,	// [IN] boolean flag to disable half-profile detection (currently does nothing)
																  int no_profiles,		// [IN] boolean flab to disable full profile detection
																  int min_certainty,		// [IN] int value 0-100, 0 == native certainty (faster), 1->100 min->max certainty, native is about 96
																  int min_face_dim,			// [IN] search for faces >= to this face size (>=24)
																  int video_frame_stagger);// [IN] video stagger level (0=none; 1=2x2; 2=4x4; 3=6x6)


LINKAGE  FACE_DETECT_LIB_API int runVideoFaceDetector(FDP fd,								// [IN] initialized video FDP
													  unsigned char* framepix,		// [IN] pointer to the image data (should be recast as UINT8*)
													  int pixlen,							// [IN] length of pix in bytes
													  int bpp,								// [IN] bytes per pixel
													  int pixType,						// [IN] _GRAY_PIX_ (=0x01); _YCC_PIX_ (=0x02); _RGB_PIX_ (=0x03); _BGR_PIX_ (=0x04)
													  int width,							// [IN] width of the input image
													  int height,							// [IN] height of the input image
													  int frameTime);					// [IN] frame time in msec

LINKAGE  FACE_DETECT_LIB_API int clearPersistentFaces(FDP fd);				// [IN] initialized video FDP

LINKAGE  FACE_DETECT_LIB_API int getPersistentFaceCount(FDP fd,				// [IN] initialized video FDP
														int *num_faces);	// [OUT] number of faces detected in the persistentFaces list

LINKAGE  FACE_DETECT_LIB_API int getPersistentFace(FDP fd,										// [IN] initialized video FDP
												   int face_index,							// [IN] index of face in list of persistent faces (0<=face_index<num_faces)
												   int upscale_perc,							// [IN] number >= 100 giving percentage to upscale face dimensions
												   int *xpos,									// [OUT] horizontal centre of face in pixels
												   int *ypos,									// [OUT] vertical centre of face in pixels
												   int *radius,								// [OUT] radius of face in pixels
												   int *in_plane_rotation,					// [OUT] (in degrees), number between 0 & 359
												   int *horiz_out_of_plane_rotation,	// [OUT] (in degrees), number between -180 and 180
												   int *vert_out_of_plane_rotation,		// [OUT] (in degrees), number between -180 and 180
												   int *face_ID,								// [OUT] unique ID for this face in the list, number between 0 & 299
												   int *time_since_last_detect,			// [OUT] (in msec), time since last detected this face
												   int *confidence);							// [OUT] confidence this is a face, between 0 & 255

LINKAGE  FACE_DETECT_LIB_API int getInvertedPersistentFace(FDP fd,										// [IN] initialized video FDP
														   int face_index,							// [IN] index of face in list of persistent faces (0<=face_index<num_faces)
														   int upscale_perc,							// [IN] number >= 100 giving percentage to upscale face dimensions
														   int *xpos,									// [OUT] horizontal centre of face in pixels
														   int *ypos,									// [OUT] vertical centre of face in pixels
														   int *radius,								// [OUT] radius of face in pixels
														   int *in_plane_rotation,					// [OUT] (in degrees), number between 0 & 359
														   int *horiz_out_of_plane_rotation,	// [OUT] (in degrees), number between -180 and 180
														   int *vert_out_of_plane_rotation,		// [OUT] (in degrees), number between -180 and 180
														   int *face_ID,								// [OUT] unique ID for this face in the list, number between 0 & 299
														   int *time_since_last_detect,			// [OUT] (in msec), time since last detected this face
														   int *confidence);							// [OUT] confidence this is a face, between 0 & 255

LINKAGE FACE_DETECT_LIB_API int isVideoInput(FDP fd,			// [IN] initialized FDP
											 int *is_video);	// [OUT] ==0 if FDP is NOT video, nonzero otherwise

// define a crop rectangle as a percentage of the width and height of the image (can be negative too) for a bottom up image
LINKAGE FACE_DETECT_LIB_API int	setCropPercentages(FDP fd,				// [IN] initialized FDP
												   float	top,		// [IN] crop percentage for top border [-50.0...100.0)
												   float	left,		// [IN] crop percentage for left border [-50.0...100.0)
												   float	bottom,		// [IN] crop percentage for bottom border [-50.0...100.0-top)
												   float	right);		// [IN] crop percentage for right border [-50.0...100.0-left)

// define a crop rectangle as a percentage of the width and height of the image (can be negative too) for a top down image
LINKAGE FACE_DETECT_LIB_API int	setInvertedCropPercentages(FDP fd,				// [IN] initialized FDP
														   float	top,		// [IN] crop percentage for top border [-50.0...100.0)
														   float	left,		// [IN] crop percentage for left border [-50.0...100.0)
														   float	bottom,		// [IN] crop percentage for bottom border [-50.0...100.0-top)
														   float	right);		// [IN] crop percentage for right border [-50.0...100.0-left)

// reset all the crop percentages to 0
LINKAGE FACE_DETECT_LIB_API int	unsetCropPercentages(FDP fd);					// [IN] initialized FDP

#endif // FACE_DETECT_LIB
