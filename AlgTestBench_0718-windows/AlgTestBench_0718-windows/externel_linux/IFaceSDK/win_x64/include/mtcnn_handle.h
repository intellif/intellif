#ifndef IF_MTCNN_HANDLE_HPP
#define IF_MTCNN_HANDLE_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

using std::string;

struct Bbox {
    Bbox(int   xx1, 
         int   yy1, 
         int   xx2, 
         int   yy2, 
         float box_score,
         float dxx1=0.,
         float dyy1=0., 
         float dxx2=0.,
         float dyy2=0.)
    :x1(xx1),  y1(yy1),  x2(xx2),  y2(yy2),
     score(box_score),
     dx1(dxx1),dy1(dyy1),dx2(dxx2),dy2(dyy2) 
    {

    }
  
    inline cv::Rect GetRect() const {
        return cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    }
  
    int   x1,  y1,  x2,  y2;
    float score;
    float dx1, dy1, dx2, dy2;
};

typedef std::vector<Bbox> Vbbox;

class MtcnnCaffe;

class MtcnnCaffeHandle {

public:

    MtcnnCaffeHandle(string                     strModelPath = "model",
                     int                        min_size     = -1,
                     const std::vector<float> & thresh       = {(float)0.7,(float)0.7,(float)0.8},
                     float                      factor       = 0.709, 
                     bool                       profile      = false, 
                     int                        gpu          = -1,
                     bool                       square       = false,
                     float                      exth_scale   = 0.0,
                     float                      extw_scale   = 0.0);

    ~MtcnnCaffeHandle();

    int MtcnnFaceDetect(cv::Mat mat4Detect,  Vbbox & vecFaceRect);

    static void MtcnnStaticInit(void);

private:

    MtcnnCaffe * pMtcnnCaffeImpl;
};


#endif //IF_MTCNN_HANDLE_HPP

