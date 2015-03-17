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
#include <Windows.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define epsilon 0.000001

#pragma region HE_STRUCT
struct HE_vert;
struct HE_face;

struct HE_edge{
	HE_vert* vert;
	HE_edge* pair;
	HE_face* face;
	HE_edge* next;
};

struct HE_vert{
	float x;
	float y;
	float z;
	glm::vec3 norm;

	glm::vec3 new_point;
	glm::vec3 new_edge;

	HE_edge* edge;
};

struct HE_face{
	HE_edge* edge;
	glm::vec3 avg_face;
};

inline bool operator==(HE_vert lhs, const HE_vert& rhs)
{
	if(abs(lhs.x - rhs.x) < epsilon)
		if(abs(lhs.y - rhs.y) < epsilon)
			if(abs(lhs.z - rhs.z) < epsilon)
				return true;
	return false;
}

inline bool operator!=(HE_vert lhs, const HE_vert& rhs)
{
	return !(lhs == rhs);
}
#pragma endregion HE_STRUCT

bool moveCamera = true;
bool showMesh = true;
std::vector<glm::vec3> obj_vertices;
std::vector<glm::vec3> obj_normals;
std::vector<HE_edge*> edges;

#pragma region MOVE_CAMERA

#pragma endregion MOVE_CAMERA

#pragma region CREATE_HE

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

void create_he(){
	// Create a half edge structure for each half edge in the model
	for(int i = 0; i < obj_vertices.size(); i+=4){
		HE_edge *edge1 = new HE_edge; HE_edge *edge2 = new HE_edge;
		HE_edge *edge3 = new HE_edge; HE_edge *edge4 = new HE_edge;
		HE_vert *vert1 = new HE_vert; HE_vert *vert2 = new HE_vert;
		HE_vert *vert3 = new HE_vert; HE_vert *vert4 = new HE_vert;
		HE_face *face1 = new HE_face; HE_face *face2 = new HE_face;
		HE_face *face3 = new HE_face; HE_face *face4 = new HE_face;

		// Calculate the average face value
		glm::vec3 avg_face = obj_vertices[i] + obj_vertices[i+1] + obj_vertices[i+2] + obj_vertices[i+3];
		avg_face /= 4;
		
		vert1->x = obj_vertices[i].x;
		vert1->y = obj_vertices[i].y;
		vert1->z = obj_vertices[i].z;
		vert1->edge = edge1;
		face1->edge = edge1;

		vert2->x = obj_vertices[i+1].x;
		vert2->y = obj_vertices[i+1].y;
		vert2->z = obj_vertices[i+1].z;
		vert2->edge = edge2;
		face2->edge = edge2;

		vert3->x = obj_vertices[i+2].x;
		vert3->y = obj_vertices[i+2].y;
		vert3->z = obj_vertices[i+2].z;
		vert3->edge = edge3;
		face3->edge = edge3;

		vert4->x = obj_vertices[i+3].x;
		vert4->y = obj_vertices[i+3].y;
		vert4->z = obj_vertices[i+3].z;
		vert4->edge = edge4;
		face4->edge = edge4;

		edge1->vert = vert1;
		edge1->face = face1;
		edge1->face->avg_face = avg_face;
		edge1->next = edge2;
		edge1->pair = NULL;

		edge2->vert = vert2;
		edge2->face = face2;
		edge2->face->avg_face = avg_face;
		edge2->next = edge3;
		edge2->pair = NULL;

		edge3->vert = vert3;
		edge3->face = face3;
		edge3->face->avg_face = avg_face;
		edge3->next = edge4;
		edge3->pair = NULL;

		edge4->vert = vert4;
		edge4->face = face4;
		edge4->face->avg_face = avg_face;
		edge4->next = edge1;
		edge4->pair = NULL;

		// Push the edges into the vector of edges
		edges.push_back(edge1);
		edges.push_back(edge2);
		edges.push_back(edge3);
		edges.push_back(edge4);
	}	


	// Loop through all half edges looking for the opposite half edge
	int n=0;
	for(int i = 0; i < edges.size(); i++){
		// Find the current and next vertices
		HE_vert *curr = edges[i]->vert;
		HE_vert *next = edges[i]->next->vert;

		// Locate the edge that starts at vertex next, with its next vertex equal to curr
		HE_edge *tmp;
		for(int j = 0; j < edges.size(); j++){
			tmp = edges[j];
			if(*(tmp->vert) == *next){
				if(*(tmp->next->vert) == *curr){
					// Store this edge as the current edges pair
					n++;
					edges[i]->pair = tmp;
					break;
				}
			}
		}
	}
	printf("number of pairs found: %d\n", n);
	printf("number of half edges: %d\n", edges.size());

	// Set the normals for each vertex
	for(unsigned int i = 0; i < edges.size(); i++){
		glm::vec3 normal;
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;

		HE_vert* next = edges[i]->pair->next->vert;
		int n = 1;
		glm::vec3 vertex1;
		glm::vec3 vertex2;
		glm::vec3 vertex3;
				
		vertex1.x = edges[i]->vert->x;
		vertex1.y = edges[i]->vert->y;
		vertex1.z = edges[i]->vert->z;

		vertex3.x = edges[i]->next->vert->x;
		vertex3.y = edges[i]->next->vert->y;
		vertex3.z = edges[i]->next->vert->z;

		vertex2.x = edges[i]->next->next->next->vert->x;
		vertex2.y = edges[i]->next->next->next->vert->y;
		vertex2.z = edges[i]->next->next->next->vert->z;

		normal += normalFromVertex(vertex1, vertex2, vertex3);

		while(edges[i]->next->vert != next->edge->next->vert){
			
			vertex1.x = next->x;
			vertex1.y = next->y;
			vertex1.z = next->z;

			vertex3.x = next->edge->next->vert->x;
			vertex3.y = next->edge->next->vert->y;
			vertex3.z = next->edge->next->vert->z;

			vertex2.x = next->edge->next->next->next->vert->x;
			vertex2.y = next->edge->next->next->next->vert->y;
			vertex2.z = next->edge->next->next->next->vert->z;

			normal = normal + normalFromVertex(vertex1, vertex2, vertex3);
			next = next->edge->pair->next->vert;
			n++;
		}
		normal /= n;
		normal = normalize(normal);
		edges[i]->vert->norm = normal;
	}
}

