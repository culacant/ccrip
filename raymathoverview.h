#define PI 3.14159265358979323846
#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)
typedef struct Vector2 {
  float x;
  float y;
} Vector2;
typedef struct Vector3 {
  float x;
  float y;
  float z;
} Vector3;
typedef struct Matrix {
  float m0, m4, m8, m12;
  float m1, m5, m9, m13;
  float m2, m6, m10, m14;
  float m3, m7, m11, m15;
} Matrix;
typedef struct Quaternion {
    float x;
    float y;
    float z;
    float w;
} Quaternion;
// Clamp float value
float Clamp(float value, float min, float max)
//----------------------------------------------------------------------------------
// Module Functions Definition - Vector2 math
//----------------------------------------------------------------------------------
// Vector with components value 0.0f
Vector2 Vector2Zero(void)
// Vector with components value 1.0f
Vector2 Vector2One(void)
// Add two vectors (v1 + v2)
Vector2 Vector2Add(Vector2 v1, Vector2 v2)
// Subtract two vectors (v1 - v2)
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2)
// Calculate vector length
float Vector2Length(Vector2 v)
// Calculate two vectors dot product
float Vector2DotProduct(Vector2 v1, Vector2 v2)
// Calculate distance between two vectors
float Vector2Distance(Vector2 v1, Vector2 v2)
// Calculate angle from two vectors in X-axis
float Vector2Angle(Vector2 v1, Vector2 v2)
// Scale vector (multiply by value)
Vector2 Vector2Scale(Vector2 v, float scale)
// Negate vector
Vector2 Vector2Negate(Vector2 v)
// Divide vector by a float value
Vector2 Vector2Divide(Vector2 v, float div)
// Normalize provided vector
Vector2 Vector2Normalize(Vector2 v)

//----------------------------------------------------------------------------------
// Module Functions Definition - Vector3 math
//----------------------------------------------------------------------------------
// Vector with components value 0.0f
Vector3 Vector3Zero(void)
// Vector with components value 1.0f
Vector3 Vector3One(void)
// Add two vectors
Vector3 Vector3Add(Vector3 v1, Vector3 v2)
// Substract two vectors
Vector3 Vector3Subtract(Vector3 v1, Vector3 v2)
// Multiply vector by scalar
Vector3 Vector3Multiply(Vector3 v, float scalar)
// Multiply vector by vector
Vector3 Vector3MultiplyV(Vector3 v1, Vector3 v2)
// Calculate two vectors cross product
Vector3 Vector3CrossProduct(Vector3 v1, Vector3 v2)
// Calculate one vector perpendicular vector
Vector3 Vector3Perpendicular(Vector3 v)
// Calculate vector length
float Vector3Length(const Vector3 v)
// Calculate two vectors dot product
float Vector3DotProduct(Vector3 v1, Vector3 v2)
// Calculate distance between two vectors
float Vector3Distance(Vector3 v1, Vector3 v2)
// Scale provided vector
Vector3 Vector3Scale(Vector3 v, float scale)
// Negate provided vector (invert direction)
Vector3 Vector3Negate(Vector3 v)
// Normalize provided vector
Vector3 Vector3Normalize(Vector3 v)
// Transforms a Vector3 by a given Matrix
Vector3 Vector3Transform(Vector3 v, Matrix mat)
// Calculate linear interpolation between two vectors
Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, float amount)
// Calculate reflected vector to normal
Vector3 Vector3Reflect(Vector3 v, Vector3 normal)
// Return min value for each pair of components
Vector3 Vector3Min(Vector3 v1, Vector3 v2)
// Return max value for each pair of components
Vector3 Vector3Max(Vector3 v1, Vector3 v2)
// Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c)
// NOTE: Assumes P is on the plane of the triangle
Vector3 Vector3Barycenter(Vector3 p, Vector3 a, Vector3 b, Vector3 c)
// Returns Vector3 as float array
float3 Vector3ToFloatV(Vector3 v)

