#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include <SOIL/SOIL.h>
#include <Windows.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variables to store the model in
std::vector<glm::vec3> obj_vertices;
std::vector<glm::vec3> obj_normals;
std::vector<glm::vec2> obj_uvs;

GLuint vertexbuffer;
GLuint normalbuffer;
GLuint uvbuffer;

GLuint texture[3];

int use_shader = 1;
int use_background = 1;
int use_texture = 1;

#pragma region IMG_LOADING

int LoadGLTextures(){
	// Image texture
	// Load an image file directly as a new OpenGL texture
	texture[0] = SOIL_load_OGL_texture(
		"Data/texture3.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y);

	if(texture[0] == 0)
		return false;

	// Sphere_map
	texture[1] = SOIL_load_OGL_texture(
		"Data/sphere_map.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y);

	if(texture[1] == 0)
		return false;

	// Background image
	texture[2] = SOIL_load_OGL_texture(
		"Data/background.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y);

	if(texture[2] == 0)
		return false;
	return true;
}

#pragma endregion IMG_LOADING

#pragma region OBJ_LOADER
glm::vec3 cross(glm::vec3 v, glm::vec3 u){
	glm::vec3 res;
	res.x = v.y*u.z-v.z*u.y;
	res.y = v.z*u.x-v.x*u.z;
	res.z = v.x*u.y-v.y*u.x;
	return res;
}

GLfloat length(glm::vec3 v){
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

glm::vec3 normalize(glm::vec3 v){
	return v / length(v);
}

glm::vec3 normalFromVertex(glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3){
	glm::vec3 normal = cross(vertex2-vertex1, vertex3-vertex1);
	float sin_alpha = length(normal) / (length(vertex2-vertex1) * length(vertex3 - vertex1));
	return normalize(normal) * asin(sin_alpha);
}

void load_obj(const char* path){
	printf("Opening file\n");

	FILE * file = fopen(path, "r");
	if(file == NULL){
		printf("Failed to open file\n");
		return;
	}

	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> tmp_vertices;
	std::vector<glm::vec3> tmp_normals;
	std::vector<glm::vec2> tmp_uvs;

	printf("Loading model\n");
	while(1){
		char lineHeader[256];
		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break;

		if(strcmp(lineHeader, "v") == 0){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tmp_vertices.push_back(vertex);
		}
		else if(strcmp(lineHeader, "f") == 0){
			unsigned int vertexIndex[3];
			fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
		}
		else{
			// Do nothing
		}
	}

	// Calculate normals
	printf("Calculating normals\n");
	int xx = tmp_vertices.size();
	for(unsigned int i = 0; i < tmp_vertices.size(); i++){	
		glm::vec3 normal;
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;
		for(unsigned int j = 0; j < vertexIndices.size(); j++){
			if(vertexIndices[j] == i+1){
				glm::vec3 vertex1;
				glm::vec3 vertex2;
				glm::vec3 vertex3;
				if(j%3 == 0){
					vertex1 = tmp_vertices[vertexIndices[j]-1];
					vertex2 = tmp_vertices[vertexIndices[j+1]-1];
					vertex3 = tmp_vertices[vertexIndices[j+2]-1];
				}
				else if(j%3 == 1){
					vertex1 = tmp_vertices[vertexIndices[j]-1];
					vertex2 = tmp_vertices[vertexIndices[j+1]-1];
					vertex3 = tmp_vertices[vertexIndices[j-1]-1];
				}
				else{
					vertex1 = tmp_vertices[vertexIndices[j]-1];
					vertex2 = tmp_vertices[vertexIndices[j-2]-1];
					vertex3 = tmp_vertices[vertexIndices[j-1]-1];
				}
				normal = normal + normalFromVertex(vertex1, vertex2, vertex3);
			}
		}
		normal = normalize(normal);
		//printf("normal[%d/%d]: %f, %f, %f\n", i+1, xx, normal.x, normal.y, normal.z);
		tmp_normals.push_back(normal);
	}

	// Reorder the vertices
	printf("Reordering vertices\n");
	for(unsigned int i=0; i < vertexIndices.size(); i++){
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = tmp_vertices[vertexIndex-1];
		glm::vec3 normal = tmp_normals[vertexIndex-1];
		glm::vec2 uv;
		float theta = atan2(vertex.z,vertex.x);
        //uv.s = (theta + M_PI) / (2*M_PI);
        //uv.t = (vertex.y / 2.0);
		uv.s = vertex.x;
		uv.t = vertex.y;
		obj_vertices.push_back(vertex);
		obj_normals.push_back(normal);
		obj_uvs.push_back(uv);
	}

	printf("Object fully loaded\n");
}

void drawObject(){
	glBegin(GL_TRIANGLES);
		for(int i = 0; i < obj_vertices.size(); i++){
			glNormal3f(obj_normals[i].x, obj_normals[i].y, obj_normals[i].z);		// Set normal
			glTexCoord2f(obj_uvs[i].s, obj_uvs[i].t);		// Set first texture
			glVertex3f(obj_vertices[i].x, obj_vertices[i].y, obj_vertices[i].z);	// Set vertex
		}	
	glEnd();
}
#pragma endregion OBJ_LOADER


char *loadshader(char *filename)
{
	std::string strbuf;
	std::string line;
	std::ifstream in(filename);
	while(std::getline(in,line))
		strbuf += line + '\n';

	char *buf = (char *)malloc(strbuf.size()*sizeof(char));
	strcpy(buf,strbuf.c_str());

	return buf;
}

GLuint programObj = 0;
GLint vstime;

void init(char *vsfilename, char *fsfilename, char *objfilename) 
{
	GLfloat white[] = {1.0,1.0,1.0,1.0};
	GLfloat redamb[] = {0.3,0.3,0.3,0.0};
	GLfloat reddiff[] = {0.8,0.8,0.8,0.0};
	GLfloat lpos[] = {2.0,5.0,2.0,1.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, redamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, reddiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH); 
	glEnable(GL_DEPTH_TEST);

	GLchar *vertexShaderCodeStr = loadshader(vsfilename);
	const GLchar **vertexShaderCode = (const GLchar **)&vertexShaderCodeStr;

	GLchar *fragmentShaderCodeStr = loadshader(fsfilename);
	const GLchar **fragmentShaderCode = (const GLchar **)&fragmentShaderCodeStr;

	int status;
	GLint infologLength = 0;
	GLint charsWritten = 0;
	GLchar infoLog[10000];

	GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObj, 1, vertexShaderCode, 0);
	glCompileShader(vertexShaderObj); /* Converts to GPU code */

	GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObj, 1, fragmentShaderCode, 0);
	glCompileShader(fragmentShaderObj); /* Converts to GPU code */

//	GLuint
	programObj = glCreateProgram();
	glAttachObjectARB(programObj,vertexShaderObj);
	glAttachObjectARB(programObj,fragmentShaderObj);
	glLinkProgram(programObj);	/* Connects shaders & variables */
	glUseProgram(programObj); /* OpenGL now uses the shader */

	vstime = glGetUniformLocationARB(programObj,"time");

	glGetProgramivARB(programObj, GL_LINK_STATUS, &status);

	glGetObjectParameterivARB(programObj,GL_OBJECT_INFO_LOG_LENGTH_ARB,&infologLength);

	if(infologLength > 0) {
		glGetInfoLogARB(programObj,infologLength,&charsWritten,infoLog);
		if(charsWritten)
			printf("InfoLog:\n%s\n\n",infoLog);
	}

	//load the obj
	load_obj(objfilename);

	// load the textures
	if(!LoadGLTextures()){
		printf("Textures failed to load\n");
		return;
	}
	else{
		printf("Textures loaded\n");
	}
	// Texture 1 - Base texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture 2 - spheremap
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	// Texture 3 - Background image
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glMatrixMode(GL_MODELVIEW);
}

bool spinobj = true, spinlight = true;

void drawBackground(){
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0, 0);
		glVertex3f(-13, -10, -11);	
		glTexCoord2f(0, 1);
		glVertex3f(-13, 20, -11);
		glTexCoord2f(1, 0);
		glVertex3f(18, -10, -11);

		glTexCoord2f(0, 1);
		glVertex3f(-13, 20, -11);
		glTexCoord2f(1, 0);
		glVertex3f(18, -10, -11);
		glTexCoord2f(1, 1);
		glVertex3f(18, 20, -11);
	glEnd();
}

void display(void)
{
	static GLfloat angle = 0.0;

	GLfloat lpos[4], theta = M_PI*angle/180.0;
	if (spinlight) {
		lpos[0] = 5.0*sin(theta);
		lpos[1] = 5+5.0*cos(theta);
		lpos[2] = 2.0;
		lpos[3] = 1.0;
	} else {
		lpos[0] = 0.0; lpos[1] = 5.0; lpos[2] = 2.0; lpos[3] = 1.0;
	}

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);

	glUniform1fARB(vstime,angle);	/* use angle for the time variable */

	glLoadIdentity ();             /* clear the ModelView matrix */
	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt(0.0, 2.0, 7.0, 0.0, 2.0, 0.0, 0.0, 2.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);	/* light transformed by lookat but not rotate */
	
	glPushMatrix();
	if (spinobj)
		glRotatef(angle, 0.0, 1.0, 0.0);

	if(use_shader)
		glUseProgram(programObj); /* OpenGL now uses the shader */
	else
		glUseProgram(0); // Use default shader

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
	
	if(use_texture){
		// Texture 1 - Base texture
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	}
	else{
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	GLfloat amb[] = {0.3,0.3,0.3,0.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	drawObject();  // Draw the teapot

	glPopMatrix();

	if(use_background){
		GLfloat amb2[] = {0.8,0.8,0.8,0.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb2);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Texture 1 - Base texture
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glUseProgram(0); // Force opengl to use default shaders for background
		drawBackground();
	}

	glFlush();
	glutSwapBuffers();

	angle += 1.0;

	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
	  case 's':
		 if(use_shader)
			 use_shader = 0;
		 else
			 use_shader = 1;
		 break;
	  case 'b':
		  if(use_background)
			  use_background = 0;
		  else
			  use_background = 1;
		  break;
	  case 't':
		  if(use_texture){
			  use_texture = 0;
			  use_shader = 0;
		  }
		  else
			  use_texture = 1;
		  break; 
	  case 'l':
		  spinlight = !spinlight;
		  break;
	  case 'o':
		  spinobj = !spinobj;
		  break;
      case 27:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   glewInit();
   init(argv[1],argv[2],argv[3]);
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
