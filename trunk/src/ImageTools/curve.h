
#ifndef __CURVE_H__
#define __CURVE_H__

#include "vector.h"

// 曲线描述结构
typedef struct {
    /** 
        曲线类型
        CURVE_SMOOTH 平滑曲线
        CURVE_FREE 手工绘制
     */
	CurveType curve_type;
	// 控制点数目，控制点是曲线经过的某几个固定点，0.0 <= x, y <= 1.0
	int       n_points;
	// 控制点数组
	Vector2   *points;
	// 样本点数目，样本点是根据控制点计算生成的x轴均匀分布的曲线上的点
	int       n_samples;
	// 样板点的y值，x值是均匀的，所以不需要保存x值
	double    *samples;
	// 是否x-y映射是相等映射，即y=x
	bool      identity;
} Curve;

void        curve_init              (Curve     *curve);

void        curve_finalize          (Curve     *curve);

void        curve_reset             (Curve     *curve,
									 bool      reset_type);

void        curve_set_curve_type    (Curve     *curve,
									 CurveType curve_type);
CurveType   curve_get_curve_type    (Curve     *curve);

void        curve_set_n_points      (Curve *curve,
                                     int       n_points);
                                     
void        curve_set_n_samples     (Curve *curve,
                                     int       n_samples);
                                     
int         curve_get_n_points      (Curve     *curve);
int         curve_get_n_samples     (Curve     *curve);

int         curve_get_closest_point (Curve     *curve,
									 double    x);

void        curve_set_point         (Curve     *curve,
									 int       point,
									 double    x,
									 double    y);
void        curve_move_point        (Curve     *curve,
									 int       point,
									 double    y);
void        curve_get_point         (Curve     *curve,
									 int       point,
									 double    *x,
									 double    *y);

void        curve_set_curve         (Curve     *curve,
									 double    x,
									 double    y);

bool        curve_is_identity       (Curve     *curve);

void        curve_get_uchar         (Curve         *curve,
									 int           n_samples,
									 unsigned char *samples);

void        curve_calculate         (Curve     *curve);
#endif /* __CURVE_H__ */
