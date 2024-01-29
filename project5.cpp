#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <glm.hpp>
#include <glew.h>
#include <freeglut.h> 
#include "getBMP.h"
#include <fstream>


static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
static float speed = 1.0;   // a float to determine the speed of the car
static float angleSpeed = 1.0 * M_PI / 180.0;   // determines the speed of the car rotation
static float angle = 0.0;  // angle of the car rotation measured counter-clockwise with the -ve z axis
static unsigned int car;
int anglee = 0.0; // to rotate the textures with the scene 
//array of textures 
static unsigned int texture[2];

// defining the eye, center and up vectors
glm::vec3 eye = glm::vec3(0.0, 10.0, 15);
glm::vec3 center = glm::vec3(0.0, 10.0, 0.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 lightPoss1 = glm::vec3(center.x - 2, center.y, center.z - 2 );
glm::vec3 lightPoss2 = glm::vec3(center.x + 2, center.y, center.z - 2);

static float spotAngle = 10.0; // Spotlight cone half-angle.
// to rotate the direction of the spotlight with the scene 
float trans_x1= glm::normalize(center - eye).x;
float trans_y1 = glm::normalize(center - eye).y;
float trans_z1 = glm::normalize(center - eye).z;

float trans_x2 = glm::normalize(center - eye).x;
float trans_y2 = glm::normalize(center - eye).y;
float trans_z2 = glm::normalize(center - eye).z;
static float spotExponent = 2.5; // Spotlight attenuation exponent.

// Load external textures.
void loadTextures()
{
    // Local storage for bmp image data.
    imageFile* image[2];

    // Load the images.
    image[0] = getBMP("sunset.bmp");
    image[1] = getBMP("street.bmp");

    // Bind sunset image to texture object texture[0].
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Bind street or grass image to texture object texture[1]
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

// function for drawing bitmapped text
void writeBitmapString(void* font, const char* string)
{
    const char* c;

    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// obstacle class.
class obstacle
{
public:
    obstacle();
    obstacle(float x, float y, float z);
    float getCenterX() { return centerX; }
    float getCenterY() { return centerY; }
    float getCenterZ() { return centerZ; }
    void draw();

private:
    float centerX, centerY, centerZ;
};

// obstacle default constructor.
obstacle::obstacle()
{
    centerX = 0.0;
    centerY = 0.0;
    centerZ = 0.0;
}

// obstacle constructor.
obstacle::obstacle(float x, float y, float z)
{
    centerX = x;
    centerY = y;
    centerZ = z;
}

// Function to draw obstacles.
void obstacle::draw()
{
    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glColor3f(0.0, 0.0, 1.0);
    glutSolidCube(2.0);
    glPopMatrix();
}

obstacle arrayObstacles[4];

// collision detection
bool detectCollision(void)
{
    // measuring distance between the center of the car and the obstacles
    // If the distance is equal to or less than the sum of half their widths then collison occured
    int i;
    //loop through the four obstacles
    for (i = 0; i < 4; i++) {
        float distance = glm::length(center - glm::vec3(arrayObstacles[i].getCenterX(), arrayObstacles[i].getCenterY(), arrayObstacles[i].getCenterZ()));
        if (distance <= 4)
        {
            return true;
        }

    }
    return false;
}
//victory detection
bool detectVictory(void)
{
    // measuring distance between the center of the car and the target cube
    // If the distance is equal to or less than the sum of half their widths then you won
    float distance = glm::length(center - glm::vec3(10.0, 10.0, 13.0));
    if (distance <= 4)
    {
        return true;
    }
    else {
        return false;
    }
}
void resetGame(int val) {
    if (val != 0) {
        // Reset the game to intial position
        eye = glm::vec3(0.0, 10.0, 15);
        center = glm::vec3(0.0, 10.0, 0.0);
        angle = 0.0;
        anglee = 0.0;
    }
}

// Drawing routine.
void drawScene(void)

{
    //first spotlight position
    float lightPos1[] = { center.x - 2, center.y, center.z - 2 , 1.0 };
    //second spotlight position
    float lightPos2[] = { center.x + 2, center.y, center.z - 2, 1.0 };
    // Spotlight direction at the line of sight.
    float spotDirection1[] = {trans_x1, trans_y1, trans_z1}; 
    // Spotlight direction at the line of sight.
    float spotDirection2[] = {trans_x2, trans_y2, trans_z2}; 
    trans_x1 = (lightPoss1.x - eye.x);
    trans_y1 = (lightPoss1[1] - eye.y);
    trans_z1 = (lightPoss1[2] - eye.z);
    trans_x2 = (lightPoss2[0] - eye.x);
    trans_y2 = (lightPoss2[1] - eye.y);
    trans_z2 = (lightPoss2[2] - eye.z);
    // sun Light position vector, last index is zero because it is directional light
    float lightPos0[] = { center.x, center.y - 0.5, center.z, 0.0 };
    
    // Material property vectors. for the black car components
    float matAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float matDif[] = { 0.0, 0.0, 0.0, 1.0 };
    float matSpec[] = { 0.5, 0.5, 0.5, 1.0 };
    float matShine[] = { 0.6};
    float matEmission[] = { 0.0, 0.0, 0.0, 1.0 };
    // Material property vectors. for the blue obstacles
    float matAmbb[] = { 0.0, 0.0, 1.0, 1.0 };
    float matDifb[] = { 0.0, 0.0, 1.0, 1.0 };
    float matSpecb[] = { 0.1, 0.1, 0.1, 1.0 };
    float matShineb[] = { 0.9 };
    float matEmissionb[] = { 0.0, 0.0, 1.0, 1.0 };
    //Material property vectors. for the red target
    float matAmbr[] = { 1.0, 0.0, 0.0, 1.0 };
    float matDifr[] = { 1.0, 0.0, 0.0, 1.0 };
    float matSpecr[] = { 0.1, 0.1, 0.1, 1.0 };
    float matShiner[] = { 0.9 };
    float matEmissionr[] = { 1.0, 0.0, 0.0, 1.0 };

    

    int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 0.0);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    // Spotlight1 position.
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    // Spotlight2 position.
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);
    // Spotlights properties.
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection1);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spotExponent);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDirection2);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, spotExponent);
    //transfer the image with the car movement
    glPushMatrix();
    glTranslatef(center.x, 0.0, center.z);
    glRotatef(anglee, 0, 1, 0);
   
    // Map the sky texture onto a rectangle parallel to the xy-plane.
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.1, 0.1); glVertex3f(-100.0, 0.0, -70.0);
    glTexCoord2f(0.9, 0.1); glVertex3f(100.0, 0.0, -70.0);
    glTexCoord2f(0.9, 0.9); glVertex3f(100.0, 120.0, -70.0);
    glTexCoord2f(0.1, 0.9); glVertex3f(-100.0, 120.0, -70.0);
    glEnd(); 
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(center.x, 0, center.z );
    // Map the street texture onto a rectangle along the xz-plane.
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 0.0, 100.0);
    glTexCoord2f(8.0, 0.0); glVertex3f(100.0, 0.0, 100.0);
    glTexCoord2f(8.0, 8.0); glVertex3f(100.0, 0.0, -100.0);
    glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, 0.0, -100.0);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
    // draw four cubes which are the obstacles with blue colour
    for (i = 0; i < 4; i++)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, matDifb);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecb);
        glMaterialfv(GL_FRONT, GL_SHININESS, matShineb);
        glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionb);
        arrayObstacles[i].draw();
    }
    //lower part of the car
    glPushMatrix();
    // Material properties of car.
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glTranslatef(center.x, center.y, center.z);
    glScalef(1.0, 0.25, 1.5);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidCube(6.0);
    glPopMatrix();
    //upper part of the car
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glTranslatef(center.x, center.y + 1.5, center.z);
    glScalef(2.0, 1, 4);
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(4.0f);
    glutWireCube(2.0);
    glPopMatrix();

    //the 4 wheels
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glTranslatef(center.x - 2, center.y - 1.5, center.z - 2);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(1.0f);
    glutWireSphere(1.0, 15, 15);
    glPopMatrix();

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glTranslatef(center.x + 2, center.y - 1.5, center.z + 2);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(1.0f);
    glutWireSphere(1.0, 15, 15);
    glPopMatrix();

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glTranslatef(center.x - 2, center.y - 1.5, center.z + 2);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(1.0f);
    glutWireSphere(1.0, 15, 15);
    glPopMatrix();

    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glTranslatef(center.x + 2, center.y - 1.5, center.z - 2);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(1.0f);
    glutWireSphere(1.0, 15, 15);
    glPopMatrix();


    // The target cube with red colour
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbr);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDifr);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecr);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShiner);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmissionr);
    glTranslatef(10, 10.0, 13);
    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(2);
    glPopMatrix();

    // check if the car collides
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos3f(center.x, center.y + 7, center.z);
    if (detectCollision()) {
        writeBitmapString((void*)font, "You Lose!");
        //wait three seconds then call the reset function 
        glutTimerFunc(3000, resetGame, 1);
        glutPostRedisplay();
    }
    glPopMatrix();
    // check if the car reaches the target
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos3f(center.x, center.y + 5, center.z);
    if (detectVictory()) {
        writeBitmapString((void*)font, "You Win!");
        //wait three seconds then call the reset function
        glutTimerFunc(3000, resetGame, 1);
        glutPostRedisplay();
    }
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glutSwapBuffers();
}

