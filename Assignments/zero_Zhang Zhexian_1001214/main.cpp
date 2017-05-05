#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
using namespace std;

// Globals

int rotationIndex;
int delay;
float angle;
float cameraZ = 5.0;
bool isRotating;

int a;
int c;
int d;
int f;
int g;
int i;

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

// These are global variables for color and position changes

int color_index;
float position_x = 1.0f;
float position_y = 1.0f;

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
		// Change color
        if(color_index<7){
            color_index++;
        }
        else{
            color_index = 0;
        }
        break;
	case 'r':
		// Toggle rotation
		isRotating = !isRotating;
		if(isRotating){
			StepRot(0);
		}
		break;
	default:
        cout << "Unhandled key press " << key << "." << endl; 
		break;
    }

	// This will refresh the screen so that the user sees the changes
    glutPostRedisplay();
}


// This is for step rotation
void StepRot(int n)
{
	n++;
	angle+=5;
	/*
	if(n<10000){
		glutTimerFunc(delay,StepRot,n);
	}
	*/
	if(isRotating){
	glutTimerFunc(delay,StepRot,n);
	}
	glutPostRedisplay();
	
}


// This is for mouse functions

void mouseFunc( int button, int state, int x, int y){

	if (button ==  3 && state == GLUT_DOWN) // Scroll up
	{ 
		// Zoom in
		cameraZ -= 0.5;
	}
	else if (button == 4 && state == GLUT_DOWN) // Scroll down
	{ 
		// Zoom out
		cameraZ += 0.5;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{ 
		// Rotate to the left
		angle-=5;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) 
	{ 
		// Rotate to the right
		angle+=5;
	}

	// This will refresh the screen so that the user sees the changes
	glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // Move light up
        position_y+=0.5f;
		break;
    case GLUT_KEY_DOWN:       
        // Move light down
        position_y-=0.5f;
		break;
    case GLUT_KEY_LEFT:
        // Move light left
        position_x-=0.5f;
		break;
    case GLUT_KEY_RIGHT:
        // Move light right
        position_x+=0.5f;
		break;
    }

	// This will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,cameraZ], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, cameraZ,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors
    GLfloat diffColors[7][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0},
                                 {0.8, 0.3, 0.9, 1.0},
                                 {0.5, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.5, 1.0}};
    
	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[color_index]);


	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};

    // Light position
	GLfloat Lt0pos[] = {position_x, position_y, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// Rotation
	glRotatef(angle,0,1,0);

	// This GLUT method draws a teapot
	
	//glutSolidTeapot(1.0);
	
	// This draws the loaded object

	for(unsigned int k=0; k < vecv.size(); k++) {
		a = vecf[k][0]; 
		c = vecf[k][2];
		d = vecf[k][3];
		f = vecf[k][5];
		g = vecf[k][6];
		i = vecf[k][8];
		glBegin(GL_TRIANGLES);

		glNormal3d(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
		glVertex3d(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);
		glNormal3d(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
		glVertex3d(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);
		glNormal3d(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
		glVertex3d(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);

		glEnd();
	}


	

    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	//int MAX_BUFFER_SIZE = 2000;
    
	char buffer[2000];
	Vector3f v;
	//vector<unsigned> vf;
	unsigned tempUnsigned;
	string s;

	while(true){
		if(cin.getline(buffer, 2000)==0){
			std::cout << "end of file" << std::endl;
			break;
		}
		stringstream ss(buffer); 
		ss >> s;
		ss >> v[0] >> v[1] >> v[2];
		if (s == "v") {
			//cout << "this is v \n";
			vecv.push_back(v);
		}
		else if (s == "vn"){		
			//cout << "this is vn \n";
			vecn.push_back(v);
		}
		else if (s == "f"){
			//cout << "this is f \n";
			vector<unsigned> vf;

			std::string withoutSpace;
			std::string withoutSlash;
			std::string text(buffer);
			
			std::istringstream tempA(text);
			while ( getline(tempA, withoutSpace, ' ') )
			{
				std::istringstream tempB(withoutSpace);
				if(withoutSpace != "f") {
					while ( getline(tempB, withoutSlash, '/') )
					{
						//std::cout << withoutSlash << std::endl;
						tempUnsigned = stoul(withoutSlash);
						vf.push_back(tempUnsigned);
					}
				}
				
			}
			vecf.push_back(vf);
		}

	}



}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0__Zhang Zhexian__1001214");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
	glutMouseFunc(mouseFunc); // Handles mouse functions
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
