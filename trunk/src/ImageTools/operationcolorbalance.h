/*
 *  colorbalance.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

// 色彩平衡的配置参数结构
typedef struct {
    // 是否保持亮度
	bool		preserve_luminosity;
	// 红色调整量，3个数字分别是背景
	double		cyan_red[3];
	double		magenta_green[3];
	double		yellow_blue[3];

} color_balance_config;

/** 
    色彩平衡
    
    @param in_buf 输入图像像素数组，ARGB，32bpp格式
    @param out_buf 输出图像像素数组，ARGB，32bpp格式
    @param samples 像素数目
    @param config 配置参数
    
    @return 布尔值，成功与否
 */
bool operation_color_balance(void * in_buf, void * out_buf, long samples, const color_balance_config * config);