// to move the car in the forward direction
void moveForward(void)
{
    glm::vec3 direction = glm::normalize(center - eye);
    eye += speed * direction;
    center += speed * direction;
    trans_x1 = (lightPoss1[0] - eye.x);
    trans_y1 = (lightPoss1[1] - eye.y);
    trans_z1 = (lightPoss1[2] - eye.z);
    trans_x2 = (lightPoss2[0] - eye.x);
    trans_y2 = (lightPoss2[1] - eye.y);
    trans_z2 = (lightPoss2[2] - eye.z);

}

// to move the car in the backward direction
void moveBackward(void)
{
    glm::vec3 direction = glm::normalize(center - eye);
    eye -= speed * direction;
    center -= speed * direction;
    trans_x1 = (lightPoss1[0] - eye.x);
    trans_y1 = (lightPoss1[1] - eye.y);
    trans_z1 = (lightPoss1[2] - eye.z);
    trans_x2 = (lightPoss2[0] - eye.x);
    trans_y2 = (lightPoss2[1] - eye.y);
    trans_z2 = (lightPoss2[2] - eye.z);
}


//to rotate the car to the left
void rotateLeft(void)
{
    float distance = glm::length(center - eye);
    angle += angleSpeed;
    center.x = eye.x - distance * sin(angle);
    center.z = eye.z - distance * cos(angle);
    anglee += 1;
    trans_x1 = (lightPoss1[0] - eye.x);
    trans_y1 = (lightPoss1[1] - eye.y);
    trans_z1 = (lightPoss1[2] - eye.z);
    trans_x2 = (lightPoss2[0] - eye.x);
    trans_y2 = (lightPoss2[1] - eye.y);
    trans_z2 = (lightPoss2[2] - eye.z);

}
//to rotate the car to the right
void rotateRight(void)
{
    float distance = glm::length(center - eye);
    angle -= angleSpeed;
    center.x = eye.x - distance * sin(angle);
    center.z = eye.z - distance * cos(angle);
    anglee -= 1;
    trans_x1 = (lightPoss1[0] - eye.x);
    trans_y1 = (lightPoss1[1] - eye.y);
    trans_z1 = (lightPoss1[2] - eye.z);
    trans_x2 = (lightPoss2[0] - eye.x);
    trans_y2 = (lightPoss2[1] - eye.y);
    trans_z2 = (lightPoss2[2] - eye.z);
}


