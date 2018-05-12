#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>


#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <GL/wglew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


using namespace std;

#define max(a,b)            (((a) > (b)) ? (a) : (b))

GLuint g_programID;


GLuint VertexBufferID[2];
GLuint VertexArrayID;
GLuint VertexArrayID_2;

GLuint IndexBufferID;


vector<float> g_vertices;


vector<byte*> g_indices;

bool is2D = true;
bool is3D = false;


glm::mat4 M;
//x �� -0.5 �̵��� ���
GLfloat matv[2][16] = {
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	},
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0.5, 0, 0, 1
	}
};
GLfloat iMat[] = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};


bool isAxisRotate = false;
bool isMoved[2] = { false, false };

int x_rotate_count = 0;
int y_rotate_count = 0;

vector<byte> LineLoopToUnitObj();

vector<byte> indices;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}
	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open())
	{
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	//Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void rotate() {
	glm::mat4 x_mat_rotate = glm::rotate(glm::mat4(), glm::radians(10.0f * x_rotate_count), glm::vec3(1, 0, 0));//x�� ���� 90�� �����ڴ�
	glm::mat4 y_mat_rotate = glm::rotate(glm::mat4(), glm::radians(10.0f * y_rotate_count), glm::vec3(0, 1, 0));
	M = glm::matrixCompMult(x_mat_rotate, y_mat_rotate);

}
void rotate(float ax_x, float ax_y, float ax_z) {

	
	//glUniformMatrix4fv(rotateLoc, 1, GL_FALSE, &mat_rotate[0][0]);


	/*glm::vec3 x, xp, y, z;

	z = glm::vec3(ax_x, ax_y, ax_z);
	z = glm::normalize(z);
	xp = glm::vec3(0, 0, 1);
	y = glm::cross(z, xp);
	x = glm::cross(y, z);

	y = glm::normalize(y);
	x = glm::normalize(x);

	M[0] = glm::vec4(x, 0);
	M[1] = glm::vec4(y, 0);
	M[2] = glm::vec4(z, 0);
	M[3] = glm::vec4(0, 0, 0, 1);*/


	//glm::vec3 Z = glm::vec3(nx, ny,nx+ny);//�ٶ󺸴� ����?
	//Z= glm::normalize(Z);
	//glm::vec3 Xp = glm::vec3(1, 1, 1);//���� ���� ��
	//glm::vec3 Y = glm::cross(Z,Xp);
	//glm::vec3 X = glm::cross(Y, Z);
	//Y = glm::normalize(Y);
	//X= glm::normalize(X);
	//glm::mat4 glmatRotate = glm::mat4 (glm::vec4(X, 0), glm::vec4(Y, 0), glm::vec4(Z, 0), glm::vec4(0, 0, 0, 1));
	//GLuint rotateLoc = glGetUniformLocation(g_programID, "rotate");
	//glUniformMatrix4fv(rotateLoc, 1, GL_FALSE, &glmatRotate[0][0]);
}

void myMouse(int button, int state, int x, int y) {

	static float r = 1.0;
	static float g = 0.0;
	static float b = 0.0;

	//rotate(x, y, 0);

	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		float nx, ny;

		nx = 2.0 * (float)x / (float)479 - 1.0;
		ny = -2.0 * (float)y / (float)479 + 1.0;

		r = (rand() % 100) / 99.f;
		g = (rand() % 100) / 99.f;
		b = (rand() % 100) / 99.f;
		
		if (is2D) {
			g_vertices.push_back(nx);
			g_vertices.push_back(ny);
			g_vertices.push_back(0);
			g_vertices.push_back(r);
			g_vertices.push_back(g);
			g_vertices.push_back(b);
		}
		//rotate(nx, ny, 0);
		//isAxisRotate = true;

		glutPostRedisplay(); //���� �ٽ� �׸���
	}
}

