#ifndef BALL_H
#define BALL_H

// �{�[�����`����N���X
// TODO: DrawBall()�̒��g
// TODO: �{�[���̈ړ����l����

#include "vec3.h"

#define BALL_WEIGHT 0.17	// �{�[���̎��ʁi�P�ʂ�kg�j
#define BALL_RADIUS 0.02855	// �{�[���̔��a�i�P�ʂ̓��[�g���j

class Ball {
public:
	vec3 position;	// �{�[���̒��S�ʒu
	vec3 velocity;	// ���x
	/*	
	// velocity�̑���ɂ��������g����������Ȃ�
	vec3 direction	// �^���̌���
	float speed;	// ����
	*/
	float weight;	// ���ʁi�P�ʂ�kg�j
	float radius;	// ���a�i�P�ʂ̓��[�g���j

	Ball(){}
	void DrawBall(){}
	void setPos(vec3 pos){}
	vec3 getPos(){}

private:
	vec3 preFramePosition;
	vec3 preFrameVelocity;
	/*
	// velocity�̑���ɂ��������g����������Ȃ�
	vec3 preFrameDirection	// �^���̌���
	float preFrameSpeed;	// ����
	*/
};

Ball::Ball() {}

void Ball::DrawBall () {

}

void Ball::setPos(vec3 pos) {
	position = pos;
}

vec3 Ball::getPos() {
	return position;
}


#endif
