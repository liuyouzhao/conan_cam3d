#ifndef CAMDEVICE_H
#define CAMDEVICE_H

#include <OpenNI.h>

using namespace openni;

class CamDevice
{
public:
    CamDevice();
    ~CamDevice();

    static void listDevices();

    int openDevice(const char *uuid = 0);
    int closeDevice();

    int startStream(int which = 0);
    int stopStream(int which = 0);
    int stopAllStream();

    int loopRead();

    void (*onDepthDataCallback) (unsigned short *data, int width, int height);
    void (*onColorDataCallback) (unsigned char *data, int width, int height);
    void (*onIrDataCallback) (unsigned char *data, int width, int height);
private:

    static int s_count;

    Device m_device;

    VideoStream m_depthStream;
    VideoStream m_colorStream;
    VideoStream m_irStream;

    int m_mode;
};

#endif // CAMDEVICE_H
