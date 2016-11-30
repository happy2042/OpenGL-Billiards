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
	vec3 preFramePosition;
	vec3 preFrameVelocity;
	/*
	// velocity�̑���ɂ��������g����������Ȃ�
	vec3 preFrameDirection	// �^���̌���
	float preFrameSpeed;	// ����
	*/

public:
	Ball(){}
	~Ball(){}
	void setParam(vec3 pos, GLfloat* color, float weight, float radius);
	void DrawBall();
	void setPos(vec3 pos);
	vec3 getPos();
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
void Ball::DrawBall () {
	glPushMatrix();
		glTranslated(m_position.x, m_position.y, m_position.z);		// ���s�ړ��l�̐ݒ�
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_color);				// �F�̐ݒ�
		glutSolidSphere(m_radius, DIVISION, DIVISION);				// �����F(���a, Z���܂��̕�����, Z���ɉ�����������)
	glPopMatrix();
}

// �|�W�V�����̑�������郁�\�b�h
void Ball::setPos(vec3 pos) {
	m_position = pos;
}

// �|�W�V�������擾���郁�\�b�h
vec3 Ball::getPos() {
	return m_position;
}


#endif
