
#include "math1.h"
#include "vector.h"

/*************************/
/* Some useful constants */
/*************************/

static const Vector2 vector2_zero =   { 0.0, 0.0 };
static const Vector2 vector2_unit_x = { 1.0, 0.0 };
static const Vector2 vector2_unit_y = { 0.0, 1.0 };

static const Vector3 vector3_zero =   { 0.0, 0.0, 0.0 };
static const Vector3 vector3_unit_x = { 1.0, 0.0, 0.0 };
static const Vector3 vector3_unit_y = { 0.0, 1.0, 0.0 };
static const Vector3 vector3_unit_z = { 0.0, 0.0, 1.0 };

/**************************************/
/* Two   dimensional vector functions */
/**************************************/

/**
 * vector2_new:
 * @x: the X coordinate.
 * @y: the Y coordinate.
 *
 * Creates a #Vector2 of coordinates @x and @y.
 *
 * Returns: the resulting #Vector2.
 **/
Vector2
vector2_new (double x,
                  double y)
{
  Vector2 vector;

  vector.x = x;
  vector.y = y;

  return vector;
}

/**
 * vector2_set:
 * @vector: a pointer to a #Vector2.
 * @x: the X coordinate.
 * @y: the Y coordinate.
 *
 * Sets the X and Y coordinates of @vector to @x and @y.
 **/
void
vector2_set (Vector2 *vector,
                  double      x,
                  double      y)
{
  vector->x = x;
  vector->y = y;
}

/**
 * vector2_length:
 * @vector: a pointer to a #Vector2.
 *
 * Computes the length of a 2D vector.
 *
 * Returns: the length of @vector (a positive double).
 **/
double
vector2_length (const Vector2 *vector)
{
  return (sqrt (vector->x * vector->x + vector->y * vector->y));
}

/**
 * vector2_length_val:
 * @vector: a #Vector2.
 *
 * This function is identical to vector2_length() but the
 * vector is passed by value rather than by reference.
 *
 * Returns: the length of @vector (a positive double).
 **/
double
vector2_length_val (Vector2 vector)
{
  return (sqrt (vector.x * vector.x + vector.y * vector.y));
}

/**
 * vector2_mul:
 * @vector: a pointer to a #Vector2.
 * @factor: a scalar.
 *
 * Multiplies each component of the @vector by @factor. Note that this
 * is equivalent to multiplying the vectors length by @factor.
 **/
void
vector2_mul (Vector2 *vector,
                  double      factor)
{
  vector->x *= factor;
  vector->y *= factor;
}

/**
 * vector2_mul_val:
 * @vector: a #Vector2.
 * @factor: a scalar.
 *
 * This function is identical to vector2_mul() but the vector is
 * passed by value rather than by reference.
 *
 * Returns: the resulting #Vector2.
 **/
Vector2
vector2_mul_val (Vector2 vector,
                      double     factor)
{
  Vector2 result;

  result.x = vector.x * factor;
  result.y = vector.y * factor;

  return result;
}


/**
 * vector2_normalize:
 * @vector: a pointer to a #Vector2.
 *
 * Normalizes the @vector so the length of the @vector is 1.0. The nul
 * vector will not be changed.
 **/
void
vector2_normalize (Vector2 *vector)
{
  double len;

  len = vector2_length (vector);

  if (len != 0.0)
    {
      len = 1.0 / len;
      vector->x *= len;
      vector->y *= len;
    }
  else
    {
      *vector = vector2_zero;
    }
}

/**
 * vector2_normalize_val:
 * @vector: a #Vector2.
 *
 * This function is identical to vector2_normalize() but the
 * vector is passed by value rather than by reference.
 *
 * Returns: a #Vector2 parallel to @vector, pointing in the same
 * direction but with a length of 1.0.
 **/
