#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

// freeglut使うときはインクルード
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut使うときはインクルード(ここまで)

#include "Ball.h"

// ビリヤード作るぞ！！！！！！
// TODO: Unityで言うdeltaTimeの計算を実装
// TODO: 速度計算をdeltaTime(仮名)に合わせて行う（現状は1フレームごとの計算になってしまっている）
// TODO: 台を実寸通りの大きさに調整
// TODO: 台のインスタンスを生成
// TODO: 台を表示させる
// TODO: カメラを移動させる（優先度低）
// TODO: 「シーンを奥に移動」から「カメラ位置を移動」に変える（優先度低）

// 初期ウィンドウのサイズ指定
#define WINDOW_WIDTH 640	// ウィンドウの横の長さ
#define WINDOW_HEIGHT 480	// ウィンドウの横の長さ

// ボールに関するパラメータなど
#define BALL_NUMBER 1			// ボールの個数
#define BALL_WEIGHT 0.17		// ボールの質量（単位はkg）
#define BALL_RADIUS 0.02855		// ボールの半径（単位はメートル）
static Ball* ballAry;				// ボールのインスタンス生成用

#define PX 0.0                     /* 初期位置　　　　　 */
#define PZ 0.0                     /* 初期位置　　　　　 */
#define W 4                        /* 台の幅の２分の１　 */
#define D 6                        /* 台の長さの２分の１ */
#define H 0.5                      /* 壁の高さ　　　　　 */
#define R 0.3                      /* パックの半径　　　 */
#define TIMESCALE 0.01             /* フレームごとの時間 */
#define SPEED 30.0                 /* パックの初速度　　 */
#define MU 0.5                     /* 台との摩擦係数　　 */
#define WEIGHT 1.0                 /* パックの質量　　　 */

static int wh;                     /* ウィンドウの高さ　 */
static int frame = 0;              /* 現在のフレーム数　 */
static double vx0, vz0;            /* パックの初速度　　 */
static double px0 = PX, pz0 = PZ;  /* パックの初期位置　 */

