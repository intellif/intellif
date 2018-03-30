
#ifndef FACE_DETECT_H
#define FACE_DETECT_H

// The following defines are necessary when compiling for WIN32
   #define INT8   char
   #define UINT8  unsigned char
   #define INT16  short
   #define UINT16 unsigned short
   #define INT32  int
   #define UINT32 unsigned int   
	#ifndef VERIFY  
		#define VERIFY(x) { }
	#endif
   #include <time.h>

   inline UINT32 BinfTickGet() { return (UINT32)clock();}
   inline UINT32 BinfTicksPerSec() { return CLOCKS_PER_SEC; }

#ifndef MAX
#define MAX(A,B) ((A) > (B)) ?  (A) : (B)
#endif
#ifndef MIN
#define MIN(A,B) ((A) < (B)) ?  (A) : (B)
#endif
#define ABS(A)   ((A) <  0   ? -(A) : (A))

#define QBITS 10                                   // fixed-point fractional bits
#define HALF (1 << (QBITS-1))                      // value of 0.5 
#define MULT_FP(x, y) (((x)*(y) + HALF) >> QBITS)  // fixed-point multiply

#define MAXDETECTIONS            300
#define STEP_SIZE       (UINT32)(2.00 * (1 << QBITS))
#define SCALE_FACTOR    (UINT32)(1.25 * (1 << QBITS))
#define SCALE_FACTOR_2	(UINT32)(1.5625 * (1 << QBITS))	// for variance features, need the scale^2
#define FACE_SIDE       (UINT32)(24)
#define GRID_OFFSET					 5					  // number of pixels on the image border to miss out
#define MIN_FACE_SIDE	(UINT32)(24)
#define POSE_DETECTOR_DEPTH 30
#define MAX_VIEWS 60


#define		HORIZONTAL_EDGE			0x000
#define		VERTICAL_EDGE				0x001
#define		HORIZONTAL_LINE			0x002
#define		VERTICAL_LINE				0x003
#define		CHECKERBOARD				0x004
#define		CENTREPOINT					0x005

#define		WIDE_HORIZONTAL_LINE		0x006	
#define		WIDE_VERTICAL_LINE		0x007
#define		VERTICAL_LOWER_STEP		0x008
#define		VERTICAL_UPPER_STEP		0x009
#define		HORIZONTAL_UPPER_STEP	0x00a
#define		HORIZONTAL_LOWER_STEP	0x00b

#ifdef _FD_TRAIN_
#define		ANNULUS						0x00c	// this is a different definition: {tlx,tly,w,h} == {tlx,tlx2,w1,w2}
#endif // _FD_TRAIN_

#define		FEATURE_BASE_MASK			0x00f // all features have a base type between 0 and 15


//////////////////////////////////////////////////////////////////////////////////////////////
// rotation states
enum {
	// NOT A FACE
	NOTFACE				=0x0,

	// DEACTIVATED_FACE
	DEACTIVATED			=0x0000001,

	// IN_PLANE_ROTATIONS
	IPR_0					=0x0000002,
	IPR_30				=0x0000004,	// IPR_0  <<1
	IPR_60				=0x0000008,	// IPR_30 <<1
	IPR_90				=0x0000010,
	IPR_120				=0x0000020,	
	IPR_150				=0x0000040,
	IPR_180				=0x0000080,
	IPR_210				=0x0000100,	
	IPR_240				=0x0000200,
	IPR_270				=0x0000400,
	IPR_300				=0x0000800,	
	IPR_330				=0x0001000,	

	// HORIZ_OUT_OF_PLANE_ROTATIONS
	HOOP_MINUS_90		=0x0002000,
	HOOP_MINUS_60		=0x0004000,
	HOOP_MINUS_30		=0x0008000,
	HOOP_0				=0x0010000,
	HOOP_PLUS_30		=0x0020000,
	HOOP_PLUS_60		=0x0040000,
	HOOP_PLUS_90		=0x0080000,

	// VERT_OUT_OF_PLANE_ROTATIONS
	VOOP_MINUS_90		=0x0100000,
	VOOP_MINUS_60		=0x0200000,
	VOOP_MINUS_30		=0x0400000,
	VOOP_0				=0x0800000,
	VOOP_PLUS_30		=0x1000000,
	VOOP_PLUS_60		=0x2000000,
	VOOP_PLUS_90		=0x4000000,

	// flag for (in plane) rotation invariant classifier 
	IPR_INVARIANT		=0x8000000
};