Vector2
vector2_normalize_val (Vector2 vector)
{
  Vector2 normalized;
  double     len;

  len = vector2_length_val (vector);

  if (len != 0.0)
    {
      len = 1.0 / len;
      normalized.x = vector.x * len;
      normalized.y = vector.y * len;
      return normalized;
    }
  else
    {
      return vector2_zero;
    }
}

/**
 * vector2_neg:
 * @vector: a pointer to a #Vector2.
 *
 * Negates the @vector (i.e. negate all its coordinates).
 **/
void
vector2_neg (Vector2 *vector)
{
  vector->x *= -1.0;
  vector->y *= -1.0;
}

/**
 * vector2_neg_val:
 * @vector: a #Vector2.
 *
 * This function is identical to vector2_neg() but the vector
 * is passed by value rather than by reference.
 *
 * Returns: the negated #Vector2.
 **/
Vector2
vector2_neg_val (Vector2 vector)
{
  Vector2 result;

  result.x = vector.x * -1.0;
  result.y = vector.y * -1.0;

  return result;
}

/**
 * vector2_add:
 * @result: destination for the resulting #Vector2.
 * @vector1: a pointer to the first #Vector2.
 * @vector2: a pointer to the second #Vector2.
 *
 * Computes the sum of two 2D vectors. The resulting #Vector2 is
 * stored in @result.
 **/
void
vector2_add (Vector2       *result,
                  const Vector2 *vector1,
                  const Vector2 *vector2)
{
  result->x = vector1->x + vector2->x;
  result->y = vector1->y + vector2->y;
}

/**
 * vector2_add_val:
 * @vector1: the first #Vector2.
 * @vector2: the second #Vector2.
 *
 * This function is identical to vector2_add() but the vectors
 * are passed by value rather than by reference.
 *
 * Returns: the resulting #Vector2.
 **/
Vector2
vector2_add_val (Vector2 vector1,
                      Vector2 vector2)
{
  Vector2 result;

  result.x = vector1.x + vector2.x;
  result.y = vector1.y + vector2.y;

  return result;
}

/**
 * vector2_sub:
 * @result: the destination for the resulting #Vector2.
 * @vector1: a pointer to the first #Vector2.
 * @vector2: a pointer to the second #Vector2.
 *
 * Computes the difference of two 2D vectors (@vector1 minus @vector2).
 * The resulting #Vector2 is stored in @result.
 **/
void
vector2_sub (Vector2       *result,
                  const Vector2 *vector1,
                  const Vector2 *vector2)
{
  result->x = vector1->x - vector2->x;
  result->y = vector1->y - vector2->y;
}

/**
 * vector2_sub_val:
 * @vector1: the first #Vector2.
 * @vector2: the second #Vector2.
 *
 * This function is identical to vector2_sub() but the vectors
 * are passed by value rather than by reference.
 *
 * Returns: the resulting #Vector2.
 **/
Vector2
vector2_sub_val (Vector2 vector1,
                      Vector2 vector2)
{
  Vector2 result;

  result.x = vector1.x - vector2.x;
  result.y = vector1.y - vector2.y;

  return result;
}

/**
 * vector2_inner_product:
 * @vector1: a pointer to the first #Vector2.
 * @vector2: a pointer to the second #Vector2.
 *
 * Computes the inner (dot) product of two 2D vectors.
 * This product is zero if and only if the two vectors are orthognal.
 *
 * Returns: The inner product.
 **/
double
vector2_inner_product (const Vector2 *vector1,
                            const Vector2 *vector2)
{
  return (vector1->x * vector2->x + vector1->y * vector2->y);
}

/**
 * vector2_inner_product_val:
 * @vector1: the first #Vector2.
 * @vector2: the second #Vector2.
 *
 * This function is identical to vector2_inner_product() but the
 * vectors are passed by value rather than by reference.
 *
 * Returns: The inner product.
 **/
double
vector2_inner_product_val (Vector2 vector1,
                                Vector2 vector2)
{
  return (vector1.x * vector2.x + vector1.y * vector2.y);
}

