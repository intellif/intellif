#ifndef IF_GLUON_HANDLE_HPP
#define IF_GLUON_HANDLE_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef MXNET_STATIC_LIB
#ifdef GLUONWRAPPER_EXPORTS
#define MXNET_GLUON_API __declspec(dllexport)
#else
#define MXNET_GLUON_API __declspec(dllimport)
#endif
#else
#define MXNET_GLUON_API
#endif
#else
#define MXNET_GLUON_API
#endif


class Gluon;

class MXNET_GLUON_API GluonMxnetHandle {

public:

    GluonMxnetHandle(const std::string & model_path,
                           int           height,
                           int           width,
                           int           batch_size  = 1,
                           int           gpu         = -1,
                           float         exth_scale  = 0.083f,
                           float         extw_scale  = 0.0f);

    ~GluonMxnetHandle();

   int GetStatus(void);

   /**
   * extrace feature of a Mat object
   * @param img Mat object will used for feature extracted
   * @return extracted feature
   */
   int Feature(const cv::Mat &img, int x1, int y1, int x2, int y2, std::vector<float> & vecFeature);
  
   /**
   * extrace features of batch_size list of cv::Mat, using batch size to speed up,
   * especially for gpu parallel
   * @param imgs  list of cv::Mat to be extract features
   * @return extracted feature
   */
   int BatchFeature(const std::vector<cv::Mat> & imgs, std::vector<std::vector<float> > & vecFeature);

private:

    Gluon * pGluonMxnetImpl;

};


#endif //IF_GLUON_HANDLE_HPP