#define NUM_IPR			12
#define NUM_HOOP		7
#define NUM_VOOP		7
// NUM_ROTATIONS is NUM_IPR*NUM_HOOP*NUM_VOOP + 2 (NONOBJECT & DEACTIVATED)
#define NUM_ROTATIONS	590 

// rotation state functions
#define IN_PLANE_ROTATIONS				(IPR_0|IPR_30|IPR_60|IPR_90|IPR_120|IPR_150|IPR_180|IPR_210|IPR_240|IPR_270|IPR_300|IPR_330)
#define HORIZ_OUT_OF_PLANE_ROTATIONS	(HOOP_MINUS_90|HOOP_MINUS_60|HOOP_MINUS_30|HOOP_0|HOOP_PLUS_30|HOOP_PLUS_60|HOOP_PLUS_90)
#define VERT_OUT_OF_PLANE_ROTATIONS		(VOOP_MINUS_90|VOOP_MINUS_60|VOOP_MINUS_30|VOOP_0|VOOP_PLUS_30|VOOP_PLUS_60|VOOP_PLUS_90)
#define OUT_OF_PLANE_ROTATIONS			(HORIZ_OUT_OF_PLANE_ROTATIONS|VERT_OUT_OF_PLANE_ROTATIONS)

#define IS_SYMMETRIC(type)	((type & HORIZ_OUT_OF_PLANE_ROTATIONS) == HOOP_0)

#define FACE	(IN_PLANE_ROTATIONS|HORIZ_OUT_OF_PLANE_ROTATIONS|VERT_OUT_OF_PLANE_ROTATIONS|DEACTIVATED)

/*
// got this from http://graphics.stanford.edu/~seander/bithacks.html
const unsigned int b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 0xFF00FF00, 
			  0xFFFF0000};
register unsigned int c = (v & b[0]) != 0;
for (i = 4; i > 0; i--) // unroll for speed...
{
  c |= ((v & b[i]) != 0) << i;
}*/

// return logbase2 of the given integer
#define LOGBASE2(v) (((v & 0xAAAAAAAA) != 0) | (((v & 0xCCCCCCCC) != 0) << 1) | (((v & 0xF0F0F0F0) != 0) << 2) | (((v & 0xFF00FF00) != 0) << 3) | (((v & 0xFFFF0000) != 0) << 4))

// return the rotation state corresponding to degrees supplied
#define IPR_CLASS(ipr)		(IPR_0 << ((ipr / 30) % 12))
#define HOOP_CLASS(hoop)	(HOOP_MINUS_90 << (((hoop / 30) + 3) % 7))
#define VOOP_CLASS(voop)	(VOOP_MINUS_90 << (((voop / 30) + 3) % 7))

// return the rotation degrees corresponding to the rotation state supplied
#define IPR_DEGREES(ipr_class) (30 * (LOGBASE2((ipr_class/IPR_0))))
#define HOOP_DEGREES(hoop_class) (30 * (LOGBASE2((hoop_class/HOOP_MINUS_90))) - 90)
#define VOOP_DEGREES(voop_class) (30 * (LOGBASE2((voop_class/VOOP_MINUS_90))) - 90)

// return the reflected classes (reflection about the horizontal axis)
#define REFLECTED_IPR_CLASS(ipr_class)		((ipr_class==IPR_0)?IPR_0:(IPR_CLASS((360-(IPR_DEGREES(ipr_class))))))
#define REFLECTED_HOOP_CLASS(hoop_class)	(HOOP_CLASS(-(HOOP_DEGREES(hoop_class))))
#define REFLECTED_VOOP_CLASS(voop_class)	(voop_class)


typedef struct {
	int tlx, tly; // top left coordinates
	int brx, bry; // bottom right coordinates
	int rotation_state;	  // rotation state of face this box points to	
	UINT8 confidence;			// confidence (0-255) that this box is a face	
} Box;


// Array of boxes class
class BoxList
{
public:
   BoxList() { length = 0; entry = NULL; };
   ~BoxList() { if ( entry != NULL ) delete [] entry; };

   int length;  // number of BoxListEntry elements
	Box *entry;  // pointer to BoxListEntry array
};

bool BoxesOverlap(Box box1, Box box2);

int MedianBoxes(Box *box_cluster, int boxes_in_cluster, Box *final_box);

int AverageBoxes(Box *box_cluster, int boxes_in_cluster, Box *final_box);

