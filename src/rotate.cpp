#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <string.h>

#include "model.hpp"

using namespace std;

bool fullscreen = false;
bool mouseDown = false;
float xrot = 0.0f;
float yrot = 0.0f;
float xdiff = 0.0f;
float ydiff = 0.0f;
float xcam = 0.0f;
float ycam = 0.0f;
float zcam = 8.0f;
float zoom = 1.0f;
float xup = 0.0f;
float yup = 1.0f;
float zup = 0.0f;
bool help = true;
MapModel model;

// Two dimensional rotate
vector<float> rotatePoint(float cx,float cy,float angle, float x, float y) {
    const auto PI = std::acos(-1); // find PI
    float sinAngle = sin(angle * PI/180);
    float cosAngle = cos(angle * PI/180);

    // translate point to origin
    x -= cx;
    y -= cx;

    // rotate point
    vector<float> point = {
        (x * cosAngle - y * sinAngle) + cx, (x * sinAngle + y * cosAngle) + cy
    };

    return point;

}

vector<float> crossProduct(vector<float> v_A, vector<float> v_B) {
    vector<float> result;
    result.push_back(v_A[1] * v_B[2] - v_A[2] * v_B[1]);
    result.push_back(-(v_A[0] * v_B[2] - v_A[2] * v_B[0]));
    result.push_back(v_A[0] * v_B[1] - v_A[1] * v_B[0]);
    return result;
}

vector<float> calculateNormal(vector<float> vBefore, vector<float> v, vector<float> vAfter) {
    // v1 is vAfter - v
    vector<float> v1 = {
        vAfter[0] - v[0], vAfter[1] - v[1], vAfter[2] - v[1]
    };

    // v2 is vBefore - v
    vector<float> v2 = {
        vBefore[0] - v[0], vBefore[1] - v[1], vBefore[2] - v[2]
    };

    vector<float> vResult = crossProduct(vBefore, vAfter);
    return vResult;
}

void drawPolygonFromModel() {
    for (auto item = model.begin(); item != model.end(); item++) {
        glPushMatrix();
        if ("silinder" == item->first) {
            GLUquadricObj *quadric;
            quadric = gluNewQuadric();
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(item->second[0][0], item->second[0][1], item->second[0][2]);
            glColor3f(item->second[1][0], item->second[1][1], item->second[1][2]);
            gluCylinder(quadric, 0.1f , 0.5f, 0.15f, 32, 32);
        } else {
            bool lastEl = false;
            for (int i = 0; i < item->second.size()-1; i++) {
                if (i == 0) {
                    glColor3f(item->second[item->second.size()-1][0],
                              item->second[item->second.size()-1][1],
                              item->second[item->second.size()-1][2]);
                    glBegin(GL_POLYGON);
                } else if (i == item->second.size() - 2) {
                    lastEl = true;
                }
                else  if (i % 4 == 0) {
                    glEnd();
                    glBegin(GL_POLYGON);
                }
                // Calculate normal for every odd index in item
                if ((i != 0) && (i != item->second.size()-2)) {
                    vector<float> normalVector = calculateNormal(
                                                                    item->second[i-1],
                                                                    item->second[i],
                                                                    item->second[i+1]
                                                                );
                    glNormal3f(normalVector[0], normalVector[1], normalVector[2]);
                }
                glVertex3f(item->second[i][0], item->second[i][1], item->second[i][2]);
                if (lastEl) {
                    glEnd();
                }
            }
        }
        glPopMatrix();
    }
}

void loadModelFromFile() {
    Model classModel;
    classModel.loadFromFile();
    model = classModel.getMapModel();
}

bool init()
{
    GLfloat light_position[] = { 3.0, 0.0, 3.0, 0.0 }; //1.0 is point light, 0.0 is directional light
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0 };
	float specular_reflec[] = { 0.5f, 0.5f, 0.2f, 1.5 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflec);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	// Set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_SMOOTH);
    return true;
}

void reset() {
    xrot = 0.0f;
    yrot = 0.0f;
    xdiff = 0.0f;
    ydiff = 0.0f;
    xcam = 0.0f;
    ycam = 0.0f;
    zcam = 8.0f;
    zoom = 1.0f;
    xup = 0.0f;
    yup = 1.0f;
    zup = 0.0f;
}

char* strToChr(string str) {
    return const_cast<char*>(str.c_str());
}

