/*
 * Copyright(C) 2024 ZhuiGuang Elec. All rights reserved.
 */
/*
 * Breath.cpp
 * Original Author: yaozhihui 1009764538@qq.com, 2024年5月29日
 *
 * 
 *
 * History
 *  v1.0    yaozhihui 1009764538@qq.com 2024年5月29日
 *          创建初稿
 */

#include "Breath.hpp"

#include "pico/rand.h"

Breath::Breath(PixelMatrix &matrix) : Animation(matrix) {
}

void Breath::Animate(RGB (&frame)[100]) {
    if (!time_reached(this->nextRunTime)) {
        return;
    }

    for (auto &col : matrix->pixels) {
        for (auto &pixel : col) {
            if (pixel.index == NO_PIXEL.index) {
                continue;
            }
            RGB color = RGB::wheel(this->color_pos);
            color.rgbbrightness(color, this->bright);
            for (auto &pos : pixel.positions) {
                frame[pos] = color;
            }
        }
    }

    if (reverse) {
        bright -= 0.02;
        if (bright < 0) {
            bright = 0;
            /* 熄灭的时候改颜色 颜色随机 */
            this->color_pos = get_rand_32() % 0x100;
            reverse = false;
        }
    } else {
        bright += 0.02;
        if (bright > 1) {
            bright = 1;
            reverse = true;
        }
    }

    this->nextRunTime = make_timeout_time_ms(45);
}

void Breath::ParameterUp() {
}

void Breath::ParameterDown() {
}