void renderScene(void)
{
	//�Է¹��� ���� ����ϱ� ( vertex shader�� ���)

	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Let's draw something here
	//Draw Line

	glBindVertexArray(VertexArrayID);

	
	//ȸ��
	GLuint axisLoc = glGetUniformLocation(g_programID, "rot");
	glUniformMatrix4fv(axisLoc, 1, GL_FALSE, &M[0][0]);


	GLuint moveLoc = glGetUniformLocation(g_programID, "move");
	GLuint colLoc = glGetAttribLocation(g_programID, "iColor");
	
	glUniformMatrix4fv(moveLoc, 1, GL_FALSE, matv[0]);
	
	//2D Line Loop
	if (is2D) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*g_vertices.size(), g_vertices.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINE_LOOP, 0, (g_vertices.size() / 6));
	}
	else {

		if (!is3D) {
			indices = LineLoopToUnitObj();
			is3D = true;
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(byte) * indices.size(), indices.data(), GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, 0);
	}
	//glUniformMatrix4fv(moveLoc, 1, GL_FALSE, iMat);

	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
	

	//Double buffer
	glutSwapBuffers();
}

vector<byte> LineLoopToUnitObj() {
	int polygon_level = g_vertices.size() / 6;
	int initial_size = g_vertices.size();
	//create back area
	for (int i = 0; i < initial_size; i += 6) {
		g_vertices.push_back(g_vertices.at(i));
		g_vertices.push_back(g_vertices.at(i + 1));
		g_vertices.push_back(g_vertices.at(i + 2) + 0.5);
		g_vertices.push_back(g_vertices.at(i + 3));
		g_vertices.push_back(g_vertices.at(i + 4));
		g_vertices.push_back(g_vertices.at(i + 5));
		
	}

	//calculate center of front and back (average)
	float total_x = 0;
	float total_y = 0;
	for (int i = 0; i < initial_size; i += 6) {
		total_x += g_vertices.at(i);
		total_y += g_vertices.at(i + 1);
	}
	float avg_x = total_x/(float)polygon_level;
	float avg_y = total_y/(float)polygon_level;

	g_vertices.push_back(avg_x);
	g_vertices.push_back(avg_y);
	g_vertices.push_back(0);
	g_vertices.push_back(0);
	g_vertices.push_back(0);
	g_vertices.push_back(0);

	g_vertices.push_back(avg_x);
	g_vertices.push_back(avg_y);
	g_vertices.push_back(0.1);
	g_vertices.push_back(0);
	g_vertices.push_back(0);
	g_vertices.push_back(0);

	vector<byte> indices;

	//create indices

	//front indice
	for (int i = 0; i < polygon_level-1; i ++) {
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(g_vertices.size()-2);
	}
	indices.push_back(polygon_level);
	indices.push_back(0);
	indices.push_back(g_vertices.size() - 2);
	//back indice
	for (int i = polygon_level+1; i < polygon_level * 2 - 1; i++) {
		indices.push_back(i);
		indices.push_back(g_vertices.size()-1);
		indices.push_back(i + 1);
	}
	indices.push_back(polygon_level * 2 -1);
	indices.push_back(g_vertices.size() - 1);
	indices.push_back(polygon_level);
	//side indice
	for (int i = 0; i < polygon_level - 1; i++) {
		indices.push_back( i );
		indices.push_back( i + polygon_level );
		indices.push_back( (i + 1) + polygon_level );

		indices.push_back( i );
		indices.push_back((i + 1) + polygon_level);
		indices.push_back( i + 1 );
	}
	indices.push_back(polygon_level - 1);
	indices.push_back(polygon_level * 2 - 1);
	indices.push_back(polygon_level);

	indices.push_back(polygon_level - 1);
	indices.push_back(polygon_level);
	indices.push_back(0);

	
	return indices;

}


void init()
{
	//initilize the glew and check the errors.

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
	}

	//select the background color

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);


	glEnable(GL_DEPTH_TEST);	//���� Ȱ��ȭ
								//glDepthRange(-0.1, 1.0);
								//glDepthFunc(GL_GREATER);
}