// test all the boxes in the array and combine overlapping boxes using average or median 
int CombineBoxes(Box *boxes,				// the combined boxes are written back into this array
					  int num_detections,
					  bool use_median);

#define SKINLUT_DIM 256
#define SKINLUT_SHIFT 0

typedef enum {
	YCC_PIX,
	LAB_PIX,
	LHC_PIX
} pix_fmt;

struct SKINLUT {
	bool		*lut[SKINLUT_DIM];
	bool		lut_ptr[SKINLUT_DIM * SKINLUT_DIM];
	int		threshold;
	UINT8		minLum;
	UINT8		maxLum;
	pix_fmt  fmt;
	SKINLUT() {
		int n;
		threshold = -1; 
		int ptr_idx = 0;
		fmt = YCC_PIX;
		minLum = 0;
		maxLum = 255;
		for ( n=0; n < SKINLUT_DIM; ++n ) {
			// index into the array block
			lut[n] = (lut_ptr+ptr_idx);
			ptr_idx += SKINLUT_DIM;
		}
		
		// set everything to "false"
		for (n=0; n < SKINLUT_DIM * SKINLUT_DIM; ++n ) lut_ptr[n] = false;
		
	};
};


class IImage
{
public:

	IImage();
   IImage(int w, int h, UINT8 *pBuf, UINT32 bufferSizeInBytes);

   ~IImage() {}; // no memory allocated in this class

	bool Reset(int w, int h, UINT8 *pBuf, UINT32 bufferSizeInBytes);

	static int GetBufSize(int w, int h);

   INT32 **ptr;
   int w;
   int h;
};

void ComputeIntegralImages(UINT8 *pData, 
									int bytesPerCol, 
									int bytesPerRow,
									int width,
									int height,
									IImage *iimg,
									IImage *iimg_sq
									);

void ComputeIntegralImages(UINT8 *pData, 
									int bytesPerCol, 
									int bytesPerRow,
									int width,
									int height,
									SKINLUT *skin,
									IImage *iimg_skin,
									IImage *iimg,
									IImage *iimg_sq
									);

// compute the rotated integral and squared integral image from pData, format is as above. 
// The rotation amount (degrees) *must* be one of 30 or 330 degrees.
void ComputeRotatedIntegralImages(UINT8 *pData, 
									int bytesPerCol, 
									int bytesPerRow,
									int width,
									int height,
									int degrees,
									IImage *riimg);

// compute both the upright and rotated integral images from pData, format is as above.
void ComputeIntegralImages(UINT8 *pData, 
									int bytesPerCol, 
									int bytesPerRow,
									int width,
									int height,
									IImage *iimg0,
									IImage *iimg30,
									IImage *iimg330,
									IImage *iimg0_sq
									);

// compute the upright, rotated and skin integral images from pData, format is as above
void ComputeIntegralImages(UINT8 *pData, 
									int bytesPerCol, 
									int bytesPerRow,
									int width,
									int height,
									SKINLUT *skin,
									IImage *iimg_skin,
									IImage *iimg0,
									IImage *iimg30,
									IImage *iimg330,
									IImage *iimg0_sq
									);

// compute the patch normalization constants (both for mean and variance features) using the upright integral image
void ComputeNormConstants(int yoff, 
								  int xoff, 
								  IImage* iimg,
								  IImage* iimg_sq,
								  int patch_side, 
								  int patch_mult, 
								  int scale,
								  int &meanX,
								  int &varX,
								  int &norm_const);


// compute the stagger parameters for video sampling
void ComputeStaggerParams(UINT8 frame_index, 
								  UINT8 stagger_level, 
								  int &stagger_step, 
								  int &vstep_off, 
								  int &hstep_off);

typedef struct {
	INT16  tlx, tly;        //  4 bytes 
	UINT16 w, h;            //  4 bytes
	UINT8 type;             //  1 byte
#ifdef EARLY_OPTOUT
	// need 4 byte integers here for the early optout versions of the cascades
	INT32 threshold;        //  4 bytes
	INT32 alpha_p, alpha_n; //  8 bytes  
#else
	INT16 threshold;        //  2 bytes
	INT16 alpha_p, alpha_n; //  4 bytes  (signed 4.12 format)
#endif // EARLY_OPTOUT
} FEATURE;						// 17 bytes total

