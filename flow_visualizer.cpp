/**
 * Copyright (c) 2019, Chunlin Li(me@chunlin.li). All rights reserved
 * 
 * @file flow_visualizer.cpp
 *       OpenCV based optical flow visualizer implementation
 */

#include "flow_visualizer.h"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;

/**
 * The function comes from sample code companioned with opencv release
 */
static Vec3b computeColor(float fx, float fy)
{
    static bool first = true;

    // relative lengths of color transitions:
    // these are chosen based on perceptual similarity
    // (e.g. one can distinguish more shades between red and yellow
    //  than between yellow and green)
    const int RY = 15;
    const int YG = 6;
    const int GC = 4;
    const int CB = 11;
    const int BM = 13;
    const int MR = 6;
    const int NCOLS = RY + YG + GC + CB + BM + MR;
    static Vec3i colorWheel[NCOLS];

    if (first)
    {
        int k = 0;

        for (int i = 0; i < RY; ++i, ++k)
            colorWheel[k] = Vec3i(255, 255 * i / RY, 0);

        for (int i = 0; i < YG; ++i, ++k)
            colorWheel[k] = Vec3i(255 - 255 * i / YG, 255, 0);

        for (int i = 0; i < GC; ++i, ++k)
            colorWheel[k] = Vec3i(0, 255, 255 * i / GC);

        for (int i = 0; i < CB; ++i, ++k)
            colorWheel[k] = Vec3i(0, 255 - 255 * i / CB, 255);

        for (int i = 0; i < BM; ++i, ++k)
            colorWheel[k] = Vec3i(255 * i / BM, 0, 255);

        for (int i = 0; i < MR; ++i, ++k)
            colorWheel[k] = Vec3i(255, 0, 255 - 255 * i / MR);

        first = false;
    }

    const float rad = sqrt(fx * fx + fy * fy);
    const float a = atan2(-fy, -fx) / (float)CV_PI;

    const float fk = (a + 1.0f) / 2.0f * (NCOLS - 1);
    const int k0 = static_cast<int>(fk);
    const int k1 = (k0 + 1) % NCOLS;
    const float f = fk - k0;

    Vec3b pix;

    for (int b = 0; b < 3; b++)
    {
        const float col0 = colorWheel[k0][b] / 255.0f;
        const float col1 = colorWheel[k1][b] / 255.0f;

        float col = (1 - f) * col0 + f * col1;

        if (rad <= 1)
            col = 1 - rad * (1 - col); // increase saturation with radius
        else
            col *= .75; // out of range

        pix[2 - b] = static_cast<uchar>(255.0 * col);
    }

    return pix;
}

FlowVisualizer::FlowVisualizer()
:_data(NULL), _width(0), _height(0)
{}

FlowVisualizer::~FlowVisualizer()
{
    if (_data) delete[] _data;
}

bool FlowVisualizer::load(const std::string &path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) return false;

    uint32_t tag;
    file.read((char*) &tag, sizeof(uint32_t));
    if (tag != TAG_STRING) return false;

    uint32_t width, height;
    file.read((char *)&width, sizeof(uint32_t));
    file.read((char *)&height, sizeof(uint32_t));
    if (!file) return false;

    uint32_t size = sizeof(float) * 2 * width * height;
    char* data = new char[size];
    file.read((char *)data, size);
    if (!file)
    {
        delete [] data;
        return false;
    } 

    /* Only give an update on flow information after loading is successfully done */
    _width = width;
    _height = height;
    if (_data) delete[] _data; /*< clear the data from last load */
    _data = (uint8_t*) data;

    return true;
}

bool FlowVisualizer::show()
{
    if (!_width || !_height || !_data) return false;

    Mat cvflow(_height, _width, CV_32FC2, _data);
    Mat planes[2];
    split(cvflow, planes);
    Mat_<float> planex = planes[0];
    Mat_<float> planey = planes[1];
    Mat cvrgb(_height, _width, CV_8UC3);

    cvrgb.setTo(Scalar::all(0));

    // determine motion range:
    float maxrad = 1.0;
    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            Point2f u(planex(y, x), planey(y, x));

            if (cvIsNaN(u.x) || cvIsNaN(u.y) || fabs(u.x) >= 1e9 || fabs(u.y) >= 1e9)
                continue;

            maxrad = max(maxrad, u.x * u.x + u.y * u.y);
        }
    }

    maxrad = sqrt(maxrad);
    
    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            Point2f u(planex(y, x), planey(y, x));

            if (cvIsNaN(u.x) || cvIsNaN(u.y) || fabs(u.x) >= 1e9 || fabs(u.y) >= 1e9)
                continue;

            cvrgb.at<Vec3b>(y, x) = computeColor(u.x / maxrad, u.y / maxrad);
        }
    }

    imshow("window", cvrgb);
    waitKey(0);
}
