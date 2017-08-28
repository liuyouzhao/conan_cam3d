#include <QCoreApplication>
#include <pthread.h>
#include "CamDevice.h"

#include "opencv2/opencv.hpp"

using namespace cv;

static unsigned char *s_data = 0;
static Mat s_col;
static Mat s_dep;
static pthread_mutex_t s_mutex;

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


    pthread_mutex_lock(&s_mutex);
    s_col = col.clone();
    pthread_mutex_unlock(&s_mutex);
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

    pthread_mutex_lock(&s_mutex);
    s_dep = dep.clone();
    pthread_mutex_unlock(&s_mutex);
}

static void onIrData(unsigned short *data, int w, int h)
{
    Mat ir ( h,
             w,
             CV_16U, data );
    Mat gray;
    ir.convertTo(gray, CV_8U);

    pthread_mutex_lock(&s_mutex);
    s_dep = Mat(h, w, CV_8UC3);
    for(int i = 0; i < h; i ++)
    {
        for(int j = 0; j < w; j ++)
        {
            int index = i * w + j;
            s_dep.data[index * 3] = gray.data[index];
            s_dep.data[index * 3 + 1] = gray.data[index];
            s_dep.data[index * 3 + 2] = gray.data[index];
        }
    }
    pthread_mutex_unlock(&s_mutex);

    cv::imshow("ir", gray);

}

static void save_calc_img()
{
    int wleft = s_dep.cols;
    int wright = s_col.cols;
    int wmat = wleft + wright;
    int height = s_col.rows;

    Mat mat = cv::Mat(height, wmat, CV_8UC3);

    for(int i = 0; i < s_dep.rows; i ++)
    {
        for(int j = 0; j < s_dep.cols; j ++)
        {
            int index1 = i * wmat + j;
            int index2 = i * wleft + j;
            mat.data[index1 * 3] = s_dep.data[index2 * 3];
            mat.data[index1 * 3 + 1] = s_dep.data[index2 * 3 + 1];
            mat.data[index1 * 3 + 2] = s_dep.data[index2 * 3 + 2];
        }
    }

    for(int i = 0; i < s_col.rows; i ++)
    {
        for(int j = 0; j < s_col.cols; j ++)
        {
            int index1 = i * wmat + (j + wleft);
            int index2 = i * wright + j;
            mat.data[index1 * 3] = s_col.data[index2 * 3];
            mat.data[index1 * 3 + 1] = s_col.data[index2 * 3 + 1];
            mat.data[index1 * 3 + 2] = s_col.data[index2 * 3 + 2];
        }
    }


    static int num = 0;
    char file[128] = {0};
    sprintf(file, "../resource/cali-%d.jpg", num);
    imwrite(file, mat);
    imshow("save", mat);
    num ++;
}

static void loop_read()
{
    /// Init mutex
    pthread_mutex_init(&s_mutex, NULL);


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

        char key = cv::waitKey(33);

        switch(key)
        {
        case 27:
            exit(0);
            break;
        case 32:
            pthread_mutex_lock(&s_mutex);
            save_calc_img();
            pthread_mutex_unlock(&s_mutex);
            break;
        }
    }
}

static void calibrate()
{

}

static void commands()
{
    char cmd[64] = {0};

    printf("Welcome to steven 3d camera program.\n");
    while(1)
    {
        memset(cmd, 0, 64);

        printf(">>");
        scanf("%s", cmd);
        if(strcmp("cap", cmd) == 0 || strcmp("capture", cmd) == 0)
        {
            loop_read();
            return;
        }
        else if(strcmp("cali", cmd) == 0)
        {

        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    commands();

    return a.exec();
}