/**
 * vector2_cross_product:
 * @vector1: a pointer to the first #Vector2.
 * @vector2: a pointer to the second #Vector2.
 *
 * Compute the cross product of two vectors. The result is a
 * #Vector2 which is orthognal to both @vector1 and @vector2. If
 * @vector1 and @vector2 are parallel, the result will be the nul
 * vector.
 *
 * Note that in 2D, this function is useful to test if two vectors are
 * parallel or not, or to compute the area spawned by two vectors.
 *
 * Returns: The cross product.
 **/
Vector2
vector2_cross_product (const Vector2 *vector1,
                            const Vector2 *vector2)
{
  Vector2 normal;

  normal.x = vector1->x * vector2->y - vector1->y * vector2->x;
  normal.y = vector1->y * vector2->x - vector1->x * vector2->y;

  return normal;
}

/**
 * vector2_cross_product_val:
 * @vector1: the first #Vector2.
 * @vector2: the second #Vector2.
 *
 * This function is identical to vector2_cross_product() but the
 * vectors are passed by value rather than by reference.
 *
 * Returns: The cross product.
 **/
Vector2
vector2_cross_product_val (Vector2 vector1,
                                Vector2 vector2)
{
  Vector2 normal;

  normal.x = vector1.x * vector2.y - vector1.y * vector2.x;
  normal.y = vector1.y * vector2.x - vector1.x * vector2.y;

  return normal;
}

/**
 * vector2_rotate:
 * @vector: a pointer to a #Vector2.
 * @alpha: an angle (in radians).
 *
 * Rotates the @vector counterclockwise by @alpha radians.
 **/
void
vector2_rotate (Vector2 *vector,
                     double      alpha)
{
  Vector2 result;

  result.x = cos (alpha) * vector->x + sin (alpha) * vector->y;
  result.y = cos (alpha) * vector->y - sin (alpha) * vector->x;

  *vector = result;
}

/**
 * vector2_rotate_val:
 * @vector: a #Vector2.
 * @alpha: an angle (in radians).
 *
 * This function is identical to vector2_rotate() but the vector
 * is passed by value rather than by reference.
 *
 * Returns: a #Vector2 representing @vector rotated by @alpha
 * radians.
 **/
Vector2
vector2_rotate_val (Vector2 vector,
                         double     alpha)
{
  Vector2 result;

  result.x = cos (alpha) * vector.x + sin (alpha) * vector.y;
  result.y = cos (alpha) * vector.y - sin (alpha) * vector.x;

  return result;
}

/**************************************/
/* Three dimensional vector functions */
/**************************************/

/**
 * vector3_new:
 * @x: the X coordinate.
 * @y: the Y coordinate.
 * @z: the Z coordinate.
 *
 * Creates a #Vector3 of coordinate @x, @y and @z.
 *
 * Returns: the resulting #Vector3.
 **/
Vector3
vector3_new (double  x,
                  double  y,
                  double  z)
{
  Vector3 vector;

  vector.x = x;
  vector.y = y;
  vector.z = z;

  return vector;
}

/**
 * vector3_set:
 * @vector: a pointer to a #Vector3.
 * @x: the X coordinate.
 * @y: the Y coordinate.
 * @z: the Z coordinate.
 *
 * Sets the X, Y and Z coordinates of @vector to @x, @y and @z.
 **/
void
vector3_set (Vector3 *vector,
                  double      x,
                  double      y,
                  double      z)
{
  vector->x = x;
  vector->y = y;
  vector->z = z;
}

/**
 * vector3_length:
 * @vector: a pointer to a #Vector3.
 *
 * Computes the length of a 3D vector.
 *
 * Returns: the length of @vector (a positive double).
 **/
double
vector3_length (const Vector3 *vector)
{
  return (sqrt (vector->x * vector->x +
                vector->y * vector->y +
                vector->z * vector->z));
}