// Initialization routine.
void setup(void)
{
    int i;
    //create 4 cubes with random center x and z coordianates 
    for (i = 0; i < 4; i++) {
        arrayObstacles[i] = obstacle(rand() % 50, 10.0, rand() % 50);
    }

    glClearColor(0.38, 0.35, 0.33, 0.0);
    // Create texture ids.
    glGenTextures(2, texture);

    // Load external textures.
    loadTextures();

    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Turn on OpenGL texturing.
    glEnable(GL_TEXTURE_2D);
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    //light yellow sunlight 
    float lightDifAndSpec0[] = { 0.99, 0.99, 0.8, 1.0 };
    //white spotlights for the car
    float lightDifAndSpec1[] = { 1.0, 1.0, 1.0, 1.0 };
    //small global ambient light in the environment
    float globAmb[] ={ 0.05, 0.05, 0.05, 1.0 };
    // Light0 properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
    glEnable(GL_LIGHT0); // Enable particular light source.

    // Light1 properties.first spotlight
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec1);
    glEnable(GL_LIGHT1);
    // Light2 properties. second spotlight
    glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDifAndSpec1);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightDifAndSpec1);
    glEnable(GL_LIGHT2);

   // glEnable(GL_LIGHT1); // Enable particular light source.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint

}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

    glMatrixMode(GL_MODELVIEW);

}
// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    
    default:
        break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        moveForward();
        glutPostRedisplay();

    }
    if (key == GLUT_KEY_DOWN) {
        moveBackward();
        glutPostRedisplay();

    }
    if (key == GLUT_KEY_LEFT) {
        rotateLeft();
        glutPostRedisplay();

    }
    if (key == GLUT_KEY_RIGHT) {
        rotateRight();
        glutPostRedisplay();

    }

}



// Main routine.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("boxWithLookAt.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);
    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}


