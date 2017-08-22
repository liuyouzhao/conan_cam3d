#include <QCoreApplication>
#include "CamDevice.h"

#include "opencv2/opencv.hpp"

static void onColorData(unsigned char *data, int w, int h)
{
    printf("on Color Data: %d %d\n", w, h);
}

static void onDepthData(unsigned short *data, int w, int h)
{
    printf("on Depth Data: %d %d\n", w, h);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CamDevice::listDevices();

    CamDevice cam;
    cam.openDevice();

    cam.startStream();

    cam.onColorDataCallback = onColorData;
    cam.onDepthDataCallback = onDepthData;

    while(1)
        cam.loopRead();

    return a.exec();
}
