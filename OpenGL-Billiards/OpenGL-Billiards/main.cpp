#include <GL/glut.h>

//----------- �v���g�^�C�v�錾 --------------//
void display();
void reshape(int w, int h);
void timer(int value);
void LIGHT_INIT();

//------------- OpenGL�̏����ݒ� ------------------//
void GLUT_INIT()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //�_�u���o�b�t�@�AZ�o�b�t�@
	glutInitWindowSize(640, 480);
	glutCreateWindow("glut Primitive");
}

void GLUT_CALL_FUNC()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 17);
}

void MY_INIT()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	LIGHT_INIT();
	glEnable(GL_NORMALIZE);//�@���̗L����
	glEnable(GL_DEPTH_TEST);//Z�o�b�t�@ON
}

//�����̐ݒ�
void LIGHT_INIT()
{
	float Light0Pos[] = { 1,1,1,0 }; //�����̈ʒu
	glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);//�ʒu�����ݒ�(���Ƃ̓f�t�H���g)

	glEnable(GL_LIGHTING);//�����̗L����
	glEnable(GL_LIGHT0);
}


//------------- ���C���֐� ----------------//
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	GLUT_INIT();
	GLUT_CALL_FUNC();
	MY_INIT();

	glutMainLoop();

	return 0;
}

//------------ ��������R�[���o�b�N�֐� ------------------//
void display()
{
	static int r = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();   //�s��ޔ�
	glRotated(double(r), 0.0, 1.0, 0.0); //[2].��]
	glutSolidTorus(0.3, 0.5, 30, 30);
	glPopMatrix(); //�s���߂�


	glutSwapBuffers();

	if (++r > 360) {
		r = 0;
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION); //�s�񃂁[�h�ؑ�
	glLoadIdentity();  //�s�񏉊���
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //�s�񃂁[�h�ؑ�
	glLoadIdentity();
	gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void timer(int t)
{
	glutPostRedisplay();
	glutTimerFunc(t, timer, 17); //�^�C�}�[�֐�
}