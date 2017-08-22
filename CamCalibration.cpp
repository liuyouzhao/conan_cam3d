#include "CamCalibration.h"

CamCalibration::CamCalibration()
{

}

void CamCalibration::GetCalibPoints( char* Dir,
                                     int ImgTo,
                                     Size PSize,
                                     float Square,
                                     vector< vector<Point3f> > &Obp,
                                     vector< vector<Point2f> > &IPL,
                                     vector< vector<Point2f> > &IPR )
{
    char Filename[100];

    for( int ImgNum = 0; ImgNum < ImgTo; ImgNum++ )
    {
        sprintf(Filename, "%s%d%s", Dir, ImgNum, ".bmp");

        Mat Img (1280, 480, CV_8UC3 );
        Mat ImgCL( 640, 480, CV_8UC3 );
        Mat ImgCR( 640, 480, CV_8UC3 );

        Mat GrayImgCL, GrayImgCR;

        Img   = imread(Filename, 1);
        ImgCL = Img( Range::all(), Range(0, 640) );
        ImgCR = Img( Range::all(), Range(640, 1280) );

        if (ImgCL.rows != ImgCR.rows || ImgCL.cols != ImgCR.cols)
        {
            printf(" Number %d ImgNum Left and right images don't have the same size ! ", ImgNum);
        }

        else
        {
            // Two Images from RGB space to Gray Image
            cvtColor(ImgCL, GrayImgCL, CV_BGR2GRAY);
            cvtColor(ImgCR, GrayImgCR, CV_BGR2GRAY);

            vector<Point2f>& cornersL = IPL[ImgNum];
            vector<Point2f>& cornersR = IPR[ImgNum];

            // Find Chessboard corners from left camera
            bool patternfound1 = findChessboardCorners( GrayImgCL, PSize, cornersL,
                                                        CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE );
            if ( patternfound1 )
            {
                cornerSubPix( GrayImgCL, cornersL, Size(11, 11), Size(-1, -1),
                              TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
                              30, 0.01));

                drawChessboardCorners( ImgCL, Size(9,6), cornersL, patternfound1 );
            }

            else
            {
                printf(" Number %d ImgNum Left image can't find chessboard corners ! ", ImgNum);
            }

            // Record the Object points in 3D Space
            /* Assume that all the detected 3D points are in the Z = 0 Plane */
            int i, ii;

            for ( i = 0; i < PSize.height; i++ )
            {
                for ( ii = 0; ii < PSize.width; ii++ )
                {
                    Obp[ImgNum].push_back( Point3f(ii*Square, i*Square, 0) );
                }
            }

            // Find Chessboard corners from right camera
            bool patternfound2 = findChessboardCorners( GrayImgCR, PSize, cornersR,
                                                        CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE );
            if ( patternfound2 )
            {
               cornerSubPix( GrayImgCR, cornersR, Size(11, 11), Size(-1, -1),
                             TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
                             30, 0.01));

               drawChessboardCorners(ImgCR, Size(9,6), cornersR, patternfound2);
            }

            else
            {
                printf(" Number %d ImgNum right image can't find chessboard corners ! ", ImgNum);
            }

            /*imshow("L", ImgCL);
            imshow("R", ImgCR);
            waitKey(0);*/

        }
    }

    cout << "Obtaining chessboard corners finished !" << endl << endl;

}

void CamCalibration::BinocularCalib( vector< vector<Point3f> > Obp,
                                     vector< vector<Point2f> > IPL,
                                     vector< vector<Point2f> > IPR,
                                     Size Imgsize,
                                     Mat &m11,
                                     Mat &m12,
                                     Mat &m21,
                                     Mat &m22,
                                     Mat &q )
{
    Mat CamMatL = Mat::eye(3, 3, CV_64F);
    Mat CamMatR = Mat::eye(3, 3, CV_64F);

    Mat discoeffL, discoeffR;
    vector< vector<double> > rvecsL, tvecsL;
    vector< vector<double> > rvecsR, tvecsR;

    // Respectively calibrate left and right camera
    calibrateCamera(Obp, IPL, Imgsize, CamMatL, discoeffL, rvecsL, tvecsL);
    calibrateCamera(Obp, IPR, Imgsize, CamMatR, discoeffR, rvecsR, tvecsR);

    cout << "Left Camera Intrinsic = "  << endl << CamMatL << endl << endl
         << "Right Camera Intrinsic = " << endl << CamMatR << endl << endl ;

    // Stereo calibration of binocular system
    Mat R, T, E, F;
    /*double rms = stereoCalibrate ( Obp, IPL, IPR,
                                   CamMatL, discoeffL,
                                   CamMatR, discoeffR,
                                   Imgsize, R, T, E, F,
                                   CALIB_USE_INTRINSIC_GUESS,
                                   cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 1e-5) );*/

    double rms = stereoCalibrate ( Obp, IPL, IPR,
                                   CamMatL, discoeffL,
                                   CamMatR, discoeffR,
                                   Imgsize, R, T, E, F,
                                   TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 1e-6),
                                   CV_CALIB_FIX_INTRINSIC );

    cout << "Stereo Left Camera Intrinsic = " << endl << CamMatL << endl << endl
         << "Stereo Right Camera Intrinsic = " << endl << CamMatR << endl << endl
         << "Left distrotion = " << endl << discoeffL << endl << endl
         << "Right distrotion = " << endl << discoeffR << endl << endl;

    Mat R1, R2, P1, P2;
    Rect validRoiL, validRoiR;
    stereoRectify ( CamMatL, discoeffL,
                    CamMatR, discoeffR,
                    Imgsize, R, T, R1, R2, P1, P2, q,
                    CALIB_ZERO_DISPARITY, 0, Imgsize, &validRoiL, &validRoiR);

    cout << "rms = " << rms << endl << endl
         << "R = "   << endl << R << endl << endl
         << "T = "   << endl << T << endl << endl
         << "Q = "   << endl << q << endl << endl
         << validRoiL << endl << endl
         << validRoiR << endl << endl;

    initUndistortRectifyMap(CamMatL, discoeffL, R1, P1, Imgsize, CV_32FC1, m11, m12);
    initUndistortRectifyMap(CamMatR, discoeffR, R2, P2, Imgsize, CV_32FC1, m21, m22);

    cout << "Camera retification process finished !" << endl;

}
