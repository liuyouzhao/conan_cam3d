#include "CamDevice.h"

int CamDevice::s_count = 0;

CamDevice::CamDevice()
{
    if(s_count == 0)
    {
        int rc = OpenNI::initialize();
        if (rc != STATUS_OK)
        {
            printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
            return;
        }
        s_count ++;
    }
    m_mode = -1;
}

CamDevice::~CamDevice()
{
    s_count --;
    if(s_count == 0)
    {
        OpenNI::shutdown();
    }
}

void CamDevice::listDevices()
{
    int rc = OpenNI::initialize();
    if (rc != STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return;
    }
    Array<DeviceInfo> devices;
    OpenNI::enumerateDevices(&devices);

    for(int i = 0; i < devices.getSize(); i ++)
    {
        printf("Device[%d] %s \n", devices[i].getName());
    }
    OpenNI::shutdown();
}

int CamDevice::startStream(int which)
{
    int rc = 0;
    m_mode = which;
    switch(which)
    {
    case 0: /// for both depth and color
    {
        if (m_device.getSensorInfo(SENSOR_DEPTH) != NULL)
        {
            rc = m_depthStream.create(m_device, SENSOR_DEPTH);
            if (rc == STATUS_OK)
            {
                rc = m_depthStream.start();
                if (rc != STATUS_OK)
                {
                    printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
                    return -1;
                }
            }
            else
            {
                printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
                return -1;
            }
        }
        if (m_device.getSensorInfo(SENSOR_COLOR) != NULL)
        {
            rc = m_colorStream.create(m_device, SENSOR_COLOR);
            if (rc == STATUS_OK)
            {
                rc = m_colorStream.start();
                if (rc != STATUS_OK)
                {
                    printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
                }
            }
            else
            {
                printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
            }
        }
        return 0;
    } break;
    case 1: /// for calibration mode
    {
        if (m_device.getSensorInfo(SENSOR_IR) != NULL)
        {
            rc = m_irStream.create(m_device, SENSOR_IR);
            if (rc == STATUS_OK)
            {
                rc = m_irStream.start();
                if (rc != STATUS_OK)
                {
                    printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
                    return -1;
                }
            }
            else
            {
                printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
                return -1;
            }
        }
        if (m_device.getSensorInfo(SENSOR_COLOR) != NULL)
        {
            rc = m_colorStream.create(m_device, SENSOR_COLOR);
            if (rc == STATUS_OK)
            {
                rc = m_colorStream.start();
                if (rc != STATUS_OK)
                {
                    printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
                    return -1;
                }
            }
            else
            {
                printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
                return -1;
            }
        }
        return 0;
    }
    }
    return 2;

}

int CamDevice::stopStream(int which)
{
    switch(which)
    {
    case 0: /// dep and col
        m_depthStream.stop();
        m_colorStream.stop();
        break;
    case 1: /// cali
        m_colorStream.stop();
        m_irStream.stop();
        break;
    }
}

int CamDevice::stopAllStream()
{
    m_depthStream.stop();
    m_colorStream.stop();
    m_irStream.stop();
}

int CamDevice::openDevice(const char *uuid)
{
    int rc = 0;
    if(uuid != 0)
    {
        rc = m_device.open(uuid);
    }
    else
    {
        rc = m_device.open(ANY_DEVICE);
    }
    return rc;
}

int CamDevice::closeDevice()
{
    m_device.close();
}

int CamDevice::loopRead()
{
    int rc = 0;

    VideoFrameRef frame1;
    VideoFrameRef frame2;
    if(m_mode == 0)
    {
        m_depthStream.readFrame(&frame1);
        m_colorStream.readFrame(&frame2);

        if(onDepthDataCallback)
            onDepthDataCallback((unsigned short*)(frame1.getData()), frame1.getWidth(), frame1.getHeight());

        if(onColorDataCallback)
            onColorDataCallback((unsigned char*)(frame2.getData()), frame2.getWidth(), frame2.getHeight());
    }
    else if(m_mode == 1)
    {
        m_irStream.readFrame(&frame1);
        m_colorStream.readFrame(&frame2);

        if(onIrDataCallback)
            onIrDataCallback((unsigned char*)(frame1.getData()), frame1.getWidth(), frame1.getHeight());

        if(onColorDataCallback)
            onColorDataCallback((unsigned char*)(frame2.getData()), frame2.getWidth(), frame2.getHeight());
    }
    else
    {
        return -1;
    }
}