/**
 * vector3_length_val:
 * @vector: a #Vector3.
 *
 * This function is identical to vector3_length() but the vector
 * is passed by value rather than by reference.
 *
 * Returns: the length of @vector (a positive double).
 **/
double
vector3_length_val (Vector3 vector)
{
  return (sqrt (vector.x * vector.x +
                vector.y * vector.y +
                vector.z * vector.z));
}

/**
 * vector3_mul:
 * @vector: a pointer to a #Vector3.
 * @factor: a scalar.
 *
 * Multiplies each component of the @vector by @factor. Note that
 * this is equivalent to multiplying the vectors length by @factor.
 **/
void
vector3_mul (Vector3 *vector,
                  double      factor)
{
  vector->x *= factor;
  vector->y *= factor;
  vector->z *= factor;
}

/**
 * vector3_mul_val:
 * @vector: a #Vector3.
 * @factor: a scalar.
 *
 * This function is identical to vector3_mul() but the vector is
 * passed by value rather than by reference.
 *
 * Returns: the resulting #Vector3.
 **/
Vector3
vector3_mul_val (Vector3 vector,
                      double     factor)
{
  Vector3 result;

  result.x = vector.x * factor;
  result.y = vector.y * factor;
  result.z = vector.z * factor;

  return result;
}

/**
 * vector3_normalize:
 * @vector: a pointer to a #Vector3.
 *
 * Normalizes the @vector so the length of the @vector is 1.0. The nul
 * vector will not be changed.
 **/
void
vector3_normalize (Vector3 *vector)
{
  double len;

  len = vector3_length (vector);

  if (len != 0.0)
    {
      len = 1.0 / len;
      vector->x *= len;
      vector->y *= len;
      vector->z *= len;
    }
  else
    {
      *vector = vector3_zero;
    }
}

/**
 * vector3_normalize_val:
 * @vector: a #Vector3.
 *
 * This function is identical to vector3_normalize() but the
 * vector is passed by value rather than by reference.
 *
 * Returns: a #Vector3 parallel to @vector, pointing in the same
 * direction but with a length of 1.0.
 **/
Vector3
vector3_normalize_val (Vector3 vector)
{
  Vector3 result;
  double     len;

  len = vector3_length_val (vector);

  if (len != 0.0)
    {
      len = 1.0 / len;
      result.x = vector.x * len;
      result.y = vector.y * len;
      result.z = vector.z * len;
      return result;
    }
  else
    {
      return vector3_zero;
    }
}

/**
 * vector3_neg:
 * @vector: a pointer to a #Vector3.
 *
 * Negates the @vector (i.e. negate all its coordinates).
 **/
void
vector3_neg (Vector3 *vector)
{
  vector->x *= -1.0;
  vector->y *= -1.0;
  vector->z *= -1.0;
}

/**
 * vector3_neg_val:
 * @vector: a #Vector3.
 *
 * This function is identical to vector3_neg() but the vector
 * is passed by value rather than by reference.
 *
 * Returns: the negated #Vector3.
 **/
Vector3
vector3_neg_val (Vector3 vector)
{
  Vector3 result;

  result.x = vector.x * -1.0;
  result.y = vector.y * -1.0;
  result.z = vector.z * -1.0;

  return result;
}

/**
 * vector3_add:
 * @result: destination for the resulting #Vector3.
 * @vector1: a pointer to the first #Vector3.
 * @vector2: a pointer to the second #Vector3.
 *
 * Computes the sum of two 3D vectors. The resulting #Vector3 is
 * stored in @result.
 **/
void
vector3_add (Vector3       *result,
                  const Vector3 *vector1,
                  const Vector3 *vector2)
{
  result->x = vector1->x + vector2->x;
  result->y = vector1->y + vector2->y;
  result->z = vector1->z + vector2->z;
}

