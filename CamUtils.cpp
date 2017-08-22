#include "CamUtils.h"

CamUtils::CamUtils()
{

}


Mat CamUtils::gray2Color(Mat src)
{
    int nr = src.rows;
    int nc = src.cols;

    Mat srcColor(src.size(), CV_8UC3);

    if( src.isContinuous() )
    {
        nr = 1;
        nc = nc * src.rows;
    }

    for( int i = 0; i < nr; i++ )
    {
        uchar* inData  = src.ptr<uchar>(i);
        uchar* outData = srcColor.ptr<uchar>(i);

        for ( int j = 0; j < nc; j++ )
        {
            if ( inData[j] == 0 )
            {
                outData[3*j] = 0 ;
                outData[3*j + 1] = 0;
                outData[3*j + 2] = 0;
            }
            else if( inData[j] <= 51 )
            {
                outData[3*j] = 255 ;
                outData[3*j + 1] = inData[j] * 5;
                outData[3*j + 2] = 0;
            }
            else if( inData[j] <= 102 )
            {
                outData[3*j] = 255 - (inData[j] - 51) * 5;
                outData[3*j + 1] = 255;
                outData[3*j + 2] = 0;
            }
            else if( inData[j] <= 153 )
            {
                outData[3*j] = 0;
                outData[3*j + 1] = 255;
                outData[3*j + 2] = (inData[j] - 102)*5;
            }
            else if( inData[j] <= 204 )
            {
                outData[3*j] = 0;
                outData[3*j + 1] = 255 - uchar(128.0*(inData[j] - 153)/51.0+0.5);
                outData[3*j + 2] = 255;
            }
            else if( inData[j] <= 255 )
            {
                outData[3*j] = 0;
                outData[3*j + 1] = 127 - uchar(127.0*(inData[j] - 204)/51.0+0.5) ;
                outData[3*j + 2] = 255;
            }
        }
    }
    return srcColor;
}
