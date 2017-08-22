#ifndef CAM_UTILS_H
#define CAM_UTILS_H

#include "opencv2/opencv.hpp"

#include <stdio.h>

using namespace cv;
using namespace std;

class CamUtils
{
public:

    static Mat gray2Color( Mat src );

private:

    CamUtils();
};

#endif // CAM_UTILS_H
