/*
 * Copyright(C) 2024 ZhuiGuang Elec. All rights reserved.
 */
/*
 * Breath.hpp
 * Original Author: yaozhihui 1009764538@qq.com, 2024年5月29日
 *
 * 
 *
 * History
 *  v1.0    yaozhihui 1009764538@qq.com 2024年5月29日
 *          创建初稿
 */
#ifndef LIB_ANIMATIONSTATION_SRC_EFFECTS_BREATH_HPP_
#define LIB_ANIMATIONSTATION_SRC_EFFECTS_BREATH_HPP_

#include "../Animation.hpp"
#include "hardware/clocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../AnimationStation.hpp"

class Breath : public Animation {
public:
    Breath(PixelMatrix &matrix);
    ~Breath() {};

    void Animate(RGB (&frame)[100]);
    void ParameterUp();
    void ParameterDown();

protected:
    uint8_t color_pos = 0;
    float bright = 1;
    bool reverse = true;
    absolute_time_t nextRunTime = nil_time;
};

#endif /* LIB_ANIMATIONSTATION_SRC_EFFECTS_BREATH_HPP_ */
