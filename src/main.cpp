#include  <opencv2/opencv.hpp>

int main()
{
    cv::Mat image = cv::Mat::zeros(500, 500, CV_8UC3);
	cv::imwrite("output.png", image);	 
	return 0;
}