/**
 * vector3_add_val:
 * @vector1: a #Vector3.
 * @vector2: a #Vector3.
 *
 * This function is identical to vector3_add() but the vectors
 * are passed by value rather than by reference.
 *
 * Returns: the resulting #Vector3.
 **/
Vector3
vector3_add_val (Vector3 vector1,
                      Vector3 vector2)
{
  Vector3 result;

  result.x = vector1.x + vector2.x;
  result.y = vector1.y + vector2.y;
  result.z = vector1.z + vector2.z;

  return result;
}

/**
 * vector3_sub:
 * @result: the destination for the resulting #Vector3.
 * @vector1: a pointer to the first #Vector3.
 * @vector2: a pointer to the second #Vector3.
 *
 * Computes the difference of two 3D vectors (@vector1 minus @vector2).
 * The resulting #Vector3 is stored in @result.
 **/
void
vector3_sub (Vector3       *result,
                  const Vector3 *vector1,
                  const Vector3 *vector2)
{
  result->x = vector1->x - vector2->x;
  result->y = vector1->y - vector2->y;
  result->z = vector1->z - vector2->z;
}

/**
 * vector3_sub_val:
 * @vector1: a #Vector3.
 * @vector2: a #Vector3.
 *
 * This function is identical to vector3_sub() but the vectors
 * are passed by value rather than by reference.
 *
 * Returns: the resulting #Vector3.
 **/
Vector3
vector3_sub_val (Vector3 vector1,
                     Vector3 vector2)
{
  Vector3 result;

  result.x = vector1.x - vector2.x;
  result.y = vector1.y - vector2.y;
  result.z = vector1.z - vector2.z;

  return result;
}

/**
 * vector3_inner_product:
 * @vector1: a pointer to the first #Vector3.
 * @vector2: a pointer to the second #Vector3.
 *
 * Computes the inner (dot) product of two 3D vectors. This product
 * is zero if and only if the two vectors are orthognal.
 *
 * Returns: The inner product.
 **/
double
vector3_inner_product (const Vector3 *vector1,
                            const Vector3 *vector2)
{
  return (vector1->x * vector2->x +
          vector1->y * vector2->y +
          vector1->z * vector2->z);
}

/**
 * vector3_inner_product_val:
 * @vector1: the first #Vector3.
 * @vector2: the second #Vector3.
 *
 * This function is identical to vector3_inner_product() but the
 * vectors are passed by value rather than by reference.
 *
 * Returns: The inner product.
 **/
double
vector3_inner_product_val (Vector3 vector1,
                                Vector3 vector2)
{
  return (vector1.x * vector2.x +
          vector1.y * vector2.y +
          vector1.z * vector2.z);
}

/**
 * vector3_cross_product:
 * @vector1: a pointer to the first #Vector3.
 * @vector2: a pointer to the second #Vector3.
 *
 * Compute the cross product of two vectors. The result is a
 * #Vector3 which is orthognal to both @vector1 and @vector2. If
 * @vector1 and @vector2 and parallel, the result will be the nul
 * vector.
 *
 * This function can be used to compute the normal of the plane
 * defined by @vector1 and @vector2.
 *
 * Returns: The cross product.
 **/
Vector3
vector3_cross_product (const Vector3 *vector1,
                            const Vector3 *vector2)
{
  Vector3 normal;

  normal.x = vector1->y * vector2->z - vector1->z * vector2->y;
  normal.y = vector1->z * vector2->x - vector1->x * vector2->z;
  normal.z = vector1->x * vector2->y - vector1->y * vector2->x;

  return normal;
}

/**
 * vector3_cross_product_val:
 * @vector1: the first #Vector3.
 * @vector2: the second #Vector3.
 *
 * This function is identical to vector3_cross_product() but the
 * vectors are passed by value rather than by reference.
 *
 * Returns: The cross product.
 **/
