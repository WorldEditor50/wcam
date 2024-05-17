#include "improcess.h"
#include <opencv2/imgproc.hpp>

void imp::filter(const cv::Mat &img, cv::Mat &out)
{
    /* gray */
    cv::Mat gray;
    cv::GaussianBlur(img, gray, cv::Size(3, 3), 0);
    cv::cvtColor(gray, gray, cv::COLOR_RGB2GRAY);
    /* x-grad */
    cv::Mat xGrad;
    cv::Sobel(gray, xGrad, CV_16S, 1, 0);
    cv::convertScaleAbs(xGrad, xGrad);
    /* y-grad */
    cv::Mat yGrad;
    cv::Sobel(gray, yGrad, CV_16S, 0, 1);
    cv::convertScaleAbs(yGrad, yGrad);
    /* merge gradient */
    cv::Mat grad;
    cv::addWeighted(xGrad, 0.5, yGrad, 0.5, 0, grad);
    cv::cvtColor(grad, out, cv::COLOR_GRAY2RGB);
    return;
}