// the chaining feature takes as input the raw output of the previous stage
typedef struct {				
#ifdef EARLY_OPTOUT
	// need 4 byte integers here for the early optout versions of the cascades
	INT32 threshold;        //  4 bytes
	INT32 alpha_p, alpha_n; //  8 bytes 
#else
	INT16 threshold;        //  2 bytes
	INT16 alpha_p, alpha_n; //  4 bytes  (signed 4.12 format)
#endif // EARLY_OPTOUT
} CHAIN;							// 6 bytes total

// this struct is used to hold the stats for each stage, for the new face certainty map
typedef struct { 
	UINT8	start_stage;
	UINT8	*face_prior;
	/* UINT8 *nonface_prior = 255 - face_prior; */
	INT32	*face_mean;
	INT32	*face_sd;
	INT32	*nonface_mean;
	INT32	*nonface_sd;
} STAGE_STATS;

struct STAGE {
	int num_features;  // number of features in the stage
	int theta;         // final threshold value for this stage
	FEATURE *feature;  // pointer to array of num_features features
	CHAIN chain;		 // only use this if chaining is enabled
	STAGE() { num_features = 0; feature = NULL; };
	~STAGE() { if ( feature != NULL ) delete [] feature; };
};

struct CASCADE {
	int num_stages;	// number of stages in the cascade
	bool chaining;		// cascade uses chaining features
	bool symmetric;	// cascade is a symmetric rotation type
	INT32 type;			// rotation type of this cascade
	STAGE *stage;		// pointer to array of num_stages stages
	CASCADE() { stage = NULL; num_stages = 0; };
	~CASCADE() { 
		if ( stage != NULL ) {
			// destructor of stage should handle this
			delete [] stage;
		}
	};

};

struct DETECTOR {
	// array of cascades for different poses
	int num_cascades;
	CASCADE *cascade;

	// global prefilters - evaluated before any of the poses
	INT32 min_patch_mean;		// if >= patch mean, don't process
	INT32 max_patch_mean;		// if <= patch mean, don't process
	INT32 min_patch_variance;	// if >= patch variance, don't process
	INT32 max_patch_variance;	// if <= patch variance, don't process
	SKINLUT *skin;				// use this to create a skin integral image prefilter

	DETECTOR() { 
		num_cascades = 0; 
		cascade = NULL; 
		skin = NULL; 
		// put these parameters in so the default behaviour is sensible!
		min_patch_mean = 5;
		max_patch_mean = 250;
		min_patch_variance = 1;
		max_patch_variance = 10000; 
	};
	~DETECTOR() {
		if ( cascade != NULL ) delete [] cascade;
		if ( skin != NULL ) delete skin;
	};
};

inline int steps_to_skin(int y, int x, int patch_side, int scaled_skin_threshold, IImage *skin_iimg)
{
	int sum      = skin_iimg->ptr[y -              1][x -              1] + 
						skin_iimg->ptr[y + patch_side - 1][x + patch_side - 1] -
						skin_iimg->ptr[y -              1][x + patch_side - 1] -
						skin_iimg->ptr[y + patch_side - 1][x -              1];

	if ( sum >= scaled_skin_threshold ) return 0; // this means a success - no steps to skin

	return (scaled_skin_threshold - sum) / patch_side; // this is the max steps possible ensuring no skin
};

// evaluate the skinlut on a single pixel
inline bool Evaluate(SKINLUT *skin, UINT8 Lum, UINT8 Color1, UINT8 Color2)
{
	return (Lum >= skin->minLum && Lum <= skin->maxLum && skin->lut[(Color1>>SKINLUT_SHIFT)][(Color2>>SKINLUT_SHIFT)]);
};

//////////////////////////////////
//	Evaluate methods
/////////////////////////////////

// evaluate all the views of a detector on the upright image only at the specified location - the predicted
// rotation state of the patch is returned
int Evaluate(DETECTOR *detector,
				 int yoff, 
				 int xoff, 
				 IImage *iimg, 
				 int norm_const);

// evaluate a detector on this patch with stage stats for each view to give a patch confidence
int Evaluate(DETECTOR* detector, 
				 int yoff, 
				 int xoff, 
				 IImage *iimg,
				 int norm_const,
				 STAGE_STATS *view_stage_stats,
				 UINT8 &confidence);

// evaluate all the views of a detector on the upright, 30 and 330 degree images at the specified locations - 
// the predicted rotation state of the patch is returned
int Evaluate(DETECTOR *detector,
				 int yoff0, 
				 int xoff0,
				 int yoff30,
				 int xoff30,
				 int yoff330,
				 int xoff330,
				 IImage *iimg0,
				 IImage *iimg30,
				 IImage *iimg330,
				 int norm_const
				 );

