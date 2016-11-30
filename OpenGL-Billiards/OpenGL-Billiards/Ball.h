#ifndef BALL_H
#define BALL_H

// freeglut使うときはインクルード
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut使うときはインクルード(ここまで)

// ボールを定義するクラス
// TODO: ボールの移動を考える

#include "vec3.h"

#define DIVISION 20		// 球の分割数

class Ball {
private:
	vec3 m_position;	// ボールの中心位置
	vec3 m_velocity;	// 速度
	GLfloat* m_color;	// 基本色
	/*
	// velocityの代わりにこっちを使うかもしれない
	vec3 direction	// 運動の向き
	float speed;	// 速さ
	*/
	float m_weight;	// 質量（単位はkg）
	float m_radius;	// 半径（単位はメートル）
	vec3 preFramePosition;
	vec3 preFrameVelocity;
	/*
	// velocityの代わりにこっちを使うかもしれない
	vec3 preFrameDirection	// 運動の向き
	float preFrameSpeed;	// 速さ
	*/

public:
	Ball(){}
	~Ball(){}
	void setParam(vec3 pos, GLfloat* color, float weight, float radius);
	void DrawBall();
	void setPos(vec3 pos);
	vec3 getPos();
};

// パラメータ設定
void Ball::setParam(vec3 pos, GLfloat* color, float weight, float radius) {
	// 引数のコピー
	m_position = pos;
	m_color = color;
	m_weight = weight;
	m_radius = radius;
}

// ボールの描画
// main.cppのdisplay()で使用
void Ball::DrawBall () {
	glPushMatrix();
		glTranslated(m_position.x, m_position.y, m_position.z);		// 平行移動値の設定
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_color);				// 色の設定
		glutSolidSphere(m_radius, DIVISION, DIVISION);				// 引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
	glPopMatrix();
}

// ポジションの代入をするメソッド
void Ball::setPos(vec3 pos) {
	m_position = pos;
}

// ポジションを取得するメソッド
vec3 Ball::getPos() {
	return m_position;
}


#endif
