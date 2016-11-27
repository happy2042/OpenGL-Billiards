#ifndef BALL_H
#define BALL_H

// ボールを定義するクラス
// TODO: DrawBall()の中身
// TODO: ボールの移動を考える

#include "vec3.h"

#define BALL_WEIGHT 0.17	// ボールの質量（単位はkg）
#define BALL_RADIUS 0.02855	// ボールの半径（単位はメートル）

class Ball {
public:
	vec3 position;	// ボールの中心位置
	vec3 velocity;	// 速度
	/*	
	// velocityの代わりにこっちを使うかもしれない
	vec3 direction	// 運動の向き
	float speed;	// 速さ
	*/
	float weight;	// 質量（単位はkg）
	float radius;	// 半径（単位はメートル）

	Ball(){}
	void DrawBall(){}
	void setPos(vec3 pos){}
	vec3 getPos(){}

private:
	vec3 preFramePosition;
	vec3 preFrameVelocity;
	/*
	// velocityの代わりにこっちを使うかもしれない
	vec3 preFrameDirection	// 運動の向き
	float preFrameSpeed;	// 速さ
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
