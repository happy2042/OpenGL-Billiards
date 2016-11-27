#ifndef VEC3_H
#define VEC3_H

// 3�����x�N�g�����`����N���X
// ��Ԃ̍��W�������ꍇ�ƁA�x�N�g����\���ꍇ������
// ��L�̌���...�@���ĉ�����...�i�N���X������ׂ��Ȃ̂�??�j

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3 {
public:
	float x, y, z;

	vec3() {}
	vec3(float e0, float e1, float e2) {
		x = e0;
		y = e1;
		z = e2;
	}

	inline const vec3& operator+() const { return *this; }
	inline vec3 operator-() const {
		return vec3(-x, -y, -z);
	}
	inline float length() const {
		return sqrtf(x * x + y * y + z * z);
	}
};

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
	return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline vec3 operator*(float t, const vec3 &v) {
	return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator/(vec3 v, float t) {
	return vec3(v.x / t, v.y / t, v.z / t);
}

inline vec3 operator*(const vec3 &v, float t) {
	return vec3(t * v.x, t * v.y, t * v.z);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
	return vec3((v1.y * v2.z - v1.z * v2.y),
		(-(v1.x * v2.z - v1.z * v2.x)),
		(v1.x * v2.y - v1.y * v2.x));
}

inline vec3 unitVector(vec3 v) {
	return v / v.length();
}

#endif