// evaluate a detector on this patch with stage stats for each view to give a patch confidence
int Evaluate(DETECTOR* detector, 
				 int yoff0, 
				 int xoff0,
				 int yoff30,
				 int xoff30,
				 int yoff330,
				 int xoff330,
				 IImage *iimg0,
				 IImage *iimg30,
				 IImage *iimg330,
				 int norm_const,
				 STAGE_STATS *view_stage_stats,
				 UINT8 &confidence
				 );

// evaluate the cascade at the specified location - the predicted state of the patch is returned
int Evaluate(CASCADE* cascade, 
				 int yoff, 
				 int xoff, 
				 IImage *iimg,
				 int norm_const);

// evaluate the cascade at the specified location using stage stats to generate patch confidence
int Evaluate(CASCADE* cascade, 
			 int yoff, 
			 int xoff, 
			 IImage *iimg,
			 int norm_const,
			 STAGE_STATS *stage_stats,
			 UINT8 &confidence
			 );


// evaluate a first stage / nonchaining at the specified location - the numerical evaluation of the stage is returned
int Evaluate(STAGE* stage, 
				 int yoff, 
				 int xoff, 
				 IImage *iimg,
				 int norm_const);

// evaluate the stage at the specified location - the numerical evaluation of the stage is returned
int Evaluate(STAGE* stage, 
				 int yoff, 
				 int xoff, 
				 IImage *iimg,
				 int norm_const,
				 int last_stage_val);

inline int Evaluate(FEATURE* feature, 
				 int yoff, 
				 int xoff, 
				 IImage *iimg);

void Scale(DETECTOR* detector,
			  DETECTOR* scaled_detector,
			  UINT32 scale,
			  int patch_side);

void Scale(CASCADE* cascade, 
			  CASCADE *scaled_cascade, 
			  UINT32 scale, 
			  int patch_side);


void Rotate(DETECTOR* detector,
				DETECTOR* r_detector,
				int patch_side);


void Rotate(CASCADE* cascade,
				CASCADE* r_cascade,
				int patch_side);

// in-place rotate
void Rotate(CASCADE* cascade, 
				int rotation,		// IPR_0, IPR_90, IPR_180, IPR_270
				int patch_side);

// in-place rotate
void Rotate(FEATURE* pFeature, 
				int rotation,		// IPR_0, IPR_90, IPR_180, IPR_270
				int patch_side);

// in-place reflect
void Reflect(CASCADE* cascade,
				 int patch_side);

inline void Reflect(FEATURE *pFeature, int patch_side);


void RotateReflect(DETECTOR* detector,
						 DETECTOR* r_detector,
						 int patch_side);


void RotateReflect(CASCADE* cascade,
						 CASCADE* r_cascade,
						 int patch_side);


class FaceDetect1 {
public:
	FaceDetect1();
	~FaceDetect1();

	// this function returns the size of the memory block required by Reset
	static int GetBufSize(int width, int height, bool use_skin=false);

	// set the minimum size (in pixels) of faces to be looked for, must be >= FACE_SIDE == 24
	void SetMinFaceSide(UINT32 min_face_side) { m_min_face_side = MAX(FACE_SIDE,min_face_side); };


	void Reset(
		int width, 
		int height, 
		UINT8* pBuffer, 
		int bufferSize,
		int bytesPerCol,
		int bytesPerRow,
		bool use_skin=false
		);

	void Run(DETECTOR *detector, 
		DETECTOR *scaled_detector, 
		UINT8 *pData, 
		BoxList *face_list,
		bool use_skin=false);


	void Run(DETECTOR	*detector, 
		DETECTOR		*scaled_detector,
		STAGE_STATS		*view_stage_stats,
		UINT8			min_certainty,
		UINT8			*pData, 
		BoxList			*face_list,
		bool			use_skin = false);

	void RunVideo(
		DETECTOR	*detector,
		DETECTOR	*scaled_detector,
		UINT8		*pData,
		UINT8		frame_index,			// index of this frame in the list of frames
		UINT8		stagger_level,			// amount of stagger in the staggered grid (0==none,1=2x2,2=4x4,3=6x6)
		BoxList		*last_face_list,		// list of faces from the last frame
		BoxList		*face_list,				// output list of face bounding boxes
		bool		use_skin=false);