Vector3
vector3_cross_product_val (Vector3 vector1,
                                Vector3 vector2)
{
  Vector3 normal;

  normal.x = vector1.y * vector2.z - vector1.z * vector2.y;
  normal.y = vector1.z * vector2.x - vector1.x * vector2.z;
  normal.z = vector1.x * vector2.y - vector1.y * vector2.x;

  return normal;
}

/**
 * vector3_rotate:
 * @vector: a pointer to a #Vector3.
 * @alpha: the angle (in radian) of rotation around the Z axis.
 * @beta: the angle (in radian) of rotation around the Y axis.
 * @gamma: the angle (in radian) of rotation around the X axis.
 *
 * Rotates the @vector around the three axis (Z, Y, and X) by @alpha,
 * @beta and @gamma, respectively.
 *
 * Note that the order of the rotation is very important. If you
 * expect a vector to be rotated around X, and then around Y, you will
 * have to call this function twice. Also, it is often wise to call
 * this function with only one of @alpha, @beta and @gamma non-zero.
 **/
void
vector3_rotate (Vector3 *vector,
                     double      alpha,
                     double      beta,
                     double      gamma)
{
  Vector3 s, t;

  /* First we rotate it around the Z axis (XY plane).. */
  /* ================================================= */

  s.x = cos (alpha) * vector->x + sin (alpha) * vector->y;
  s.y = cos (alpha) * vector->y - sin (alpha) * vector->x;

  /* ..then around the Y axis (XZ plane).. */
  /* ===================================== */

  t = s;

  vector->x = cos (beta) *t.x       + sin (beta) * vector->z;
  s.z       = cos (beta) *vector->z - sin (beta) * t.x;

  /* ..and at last around the X axis (YZ plane) */
  /* ========================================== */

  vector->y = cos (gamma) * t.y + sin (gamma) * s.z;
  vector->z = cos (gamma) * s.z - sin (gamma) * t.y;
}

/**
 * vector3_rotate_val:
 * @vector: a #Vector3.
 * @alpha: the angle (in radian) of rotation around the Z axis.
 * @beta: the angle (in radian) of rotation around the Y axis.
 * @gamma: the angle (in radian) of rotation around the X axis.
 *
 * This function is identical to vector3_rotate() but the vectors
 * are passed by value rather than by reference.
 *
 * Returns: the rotated vector.
 **/
Vector3
vector3_rotate_val (Vector3 vector,
                         double     alpha,
                         double     beta,
                         double     gamma)
{
  Vector3 s, t, result;

  /* First we rotate it around the Z axis (XY plane).. */
  /* ================================================= */

  s.x = cos (alpha) * vector.x + sin (alpha) * vector.y;
  s.y = cos (alpha) * vector.y - sin (alpha) * vector.x;

  /* ..then around the Y axis (XZ plane).. */
  /* ===================================== */

  t = s;

  result.x = cos (beta) *t.x      + sin (beta) * vector.z;
  s.z      = cos (beta) *vector.z - sin (beta) * t.x;

  /* ..and at last around the X axis (YZ plane) */
  /* ========================================== */

  result.y = cos (gamma) * t.y + sin (gamma) * s.z;
  result.z = cos (gamma) * s.z - sin (gamma) * t.y;

  return result;
}

/**
 * vector_2d_to_3d:
 * @sx: the abscisse of the upper-left screen rectangle.
 * @sy: the ordinate of the upper-left screen rectangle.
 * @w: the width of the screen rectangle.
 * @h: the height of the screen rectangle.
 * @x: the abscisse of the point in the screen rectangle to map.
 * @y: the ordinate of the point in the screen rectangle to map.
 * @vp: the position of the observer.
 * @p: the resulting point.
 *
 * \"Compute screen (sx, sy) - (sx + w, sy + h) to 3D unit square
 * mapping. The plane to map to is given in the z field of p. The
 * observer is located at position vp (vp->z != 0.0).\"
 *
 * In other words, this computes the projection of the point (@x, @y)
 * to the plane z = @p->z (parallel to XY), from the @vp point of view
 * through the screen (@sx, @sy)->(@sx + @w, @sy + @h)
 **/