//----------------------------------------------------------------------------------
// Module Functions Definition - Matrix math
//----------------------------------------------------------------------------------
// Compute matrix determinant
float MatrixDeterminant(Matrix mat)
// Returns the trace of the matrix (sum of the values along the diagonal)
float MatrixTrace(Matrix mat)
// Transposes provided matrix
Matrix MatrixTranspose(Matrix mat)
// Invert provided matrix
Matrix MatrixInvert(Matrix mat)
// Normalize provided matrix
Matrix MatrixNormalize(Matrix mat)
// Returns identity matrix
Matrix MatrixIdentity(void)
// Add two matrices
Matrix MatrixAdd(Matrix left, Matrix right)
// Substract two matrices (left - right)
Matrix MatrixSubstract(Matrix left, Matrix right)
// Returns translation matrix
Matrix MatrixTranslate(float x, float y, float z)
// Create rotation matrix from axis and angle
// NOTE: Angle should be provided in radians
Matrix MatrixRotate(Vector3 axis, float angle)
// Returns x-rotation matrix (angle in radians)
Matrix MatrixRotateX(float angle)
// Returns y-rotation matrix (angle in radians)
Matrix MatrixRotateY(float angle)
// Returns z-rotation matrix (angle in radians)
Matrix MatrixRotateZ(float angle)
// Returns scaling matrix
Matrix MatrixScale(float x, float y, float z)
// Returns two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
Matrix MatrixMultiply(Matrix left, Matrix right)
// Returns perspective projection matrix
Matrix MatrixFrustum(double left, double right, double bottom, double top, double near, double far)
// Returns perspective projection matrix
// NOTE: Angle should be provided in radians
Matrix MatrixPerspective(double fovy, double aspect, double near, double far)
// Returns orthographic projection matrix
Matrix MatrixOrtho(double left, double right, double bottom, double top, double near, double far)
// Returns camera look-at matrix (view matrix)
Matrix MatrixLookAt(Vector3 eye, Vector3 target, Vector3 up)
// Returns float array of matrix data
float16 MatrixToFloatV(Matrix mat)

//----------------------------------------------------------------------------------
// Module Functions Definition - Quaternion math
//----------------------------------------------------------------------------------
// Returns identity quaternion
 Quaternion QuaternionIdentity(void)
// Computes the length of a quaternion
float QuaternionLength(Quaternion q)
// Normalize provided quaternion
Quaternion QuaternionNormalize(Quaternion q)
// Invert provided quaternion
Quaternion QuaternionInvert(Quaternion q)
// Calculate two quaternion multiplication
Quaternion QuaternionMultiply(Quaternion q1, Quaternion q2)
// Calculate linear interpolation between two quaternions
Quaternion QuaternionLerp(Quaternion q1, Quaternion q2, float amount)
// Calculate slerp-optimized interpolation between two quaternions
Quaternion QuaternionNlerp(Quaternion q1, Quaternion q2, float amount)
// Calculates spherical linear interpolation between two quaternions
Quaternion QuaternionSlerp(Quaternion q1, Quaternion q2, float amount)
// Calculate quaternion based on the rotation from one vector to another
Quaternion QuaternionFromVector3ToVector3(Vector3 from, Vector3 to)
// Returns a quaternion for a given rotation matrix
Quaternion QuaternionFromMatrix(Matrix mat)
// Returns a matrix for a given quaternion
Matrix QuaternionToMatrix(Quaternion q)
// Returns rotation quaternion for an angle and axis
// NOTE: angle must be provided in radians
Quaternion QuaternionFromAxisAngle(Vector3 axis, float angle)
// Returns the rotation angle and axis for a given quaternion
void QuaternionToAxisAngle(Quaternion q, Vector3 *outAxis, float *outAngle)
// Returns he quaternion equivalent to Euler angles
Quaternion QuaternionFromEuler(float roll, float pitch, float yaw)
// Return the Euler angles equivalent to quaternion (roll, pitch, yaw)
// NOTE: Angles are returned in a Vector3 struct in degrees
Vector3 QuaternionToEuler(Quaternion q)
// Transform a quaternion given a transformation matrix
Quaternion QuaternionTransform(Quaternion q, Matrix mat)
