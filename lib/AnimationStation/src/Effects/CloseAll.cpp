/*
 * Copyright(C) 2024 ZhuiGuang Elec. All rights reserved.
 */
/*
 * CloseAll.cpp
 * Original Author: yaozhihui 1009764538@qq.com, 2024年6月25日
 *
 * 
 *
 * History
 *  v1.0    yaozhihui 1009764538@qq.com 2024年6月25日
 *          创建初稿
 */

#include "CloseAll.hpp"

#include "pico/rand.h"

CloseAll::CloseAll(PixelMatrix &matrix) : Animation(matrix) {
}

void CloseAll::Animate(RGB (&frame)[100]) {
    if (this->first == 0) {
        return;
    }

    for (auto &col : matrix->pixels) {
        for (auto &pixel : col) {
            if (pixel.index == NO_PIXEL.index) {
                continue;
            }
            RGB color = ColorBlack;
            for (auto &pos : pixel.positions) {
                frame[pos] = color;
            }
        }
    }

    this->first = 0;
}

void CloseAll::ParameterUp() {
}

void CloseAll::ParameterDown() {
}