	void RunVideo(
		DETECTOR	*detector, 
		DETECTOR	*scaled_detector,
		STAGE_STATS	*view_stage_stats,
		UINT8		min_certainty,
		UINT8		*pData, 
		UINT8		frame_index,			// index of this frame in the list of frames
		UINT8		stagger_level,			// amount of stagger in the staggered grid (0==none,1=2x2,2=4x4,3=6x6)
		BoxList		*last_face_list,		// list of faces from the last frame
		BoxList		*face_list,
		bool		use_skin = false);

protected:

	// this member variable is set by SetMinFaceSide
	UINT32		  m_min_face_side;	// minimum size of the faces sought

	//these member variables are (re)set by Reset
   int           m_width;				// width of input image
   int           m_height;				// height of input image
   int           m_bytesPerCol;		// byte offset to move 1 input column
   int           m_bytesPerRow;		// byte offset to move 1 input row
	IImage		  m_iimageSkin;		// skin integral image 
   IImage        m_iimage;				// upright integral image
   IImage        m_iimageSqr;			// upright squared integral image
};


// class for running a detector in the 0, 30 and 330 rotated images of the supplied image.
// NOTE: the functions, their arguments and return values are identical with those in FaceDetect1
class FaceDetect3 {
public:
	FaceDetect3();
	~FaceDetect3();

	static int GetBufSize(int width, int height, bool use_skin=false);

	void SetMinFaceSide(UINT32 min_face_side) { m_min_face_side = MAX(FACE_SIDE,min_face_side); };

	void Reset(
		int width,				// this is the width of the upright image
		int height,				// this is the height of the upright image
		UINT8* pBuffer, 
		int bufferSize,
		int bytesPerCol,
		int bytesPerRow,
		bool use_skin = false
		);

	void Run(DETECTOR *detector, 
		DETECTOR *scaled_detector, 
		UINT8 *pYData, 
		BoxList *boxList,
		bool use_skin=false);

	void Run(DETECTOR	*detector, 
		DETECTOR	*scaled_detector,
		STAGE_STATS *view_stage_stats,
		UINT8		min_certainty,
		UINT8		*pData, 
		BoxList		*face_list,
		bool		use_skin = false);

	void RunVideo(DETECTOR	*detector,
		DETECTOR		*scaled_detector,
		UINT8						*pData,
		UINT8						frame_index,			// index of this frame in the list of frames
		UINT8						stagger_level,			// amount of stagger in the staggered grid (0==none,1=2x2,2=4x4,3=6x6)
		BoxList					*last_face_list,		// list of faces from the last frame
		BoxList					*face_list,				// output list of face bounding boxes
		bool						use_skin=false);

	
	void RunVideo(DETECTOR		*detector, 
		DETECTOR		*scaled_detector,
		STAGE_STATS	*view_stage_stats,
		UINT8		min_certainty,
		UINT8		*pData, 
		UINT8		frame_index,			// index of this frame in the list of frames
		UINT8		stagger_level,			// amount of stagger in the staggered grid (0==none,1=2x2,2=4x4,3=6x6)
		BoxList		*lastBoxList,		// list of faces from the last frame
		BoxList		*face_list,
		bool			use_skin=false);

protected:

	// this member variable is set by SetMinFaceSide
	UINT32		  m_min_face_side;	// minimum size of the faces sought

   //these member variables are (re)set by Reset
	int           m_width0;				// width of input image
   int           m_height0;			// height of input image
   int           m_width30;			// width of input image (30 degree rotated)
   int           m_height30;			// height of input image (30 degree rotated)
   int           m_width330;			// width of input image (330 degree rotated)
   int           m_height330;			// height of input image (330 degree rotated)
   int           m_bytesPerCol;		// byte offset to move 1 input column
   int           m_bytesPerRow;		// byte offset to move 1 input row
	IImage		  m_iimageSkin;		// skin integral image 
   IImage        m_iimage0;			// upright integral image
   IImage        m_iimageSqr0;		// upright squared integral image
   IImage        m_iimage30;			// 30 degree integral image
   IImage        m_iimage330;			// 330 degree integral image
};

/*
// some test methods to do with image rotation
void Warp(unsigned char *imin,  // pointer to input data
                int hin, int win,     // height and width of input
					 int bytesPerCol,
					 int bytesPerRow,
					 int degrees,
                unsigned char *imout, // pointer to output data
                int hout, int wout    // height and width of output
					 );

#include "ImageUINT8.h"
ImageUINT8* Warp(ImageUINT8* imin, int degree);
*/

#endif
