// The dancing I.


#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <GL/glew.h>
#include <GL/glut.h>

#define M_PI 3.1415926535897932384626433832795

int nFPS = 60;
int wireFrame = 0;
int outline = 1;
float count = 0.0f;

void init(void)
{
	// init your data, setup OpenGL environment here
	glClearColor(0.9,0.9,0.9,1.0); // clear color is gray		
	glPointSize(4.0);
}

void display(void)
{
	// Vertices
	float v0[3] =  {-0.45,  0.45, 0.0};
	float v1[3] =  {-0.45,  0.75, 0.0};
	float v2[3] =  { 0.45,  0.75, 0.0};
	float v3[3] =  { 0.45,  0.45, 0.0};
	float v4[3] =  { 0.15,  0.45, 0.0};
	float v5[3] =  { 0.15, -0.45, 0.0};
	float v6[3] =  { 0.45, -0.45, 0.0};
	float v7[3] =  { 0.45, -0.75, 0.0};
	float v8[3] =  {-0.45, -0.75, 0.0};
	float v9[3] =  {-0.45, -0.45, 0.0};
	float v10[3] = {-0.15, -0.45, 0.0};
	float v11[3] = {-0.15,  0.45, 0.0};

	float v12[3] = {-0.6,  0.9, 0.0};
	float v13[3] = { 0.6,  0.9, 0.0};
	float v14[3] = { 0.6,  0.3, 0.0};
	float v15[3] = { 0.3,  0.3, 0.0};
	float v16[3] = { 0.3, -0.3, 0.0};
	float v17[3] = { 0.6, -0.3, 0.0};
	float v18[3] = { 0.6, -0.9, 0.0};
	float v19[3] = {-0.6, -0.9, 0.0};
	float v20[3] = {-0.6, -0.3, 0.0};
	float v21[3] = {-0.3, -0.3, 0.0};
	float v22[3] = {-0.3,  0.3, 0.0};
	float v23[3] = {-0.6,  0.3, 0.0};

	// put your OpenGL display commands here
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// reset OpenGL transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // reset transformation matrix to identity

	// setup look at transformation so that 
	// eye is at : (0,0,3)
	// look at center is at : (0,0,0)
	// up direction is +y axis
	gluLookAt(0.f,0.f,3.f,0.f,0.f,0.f,0.f,1.f,0.f);
	
	// Set up different offsets to use for moving the vertices
	float offset1 = cos(count);
	float offset2 = sin(count);
	float offset3 = sin(2.0 * count);
	float offset4 = cos(2.0 * count);
	float offset5 = cos(count * 3.0);
	float offset6 = sin(count * 3.0);

	//Move the vertices.  Each point in the vertex is moved a different amount based on a 
	//sin or cos function, so that it will eventually return to it's original position and
	//start over again
	v0[0] += 0.02 * offset1;	v0[1] += 0.02 * offset2;
	v1[0] += 0.02 * offset1;	v1[1] += 0.02 * offset3;
	v2[0] -= 0.02 * offset4;	v2[1] -= 0.05 * offset5;
	v3[0] += 0.01 * offset6;	v3[1] += 0.03 * offset4;
	v4[0] -= 0.03 * offset3;	v4[1] -= 0.01 * offset2;
	v5[0] -= 0.04 * offset2;	v5[1] += 0.02 * offset6;
	v6[0] += 0.02 * offset1;	v6[1] += 0.02 * offset2;
	v7[0] += 0.02 * offset1;	v7[1] += 0.02 * offset3;
	v8[0] -= 0.02 * offset4;	v8[1] -= 0.02 * offset5;
	v9[0] += 0.05 * offset6;	v9[1] += 0.03 * offset4;
	v10[0] -= 0.04 * offset3;	v10[1] -= 0.01 * offset2;
	v11[0] -= 0.04 * offset2;	v11[1] += 0.02 * offset6;
	v12[0] += 0.02 * offset1;	v12[1] += 0.02 * offset2;
	v13[0] += 0.03 * offset1;	v13[1] += 0.05 * offset3;
	v14[0] -= 0.02 * offset6;	v14[1] -= 0.02 * offset1;
	v15[0] += 0.05 * offset6;	v15[1] += 0.03 * offset4;
	v16[0] -= 0.03 * offset3;	v16[1] -= 0.01 * offset2;
	v17[0] += 0.04 * offset2;	v17[1] += 0.02 * offset6;
	v18[0] -= 0.02 * offset3;	v18[1] -= 0.02 * offset3;
	v19[0] += 0.01 * offset3;	v19[1] += 0.02 * offset3;
	v20[0] += 0.05 * offset4;	v20[1] -= 0.02 * offset5;
	v21[0] += 0.01 * offset6;	v21[1] += 0.03 * offset1;
	v22[0] -= 0.03 * offset5;	v22[1] -= 0.01 * offset2;
	v23[0] -= 0.04 * offset2;	v23[1] += 0.03 * offset5;

	// Set fill mode depending on if wireframe is true or not
	if(wireFrame == 0){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Inner portion of the I
	glColor3f(0.0,0.0,0.6); // Set color to blue
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v11);
		glVertex3fv(v4);
		glVertex3fv(v10);
		glVertex3fv(v5);
		glVertex3fv(v8);
		glVertex3fv(v6);
		glVertex3fv(v7);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3fv(v1);
		glVertex3fv(v0);
		glVertex3fv(v11);

		glVertex3fv(v4);
		glVertex3fv(v3);
		glVertex3fv(v2);

		glVertex3fv(v10);
		glVertex3fv(v9);
		glVertex3fv(v8);
	glEnd();

	// Outer portion of the I
	glColor3f(1.0,0.45,0.0); // set current color to orange
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3fv(v19);
		glVertex3fv(v8);
		glVertex3fv(v18);
		glVertex3fv(v7);
		glVertex3fv(v17);
		glVertex3fv(v6);
		glVertex3fv(v16);
		glVertex3fv(v5);
		glVertex3fv(v15);
		glVertex3fv(v4);
		glVertex3fv(v14);
		glVertex3fv(v3);
		glVertex3fv(v13);
		glVertex3fv(v2);
		glVertex3fv(v12);
		glVertex3fv(v1);
		glVertex3fv(v23);
		glVertex3fv(v0);
		glVertex3fv(v22);
		glVertex3fv(v11);
		glVertex3fv(v21);
		glVertex3fv(v10);
		glVertex3fv(v20);
		glVertex3fv(v9);
		glVertex3fv(v19);
		glVertex3fv(v8);
	glEnd();
	
	// Draw the outline if it is enabled
	if(outline == 1){
		glColor3f(0.0,0.0,0.0);
		glBegin(GL_LINE_LOOP);
			glVertex3fv(v12);
			glVertex3fv(v13);
			glVertex3fv(v14);
			glVertex3fv(v15);
			glVertex3fv(v16);
			glVertex3fv(v17);
			glVertex3fv(v18);
			glVertex3fv(v19);
			glVertex3fv(v20);
			glVertex3fv(v21);
			glVertex3fv(v22);
			glVertex3fv(v23);
		glEnd();
	}

	glutSwapBuffers();	// swap front/back framebuffer to avoid flickering 
}

