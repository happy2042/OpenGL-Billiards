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
	/*
	// 使うか分からんけど前フレームの位置と速度
	vec3 preFramePosition;
	vec3 preFrameVelocity;
	// velocityの代わりにこっちを使うかもしれない
	vec3 preFrameDirection	// 運動の向き
	float preFrameSpeed;	// 速さ
	*/

public:
	Ball(){}
	~Ball(){}
	void setParam(vec3 pos, GLfloat* color, float weight, float radius);
	void drawBall();
	void moveBall(double deltaTime);

	// setメソッド群
	void setPos(vec3 pos);
	void setVelocity(vec3 velocity);
	// getメソッド群
	vec3 getPos();
	vec3 getVelocity();
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
void Ball::drawBall () {
	glPushMatrix();
		glTranslated(m_position.x, m_position.y, m_position.z);		// 平行移動値の設定
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_color);				// 色の設定
		glutSolidSphere(m_radius, DIVISION, DIVISION);				// 引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
	glPopMatrix();
}

// ボールを動かす
// positionの計算を行う
// 雑すぎるのでもう少し分割したり、考えます...
void Ball::moveBall(double deltaTime) {
	const float dFlic = 1.5f;	// 動摩擦係数（何秒後にストップするか）
	float attenuation = dFlic * (float)deltaTime;	// 速度の減衰率（暫定的に定義しておく）
	attenuation = 1.0f - attenuation;	// 減衰率の計算（割合にする演算）

	// 速度が一定以下になるまで、速度の足し込み計算をする
	if (m_velocity.length() > 0.1f) {
		// 現在の位置に速度分を加算
		m_position = m_position + m_velocity * (float)deltaTime;
		// 速度を変える（別メソッドで計算するかも）
		// velocityに関してはもう少し複雑な式になるかもしれない
		// velocityだけ計算するメソッドを作る必要があるかもしれない
		m_velocity = m_velocity * attenuation;
	}
	else
		// 速度が一定以下なら速度を0にしてしまう
		m_velocity = vec3(0.0f, 0.0f, 0.0f);
}

// 以下setメソッド群
// ボール位置
void Ball::setPos(vec3 pos) {
	m_position = pos;
}
// 速度
void Ball::setVelocity(vec3 velocity) {
	m_velocity = velocity;
}

// 以下getメソッド群
// ボール位置
vec3 Ball::getPos() {
	return m_position;
}
// 速度
vec3 Ball::getVelocity() {
	return m_velocity;
}


#endif
