#ifndef IMPROCESS_H
#define IMPROCESS_H
#include <opencv2/core.hpp>

namespace imp {
    enum Method {
        Method_None = 0,
        Method_Filter
    };

    void filter(const cv::Mat &img, cv::Mat &out);
}

#endif // IMPROCESS_H