void reshape (int w, int h)
{
	// reset viewport ( drawing screen ) size
	glViewport(0, 0, w, h);
	float fAspect = ((float)w)/h; 
	// reset OpenGL projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.f,fAspect,0.001f,30.f); 
}



void keyboard(unsigned char key, int x, int y)
{
	// put your keyboard control here
	if (key == 27) 
	{
		// ESC hit, so quit
		printf("demonstration finished.\n");
		exit(0);
	}
	else if(key == 'w' || key == 'W'){
		// Toggle wireframe mode
		if(wireFrame == 0){
			printf("wireFrame enabled\n");
			wireFrame = 1;
		}
		else{
			printf("wireFrame disabled\n");
			wireFrame = 0;
		}
	}
	else if(key == 'o' || key == 'O'){
		// Toggle outline
		if(outline == 0){
			printf("outline enabled\n");
			outline = 1;
		}
		else{
			printf("outline disabled\n");
			outline = 0;
		}
	}
}

void mouse(int button, int state, int x, int y)
{
	// process your mouse control here
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		printf("push left mouse button.\n");
}


void timer(int v)
{
	// Increase the count used for the movement functions
	count += 0.025;
	if(count > 2 * M_PI)
		count = 0.0;
	glutPostRedisplay(); // trigger display function by sending redraw into message queue
	glutTimerFunc(1000/nFPS,timer,v); // restart timer again
}

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);

	// set up for double-buffering & RGB color buffer & depth test
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ((const char*)"The Dancing I");


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error %s\n", glewGetErrorString(err));
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	if (GLEW_ARB_vertex_program)
		fprintf(stdout, "Status: ARB vertex programs available.\n");
	if (glewGetExtension("GL_ARB_fragment_program"))
		fprintf(stdout, "Status: ARB fragment programs available.\n");
	if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite"))
		fprintf(stdout, "Status: ARB point sprites available.\n");



	init(); // setting up user data & OpenGL environment
	
	// set up the call-back functions 
	glutDisplayFunc(display);  // called when drawing 
	glutReshapeFunc(reshape);  // called when change window size
	glutKeyboardFunc(keyboard); // called when received keyboard interaction
	glutMouseFunc(mouse);	    // called when received mouse interaction
	glutTimerFunc(100,timer,nFPS); // a periodic timer. Usually used for updating animation
	
	glutMainLoop(); // start the main message-callback loop

	return 0;
}
