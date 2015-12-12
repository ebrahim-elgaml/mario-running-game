#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
//#include "TextureBuilder.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#define GLUT_KEY_ESCAPE 27
using namespace std;
//g++ o.cpp -o gl  -std=c++11 -lGL -lGLU -lglut -lGLEW
void reactivateSmallObstacle(double, double);
void reactivateLargeObstacle(double, double);
void drawAllLanes();
void drawObstacles();
void generateFirstObstacle();
void generateSecondObstacle(int);
void isCollide();
void generateCoin();
void generateCoinsLine(double);
int WIDTH = 1280;
int HEIGHT = 720;
char title[] = "Mario in Egypt";
double rot = 0;
// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {x = y = z = 0;}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
	void incrementZ(GLdouble _z){
		z+=_z;
	}
};
class Lane
{
	public:
		GLdouble z;
		bool selected = false;
		bool drawBool = true;
		Lane() {z = 0;}
		Lane(GLdouble _z) : z(_z) {}
		Lane(GLdouble _z, bool s) : z(_z) {selected = s;}
		Lane(GLdouble _z, bool s, bool d) : z(_z) {selected = s; drawBool = d;}
		void draw(){
			glColor3f(0.5,0.5,0.5);
			glPushMatrix();
			glTranslated(0,0,1*z);
			drawGround();
			glPopMatrix();
		}
		void drawLan(){
			glPushMatrix();
			glScaled(2, 1, 50);
			glTranslated(0, 0, 0);
			glutSolidCube(1);
			glPopMatrix();
		}
		void drawGround(){
			drawLan();
			glPushMatrix();
			glTranslated(2,0,0);
			drawLan();
			glPopMatrix();
			glPushMatrix();
			glTranslated(-2,0,0);
			drawLan();
			glPopMatrix();
		}
};
class Target
{
	public:
		GLdouble z;
		Target() {z = 0;}
		Target(GLdouble _z) : z(_z) {}
		void draw(){
			glColor3f(1,0,0);
			glPushMatrix();
			glTranslated(0,0,-10+1*z);
			drawHouse();
			glPopMatrix();
		}
		void drawHouse(){
			glPushMatrix();
			glScaled(6, 10, 1);
			glTranslated(0, 0, 0);
//			glutSolidCube(1);
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0); // Set quad normal direction.
			glVertex3f(-0.5, 0, 0);
			glVertex3f(0.5, 0, 0);
			glVertex3f(0.5, 50, 0);
			glVertex3f(-0.5, 50, 0);
			glEnd();
			glPopMatrix();
		}
};
enum lanePosition { LEFT, MIDDLE, RIGHT };
enum direction { MOVELEFT, MOVERIGHT };

class Mario
{
	public:
		GLdouble z;
		GLdouble x = 0;
		GLdouble y = 2;
		lanePosition position = MIDDLE;
		Mario() {z= x = 0; }
		Mario(GLdouble _z) : z(_z){}
		void draw(){
			glPushMatrix();
			glColor3f(1,0,0);
			glTranslated(x,y,z);
			glutSolidSphere(0.5,10,10);
			glPopMatrix();
		}
		void setLane(){
			if(x > -0.2 && x <0.2){
				x = 0;
				position = MIDDLE;
			}else if(x > 1.3 && x <1.8){
				position = RIGHT;
				x = 1.5;
			}else if(x > -1.7 && x <-1.3){
				position = LEFT;
				x = -1.5;
			}
		}
		bool validMove(direction d){
			if(d == MOVERIGHT && position == RIGHT){
				return false;
			}
			if(d == MOVELEFT && position == LEFT){
				return false;
			}
			return true;
		}
};