#pragma endregion CREATE_HE

#pragma region SUBDIVISION



void subdivide(){
	printf("Starting subdivide\n");
	// Perform Catmull-Clark subdivision
	// Avg faces computed during creation of HE_edge structure

	// For each edge, add an edge point
	printf("Creating new edge points\n");
	for(int i = 0; i < edges.size(); i++){
		glm::vec3 point;
		point.x = edges[i]->vert->x + edges[i]->next->vert->x + edges[i]->face->avg_face.x + edges[i]->pair->face->avg_face.x;
		point.y = edges[i]->vert->y + edges[i]->next->vert->y + edges[i]->face->avg_face.y + edges[i]->pair->face->avg_face.y;
		point.z = edges[i]->vert->z + edges[i]->next->vert->z + edges[i]->face->avg_face.z + edges[i]->pair->face->avg_face.z;
		point /= 4;

		edges[i]->vert->new_edge = point;
	}

	// For each original point P
	printf("Creating new vertex positions\n");
	for(int i = 0; i < edges.size(); i++){
		int n = 1;

		glm::vec3 P;
		P.x = edges[i]->vert->x;
		P.y = edges[i]->vert->y;
		P.z = edges[i]->vert->z;

		// Calculate F
		glm::vec3 F;
		F = edges[i]->face->avg_face;		

		// Calculate R
		glm::vec3 R;
		R.x = (edges[i]->vert->x + edges[i]->next->vert->x)/2;
		R.y = (edges[i]->vert->y + edges[i]->next->vert->y)/2;
		R.z = (edges[i]->vert->z + edges[i]->next->vert->z)/2;

		// find all other edges that use this point
		HE_vert* next = edges[i]->pair->next->vert;
		while(edges[i]->next->vert != next->edge->next->vert){
			F += next->edge->face->avg_face;
			R.x += (next->x + next->edge->next->vert->x)/2;
			R.y += (next->y + next->edge->next->vert->y)/2;
			R.z += (next->z + next->edge->next->vert->z)/2;

			next = next->edge->pair->next->vert;
			n++;
		}
		R /= n;	// Average R 
		F /= n;	// Average F


		// Calculate new point position of the vertex
		glm::vec3 point;
		R *= 2;
		P *= (n-3);
		point = (F + R + P);
		point /= n;
		edges[i]->vert->new_point = point;
	}

	///// SHOULD BE CORRECT!!!!!!!!!!!!
	// recreate the obj_vertices vector
	printf("recreating the obj_vertices vector\n");
	std::vector<glm::vec3> tmp_vertices;
	for(int i = 0; i < edges.size(); i++){
		tmp_vertices.push_back(edges[i]->vert->new_point);
		tmp_vertices.push_back(edges[i]->vert->new_edge);
		tmp_vertices.push_back(edges[i]->face->avg_face);
		tmp_vertices.push_back(edges[i]->next->next->next->vert->new_edge);
	}

	obj_vertices.clear();
	obj_vertices = tmp_vertices;

	// Rebuild HE data structure
	edges.clear();
	create_he();
}
#pragma endregion SUBDIVISION

