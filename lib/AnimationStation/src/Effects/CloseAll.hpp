/*
 * Copyright(C) 2024 ZhuiGuang Elec. All rights reserved.
 */
/*
 * CloseAll.hpp
 * Original Author: yaozhihui 1009764538@qq.com, 2024年6月25日
 *
 * 
 *
 * History
 *  v1.0    yaozhihui 1009764538@qq.com 2024年6月25日
 *          创建初稿
 */
#ifndef LIB_ANIMATIONSTATION_SRC_EFFECTS_CLOSEALL_HPP_
#define LIB_ANIMATIONSTATION_SRC_EFFECTS_CLOSEALL_HPP_

#include "../Animation.hpp"
#include "hardware/clocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../AnimationStation.hpp"

class CloseAll : public Animation {
public:
    CloseAll(PixelMatrix &matrix);
    ~CloseAll() {};

    void Animate(RGB (&frame)[100]);
    void ParameterUp();
    void ParameterDown();

protected:
    bool first = true;
};

#endif /* LIB_ANIMATIONSTATION_SRC_EFFECTS_CLOSEALL_HPP_ */