class SmallObstacle
{
	public:
		GLdouble z;
		GLdouble x ;
		GLdouble y = 2;
		lanePosition position = MIDDLE;
		bool drawBool = false;
		SmallObstacle() {z= x = 0; setLane(); }
		SmallObstacle(GLdouble _z, GLdouble _x) : z(_z), x(_x){setLane();}
		void draw(){
			glPushMatrix();
			glColor3f(0, 1, 0);
			GLUquadricObj * qobj;
			qobj = gluNewQuadric();
			glTranslated(x, y, z);
			glRotated(90,1,0,0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluCylinder(qobj, 0.5, 0.5, 1, 100,100);
			glPopMatrix();
		}
		void setLane(){
			if(x > -0.2 && x <0.2){
				x = 0;
				position = MIDDLE;
			}else if(x > 1.3 && x <1.8){
				position = RIGHT;
				x = 1.5;
			}else if(x > -1.7 && x <-1.3){
				position = LEFT;
				x = -1.5;
			}
		}
		void activateAgain(double _z, double _x){
			z = _z;
			x = _x;
			drawBool = true;
			setLane();
		}
};
class LargeObstacle
{
	public:
		GLdouble z;
		GLdouble x ;
		GLdouble y = 4;
		lanePosition position = MIDDLE;
		bool drawBool = false;
		LargeObstacle() {z= x = 0; setLane(); }
		LargeObstacle(GLdouble _z, GLdouble _x) : z(_z), x(_x){setLane();}
		void draw(){
			glPushMatrix();
			glColor3f(0, 1, 0);
			GLUquadricObj * qobj;
			qobj = gluNewQuadric();
			glTranslated(x, y, z);
			glRotated(90,1,0,0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluCylinder(qobj, 0.5, 0.5, 4, 300,300);
			glPopMatrix();
		}
		void setLane(){
			if(x > -0.2 && x <0.2){
				x = 0;
				position = MIDDLE;
			}else if(x > 1.3 && x <1.8){
				position = RIGHT;
				x = 1.5;
			}else if(x > -1.7 && x <-1.3){
				position = LEFT;
				x = -1.5;
			}
		}
		void activateAgain(double _z, double _x){
			z = _z;
			x = _x;
			drawBool = true;
			setLane();
		}
//		void activateAgain(double _z, double _x){
//			z = _z;
//			x= _x;
//			setLane();
//			drawBool = true;
//		}
};

class Coin
{
	public:
		GLdouble z;
		GLdouble x ;
		GLdouble y = 2;
		bool taken = false;
		lanePosition position = MIDDLE;
		Coin() {z= x = 0; setLane(); }
		Coin(GLdouble _z, GLdouble _x) : z(_z), x(_x){setLane();}
		void draw(){
			glPushMatrix();
			glColor3f(1, 200.0/255, 0);
			glTranslated(x, y, z);
			glRotated(rot, 0, 1, 0);
			GLUquadricObj * qobj;
			qobj = gluNewQuadric();
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluDisk(qobj, 0, 0.5,100,100);
			glPopMatrix();
		}
		void setLane(){
			if(x > -0.2 && x <0.2){
				x = 0;
				position = MIDDLE;
			}else if(x > 1.3 && x <1.8){
				position = RIGHT;
				x = 1.5;
			}else if(x > -1.7 && x <-1.3){
				position = LEFT;
				x = -1.5;
			}
		}
};
//Vector Eye(20, 5, 20);
//void pushSmallObstacle(SmallObstacle);
void drawAllCoins();
bool ableToBuildAt(double, lanePosition);
Vector Eye(0, 5, 10);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);
//vector<Lane> lanes;
Lane myLanes[2]= {Lane(Eye.z - 10,true,true), Lane(Eye.z - 10,false,false)};
//LargeObstacle myLargeObstacles[2] = {LargeObstacle(0,0), LargeObstacle(0,0)};
//vector<SmallObstacle> smallObstacles;
SmallObstacle mySmallObstacles[2] = {SmallObstacle(0, 0), SmallObstacle(0, 0)};
LargeObstacle myLargeObstacles[2] = {LargeObstacle(0, 0), LargeObstacle(0, 0)};
//vector<LargeObstacle> largeObstacles;
vector<Coin> coins ;
double moveZ = -0.1;
int cameraZoom = 0;
double initialZ = Eye.z;
Mario mario(5);
double initialMario = 0;
bool moveLeftBool = false;
bool moveRightBool = false;
bool jumping = false;
double moveY = 0.1;
int myTime = 0;
double wordZ = 0;
bool lose = false;
int myScore = 0;
Target target(0);
//double wordY = 2;
// Model Variables
//Model_3DS model_house;
//Model_3DS model_tree;
// Textures
//GLTexture tex_ground;
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}
void writeWord(int x , int y , int z , string s){
		glPushMatrix();
		//glMatrixMode(GL_MODELVIEW);
		glClear(GL_COLOR_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);
		//glRasterPos3i(x, y,z);
		//glDepthMask(GL_FALSE);
		//glDisable(GL_DEPTH_TEST);
		//glRasterPos2d(x,y);
		glRasterPos3d(x,y,z);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		for(int i = 0; s[i]!='\0';i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
		}
		//glEnable(GL_DEPTH_TEST);

		glPopMatrix();




//	glPushMatrix();
//	//glMatrixMode(GL_MODELVIEW);
//	glClear(GL_COLOR_BUFFER_BIT);
//	//glDisable(GL_DEPTH_TEST);
//	//glRasterPos3i(x, y,z);
//	//glDepthMask(GL_FALSE);
//	//glDisable(GL_DEPTH_TEST);
//	//glRasterPos2d(x,y);
//	glRasterPos3d(x,y,z);
//	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
//	for(int i = 0; s[i]!='\0';i++){
//		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
//	}
//	//glEnable(GL_DEPTH_TEST);
//
//	glPopMatrix();

}
//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}
//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(92.0/256, 242.0/256, 240.0/256, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	//glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	myInit();
	//writeWord(-3,4,Eye.z,"hello");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	glDepthMask(GL_FALSE);
	if(lose){
		writeWord(-2, 4, wordZ, "Game Over with score "+to_string(myScore));
	}else{
		writeWord(-2, 4, wordZ, to_string(myScore));
	}
	//writeWord(-2, 4, wordZ, to_string(mySmallObstacles[0].drawBool));
	glDepthMask(GL_TRUE);
	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	// Draw Ground
	//RenderGround();

	// Draw House Model
