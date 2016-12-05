#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

// freeglut�g���Ƃ��̓C���N���[�h
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut�g���Ƃ��̓C���N���[�h(�����܂�)

#include "Ball.h"

// �r�����[�h��邼�I�I�I�I�I�I
// TODO: Unity�Ō���deltaTime�̌v�Z������
// TODO: ���x�v�Z��deltaTime(����)�ɍ��킹�čs���i�����1�t���[�����Ƃ̌v�Z�ɂȂ��Ă��܂��Ă���j
// TODO: ��������ʂ�̑傫���ɒ���
// TODO: ��̃C���X�^���X�𐶐�
// TODO: ���\��������
// TODO: �J�������ړ�������i�D��x��j
// TODO: �u�V�[�������Ɉړ��v����u�J�����ʒu���ړ��v�ɕς���i�D��x��j

// �����E�B���h�E�̃T�C�Y�w��
#define WINDOW_WIDTH 640	// �E�B���h�E�̉��̒���
#define WINDOW_HEIGHT 480	// �E�B���h�E�̉��̒���

// �{�[���Ɋւ���p�����[�^�Ȃ�
#define BALL_NUMBER 1			// �{�[���̌�
#define BALL_WEIGHT 0.17		// �{�[���̎��ʁi�P�ʂ�kg�j
#define BALL_RADIUS 0.02855		// �{�[���̔��a�i�P�ʂ̓��[�g���j
static Ball* ballAry;				// �{�[���̃C���X�^���X�����p

#define PX 0.0                     /* �����ʒu�@�@�@�@�@ */
#define PZ 0.0                     /* �����ʒu�@�@�@�@�@ */
#define W 4                        /* ��̕��̂Q���̂P�@ */
#define D 6                        /* ��̒����̂Q���̂P */
#define H 0.5                      /* �ǂ̍����@�@�@�@�@ */
#define R 0.3                      /* �p�b�N�̔��a�@�@�@ */
#define TIMESCALE 0.01             /* �t���[�����Ƃ̎��� */
#define SPEED 30.0                 /* �p�b�N�̏����x�@�@ */
#define MU 0.5                     /* ��Ƃ̖��C�W���@�@ */
#define WEIGHT 1.0                 /* �p�b�N�̎��ʁ@�@�@ */

static int wh;                     /* �E�B���h�E�̍����@ */
static int frame = 0;              /* ���݂̃t���[�����@ */
static double vx0, vz0;            /* �p�b�N�̏����x�@�@ */
static double px0 = PX, pz0 = PZ;  /* �p�b�N�̏����ʒu�@ */

/*
* ���`��
*/
static void myGround(double height)
{
	const static GLfloat ground[][4] = {   /* ��̐F�@�@�@ */
		{ 0.6, 0.6, 0.6, 1.0 },
		{ 0.3, 0.3, 0.3, 1.0 }
	};
	const static GLfloat wall[] = {        /* �ǂ̐F�@�@�@ */
		0.8, 0.8, 0.8, 1.0
	};
	const static GLdouble panel[][9] = { /* �ǂ̌`��f�[�^ */
		{ 0.0,  0.0,  1.0, -W, 0.0, -D, -W, H, -D },
		{ -1.0,  0.0,  0.0,  W, 0.0, -D,  W, H, -D },
		{ 0.0,  0.0, -1.0,  W, 0.0,  D,  W, H,  D },
		{ 1.0,  0.0,  0.0, -W, 0.0,  D, -W, H,  D },
		{ 0.0,  0.0,  1.0, -W, 0.0, -D, -W, H, -D },
	};
	int i, j;

	glBegin(GL_QUADS);

	/* ����`�� */
	glNormal3d(0.0, 1.0, 0.0);
	for (j = -D; j < D; ++j) {
		for (i = -W; i < W; ++i) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, ground[(i + j) & 1]);
			glVertex3d((GLdouble)i, height, (GLdouble)j);
			glVertex3d((GLdouble)i, height, (GLdouble)(j + 1));
			glVertex3d((GLdouble)(i + 1), height, (GLdouble)(j + 1));
			glVertex3d((GLdouble)(i + 1), height, (GLdouble)j);
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

/*
* �~����`��
*/
static void myCylinder(double radius, double height, int sides)
{
	double step = 6.2831853072 / (double)sides;
	int i = 0;

	/* ��� */
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	while (i < sides) {
		double t = step * (double)i++;
		glVertex3d(radius * sin(t), height, radius * cos(t));
	}
	glEnd();

	/* ��� */
	glNormal3d(0.0, -1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	while (--i >= 0) {
		double t = step * (double)i;
		glVertex3d(radius * sin(t), -height, radius * cos(t));
	}
	glEnd();

	/* ���� */
	glBegin(GL_QUAD_STRIP);
	while (i <= sides) {
		double t = step * (double)i++;
		double x = sin(t);
		double z = cos(t);

		glNormal3d(x, 0.0, z);
		glVertex3f(radius * x, height, radius * z);
		glVertex3f(radius * x, -height, radius * z);
	}
	glEnd();
}

/*
* ��ʕ\��
*/
static void display(void)
{
	const static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /* �����̈ʒu */
	const static GLfloat yellow[] = { 0.9, 0.9, 0.2, 1.0 };   /* �p�b�N�̐F */

	double t = TIMESCALE * frame;         /* �t���[�������猻�ݎ��������߂� */

	double v = exp(-MU * t / WEIGHT);                   /* �p�b�N�̑��x��@ */
	double p = WEIGHT * (1.0 - v) / MU;                 /* �p�b�N�̑��Έʒu */

	double px = vx0 * p + px0;                          /* �p�b�N�̌��݈ʒu */
	double pz = vz0 * p + pz0;                          /* �p�b�N�̌��݈ʒu */

	/*
	** �p�b�N����̕ǂɓ��������珉���ʒu�Ə����x��ύX����
	** ���x�i��j�����ȉ��ɂȂ����猻�݈ʒu�������ʒu�ɂ���
	** �A�j���[�V�������~�߂�
	** �i�����͎����ōl���Ă��������j
	*/

	/* �t���[�����i��ʕ\�����s�����񐔁j���J�E���g���� */
	++frame;

	// [�ǉ�]
	// �{�[���̈ړ�
	ballAry[0].moveBall();
	cout << ballAry[0].getVelocity().length() << endl;
	// [�ǉ�] �����܂�

	/* ��ʃN���A */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* ���f���r���[�ϊ��s��̏����� */
	glLoadIdentity();

	/* �����̈ʒu��ݒ� */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* ���_�̈ړ��i���̂̕������Ɉڂ��j*/
	glTranslated(0.0, 0.0, -20.0);
	glRotated(45.0, 1.0, 0.0, 0.0);

	/* �V�[���̕`�� */

	myGround(0.0);
	/*// �p�b�N�̕`��
	glPushMatrix();
		glTranslated(px, 0.0, pz);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		myCylinder(0.3, 0.1, 8);
	glPopMatrix();
	*/
	// �{�[���̕`��
	ballAry[0].drawBall();

	/* �V�[���̕`�悱���܂� */

	// �ĕ`���������
	//glFlush();
	glutSwapBuffers();
}

static void resize(int w, int h)
{
	/* �E�B���h�E�S�̂��r���[�|�[�g�ɂ��� */
	glViewport(0, 0, w, wh = h);

	/* �����ϊ��s��̎w�� */
	glMatrixMode(GL_PROJECTION);

	/* �����ϊ��s��̏����� */
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	/* ���f���r���[�ϊ��s��̎w�� */
	glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	// �{�[���ɑ��x��^����i�e�X�g�p�j
	case 'a':
	case 'A':
		ballAry[0].setVelocity(vec3(0.0f, 0.0f, -0.3f));
		break;
	// �I���{�^��
	case 'q':
	case 'Q':
	case '\033':  /* '\033' �� ESC �� ASCII �R�[�h */
		exit(0);
	default:
		break;
	}
}

// �}�E�X�N���b�N���̑�̍��W���v�Z����
// �ύX�\��A�������͍폜
static void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:

		/*
		** �}�E�X���N���b�N�����E�B���h�E��̍��W����
		** �\������Ă����̕\�ʂ̋�Ԉʒu�����߂�
		*/

		if (state == GLUT_DOWN) {
			GLdouble model[16], proj[16];
			GLint view[4];
			GLfloat z;
			GLdouble ox, oy, oz;

			/* �N���b�N�������Ƀt���[������ 0�i���Ȃ킿���Ԃ� 0�j�ɂ��� */
			frame = 0;

			/*
			** ���f���r���[�ϊ��s��E�����ϊ��s��E�r���[�|�[�g�ϊ��s������o��
			*/
			glGetDoublev(GL_MODELVIEW_MATRIX, model);
			glGetDoublev(GL_PROJECTION_MATRIX, proj);
			glGetIntegerv(GL_VIEWPORT, view);

			/*
			** �N���b�N�����ʒu�̉��s���� z �Ɏ��o��
			** �i��̊O���N���b�N����Ƃ܂������ǂ���������j
			*/
			glReadPixels(x, wh - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

			/* �N���b�N�����Ƃ���̑�̏�̈ʒu�����߂� */
			gluUnProject(x, wh - y, z, model, proj, view, &ox, &oy, &oz);

			/*
			** (px0, pz0) �� (ox, oz) ���g���ăp�b�N��ł��o�����������߁C
			** �p�b�N�̏����x (vx0, vz0) �����߂āC�A�j���[�V�������J�n����D
			** �i�����͎����ōl���ĉ������j
			*/

			break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	default:
		break;
		}
	}
}

static void init(void)
{
	/* �����ݒ� */
	// ���C�g�͏����ݒ�
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

// �^�C�}�[�֐�
// ��ʂ̍ĕ`����s���Ă���
void timer(int value) {
	glutPostRedisplay();
	// �����ŕ`��p�x��ݒ肷��
	glutTimerFunc(16, timer, 0);
}

// ���C���֐�
int main(int argc, char *argv[])
{
	// �����E�B���h�E�̃T�C�Y�w��
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// �ȉ��e�X�g�p�����[�^
	vec3 testPos(1.0f, 0.5f, 3.0f);
	GLfloat testColor[] = { 0.8f, 0.2f, 0.2f, 1.0f };
	float testRadius = 0.5f;
	vec3 initVelocity(0.0f, 0.0f, 0.0f);
	// �e�X�g�p�����[�^�ݒ肱���܂�

	// �{�[�������A�p�����[�^��ݒ�
	ballAry = new Ball[BALL_NUMBER];
	for (int i = 0; i < BALL_NUMBER; i++) {
		// �p�����[�^���e�{�[���ɃZ�b�g���Ă���
		ballAry[i].setParam(testPos, testColor, BALL_WEIGHT, testRadius);
		ballAry[i].setVelocity(initVelocity);
	}

	// �ȉ�glut�֌W
	glutInit(&argc, argv);
	// glutInitDisplayMode�̂��Ɓi�����Ɋւ��āj�𒲂ׂ�K�v����
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	// �^�C�}�[�֐�
	glutTimerFunc(100, timer, 0);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	init();
	glutMainLoop();
	// glut�֌W�����܂�

	// �z������
	delete[] ballAry;

	return 0;
}