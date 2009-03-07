
#ifndef __VECTOR_H__
#define __VECTOR_H__

/* For information look into the C source or the html documentation */

typedef struct {
  double x, y;
} Vector2;

typedef struct {
  double x, y, z;
} Vector3;

typedef struct {
  double x, y, z, w;
} Vector4;


/* Two dimensional vector functions */
/* ================================ */

Vector2 vector2_new               (double            x,
                                            double            y);
void        vector2_set               (Vector2       *vector,
                                            double            x,
                                            double            y);
double     vector2_length            (const Vector2 *vector);
double     vector2_length_val        (Vector2        vector);
void        vector2_mul               (Vector2       *vector,
                                            double            factor);
Vector2 vector2_mul_val           (Vector2        vector,
                                            double            factor);
void        vector2_normalize         (Vector2       *vector);
Vector2 vector2_normalize_val     (Vector2        vector);
void        vector2_neg               (Vector2       *vector);
Vector2 vector2_neg_val           (Vector2        vector);
void        vector2_add               (Vector2       *result,
                                            const Vector2 *vector1,
                                            const Vector2 *vector2);
Vector2 vector2_add_val           (Vector2        vector1,
                                            Vector2        vector2);
void        vector2_sub               (Vector2       *result,
                                            const Vector2 *vector1,
                                            const Vector2 *vector2);
Vector2 vector2_sub_val           (Vector2        vector1,
                                            Vector2        vector2);
double     vector2_inner_product     (const Vector2 *vector1,
                                            const Vector2 *vector2);
double     vector2_inner_product_val (Vector2        vector1,
                                            Vector2        vector2);
Vector2 vector2_cross_product     (const Vector2 *vector1,
                                            const Vector2 *vector2);
Vector2 vector2_cross_product_val (Vector2        vector1,
                                            Vector2        vector2);
void        vector2_rotate            (Vector2       *vector,
                                            double            alpha);
Vector2 vector2_rotate_val        (Vector2        vector,
                                            double            alpha);

/* Three dimensional vector functions */
/* ================================== */

Vector3 vector3_new               (double            x,
                                            double            y,
                                            double            z);
void        vector3_set               (Vector3       *vector,
                                            double            x,
                                            double            y,
                                            double            z);
double     vector3_length            (const Vector3 *vector);
double     vector3_length_val        (Vector3        vector);
void        vector3_mul               (Vector3       *vector,
                                            double            factor);
Vector3 vector3_mul_val           (Vector3        vector,
                                            double            factor);
void        vector3_normalize         (Vector3       *vector);
Vector3 vector3_normalize_val     (Vector3        vector);
void        vector3_neg               (Vector3       *vector);
Vector3 vector3_neg_val           (Vector3        vector);
void        vector3_add               (Vector3       *result,
                                            const Vector3 *vector1,
                                            const Vector3 *vector2);
Vector3 vector3_add_val           (Vector3        vector1,
                                            Vector3        vector2);
void        vector3_sub               (Vector3       *result,
                                            const Vector3 *vector1,
                                            const Vector3 *vector2);
Vector3 vector3_sub_val           (Vector3        vector1,
                                            Vector3        vector2);
double     vector3_inner_product     (const Vector3 *vector1,
                                            const Vector3 *vector2);
double     vector3_inner_product_val (Vector3        vector1,
                                            Vector3        vector2);
Vector3 vector3_cross_product     (const Vector3 *vector1,
                                            const Vector3 *vector2);
Vector3 vector3_cross_product_val (Vector3        vector1,
                                            Vector3        vector2);
void        vector3_rotate            (Vector3       *vector,
                                            double            alpha,
                                            double            beta,
                                            double            gamma);
Vector3 vector3_rotate_val        (Vector3        vector,
                                            double            alpha,
                                            double            beta,
                                            double            gamma);

/* 2d <-> 3d Vector projection functions */
/* ===================================== */

void        vector_2d_to_3d           (int               sx,
                                            int               sy,
                                            int               w,
                                            int               h,
                                            int               x,
                                            int               y,
                                            const Vector3 *vp,
                                            Vector3       *p);

Vector3 vector_2d_to_3d_val       (int               sx,
                                            int               sy,
                                            int               w,
                                            int               h,
                                            int               x,
                                            int               y,
                                            Vector3        vp,
                                            Vector3        p);

void        vector_3d_to_2d           (int               sx,
                                            int               sy,
                                            int               w,
                                            int               h,
                                            double           *x,
                                            double           *y,
                                            const Vector3 *vp,
                                            const Vector3 *p);


#endif  /* __VECTOR_H__ */
