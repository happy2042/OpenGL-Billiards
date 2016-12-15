#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <cstdint>
#include <Windows.h>

// freeglut使うときはインクルード
#include <GL/freeglut.h>
#pragma comment(lib, "freeglut.lib")
// freeglut使うときはインクルード(ここまで)

// 自作、持ち込みクラスはこちら
#include "Ball.h"
#include "Table.h"
#include "FPSCounter.h"

using namespace std;

// ビリヤード作るぞ！！！！！！
// TODO: 台のインスタンスを生成
// TODO: 台を表示させる
// TODO: ボールが壁にぶつかったら反射する
// TODO: ボールがボールにぶつかったら反射する
// TODO: ボールを11個並べてブレイクショットができるかどうか検証
// TODO: カメラを移動させる（優先度低）
// TODO: 「シーンを奥に移動」から「カメラ位置を移動」に変える（優先度低）

// 長さの基準はセンチメートルにする
// 物理計算でキャストがめんどくさくなったらまた考えます

// 初期ウィンドウのサイズ指定
#define WINDOW_WIDTH 640	// ウィンドウの横の長さ
#define WINDOW_HEIGHT 480	// ウィンドウの横の長さ

// ボールに関するパラメータなど
#define BALL_NUMBER 3			// ボールの個数
#define BALL_WEIGHT 1.7		// ボールの質量（単位は100g毎）
#define BALL_RADIUS 0.02855		// ボールの半径（単位はメートル）
static Ball* ballAry;				// ボールのインスタンス生成用

									// 台のインスタンス作る用
Table table;

/*
* FPS関連の変数（処理ちゃんと書けたら別ヘッダーに移す）
*/
double deltaTime;	// FPSの逆数をとったもの
static CFPSCounter FPS(10);	// FPS計測クラス
const float frameInterval = 1000.0f / 60;	// 60FPSの基準時間（とする）
float nextFrame;

int startSec, nowSec;		// 最初の時刻と現在の時刻（秒）
int sec, millisec;			// 実行開始からの時刻差分を記録する変数(秒、ミリ秒)
struct timeb timebuffer;	// 時間系をまとめた構造体（?）

							// 現在時刻を更新する関数
void getNow() {
	ftime(&timebuffer);
	nowSec = timebuffer.time;
	sec = nowSec - startSec;
	millisec = timebuffer.millitm;
	millisec += sec * 1000;
}

// 現在時刻のミリ秒を返す関数
int getNowMillisec() {
	getNow();
	return millisec;
}
/*
* FPS関連ここまで
*/

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

	double t = TIMESCALE * frame;         /* フレーム数から現在時刻を求める */

										  /* フレーム数（画面表示を行った回数）をカウントする */
										  //++frame;

										  // FPS計測、表示
										  // 変数の型がわからないのでとりあえずdoubleで
	double fps = FPS.GetFPS();	// FPS取得
	cout << fps << endl;		// FPS表示
	deltaTime = 1.0f / fps;		// deltaTimeの算出、fpsの逆数

	// 衝突判定
	// 撃力ベース（って名前らしい）
	// 複合衝突の処理が出来ていない！！！！
	float e = 0.5;	// ボール同士の反発係数
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

	// ボールの移動
	for (int i = 0; i < BALL_NUMBER; i++) {
		ballAry[i].moveBall(deltaTime);
		// 壁と反発しているとき速度を反転させる
		// 0.5は半径
		// この処理が長いので関数で別に置くか、するかもしれない
		// 速度が小さいと壁から脱出できない問題が発生！！！！
		// 壁にも撃力を適用させるべきか
		const float bound = 0.8f;	// 壁との反発係数
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

	// 現在時刻更新
	uint32_t now = getNowMillisec();

	if (now < nextFrame) {
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
		//table.DrawTable();
		myGround(0.0);
		/*// パックの描画
		glPushMatrix();
		glTranslated(px, 0.0, pz);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		myCylinder(0.3, 0.1, 8);
		glPopMatrix();
		*/
		// ボールの描画
		for (int i = 0; i < BALL_NUMBER; i++) {
			ballAry[i].drawBall();
		}
		/* シーンの描画ここまで */

		// Zバッファをスワップさせる
		// 再描画
		glutSwapBuffers();

		// 現在時刻更新
		now = getNowMillisec();

		// 描画しても時間が余ってたら
		if (now < nextFrame) {
			// 型変換が必要...
			Sleep(static_cast<uint32_t>(nextFrame - now));
		}
	}
	nextFrame += frameInterval;
	glutPostRedisplay();
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
		ballAry[0].setVelocity(vec3(0.0f, 0.0f, -10.0f));
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
	// 開始時刻の記録
	ftime(&timebuffer);
	startSec = timebuffer.time;
	millisec = timebuffer.millitm;
	// floatでとりあえずFPS管理
	float nextFrame = getNowMillisec() + frameInterval;

	// 以下テストパラメータ
	GLfloat testColor1[] = { 0.8f, 0.2f, 0.2f, 1.0f };
	GLfloat testColor2[] = { 0.2f, 0.2f, 0.8f, 1.0f };
	GLfloat testColor3[] = { 0.2f, 0.8f, 0.0f, 1.0f };
	float testRadius = 0.5f;
	vec3 testPos1(0.0f, testRadius, 3.0f);
	vec3 testPos2(testRadius, testRadius, -3.0f);
	vec3 testPos3(-testRadius, testRadius, -3.0f);
	vec3 initVelocity(0.0f, 0.0f, 0.0f);
	// テストパラメータ設定ここまで

	// ボール生成、パラメータを設定
	ballAry = new Ball[BALL_NUMBER];
	// パラメータを各ボールにセットしていく
	ballAry[0].setParam(testPos1, testColor1, BALL_WEIGHT, testRadius);
	ballAry[0].setVelocity(initVelocity);
	ballAry[1].setParam(testPos2, testColor2, BALL_WEIGHT, testRadius);
	ballAry[1].setVelocity(initVelocity);
	ballAry[2].setParam(testPos3, testColor3, BALL_WEIGHT, testRadius);
	ballAry[2].setVelocity(initVelocity);


	// 台のインスタンス作成
	/*
	vec3 testTablePos(0.0f, 0.0f, 0.0f);
	float groundWidth = 127;
	float groundHeight = 254;
	float wallWidth = 14;
	float wallHeight = 3.7;
	table.setParam(testTablePos, groundWidth, groundHeight, wallWidth, wallHeight);
	*/

	// 以下glut関係
	// 初期ウィンドウのサイズ指定
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInit(&argc, argv);
	// glutInitDisplayModeのこと（引数に関して）を調べる必要あり
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	//glutTimerFunc(100, timer, 0);		// タイマー関数
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