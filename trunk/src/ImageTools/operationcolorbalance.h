/*
 *  colorbalance.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

// ɫ��ƽ������ò����ṹ
typedef struct {
    // �Ƿ񱣳�����
	bool		preserve_luminosity;
	// ��ɫ��������3�����ֱַ��Ǳ���
	double		cyan_red[3];
	double		magenta_green[3];
	double		yellow_blue[3];

} color_balance_config;

/** 
    ɫ��ƽ��
    
    @param in_buf ����ͼ���������飬ARGB��32bpp��ʽ
    @param out_buf ���ͼ���������飬ARGB��32bpp��ʽ
    @param samples ������Ŀ
    @param config ���ò���
    
    @return ����ֵ���ɹ����
 */
bool operation_color_balance(void * in_buf, void * out_buf, long samples, const color_balance_config * config);