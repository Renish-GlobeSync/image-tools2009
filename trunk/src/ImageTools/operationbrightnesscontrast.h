/*
 *  brightnesscontrast.h
 *  test2
 *
 *  Created by SUKEY on 09-3-5.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

// �������ȡ����Ͷȵ����ò����ṹ
typedef struct {
    // ���ȵ�������[-1, 1]�������������ȣ�������������
	double             brightness;
    // ���Ͷȵ�������[-1, 1]���������ٱ��Ͷȣ��������ӱ��Ͷ�
	double             contrast;
} brightness_contrast_config;

/** 
    �������ȡ����Ͷ�
    
    @param in_buf ����ͼ���������飬ARGB��32bpp��ʽ
    @param out_buf ���ͼ���������飬ARGB��32bpp��ʽ
    @param samples ������Ŀ
    @param config ���ò���
    
    @return ����ֵ���ɹ����
 */
bool operation_brightness_contrast (void * in_buf, void * out_buf, long samples, const brightness_contrast_config * config);
