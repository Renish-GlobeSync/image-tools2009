/*
 *  brightnesscontrast.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

// 调整明度、饱和度的配置参数结构
typedef struct {
    // 明度调整量，[-1, 1]，负数减少明度，正数增加明度
	double             brightness;
    // 饱和度调整量，[-1, 1]，负数减少饱和度，正数增加饱和度
	double             contrast;
} brightness_contrast_config;

/** 
    调整明度、饱和度
    
    @param in_buf 输入图像像素数组，ARGB，32bpp格式
    @param out_buf 输出图像像素数组，ARGB，32bpp格式
    @param samples 像素数目
    @param config 配置参数
    
    @return 布尔值，成功与否
 */
bool operation_brightness_contrast (void * in_buf, void * out_buf, long samples, const brightness_contrast_config * config);