void specialKey(int key, int x, int y) {  
	switch (key) {
	case GLUT_KEY_ALT_L:
		is2D = !is2D;
		break;
	case GLUT_KEY_LEFT: --y_rotate_count;
		break;
	case GLUT_KEY_RIGHT: ++y_rotate_count;
		break;
	case GLUT_KEY_UP: ++x_rotate_count;
		break;
	case GLUT_KEY_DOWN: --x_rotate_count;
		break;
	}
	if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT || key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
		rotate();

	glutPostRedisplay();
}  
void myKey(unsigned char key, int x, int y) {
	
	if (key == 'q' || key == 'w' || key == 'a' || key == 's' || key == 'z' || key == 'x') {
		isMoved[0] = true;
		isMoved[1] = false;
	}
	else if (key == 'e' || key == 'r' || key == 'd' || key == 'f' || key == 'c' || key == 'v') {
		isMoved[1] = true;
		isMoved[0] = false;
	}
	switch (key) {
	case 'q':matv[0][13] += 0.1;
		break;
	case 'w':matv[0][13] -= 0.1;
		break;
	case 'e':matv[1][13] += 0.1;
		break;
	case 'r':matv[1][13] -= 0.1;
		break;
	case 'a':matv[0][12] -= 0.1;
		break;
	case 's':matv[0][12] += 0.1;
		break;
	case 'd':matv[1][12] -= 0.1;
		break;
	case 'f':matv[1][12] += 0.1;
		break;
	case 'z':matv[0][14] -= 0.1;
		break;
	case 'x':matv[0][14] += 0.1;
		break;
	case 'c':matv[1][14] -= 0.1;
		break;
	case 'v':matv[1][14] += 0.1;
		break;

	}

	glutPostRedisplay();
}
void main(int argc, char **argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed) 
	//GLUT_DEPTH enables Depth buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(480, 480);
	//This is used to define the name of the window.
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();

	glEnable(GL_POINT_SPRITE);

	//1.
	//Generate VAO
	//GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenVertexArrays(1, &VertexArrayID_2);

	//3. 
	g_programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	glUseProgram(g_programID);

	//Cube
	/*float vtxData[] = {
		0.2 , 0.2 , 0.2 , 0.0 , 0.0 , 0.0 ,
		-0.2, 0.2 , 0.2 , 0.0 , 0.0 , 1.0 ,
		-0.2,-0.2 , 0.2 , 0.0 , 1.0 , 0.0 ,
		0.2 ,-0.2 , 0.2 , 0.0 , 1.0 , 1.0 ,
		0.2 ,-0.2 ,-0.2 , 1.0 , 0.0 , 0.0 ,
		0.2 , 0.2 ,-0.2 , 1.0 , 0.0 , 1.0 ,
		-0.2, 0.2 ,-0.2 , 1.0 , 1.0 , 0.0 ,
		-0.2,-0.2 ,-0.2 , 1.0 , 1.0 , 1.0
	};*/


	/*byte idxData[] = {
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,1,
		1,5,6,
		1,6,7,
		1,7,2,
		2,3,4,
		2,4,7,
		4,7,6,
		4,6,5
	};*/

	//g_indices.push_back(idxData);


	//Create Vertex Buffer
	//GLuint VertexBufferID;
	glGenBuffers(2, VertexBufferID);



	GLuint posLoc;

	posLoc = glGetAttribLocation(g_programID, "pos");
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[0]);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)(0));
	glEnableVertexAttribArray(posLoc);


	//Color
	GLuint colLoc = glGetAttribLocation(g_programID, "iColor");
	glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(colLoc);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 36, vtxData, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 8, vtxData, GL_STATIC_DRAW);


	//Create Index Buffer & Bind
	glGenBuffers(1, &IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	


	glutMouseFunc(myMouse);	//���콺 �ݹ� ���
	glutKeyboardFunc(myKey);
	glutSpecialFunc(specialKey);

	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteVertexArrays(1, &VertexArrayID_2);


}

