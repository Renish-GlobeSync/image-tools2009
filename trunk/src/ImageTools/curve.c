
#include "color.h"
#include "math1.h"
#include "base_enums.h"

#include "curve.h"

#include <stdlib.h>

enum
{
	PROP_0,
	PROP_CURVE_TYPE,
	PROP_N_POINTS,
	PROP_POINTS,
	PROP_N_SAMPLES,
	PROP_SAMPLES
};

static void
curve_plot (Curve *curve,
			int       p1,
			int       p2,
			int       p3,
			int       p4);

/*  public functions  */
void
curve_reset (Curve *curve,
			 bool   reset_type)
{
	int i;
		
	for (i = 0; i < curve->n_samples; i++)
		curve->samples[i] = (double) i / (double) (curve->n_samples - 1);
		
	curve->points[0].x = 0.0;
	curve->points[0].y = 0.0;
	
	for (i = 1; i < curve->n_points - 1; i++)
    {
		curve->points[i].x = -1.0;
		curve->points[i].y = -1.0;
    }
	
	curve->points[curve->n_points - 1].x = 1.0;
	curve->points[curve->n_points - 1].y = 1.0;
	
	if (reset_type)
    {
		curve->curve_type = CURVE_SMOOTH;
    }
	
	curve->identity = TRUE;
}

void
curve_set_curve_type (Curve     *curve,
					  CurveType  curve_type)
{
	if (curve->curve_type != curve_type)
    {
		curve->curve_type = curve_type;
		
		if (curve_type == CURVE_SMOOTH)
        {
			int n_points;
			int i;
			
			for (i = 0; i < curve->n_points; i++)
            {
				curve->points[i].x = -1;
				curve->points[i].y = -1;
            }
			
			/*  pick some points from the curve and make them control
			 *  points
			 */
			n_points = CLAMP (9, curve->n_points / 2, curve->n_points);
			
			for (i = 0; i < n_points; i++)
            {
				int sample = i * (curve->n_samples - 1) / (n_points - 1);
				int point  = i * (curve->n_points  - 1) / (n_points - 1);
				
				curve->points[point].x = ((double) sample /
										  (double) (curve->n_samples - 1));
				curve->points[point].y = curve->samples[sample];
            }
        }
    }
}

CurveType
curve_get_curve_type (Curve *curve)
{
	return curve->curve_type;
}

static void
curve_set_n_points (Curve *curve,
					int       n_points)
{
	if (n_points != curve->n_points)
    {
		int i;
		
		curve->n_points = n_points;
		
		free(curve->points);
		curve->points = malloc(sizeof(curve->points[0]) * curve->n_points);
		
		curve->points[0].x = 0.0;
		curve->points[0].y = 0.0;
		
		for (i = 1; i < curve->n_points - 1; i++)
        {
			curve->points[i].x = -1.0;
			curve->points[i].y = -1.0;
        }
		
		curve->points[curve->n_points - 1].x = 1.0;
		curve->points[curve->n_points - 1].y = 1.0;
		
		if (curve->curve_type == CURVE_SMOOTH)
			curve->identity = TRUE;
    }
}

int
curve_get_n_points (Curve *curve)
{
	return curve->n_points;
}

static void
curve_set_n_samples (Curve *curve,
					 int       n_samples)
{
	if (n_samples != curve->n_samples)
    {
		int i;
		
		curve->n_samples = n_samples;

		free(curve->samples);
		curve->samples = malloc(sizeof(curve->samples[0]) * curve->n_samples);
		
		for (i = 0; i < curve->n_samples; i++)
			curve->samples[i] = (double) i / (double) (curve->n_samples - 1);
				
		if (curve->curve_type == CURVE_FREE)
			curve->identity = TRUE;
    }
}

int
curve_get_n_samples (Curve *curve)
{
	return curve->n_samples;
}