//	glPushMatrix();
//	glTranslatef(10, 0, 0);
//	glScalef(0.7, 0.7, 0.7);
//	//model_tree.Draw();
//	glPopMatrix();

	// Draw Tree Model
//	glPushMatrix();
//	glRotatef(90.f, 1, 0, 0);
//	//model_house.Draw();
//	glPopMatrix();
//	drawGround();
	//Lane l(0);
	//l.draw();
//	if(lanes.size()>0){
//		lanes[0].draw();
//		Lane l(0);
//		l.draw();
//	}
	drawAllLanes();
	mario.draw();
	drawObstacles();
	drawAllCoins();
	target.draw();
	glutSwapBuffers();

}
void drawAllCoins(){
	for(Coin c: coins){
		c.draw();
	}
}
void initGround(){
	if(Eye.z <= initialZ -15 ){
		if(myLanes[0].selected){
			myLanes[0].selected = false;
			myLanes[1].z = Eye.z - 10;
			myLanes[1].selected = true;
		}else{
			myLanes[1].selected = false;
			myLanes[0].z = Eye.z - 10;
			myLanes[0].selected = true;
		}
		myLanes[1].drawBool = true;
		initialZ = Eye.z;
	}
	if(!lose){
		target.z += moveZ;
	}
//	if(lanes.size() == 0){
//		lanes.push_back(Lane(Eye.z - 10));
//	}else{
//		if(Eye.z <= initialZ -15 ){
//			if(myLanes[0].selected){
//				myLanes[0].selected = false;
//				myLanes[1].z = Eye.z - 10;
//			}else{
//				myLanes[1].selected = false;
//				myLanes[0].z = Eye.z - 10;
//			}
////			lanes.pop_back();
////			lanes.push_back(Lane(Eye.z - 10));
////			initialZ = Eye.z;
//		}
//	}
}
void removeObstacles(){
//	for(int i = 0 ; i<smallObstacles.size(); i++){
//		SmallObstacle s = smallObstacles.at(i);
//		if(s.z > mario.z){
//			smallObstacles.erase(smallObstacles.begin() + i);
//		}
//	}
	if(mySmallObstacles[0].drawBool && mySmallObstacles[0].z>mario.z){
		mySmallObstacles[0].drawBool = false;
	}
	if(mySmallObstacles[1].drawBool && mySmallObstacles[1].z>mario.z){
		mySmallObstacles[1].drawBool = false;
	}
	if(myLargeObstacles[0].drawBool && myLargeObstacles[0].z>mario.z){
		myLargeObstacles[0].drawBool = false;
	}
	if(myLargeObstacles[1].drawBool && myLargeObstacles[1].z>mario.z){
		myLargeObstacles[1].drawBool = false;
	}
//	for(int i = 0 ; i<largeObstacles.size(); i++){
//		LargeObstacle s = largeObstacles.at(i);
//		if(s.z > mario.z ){
//			largeObstacles.erase(largeObstacles.begin() + i);
//		}
//	}
//	for(int i = 0 ; i<2; i++){
//		LargeObstacle s = myLargeObstacles[i];
//		if(s.z > mario.z && s.drawBool){
//			s.drawBool = false;
//			//largeObstacles.erase(largeObstacles.begin() + i);
//		}
//	}
}
void removeCoins(){
	for(int i = 0 ; i<coins.size(); i++){
		Coin s = coins.at(i);
		if(s.z > mario.z+1.5){
			coins.erase(coins.begin() + i);
		}
	}
}
//void pushSmallObstacle(SmallObstacle s){
//	if(ableToBuildAt(s.z, s.position)){
//		smallObstacles.push_back(s);
//	}
//}
void reactivateSmallObstacle(double z, double x){
	if(!mySmallObstacles[0].drawBool){
		mySmallObstacles[0].activateAgain(z, x);
		return;
	}
	if(!mySmallObstacles[1].drawBool){
		mySmallObstacles[1].activateAgain(z, x);
		return;
	}
}
void reactivateLargeObstacle(double z, double x){
	if(!myLargeObstacles[0].drawBool){
		myLargeObstacles[0].activateAgain(z, x);
		return;
	}
	if(!myLargeObstacles[1].drawBool){
		myLargeObstacles[1].activateAgain(z, x);
		return;
	}
}
//void pushLargeObstacle(LargeObstacle s){
//	if(ableToBuildAt(s.z, s.position)){
//		largeObstacles.push_back(s);
//	}
//}
void generateObstacle(){
	generateFirstObstacle();
}
//void activateLargeObstacle(double z, double position){
//	if(!myLargeObstacles[0].drawBool){
//		myLargeObstacles[0].activateAgain(z, position);
//		return;
//	}
//	if(!myLargeObstacles[1].drawBool){
//		myLargeObstacles[1].activateAgain(z, position);
//		return;
//	}
//}
void generateFirstObstacle(){
	// 0 --> smallObject.
	// 1 --> largeObject.
	int t = rand() % 2;
	if(t==0){
		// 0 --> left.
		// 1 --> middle.
		// 2 --> right.
		t = rand() % 3 + 1;
		switch(t){
			case 1 : reactivateSmallObstacle(Eye.z - 20, -1.5);break;
			case 2 : reactivateSmallObstacle(Eye.z - 20, 0);break;
			case 3 : reactivateSmallObstacle(Eye.z - 20, 1.5);break;
			default : reactivateSmallObstacle(Eye.z - 20, 1.5);
		}
		generateSecondObstacle(t);
	}else{
		// 0 --> left.
		// 1 --> middle.
		// 2 --> right.
		t = rand() % 3 +1;
		switch(t){
			case 1 : reactivateLargeObstacle(Eye.z - 20, -1.5); ;break;
			case 2 : reactivateLargeObstacle(Eye.z - 20, 0);break;
			case 3 : reactivateLargeObstacle(Eye.z - 20, 1.5);break;
			default : reactivateLargeObstacle(Eye.z - 20, 1.5);
		}
		generateSecondObstacle(t);
	}
}
void generateSecondObstacle(int last){
	// 0 --> smallObject.
	// 1 --> largeObject.
	int t = rand() % 2;
	if(t==0){
		// 0 --> left.
		// 1 --> middle.
		// 2 --> right.
		t = rand() % 3 +1;
		while(t == last){
			t = rand() % 3 +1;
		}
		switch(t){
			case 1 : reactivateSmallObstacle(Eye.z - 20, -1.5);break;
			case 2 : reactivateSmallObstacle(Eye.z - 20, 0);break;
			case 3 : reactivateSmallObstacle(Eye.z - 20, 1.5);break;
			default : reactivateSmallObstacle(Eye.z - 20, 1.5);
		}
	}else{
		// 0 --> left.
		// 1 --> middle.
		// 2 --> right.
		t = rand() % 3 +1;
		while(t == last){
			t = rand() % 3 +1;
		}
		switch(t){
			case 1 : reactivateLargeObstacle(Eye.z - 20, -1.5); ;break;
			case 2 : reactivateLargeObstacle(Eye.z - 20, 0);break;
			case 3 : reactivateLargeObstacle(Eye.z - 20, 1.5);break;
			default : reactivateLargeObstacle(Eye.z - 20, 1.5);
		}
	}
}

