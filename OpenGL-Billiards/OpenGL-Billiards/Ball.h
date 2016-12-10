#ifndef BALL_H
#define BALL_H

// freeglut�g���Ƃ��̓C���N���[�h
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut�g���Ƃ��̓C���N���[�h(�����܂�)

// �{�[�����`����N���X
// TODO: �{�[���̈ړ����l����

#include "vec3.h"

#define DIVISION 20		// ���̕�����

class Ball {
private:
	vec3 m_position;	// �{�[���̒��S�ʒu
	vec3 m_velocity;	// ���x
	GLfloat* m_color;	// ��{�F
	/*
	// velocity�̑���ɂ��������g����������Ȃ�
	vec3 direction	// �^���̌���
	float speed;	// ����
	*/
	float m_weight;	// ���ʁi�P�ʂ�kg�j
	float m_radius;	// ���a�i�P�ʂ̓��[�g���j
	/*
	// �g����������񂯂ǑO�t���[���̈ʒu�Ƒ��x
	vec3 preFramePosition;
	vec3 preFrameVelocity;
	// velocity�̑���ɂ��������g����������Ȃ�
	vec3 preFrameDirection	// �^���̌���
	float preFrameSpeed;	// ����
	*/

public:
	Ball(){}
	~Ball(){}
	void setParam(vec3 pos, GLfloat* color, float weight, float radius);
	void drawBall();
	void moveBall(double deltaTime);

	// set���\�b�h�Q
	void setPos(vec3 pos);
	void setVelocity(vec3 velocity);
	// get���\�b�h�Q
	vec3 getPos();
	vec3 getVelocity();
};

// �p�����[�^�ݒ�
void Ball::setParam(vec3 pos, GLfloat* color, float weight, float radius) {
	// �����̃R�s�[
	m_position = pos;
	m_color = color;
	m_weight = weight;
	m_radius = radius;
}

// �{�[���̕`��
// main.cpp��display()�Ŏg�p
void Ball::drawBall () {
	glPushMatrix();
		glTranslated(m_position.x, m_position.y, m_position.z);		// ���s�ړ��l�̐ݒ�
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_color);				// �F�̐ݒ�
		glutSolidSphere(m_radius, DIVISION, DIVISION);				// �����F(���a, Z���܂��̕�����, Z���ɉ�����������)
	glPopMatrix();
}

// �{�[���𓮂���
// position�̌v�Z���s��
// �G������̂ł�����������������A�l���܂�...
void Ball::moveBall(double deltaTime) {
	const float dFlic = 1.5f;	// �����C�W���i���b��ɃX�g�b�v���邩�j
	float attenuation = dFlic * (float)deltaTime;	// ���x�̌������i�b��I�ɒ�`���Ă����j
	attenuation = 1.0f - attenuation;	// �������̌v�Z�i�����ɂ��鉉�Z�j

	// ���x�����ȉ��ɂȂ�܂ŁA���x�̑������݌v�Z������
	if (m_velocity.length() > 0.1f) {
		// ���݂̈ʒu�ɑ��x�������Z
		m_position = m_position + m_velocity * (float)deltaTime;
		// ���x��ς���i�ʃ��\�b�h�Ōv�Z���邩���j
		// velocity�Ɋւ��Ă͂����������G�Ȏ��ɂȂ邩������Ȃ�
		// velocity�����v�Z���郁�\�b�h�����K�v�����邩������Ȃ�
		m_velocity = m_velocity * attenuation;
	}
	else
		// ���x�����ȉ��Ȃ瑬�x��0�ɂ��Ă��܂�
		m_velocity = vec3(0.0f, 0.0f, 0.0f);
}

// �ȉ�set���\�b�h�Q
// �{�[���ʒu
void Ball::setPos(vec3 pos) {
	m_position = pos;
}
// ���x
void Ball::setVelocity(vec3 velocity) {
	m_velocity = velocity;
}

// �ȉ�get���\�b�h�Q
// �{�[���ʒu
vec3 Ball::getPos() {
	return m_position;
}
// ���x
vec3 Ball::getVelocity() {
	return m_velocity;
}


#endif
