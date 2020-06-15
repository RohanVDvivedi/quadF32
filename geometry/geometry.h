#ifndef GEOMETRY_H
#define GEOMETRY_H

#include<math.h>

typedef struct vector vector;
struct vector
{
	float xi;
	float yj;
	float zk;
};

typedef struct quaternion quaternion;
struct quaternion
{
	// scalar component
	float sc;

	// i, j, and k component
	float xi;
	float yj;
	float zk;
};

typedef struct quat_raw quat_raw;
struct quat_raw
{
	// the rotation amount in angle degrees
	float theta;

	// vector about which rotation is done
	vector vectr;
};

extern vector zero_vector;

// C = A + B
void sum(vector* C, vector* A, vector* B);

// C = A - B
void diff(vector* C, vector* A, vector* B);

// c = A * sc
void multiply_scalar(vector* C, vector* A, float sc);

// C = A X B
void cross(vector* C, vector* A, vector* B);

// A.B
float dot(vector* A, vector* B);

// angle between vectors in degrees
float angle_between_vectors(vector* A, vector* B);

// finding unit vector fails for the vector (0,0,0), hence will return -1 else returns 0
int unit_vector(vector* unitResult, vector* A);

// C = component of A parallel to B
void parallel_component(vector* C, vector* A, vector* B);

// C = component of A perpendicular to B
void perpendicular_component(vector* C, vector* A, vector* B);

// get magnitude of the vector
float magnitude_vector(vector* D);

// get magnitude of the quaternion
float norm(quaternion* D);

// multiply quaternions with hamilton product in order as C = AB
void hamilton_product(quaternion* C, quaternion* A, quaternion* B);

void to_quaternion(quaternion* destination, quat_raw* source);

void get_unit_rotation_axis(vector* axis, quaternion* source);

void conjugate(quaternion* destination, quaternion* source);

void reciprocal(quaternion* destination, quaternion* source);

// rotates a given vector by a given quaternion, must return 0
float rotate_vector(vector* result, quaternion* rotation_quaternion, vector* initial);

// averaged update
void update_vector(vector* result, vector* updated_value, float factor);

// Spherical linear interpolation of two quaternions
void slerp_quaternion(quaternion* Result, quaternion* A, float factorA, quaternion* B);

// this method returns the quaternion rotation that caused tips of Ai and Bi non collinear vectors
// of the object, to move to final positions Af and Bf respectively
void get_quaternion_from_vectors_changes(quaternion* quat, vector* Af, vector* Ai, vector* Bf, vector* Bi);

#endif