void removeLanes(){
	// TODO remove lanes that camera has finished
}
bool ableToBuildAt(double z, lanePosition p){
//	for(LargeObstacle l : largeObstacles){
//		if(l.position == p && l.z >= z-0.2 && l.z <= z+0.2){
//			return false;
//		}
//	}
//	for(SmallObstacle l : smallObstacles){
//		if(l.position == p && l.z >= z-0.2 && l.z <= z+0.2){
//			return false;
//		}
//	}
	for(LargeObstacle l : myLargeObstacles){
		if(l.drawBool && l.position == p && l.z >= z-0.2 && l.z <= z+0.2){
			return false;
		}
	}
	for(SmallObstacle l : mySmallObstacles){
		if(l.drawBool && l.position == p && l.z >= z-0.2 && l.z <= z+0.2){
			return false;
		}
	}
	for(Coin l : coins){
		if(l.position == p && l.z >= z-0.2 && l.z <= z+0.2){
			return false;
		}
	}
	return true;
}
void isCollide(){
//	for(LargeObstacle l : largeObstacles){
//		if(l.position == mario.position && l.z >= mario.z-0.2 && l.z <= mario.z+0.2){
//			lose = true;
//			return;
//		}
//	}
//	for(SmallObstacle s : smallObstacles){
//		if(s.position == mario.position && s.z >= mario.z-0.2 && s.z <= mario.z+0.2 && !jumping){
//			lose = true;
//			return;
//		}
//	}
	for(LargeObstacle l : myLargeObstacles){
		if(l.drawBool &&l.position == mario.position && l.z >= mario.z-0.2 && l.z <= mario.z+0.2){
			lose = true;
			return;
		}
	}
	for(SmallObstacle s : mySmallObstacles){
		if(s.drawBool && s.position == mario.position && s.z >= mario.z-0.2 && s.z <= mario.z+0.2 && !jumping){
			lose = true;
			return;
		}
	}
}

