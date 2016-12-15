#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <cstdint>
#include <Windows.h>

// freeglut�g���Ƃ��̓C���N���[�h
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut�g���Ƃ��̓C���N���[�h(�����܂�)

// ����A�������݃N���X�͂�����
#include "Ball.h"
#include "Table.h"
#include "FPSCounter.h"

using namespace std;

// �r�����[�h��邼�I�I�I�I�I�I
// TODO: ��̃C���X�^���X�𐶐�
// TODO: ���\��������
// TODO: �{�[�����ǂɂԂ������甽�˂���
// TODO: �{�[�����{�[���ɂԂ������甽�˂���
// TODO: �{�[����11���ׂău���C�N�V���b�g���ł��邩�ǂ�������
// TODO: �J�������ړ�������i�D��x��j
// TODO: �u�V�[�������Ɉړ��v����u�J�����ʒu���ړ��v�ɕς���i�D��x��j

// �����̊�̓Z���`���[�g���ɂ���
// �����v�Z�ŃL���X�g���߂�ǂ������Ȃ�����܂��l���܂�

// �����E�B���h�E�̃T�C�Y�w��
#define WINDOW_WIDTH 640	// �E�B���h�E�̉��̒���
#define WINDOW_HEIGHT 480	// �E�B���h�E�̉��̒���

// �{�[���Ɋւ���p�����[�^�Ȃ�
#define BALL_NUMBER 3			// �{�[���̌�
#define BALL_WEIGHT 1.7		// �{�[���̎��ʁi�P�ʂ�100g���j
#define BALL_RADIUS 0.02855		// �{�[���̔��a�i�P�ʂ̓��[�g���j
static Ball* ballAry;				// �{�[���̃C���X�^���X�����p

									// ��̃C���X�^���X���p
Table table;

/*
* FPS�֘A�̕ϐ��i���������Ə�������ʃw�b�_�[�Ɉڂ��j
*/
double deltaTime;	// FPS�̋t�����Ƃ�������
static CFPSCounter FPS(10);	// FPS�v���N���X
const float frameInterval = 1000.0f / 60;	// 60FPS�̊���ԁi�Ƃ���j
float nextFrame;

int startSec, nowSec;		// �ŏ��̎����ƌ��݂̎����i�b�j
int sec, millisec;			// ���s�J�n����̎����������L�^����ϐ�(�b�A�~���b)
struct timeb timebuffer;	// ���Ԍn���܂Ƃ߂��\���́i?�j

							// ���ݎ������X�V����֐�
void getNow() {
	ftime(&timebuffer);
	nowSec = timebuffer.time;
	sec = nowSec - startSec;
	millisec = timebuffer.millitm;
	millisec += sec * 1000;
}

// ���ݎ����̃~���b��Ԃ��֐�
int getNowMillisec() {
	getNow();
	return millisec;
}
/*
* FPS�֘A�����܂�
*/

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

	double t = TIMESCALE * frame;         /* �t���[�������猻�ݎ��������߂� */

										  /* �t���[�����i��ʕ\�����s�����񐔁j���J�E���g���� */
										  //++frame;

										  // FPS�v���A�\��
										  // �ϐ��̌^���킩��Ȃ��̂łƂ肠����double��
	double fps = FPS.GetFPS();	// FPS�擾
	cout << fps << endl;		// FPS�\��
	deltaTime = 1.0f / fps;		// deltaTime�̎Z�o�Afps�̋t��

	// �Փ˔���
	// ���̓x�[�X�i���Ė��O�炵���j
	// �����Փ˂̏������o���Ă��Ȃ��I�I�I�I
	float e = 0.5;	// �{�[�����m�̔����W��
	for (int i = 0; i < (BALL_NUMBER - 1); i++) {
		for (int j = (i + 1); j < BALL_NUMBER; j++) {
			if (vec3(ballAry[i].getPos() - ballAry[j].getPos()).length() < (ballAry[i].getRadius() + ballAry[j].getRadius())) {
				vec3 n = unitVector(ballAry[i].getPos() - ballAry[j].getPos());
				float J = -(e + 1) * dot(n, (ballAry[i].getVelocity() - ballAry[j].getVelocity()))
					/ (1 / BALL_WEIGHT) + (1 / BALL_WEIGHT);
				ballAry[i].setVelocity(ballAry[i].getVelocity() + n*J / BALL_WEIGHT);
				ballAry[j].setVelocity(ballAry[j].getVelocity() - n*J / BALL_WEIGHT);
			}
		}
	}

	// �{�[���̈ړ�
	for (int i = 0; i < BALL_NUMBER; i++) {
		ballAry[i].moveBall(deltaTime);
		// �ǂƔ������Ă���Ƃ����x�𔽓]������
		// 0.5�͔��a
		// ���̏����������̂Ŋ֐��ŕʂɒu�����A���邩������Ȃ�
		// ���x���������ƕǂ���E�o�ł��Ȃ���肪�����I�I�I�I
		// �ǂɂ����͂�K�p������ׂ���
		const float bound = 0.8f;	// �ǂƂ̔����W��
		if (ballAry[i].getPos().x < -(W - 0.5)) {
			ballAry[i].setVelocity(
				vec3(ballAry[i].getVelocity().x * (-1),
					ballAry[i].getVelocity().y,
					ballAry[i].getVelocity().z) * bound);
		}
		if (ballAry[i].getPos().x > (W - 0.5)) {
			ballAry[i].setVelocity(
				vec3(ballAry[i].getVelocity().x * (-1),
					ballAry[i].getVelocity().y,
					ballAry[i].getVelocity().z) * bound);
		}
		if (ballAry[i].getPos().z < -(D - 0.5)) {
			ballAry[i].setVelocity(
				vec3(ballAry[i].getVelocity().x,
					ballAry[i].getVelocity().y,
					ballAry[i].getVelocity().z * (-1)) * bound);
		}
		if (ballAry[i].getPos().z > (D - 0.5)) {
			ballAry[i].setVelocity(
				vec3(ballAry[i].getVelocity().x,
					ballAry[i].getVelocity().y,
					ballAry[i].getVelocity().z * (-1)) * bound);
		}
	}

	// ���ݎ����X�V
	uint32_t now = getNowMillisec();

	if (now < nextFrame) {
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
		//table.DrawTable();
		myGround(0.0);
		/*// �p�b�N�̕`��
		glPushMatrix();
		glTranslated(px, 0.0, pz);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		myCylinder(0.3, 0.1, 8);
		glPopMatrix();
		*/
		// �{�[���̕`��
		for (int i = 0; i < BALL_NUMBER; i++) {
			ballAry[i].drawBall();
		}
		/* �V�[���̕`�悱���܂� */

		// Z�o�b�t�@���X���b�v������
		// �ĕ`��
		glutSwapBuffers();

		// ���ݎ����X�V
		now = getNowMillisec();

		// �`�悵�Ă����Ԃ��]���Ă���
		if (now < nextFrame) {
			// �^�ϊ����K�v...
			Sleep(static_cast<uint32_t>(nextFrame - now));
		}
	}
	nextFrame += frameInterval;
	glutPostRedisplay();
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
		ballAry[0].setVelocity(vec3(0.0f, 0.0f, -10.0f));
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
	// �J�n�����̋L�^
	ftime(&timebuffer);
	startSec = timebuffer.time;
	millisec = timebuffer.millitm;
	// float�łƂ肠����FPS�Ǘ�
	float nextFrame = getNowMillisec() + frameInterval;

	// �ȉ��e�X�g�p�����[�^
	GLfloat testColor1[] = { 0.8f, 0.2f, 0.2f, 1.0f };
	GLfloat testColor2[] = { 0.2f, 0.2f, 0.8f, 1.0f };
	GLfloat testColor3[] = { 0.2f, 0.8f, 0.0f, 1.0f };
	float testRadius = 0.5f;
	vec3 testPos1(0.0f, testRadius, 3.0f);
	vec3 testPos2(testRadius, testRadius, -3.0f);
	vec3 testPos3(-testRadius, testRadius, -3.0f);
	vec3 initVelocity(0.0f, 0.0f, 0.0f);
	// �e�X�g�p�����[�^�ݒ肱���܂�

	// �{�[�������A�p�����[�^��ݒ�
	ballAry = new Ball[BALL_NUMBER];
	// �p�����[�^���e�{�[���ɃZ�b�g���Ă���
	ballAry[0].setParam(testPos1, testColor1, BALL_WEIGHT, testRadius);
	ballAry[0].setVelocity(initVelocity);
	ballAry[1].setParam(testPos2, testColor2, BALL_WEIGHT, testRadius);
	ballAry[1].setVelocity(initVelocity);
	ballAry[2].setParam(testPos3, testColor3, BALL_WEIGHT, testRadius);
	ballAry[2].setVelocity(initVelocity);


	// ��̃C���X�^���X�쐬
	/*
	vec3 testTablePos(0.0f, 0.0f, 0.0f);
	float groundWidth = 127;
	float groundHeight = 254;
	float wallWidth = 14;
	float wallHeight = 3.7;
	table.setParam(testTablePos, groundWidth, groundHeight, wallWidth, wallHeight);
	*/

	// �ȉ�glut�֌W
	// �����E�B���h�E�̃T�C�Y�w��
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInit(&argc, argv);
	// glutInitDisplayMode�̂��Ɓi�����Ɋւ��āj�𒲂ׂ�K�v����
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	//glutTimerFunc(100, timer, 0);		// �^�C�}�[�֐�
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