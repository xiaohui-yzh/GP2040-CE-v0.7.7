/*
 * Copyright(C) 2024 ZhuiGuang Elec. All rights reserved.
 */
/*
 * ButtonBreath.cpp
 * Original Author: yaozhihui 1009764538@qq.com, 2024年6月3日
 *
 * 
 *
 * History
 *  v1.0    yaozhihui 1009764538@qq.com 2024年6月3日
 *          创建初稿
 */

#include "ButtonBreath.hpp"

#include "pico/rand.h"

ButtonBreath::ButtonBreath(PixelMatrix &matrix) : Animation(matrix) {
}

void ButtonBreath::Animate(RGB (&frame)[100]) {
    bool state = button_stat;
    static bool state_pre = 0;
    bool press = 0;

    if (state_pre == 0 && state == 1) {
        press = 1;
    } else {
        press = 0;
    }

    if (state_pre != state) {
        state_pre = state;
    }

    /* 未超时 且 没有按键按下 直接返回 */
    if (!time_reached(this->nextRunTime) && press == 0) {
        return;
    }

    if (state == 1) {
        bright = 1;
    } else {
        if (bright <= 0) {
            goto end;
        }
        bright -= 0.02;
        if (bright < 0) {
            bright = 0;
        }
    }

    if (press == 1) {
        this->color_pos += 1;
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

end:
    this->nextRunTime = make_timeout_time_ms(45);
}

void ButtonBreath::ParameterUp() {
}

void ButtonBreath::ParameterDown() {
}