/*
* 台を描く
*/
static void myGround(double height)
{
	const static GLfloat ground[][4] = {   /* 台の色　　　 */
		{ 0.6, 0.6, 0.6, 1.0 },
		{ 0.3, 0.3, 0.3, 1.0 }
	};
	const static GLfloat wall[] = {        /* 壁の色　　　 */
		0.8, 0.8, 0.8, 1.0
	};
	const static GLdouble panel[][9] = { /* 壁の形状データ */
		{ 0.0,  0.0,  1.0, -W, 0.0, -D, -W, H, -D },
		{ -1.0,  0.0,  0.0,  W, 0.0, -D,  W, H, -D },
		{ 0.0,  0.0, -1.0,  W, 0.0,  D,  W, H,  D },
		{ 1.0,  0.0,  0.0, -W, 0.0,  D, -W, H,  D },
		{ 0.0,  0.0,  1.0, -W, 0.0, -D, -W, H, -D },
	};
	int i, j;

	glBegin(GL_QUADS);

	/* 床を描く */
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

/*
* 円柱を描く
*/
static void myCylinder(double radius, double height, int sides)
{
	double step = 6.2831853072 / (double)sides;
	int i = 0;

	/* 上面 */
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	while (i < sides) {
		double t = step * (double)i++;
		glVertex3d(radius * sin(t), height, radius * cos(t));
	}
	glEnd();

	/* 底面 */
	glNormal3d(0.0, -1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	while (--i >= 0) {
		double t = step * (double)i;
		glVertex3d(radius * sin(t), -height, radius * cos(t));
	}
	glEnd();

	/* 側面 */
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
* 画面表示
*/
static void display(void)
{
	const static GLfloat lightpos[] = { 3.0, 4.0, 5.0, 1.0 }; /* 光源の位置 */
	const static GLfloat yellow[] = { 0.9, 0.9, 0.2, 1.0 };   /* パックの色 */

	double t = TIMESCALE * frame;         /* フレーム数から現在時刻を求める */

	double v = exp(-MU * t / WEIGHT);                   /* パックの速度比　 */
	double p = WEIGHT * (1.0 - v) / MU;                 /* パックの相対位置 */

	double px = vx0 * p + px0;                          /* パックの現在位置 */
	double pz = vz0 * p + pz0;                          /* パックの現在位置 */

	/*
	** パックが台の壁に当たったら初期位置と初速度を変更する
	** 速度（比）が一定以下になったら現在位置を初期位置にして
	** アニメーションを止める
	** （ここは自分で考えてください）
	*/

	/* フレーム数（画面表示を行った回数）をカウントする */
	++frame;

	// [追加]
	// ボールの移動
	ballAry[0].moveBall();
	cout << ballAry[0].getVelocity().length() << endl;
	// [追加] ここまで

	/* 画面クリア */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* モデルビュー変換行列の初期化 */
	glLoadIdentity();

	/* 光源の位置を設定 */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	/* 視点の移動（物体の方を奥に移す）*/
	glTranslated(0.0, 0.0, -20.0);
	glRotated(45.0, 1.0, 0.0, 0.0);

	/* シーンの描画 */

	myGround(0.0);
	/*// パックの描画
	glPushMatrix();
		glTranslated(px, 0.0, pz);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		myCylinder(0.3, 0.1, 8);
	glPopMatrix();
	*/
	// ボールの描画
	ballAry[0].drawBall();

	/* シーンの描画ここまで */

	// 再描画をかける
	//glFlush();
	glutSwapBuffers();
}

static void resize(int w, int h)
{
	/* ウィンドウ全体をビューポートにする */
	glViewport(0, 0, w, wh = h);

	/* 透視変換行列の指定 */
	glMatrixMode(GL_PROJECTION);

	/* 透視変換行列の初期化 */
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	/* モデルビュー変換行列の指定 */
	glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	// ボールに速度を与える（テスト用）
	case 'a':
	case 'A':
		ballAry[0].setVelocity(vec3(0.0f, 0.0f, -0.3f));
		break;
	// 終了ボタン
	case 'q':
	case 'Q':
	case '\033':  /* '\033' は ESC の ASCII コード */
		exit(0);
	default:
		break;
	}
}

// マウスクリック時の台の座標を計算する
// 変更予定、もしくは削除
static void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:

		/*
		** マウスをクリックしたウィンドウ上の座標から
		** 表示されている台の表面の空間位置を求める
		*/

		if (state == GLUT_DOWN) {
			GLdouble model[16], proj[16];
			GLint view[4];
			GLfloat z;
			GLdouble ox, oy, oz;

			/* クリックした時にフレーム数を 0（すなわち時間を 0）にする */
			frame = 0;

			/*
			** モデルビュー変換行列・透視変換行列・ビューポート変換行列を取り出す
			*/
			glGetDoublev(GL_MODELVIEW_MATRIX, model);
			glGetDoublev(GL_PROJECTION_MATRIX, proj);
			glGetIntegerv(GL_VIEWPORT, view);

			/*
			** クリックした位置の奥行きを z に取り出す
			** （台の外をクリックするとまずいけどもういいや）
			*/
			glReadPixels(x, wh - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

			/* クリックしたところの台の上の位置を求める */
			gluUnProject(x, wh - y, z, model, proj, view, &ox, &oy, &oz);

			/*
			** (px0, pz0) と (ox, oz) を使ってパックを打ち出す方向を決め，
			** パックの初速度 (vx0, vz0) を決めて，アニメーションを開始する．
			** （ここは自分で考えて下さい）
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
	/* 初期設定 */
	// ライトは初期設定
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

// タイマー関数
// 画面の再描画を行っている
void timer(int value) {
	glutPostRedisplay();
	// ここで描画頻度を設定する
	glutTimerFunc(16, timer, 0);
}

// メイン関数
int main(int argc, char *argv[])
{
	// 初期ウィンドウのサイズ指定
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	// 以下テストパラメータ
	vec3 testPos(1.0f, 0.5f, 3.0f);
	GLfloat testColor[] = { 0.8f, 0.2f, 0.2f, 1.0f };
	float testRadius = 0.5f;
	vec3 initVelocity(0.0f, 0.0f, 0.0f);
	// テストパラメータ設定ここまで

	// ボール生成、パラメータを設定
	ballAry = new Ball[BALL_NUMBER];
	for (int i = 0; i < BALL_NUMBER; i++) {
		// パラメータを各ボールにセットしていく
		ballAry[i].setParam(testPos, testColor, BALL_WEIGHT, testRadius);
		ballAry[i].setVelocity(initVelocity);
	}

	// 以下glut関係
	glutInit(&argc, argv);
	// glutInitDisplayModeのこと（引数に関して）を調べる必要あり
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	// タイマー関数
	glutTimerFunc(100, timer, 0);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	init();
	glutMainLoop();
	// glut関係ここまで

	// 配列を解放
	delete[] ballAry;

	return 0;
}