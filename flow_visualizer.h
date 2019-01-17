/**
 * Copyright (c) 2019, Chunlin Li(me@chunlin.li). All rights reserved
 * @file flow_visualizer.h
 *       class declaration of FlowVisualizer
 */
#ifndef _FLOW_VISUALIZER_H_
#define _FLOW_VISUALIZER_H_

#include <string>

/**
 * @class FlowVisualizer
 * @brief Parse and display optical flow data in the format of .flo from middleburry
 */
class FlowVisualizer
{
public:
    FlowVisualizer();
    virtual ~FlowVisualizer();

    bool load(const std::string &path);

    bool show();

private:
    uint8_t  *_data;
    uint32_t _width;
    uint32_t _height;

    /* middlebury flo format header tag: "PIEH", 
     * http://vision.middlebury.edu/flow/ 
     * */
    const static uint32_t TAG_STRING = 0x48454950; 
};

#endif