void output(float x, float y, float z, float r, float g, float b, void* font, string string)
{
  glColor3f( r, g, b );
  glRasterPos3f(x, y, z);
  int len, i;
  len = (int)strlen(strToChr(string));
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(xcam, ycam, zcam, 0.0, 0.0, 0.0, xup, yup, zup);
    glScalef(zoom,zoom,zoom);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    drawPolygonFromModel();
    if(help){
        output(-2.0,1.8,3.0,1.0,1.0,1.0,GLUT_BITMAP_HELVETICA_18,
                "+ / -  :  Untuk melakukan perubahan radius kamera terhadap pesawat\n");
        output(-2.0,1.6,3.0,1.0,1.0,1.0,GLUT_BITMAP_HELVETICA_18,
                "Up Bottom Left Right  :  Untuk melakukan rotasi vektor\n");
        output(-2.0,1.4,3.0,1.0,1.0,1.0,GLUT_BITMAP_HELVETICA_18,
                "W A S D dan Left Button Mouse  :  Untuk memutar pesawat terhadap sumbu X, Y, Z\n");
        output(-2.0,1.2,3.0,1.0,1.0,1.0,GLUT_BITMAP_HELVETICA_18,
                "I J K L  :  Untuk menggerakkan kamera mengitari pesawat\n");
        output(-2.0,1.0,3.0,1.0,1.0,1.0,GLUT_BITMAP_HELVETICA_18,
                "F5  :  Untuk mereset view ke awal\n");
        output(-2.0,0.8,3.0,1.0,1.0,1.0,GLUT_BITMAP_HELVETICA_18,
                "H  :  Untuk membuka dan menutup menu help\n");
    } else {
        output(0.0,0.0,1000.0,1.0,1.0,1.0,GLUT_BITMAP_TIMES_ROMAN_24,"");
    }
    glFlush();
    glutSwapBuffers();
}


void resize(int width, int height)
{
    if (height == 0) height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(45.0f, aspect, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    vector<float> newPoint;
    switch (key)
    {
    case 27:
        exit(1);
        break;
    case 'w':
        xrot += 10;
        break;
    case 's':
        xrot -= 10;
        break;
    case 'a':
        yrot += 10;
        break;
    case 'd':
        yrot -= 10;
        break;
    case 'j':
        newPoint = rotatePoint(0, 0, -15.0, xcam, zcam);
        xcam = newPoint[0];
        zcam = newPoint[1];
        break;
    case 'k':
        newPoint = rotatePoint(0, 0, -15.0, xcam, ycam);
        xcam = newPoint[0];
        ycam = newPoint[1];
        break;
    case 'l':
        newPoint = rotatePoint(0, 0, 15.0, xcam, zcam);
        xcam = newPoint[0];
        zcam = newPoint[1];
        break;
    case 'i':
        newPoint = rotatePoint(0, 0, 15.0, xcam, ycam);
        xcam = newPoint[0];
        ycam = newPoint[1];
        break;
    case '=':
        zoom += 0.01f;
        break;
    case '-':
        zoom -= 0.01f;
        break;
    case 'h':
        help = !help;
        break;
    }
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
    vector<float> newPoint;
    if (key == GLUT_KEY_F1)
    {
        fullscreen = !fullscreen;
        if (fullscreen)
            glutFullScreen();
        else
        {
            glutReshapeWindow(500, 500);
            glutPositionWindow(50, 50);
        }
    }
    if (key == GLUT_KEY_UP)
    {
        newPoint = rotatePoint(0, 0, 15.0, xup, yup);
        xup = newPoint[0];
        yup = newPoint[1];
    }
    if (key == GLUT_KEY_DOWN)
    {
        newPoint = rotatePoint(0, 0, -15.0, xup, yup);
        xup = newPoint[0];
        yup = newPoint[1];
    }
    if (key == GLUT_KEY_LEFT)
    {
        newPoint = rotatePoint(0, 0, -15.0, yup, zup);
        yup = newPoint[0];
        zup = newPoint[1];
    }
    if (key == GLUT_KEY_RIGHT)
    {
        newPoint = rotatePoint(0, 0, 15.0, yup, zup);
        yup = newPoint[0];
        zup = newPoint[1];
    }
    if (key == GLUT_KEY_F5)
    {
        reset();
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseDown = true;
        xdiff = x - yrot;
        ydiff = -y + xrot;
    }
    else
        mouseDown = false;
}

void mouseMotion(int x, int y)
{
    if (mouseDown)
    {
        yrot = x - xdiff;
        xrot = y + ydiff;
        glutPostRedisplay();
    }
}

int main(int argc, char *argv[])
{
    loadModelFromFile();
    glutInit(&argc, argv);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Plane");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutReshapeFunc(resize);
    if (!init())
        return 1;
    glutMainLoop();
    return 0;
}