void CollideCoin(){
	for(int i = 0 ; i< coins.size(); i++){
		Coin c = coins.at(i);
		if(c.position == mario.position && c.z >= mario.z-0.1 && c.z <= mario.z+0.1 && !jumping){
			myScore +=5;
			coins.erase(coins.begin()+i);
		}
	}
}


void generateCoin(){
	int t = rand() % 3 + 1;
	switch(t){
		case 1 : generateCoinsLine(-1.5);break;
		case 2 : generateCoinsLine(0);break;
		case 3 : generateCoinsLine(1.5);break;
		default : generateCoinsLine(1.5);
	}
}
void pushCoin(Coin c){
	if(ableToBuildAt(c.z, c.position)){
		coins.push_back(c);
	}
}
void generateCoinsLine(double x){
	pushCoin(Coin(Eye.z - 17, x ));
	pushCoin(Coin(Eye.z - 20, x ));
	pushCoin(Coin(Eye.z - 23, x ));
}

void drawAllLanes(){
//	for(Lane l : lanes){
//		if(l.drawBool){
//			l.draw();
//		}
//	}
	myLanes[0].draw();
	if(myLanes[1].drawBool){
		myLanes[1].draw();
	}
}
void drawObstacles(){
//	for(SmallObstacle s : smallObstacles){
//		s.draw();
//	}
	for(SmallObstacle s : mySmallObstacles){
		if(s.drawBool){
			s.draw();
		}
	}
	for(LargeObstacle s : myLargeObstacles){
		if(s.drawBool){
			s.draw();
		}
	}
//	for(LargeObstacle s : myLargeObstacles){
//		if(s.drawBool){
//			s.draw();
//		}
//	}
}