int
curve_get_closest_point (Curve *curve,
						 double    x)
{
	int    closest_point = 0;
	double distance      = DBL_MAX;
	int    i;
	
	for (i = 0; i < curve->n_points; i++)
    {
		if (curve->points[i].x >= 0.0 &&
			fabs (x - curve->points[i].x) < distance)
        {
			distance = fabs (x - curve->points[i].x);
			closest_point = i;
        }
    }
	
	if (distance > (1.0 / (curve->n_points * 2.0)))
		closest_point = ROUND (x * (double) (curve->n_points - 1));
	
	return closest_point;
}

void
curve_set_point (Curve *curve,
				 int       point,
				 double    x,
				 double    y)
{
	if (!(point >= 0 && point < curve->n_points))
		return;
	if (!(x == -1.0 || (x >= 0 && x <= 1.0)))
		return;
	if (!(y == -1.0 || (y >= 0 && y <= 1.0)))
		return;
	
	if (curve->curve_type == CURVE_FREE)
		return;
		
	curve->points[point].x = x;
	curve->points[point].y = y;
}

void
curve_move_point (Curve *curve,
				  int       point,
				  double    y)
{
	if (!(point >= 0 && point < curve->n_points))
		return;
	if (!(y >= 0 && y <= 1.0))
		return;
	
	if (curve->curve_type == CURVE_FREE)
		return;
	
	curve->points[point].y = y;
}

void
curve_get_point (Curve *curve,
				 int       point,
				 double   *x,
				 double   *y)
{
	if (!(point >= 0 && point < curve->n_points))
		return;
	
	if (curve->curve_type == CURVE_FREE)
		return;
	
	if (x) *x = curve->points[point].x;
	if (y) *y = curve->points[point].y;
}

void
curve_set_curve (Curve *curve,
				 double    x,
				 double    y)
{
	if (!(x >= 0 && x <= 1.0))
		return;
	if (!(y >= 0 && y <= 1.0))
		return;
	
	if (curve->curve_type == CURVE_SMOOTH)
		return;
	
	curve->samples[ROUND (x * (double) (curve->n_samples - 1))] = y;
}

/**
 * curve_is_identity:
 * @curve: a #Curve object
 *
 * If this function returns %TRUE, then the curve maps each value to
 * itself. If it returns %FALSE, then this assumption can not be made.
 *
 * Return value: %TRUE if the curve is an identity mapping, %FALSE otherwise.
 **/
bool
curve_is_identity (Curve *curve)
{
	return curve->identity;
}

void
curve_get_uchar (Curve *curve,
				 int       n_samples,
				 unsigned char    *samples)
{
	int i;
	
	if (n_samples != curve->n_samples)
		return;
	if (samples == NULL)
		return;
	
	for (i = 0; i < curve->n_samples; i++)
		samples[i] = curve->samples[i] * 255.999;
}


/*  private functions  */

static void
curve_calculate (Curve *curve)
{
	int *points;
	int  i;
	int  num_pts;
	int  p1, p2, p3, p4;
	
	points = malloc(sizeof(points[0]) * curve->n_points);
	
	switch (curve->curve_type)
    {
		case CURVE_SMOOTH:
			/*  cycle through the curves  */
			num_pts = 0;
			for (i = 0; i < curve->n_points; i++)
				if (curve->points[i].x >= 0.0)
					points[num_pts++] = i;
			
			/*  Initialize boundary curve points */
			if (num_pts != 0)
			{
				Vector2 point;
				int        boundary;
				
				point    = curve->points[points[0]];
				boundary = ROUND (point.x * (double) (curve->n_samples - 1));
				
				for (i = 0; i < boundary; i++)
					curve->samples[i] = point.y;
				
				point    = curve->points[points[num_pts - 1]];
				boundary = ROUND (point.x * (double) (curve->n_samples - 1));
				
				for (i = boundary; i < curve->n_samples; i++)
					curve->samples[i] = point.y;
			}
			
			for (i = 0; i < num_pts - 1; i++)
			{
				p1 = points[MAX (i - 1, 0)];
				p2 = points[i];
				p3 = points[i + 1];
				p4 = points[MIN (i + 2, num_pts - 1)];
				
				curve_plot (curve, p1, p2, p3, p4);
			}
			
			/* ensure that the control points are used exactly */
			for (i = 0; i < num_pts; i++)
			{
				double x = curve->points[points[i]].x;
				double y = curve->points[points[i]].y;
				
				curve->samples[ROUND (x * (double) (curve->n_samples - 1))] = y;
			}
			break;
			
		case CURVE_FREE:
			break;
    }
	free(points);
}

