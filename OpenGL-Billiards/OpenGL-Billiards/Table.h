#ifndef TABLE_H
#define TABLE_H

// ����`����N���X
// TODO: DrawTable()�̒��g
// TODO: ����\������4�̕ǁA��̐F�Əꏊ���`����

#include <iostream>
#include <string.h>
using namespace std;

// freeglut�g���Ƃ��̓C���N���[�h
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut�g���Ƃ��̓C���N���[�h(�����܂�)

#include "vec3.h"

/* ��̏������̐F */
const static GLfloat ground[][4] = { 
	{ 0.35, 0.75, 0.25, 1.0 }, 
	{ 0.13, 0.75, 0.31, 1.0} 
};

/* �ǂ̐F */
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
	// �`��p�̓_
	GLfloat m_drawPoints[][3];

public:
	Table(){}
	//Table(vec3 pos, float gw, float gh, float ww, float wh){}
	void setParam(vec3 pos, float gw, float gh, float ww, float wh);
	void DrawTable();
};

// �R���X�g���N�^
/*
Table::Table(vec3 pos, float gw, float gh, float ww, float wh){
	// �����̃Z�b�g
	m_position = pos;
	m_groundWidth = gw;
	m_groundHeight = gh;
	m_wallWidth = ww;
	m_wallHeight = wh;

	// �����o�ϐ��̌v�Z
	m_gwHalf = gw / 2;
	m_ghHalf = gh / 2;
}
*/

void Table::setParam(vec3 pos, float gw, float gh, float ww, float wh) {
	// �����̃Z�b�g
	m_position = pos;
	m_groundWidth = gw;
	m_groundHeight = gh;
	m_wallWidth = ww;
	m_wallHeight = wh;

	// �����o�ϐ��̌v�Z
	m_gwHalf = gw / 2;
	m_ghHalf = gh / 2;
}

// ��̕`��
void Table::DrawTable() {
	const static GLdouble panel[][9] = { /* �ǂ̌`��f�[�^ */
		{ 0.0,  0.0,  1.0, -m_gwHalf, 0.0, -m_ghHalf, -m_gwHalf, m_wallHeight, -m_ghHalf },
		{ -1.0,  0.0,  0.0,  m_gwHalf, 0.0, -m_ghHalf,  m_gwHalf, m_wallHeight, -m_ghHalf },
		{ 0.0,  0.0, -1.0,  m_gwHalf, 0.0,  m_ghHalf,  m_gwHalf, m_wallHeight,  m_ghHalf },
		{ 1.0,  0.0,  0.0, -m_gwHalf, 0.0,  m_ghHalf, -m_gwHalf, m_wallHeight,  m_ghHalf },
		{ 0.0,  0.0,  1.0, -m_gwHalf, 0.0, -m_ghHalf, -m_gwHalf, m_wallHeight, -m_ghHalf },
	};
	int i, j;

	glBegin(GL_QUADS);

	/* ����`�� */
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

	/* �ǂ�`�� */
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