//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'a':
		if(!moveLeftBool && !moveRightBool){
			direction d = MOVELEFT;
			if(mario.validMove(d)){
				moveLeftBool = true;
				initialMario = mario.x;
			}
		}
		break;
	case 'd':
		if(!moveLeftBool && !moveRightBool){
			direction d = MOVERIGHT;
			if(mario.validMove(d)){
				moveRightBool = true;
				initialMario = mario.x;
			}
		}
		break;
	case 32:
		jumping = true;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
void keySpecial (int key, int x, int y) {
	if(key == GLUT_KEY_LEFT){
		if(!moveLeftBool && !moveRightBool){
			direction d = MOVELEFT;
			if(mario.validMove(d)){
				moveLeftBool = true;
				initialMario = mario.x;
			}
		}
	}else if (key == GLUT_KEY_RIGHT){
		if(!moveLeftBool && !moveRightBool){
			direction d = MOVERIGHT;
			if(mario.validMove(d)){
				moveRightBool = true;
				initialMario = mario.x;
			}
		}
	}else if (key == GLUT_KEY_UP){
		jumping = true;
	}

}
//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	//model_house.Load("Models/house/house.3ds");
	//model_tree.Load("Models/tree/tree1.3ds");

	// Loading texture files
	//tex_ground.Load("Textures/ground.bmp");
}
void moveMario(){
	if(moveRightBool){
			mario.x += 0.1;
			if(mario.x - initialMario >= 1.5){
				mario.setLane();
				initialMario = 0;
				moveRightBool = false;
			}
		}
		if(moveLeftBool){
			mario.x -= 0.1;
			if(initialMario - mario.x  >= 1.5){
				mario.setLane();
				initialMario = 0;
				moveLeftBool = false;
			}
		}
		if(jumping){
			if(mario.y >= 4){
				moveY = -0.1;
			}
			mario.y += moveY;
			if(mario.y<= 2){
				mario.y = 2;
				jumping = false;
				moveY = 0.1;
			}
		}

}
void Anim(){
	rot+=0.5;
	initGround();
	removeLanes();
	if(!lose){
		Eye.incrementZ(moveZ);
		At.incrementZ(moveZ);
		mario.z += moveZ;
		wordZ += moveZ;
		moveMario();
		removeObstacles();
		removeCoins();
		isCollide();
		CollideCoin();
	}
	glutPostRedisplay();
}
void Timer(int value) {
  // ask OpenGL to recall the display function to reflect the changes on the window
  glutPostRedisplay();
  if(!lose){
	  myTime+=1;
	  if(myTime % 4 == 0){
		  generateObstacle();
	  }else if(myTime%2 == 0 ){
		  generateCoin();
	  }
  }
  // recall the Timer function after 20 seconds (20,000 milliseconds)
  glutTimerFunc(1000, Timer, 0);
}

//=======================================================================
// Main Function
//=======================================================================
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(keySpecial);

	glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);
	glutIdleFunc(Anim);
	glutTimerFunc(0, Timer, 0);
//	myInit();

	LoadAssets();

	glutMainLoop();
	return 0;
}
