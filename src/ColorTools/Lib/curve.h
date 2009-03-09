
#ifndef __CURVE_H__
#define __CURVE_H__

#include "vector.h"


typedef struct {
	CurveType curve_type;

	int       n_points;

	Vector2   *points;
	
	int       n_samples;
	
	double    *samples;
	
	bool      identity;
} Curve;

void        curve_init              (Curve     *curve);

void        curve_finalize          (Curve     *curve);

void        curve_reset             (Curve     *curve,
									 bool      reset_type);

void        curve_dirty             (Curve     *curve);

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
#endif 