/*
 * This function calculates the curve values between the control points
 * p2 and p3, taking the potentially existing neighbors p1 and p4 into
 * account.
 *
 * This function uses a cubic bezier curve for the individual segments and
 * calculates the necessary intermediate control points depending on the
 * neighbor curve control points.
 */
static void
curve_plot (Curve *curve,
			int       p1,
			int       p2,
			int       p3,
			int       p4)
{
	int    i;
	double x0, x3;
	double y0, y1, y2, y3;
	double dx, dy;
	double slope;
	
	/* the outer control points for the bezier curve. */
	x0 = curve->points[p2].x;
	y0 = curve->points[p2].y;
	x3 = curve->points[p3].x;
	y3 = curve->points[p3].y;
	
	/*
	 * the x values of the inner control points are fixed at
	 * x1 = 2/3*x0 + 1/3*x3   and  x2 = 1/3*x0 + 2/3*x3
	 * this ensures that the x values increase linearily with the
	 * parameter t and enables us to skip the calculation of the x
	 * values altogehter - just calculate y(t) evenly spaced.
	 */
	
	dx = x3 - x0;
	dy = y3 - y0;
	
	if (dx <= 0)
		return;
	
	if (p1 == p2 && p3 == p4)
    {
		/* No information about the neighbors,
		 * calculate y1 and y2 to get a straight line
		 */
		y1 = y0 + dy / 3.0;
		y2 = y0 + dy * 2.0 / 3.0;
    }
	else if (p1 == p2 && p3 != p4)
    {
		/* only the right neighbor is available. Make the tangent at the
		 * right endpoint parallel to the line between the left endpoint
		 * and the right neighbor. Then point the tangent at the left towards
		 * the control handle of the right tangent, to ensure that the curve
		 * does not have an inflection point.
		 */
		slope = (curve->points[p4].y - y0) / (curve->points[p4].x - x0);
		
		y2 = y3 - slope * dx / 3.0;
		y1 = y0 + (y2 - y0) / 2.0;
    }
	else if (p1 != p2 && p3 == p4)
    {
		/* see previous case */
		slope = (y3 - curve->points[p1].y) / (x3 - curve->points[p1].x);
		
		y1 = y0 + slope * dx / 3.0;
		y2 = y3 + (y1 - y3) / 2.0;
    }
	else /* (p1 != p2 && p3 != p4) */
    {
		/* Both neighbors are available. Make the tangents at the endpoints
		 * parallel to the line between the opposite endpoint and the adjacent
		 * neighbor.
		 */
		slope = (y3 - curve->points[p1].y) / (x3 - curve->points[p1].x);
		
		y1 = y0 + slope * dx / 3.0;
		
		slope = (curve->points[p4].y - y0) / (curve->points[p4].x - x0);
		
		y2 = y3 - slope * dx / 3.0;
    }
	
	/*
	 * finally calculate the y(t) values for the given bezier values. We can
	 * use homogenously distributed values for t, since x(t) increases linearily.
	 */
	for (i = 0; i <= ROUND (dx * (double) (curve->n_samples - 1)); i++)
    {
		double y, t;
		int    index;
		
		t = i / dx / (double) (curve->n_samples - 1);
		y =     y0 * (1-t) * (1-t) * (1-t) +
		3 * y1 * (1-t) * (1-t) * t     +
		3 * y2 * (1-t) * t     * t     +
		y3 * t     * t     * t;
		
		index = i + ROUND (x0 * (double) (curve->n_samples - 1));
		
		if (index < curve->n_samples)
			curve->samples[index] = CLAMP (y, 0.0, 1.0);
    }
}
