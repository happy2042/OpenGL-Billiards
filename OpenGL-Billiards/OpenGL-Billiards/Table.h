#ifndef TABLE_H
#define TABLE_H

// 台を定義するクラス
// TODO: DrawTable()の中身
// TODO: 台を構成する4つの壁、底の色と場所を定義する

#include <iostream>
#include <string.h>
using namespace std;

// freeglut使うときはインクルード
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut使うときはインクルード(ここまで)

#include "vec3.h"

/* 台の床部分の色 */
const static GLfloat ground[][4] = { 
	{ 0.35, 0.75, 0.25, 1.0 }, 
	{ 0.13, 0.75, 0.31, 1.0} 
};

/* 壁の色 */
const static GLfloat wall[] = {
	0.6, 0.25, 0.1, 1.0 
};

class Table {
private:
	vec3 m_position;
	float m_groundWidth;
	float m_groundHeight;
	float m_gwHalf;
	float m_ghHalf;
	float m_wallWidth;
	float m_wallHeight;
	// 描画用の点
	GLfloat m_drawPoints[][3];

public:
	Table(){}
	//Table(vec3 pos, float gw, float gh, float ww, float wh){}
	void setParam(vec3 pos, float gw, float gh, float ww, float wh);
	void DrawTable();
};

// コンストラクタ
/*
Table::Table(vec3 pos, float gw, float gh, float ww, float wh){
	// 引数のセット
	m_position = pos;
	m_groundWidth = gw;
	m_groundHeight = gh;
	m_wallWidth = ww;
	m_wallHeight = wh;

	// メンバ変数の計算
	m_gwHalf = gw / 2;
	m_ghHalf = gh / 2;
}
*/

void Table::setParam(vec3 pos, float gw, float gh, float ww, float wh) {
	// 引数のセット
	m_position = pos;
	m_groundWidth = gw;
	m_groundHeight = gh;
	m_wallWidth = ww;
	m_wallHeight = wh;

	// メンバ変数の計算
	m_gwHalf = gw / 2;
	m_ghHalf = gh / 2;
}

// 台の描画
void Table::DrawTable() {
	const static GLdouble panel[][9] = { /* 壁の形状データ */
		{ 0.0,  0.0,  1.0, -m_gwHalf, 0.0, -m_ghHalf, -m_gwHalf, m_wallHeight, -m_ghHalf },
		{ -1.0,  0.0,  0.0,  m_gwHalf, 0.0, -m_ghHalf,  m_gwHalf, m_wallHeight, -m_ghHalf },
		{ 0.0,  0.0, -1.0,  m_gwHalf, 0.0,  m_ghHalf,  m_gwHalf, m_wallHeight,  m_ghHalf },
		{ 1.0,  0.0,  0.0, -m_gwHalf, 0.0,  m_ghHalf, -m_gwHalf, m_wallHeight,  m_ghHalf },
		{ 0.0,  0.0,  1.0, -m_gwHalf, 0.0, -m_ghHalf, -m_gwHalf, m_wallHeight, -m_ghHalf },
	};
	int i, j;

	glBegin(GL_QUADS);

	/* 床を描く */
	glNormal3d(0.0, 1.0, 0.0);
	for (j = -m_ghHalf; j < m_ghHalf; ++j) {
		for (i = -m_gwHalf; i < m_gwHalf; ++i) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
			glVertex3d((GLdouble)i, m_position.y, (GLdouble)j);
			glVertex3d((GLdouble)i, m_position.y, (GLdouble)(j + 1));
			glVertex3d((GLdouble)(i + 1), m_position.y, (GLdouble)(j + 1));
			glVertex3d((GLdouble)(i + 1), m_position.y, (GLdouble)j);
		}
	}

	/* 壁を描く */
	glMaterialfv(GL_FRONT, GL_DIFFUSE, wall);
	for (i = 0; i < 4; ++i) {
		glNormal3dv(panel[i]);
		glVertex3dv(panel[i] + 3);
		glVertex3dv(panel[i + 1] + 3);
		glVertex3dv(panel[i + 1] + 6);
		glVertex3dv(panel[i] + 6);
	}

	glEnd();
}

#endif
