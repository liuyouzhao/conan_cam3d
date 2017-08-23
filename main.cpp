#include <QCoreApplication>
#include "CamDevice.h"

#include "opencv2/opencv.hpp"

using namespace cv;

static unsigned char *s_data = 0;

static void onColorData(unsigned char *data, int w, int h)
{
    for(int i = 0; i < w * h; i ++)
    {
        unsigned char sav = data[i * 3];
        data[i * 3] = data[i * 3 + 2];
        data[i * 3 + 2] = sav;
    }
    Mat col( h,
             w,
             CV_8UC3, (char*)data );
    cv::imshow("col", col);
}

static void onDepthData(unsigned short *data, int w, int h)
{
    if(s_data == 0)
    {
        s_data = (unsigned char*) malloc(w * h);
    }

    int min = 99999;
    int max = 0;
    for(int i = 0; i < w * h; i ++)
    {
        if(data[i] > max)
        {
            max = data[i];
        }
        if(data[i] < min)
        {
            min = data[i];
        }
    }

    for(int i = 0; i < w * h; i ++)
    {
        if(data[i] == 0)
        {
            s_data[i] = 0;
        }
        else
        {
            s_data[i] = 255 - (data[i] - min) * 255 / (max - min);
        }
    }

    Mat dep( h,
             w,
             CV_8U, (char*)s_data);
    cv::imshow("dep", dep);
}

static void onIrData(unsigned short *data, int w, int h)
{
    Mat ir ( h,
             w,
             CV_16U, data );
    Mat gray;
    ir.convertTo(gray, CV_8U);
    cv::imshow("ir", gray);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CamDevice::listDevices();

    CamDevice cam;
    cam.openDevice();

    cam.startStream(1);

    cam.onColorDataCallback = onColorData;
    cam.onDepthDataCallback = onDepthData;
    cam.onIrDataCallback = onIrData;

    while(1)
    {
        cam.loopRead();

        int key = cv::waitKey(33);
        if(key == 27)
        {
            break;
        }

    }

    return a.exec();
}