void
vector_2d_to_3d (int               sx,
                      int               sy,
                      int               w,
                      int               h,
                      int               x,
                      int               y,
                      const Vector3 *vp,
                      Vector3       *p)
{
  double t = 0.0;

  if (vp->x != 0.0)
    t = (p->z - vp->z) / vp->z;

  if (t != 0.0)
    {
      p->x = vp->x + t * (vp->x - ((double) (x - sx) / (double) w));
      p->y = vp->y + t * (vp->y - ((double) (y - sy) / (double) h));
    }
  else
    {
      p->x = (double) (x - sx) / (double) w;
      p->y = (double) (y - sy) / (double) h;
    }
}

/**
 * vector_2d_to_3d_val:
 * @sx: the abscisse of the upper-left screen rectangle.
 * @sy: the ordinate of the upper-left screen rectangle.
 * @w: the width of the screen rectangle.
 * @h: the height of the screen rectangle.
 * @x: the abscisse of the point in the screen rectangle to map.
 * @y: the ordinate of the point in the screen rectangle to map.
 * @vp: position of the observer.
 * @p: the resulting point.
 *
 * This function is identical to vector_2d_to_3d() but the
 * position of the @observer and the resulting point @p are passed by
 * value rather than by reference.
 *
 * Returns: the computed #Vector3 point.
 **/
Vector3
vector_2d_to_3d_val (int        sx,
                          int        sy,
                          int        w,
                          int        h,
                          int        x,
                          int        y,
                          Vector3 vp,
                          Vector3 p)
{
  Vector3 result;
  double     t = 0.0;

  if (vp.x != 0.0)
    t = (p.z - vp.z) / vp.z;

  if (t != 0.0)
    {
      result.x = vp.x + t * (vp.x - ((double) (x - sx) / (double) w));
      result.y = vp.y + t * (vp.y - ((double) (y - sy) / (double) h));
    }
  else
    {
      result.x = (double) (x - sx) / (double) w;
      result.y = (double) (y - sy) / (double) h;
    }

  result.z = 0;
  return result;
}

/**
 * vector_3d_to_2d:
 * @sx: the abscisse of the upper-left screen rectangle.
 * @sy: the ordinate of the upper-left screen rectangle.
 * @w: the width of the screen rectangle.
 * @h: the height of the screen rectangle.
 * @x: the abscisse of the point in the screen rectangle to map (return value).
 * @y: the ordinate of the point in the screen rectangle to map (return value).
 * @vp: position of the observer.
 * @p: the 3D point to project to the plane.
 *
 * Convert the given 3D point to 2D (project it onto the viewing
 * plane, (sx, sy, 0) - (sx + w, sy + h, 0). The input is assumed to
 * be in the unit square (0, 0, z) - (1, 1, z). The viewpoint of the
 * observer is passed in vp.
 *
 * This is basically the opposite of vector_2d_to_3d().
 **/
void
vector_3d_to_2d (int               sx,
                      int               sy,
                      int               w,
                      int               h,
                      double           *x,
                      double           *y,
                      const Vector3 *vp,
                      const Vector3 *p)
{
  double     t;
  Vector3 dir;

  vector3_sub (&dir, p, vp);
  vector3_normalize (&dir);

  if (dir.z != 0.0)
    {
      t = (-1.0 * vp->z) / dir.z;
      *x = (double) sx + ((vp->x + t * dir.x) * (double) w);
      *y = (double) sy + ((vp->y + t * dir.y) * (double) h);
    }
  else
    {
      *x = (double) sx + (p->x * (double) w);
      *y = (double) sy + (p->y * (double) h);
    }
}