#pragma region OBJ_LOADER

void load_obj(const char* path){
	printf("Opening file\n");

	FILE * file = fopen(path, "r");
	if(file == NULL){
		printf("Failed to open file\n");
		return;
	}

	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> tmp_vertices;

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
			unsigned int vertexIndex[4];
			fscanf(file, "%d %d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2], &vertexIndex[3]);
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			vertexIndices.push_back(vertexIndex[3]);
		}
		else{
			// Do nothing
		}
	}

	// Reorder the vertices
	printf("Reordering vertices\n");
	unsigned int vertexIndex;
	for(unsigned int i = 0; i < vertexIndices.size(); i++){
		vertexIndex = vertexIndices[i];
		glm::vec3 vertex = tmp_vertices[vertexIndex-1];
		obj_vertices.push_back(vertex);
	}

	printf("Object fully loaded\n");
}

void drawObject(){
	glBegin(GL_QUADS);
		for(unsigned int i = 0; i < edges.size(); i++){
			glNormal3f(edges[i]->vert->norm.x, edges[i]->vert->norm.y, edges[i]->vert->norm.z);		// Set normal
			glVertex3f(edges[i]->vert->x, edges[i]->vert->y, edges[i]->vert->z);	// Set vertex
		}	
	glEnd();
}
#pragma endregion OBJ_LOADER

void init() 
{
	GLfloat white[] = {1.0,1.0,1.0,1.0};
	GLfloat orange[] = {1.0, 0.5, 0.0, 1.0};
	GLfloat amb[] = {0.2,0.2,0.2,0.0};
	GLfloat reddiff[] = {0.9,0.9,0.9,0.0};
	GLfloat lpos[] = {5.0,5.0,5.0,1.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, orange);
	glLightfv(GL_LIGHT0, GL_SPECULAR, orange);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, reddiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0);

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH); 
	glEnable(GL_DEPTH_TEST);

	// Load the obj
	load_obj("i.obj");

	// Load HE data structure
	create_he();

	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	static GLfloat angle = 0.0;
	GLfloat lpos[4];
	lpos[0] = 5.0; lpos[1] = 0.0; lpos[2] = 5.0; lpos[3] = 1.0;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);

	glLoadIdentity ();             /* clear the ModelView matrix */
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	glRotatef(angle, 0.0, 1.0, 0.0);
	angle += 0.25;

	if(showMesh)
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	drawObject();  // Draw the mesh
	//glutSolidTeapot(2);

	glFlush();
	glutSwapBuffers();

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
		 // Subdivide the object using Catmull-Clark
		 subdivide();
		 break;
	  case 'w':
		  // Turn on/off wire mesh
		  if(showMesh)
			  showMesh = false;
		  else
			  showMesh = true;
		  break;
	  case 'c':
		  // Turn on/off camera movement
		  if(moveCamera)
			  moveCamera = false;
		  else
			  moveCamera = true;
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
   glutInitWindowSize (640, 640); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   glewInit();
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
