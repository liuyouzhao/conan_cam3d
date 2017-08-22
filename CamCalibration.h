#ifndef CAM_CALIBRATION_H
#define CAM_CALIBRATION_H

#include <vector>
#include "CamUtils.h"

class CamCalibration
{
public:

    static void GetCalibPoints( char* Dir,
                                int ImgTo,
                                Size PSize,
                                float Square,
                                vector< vector<Point3f> > &Obp,
                                vector< vector<Point2f> > &IPL,
                                vector< vector<Point2f> > &IPR );

    static void BinocularCalib( vector< vector<Point3f> > Obp,
                                vector< vector<Point2f> > IPL,
                                vector< vector<Point2f> > IPR,
                                Size Imgsize,
                                Mat &m11,
                                Mat &m12,
                                Mat &m21,
                                Mat &m22,
                                Mat &q );
private:
    CamCalibration();
};

#endif // CAM_CALIBRATION_H
