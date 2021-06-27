#include "glos.h"
#include<math.h>
#include <gl.h>
#include <glu.h>
#include <glaux.h>
#include "Patrat.h"

#pragma region globalVariables
static GLfloat x = 0;
static GLfloat y = 0;
static GLfloat unghi = 0;
static GLfloat roata = 0;
static GLfloat unghiLumina = 0;
static GLfloat zoom = 1;
static GLfloat ghidon = 0;
GLUquadricObj* quadric;
GLfloat position[] = { 0.0, 350, 80, 1.0 };//pozitia luminii(soarelui)
#pragma endregion

void myinit(void) {
	glClearColor(0.37, 0.60, 0.99, 0);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);//activare iluminare
	glEnable(GL_LIGHT0);//activare sursa 0

	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	//componenta ambientala din sursa 0 este lumina cu intensitate 0
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	//componenta difuza din sursa 0 este lumina cu intensitate 1 
	//pentru fiecare componenta de culoare R, G, B
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//componenta speculara din sursa 0 este lumina cu intensitate 1 
	//pentru fiecare componenta de culoare
	GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat local_view[] = { 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);//sets the values of individual light source parameters
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

}

#pragma region functiiMutare

void CALLBACK MutaStanga(void)
{
	x = x - 10;
}

void CALLBACK MutaDreapta(void)
{
	x = x + 10;
}
void CALLBACK MutaSus(void)
{
	y += 10;
}
void CALLBACK MutaJos(void)
{
	y -= 10;
}
void CALLBACK RotStanga(void)
{
	unghi += 10;
}
void CALLBACK RotDreapta(void)
{
	unghi -= 10;
}
void CALLBACK RotStangaLumina(void)
{
	if (position[2] < 180)
		position[2] += 5;
}
void CALLBACK RotDreaptaLumina(void)
{
	if (position[2] > 0)
		position[2] -= 5;
}

void CALLBACK roataSpate(void)
{
	if (roata > -300)
		roata = roata - 10;
}

void CALLBACK roataFata(void)
{
	if (roata < 300)
		roata = roata + 10;
}

void CALLBACK zoomIn(void)
{
	zoom += 0.1;

}
void CALLBACK zoomOut(void)
{
	if (zoom > 1.1)
		zoom -= 0.1;
}

void CALLBACK rotGhidonStanga(void)
{
	if (ghidon < 40)
		ghidon += 10;
}
void CALLBACK rotGhidonDreapta(void)
{
	if (ghidon > -40)
		ghidon -= 10;
}

#pragma endregion

#pragma region functiiDeDesenare

void calcCoeficientiPlan(float P[3][3], float coef[4])
{
	float v1[3], v2[3];
	float length;
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;
	//calculeazã doi vectori din trei puncte
	v1[x] = P[0][x] - P[1][x];
	v1[y] = P[0][y] - P[1][y];
	v1[z] = P[0][z] - P[1][z];

	v2[x] = P[1][x] - P[2][x];
	v2[y] = P[1][y] - P[2][y];
	v2[z] = P[1][z] - P[2][z];

	//se calculeazã produsul vectorial al celor doi vectori
	// care reprezintã un al treilea vector perpendicular pe plan 
	// ale cãrui componente sunt chiar coeficienþii A, B, C din ecuaþia planului
	coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
	coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
	coef[z] = v1[x] * v2[y] - v1[y] * v2[x];
	//normalizeazã vectorul
	length = (float)sqrt((coef[x] * coef[x]) + (coef[y] * coef[y]) + (coef[z] * coef[z]));
	coef[x] /= length;
	coef[y] /= length;
	coef[z] /= length;
}

void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4])
{
	GLfloat coefPlan[4];
	GLfloat temp;
	//determinã coeficienþii planului
	calcCoeficientiPlan(puncte, coefPlan);
	//determinã si pe D
	coefPlan[3] = -(
		(coefPlan[0] * puncte[2][0]) +
		(coefPlan[1] * puncte[2][1]) +
		(coefPlan[2] * puncte[2][2]));
	//temp=A*xL+B*yL+C*zL+D*w
	temp = coefPlan[0] * pozSursa[0] +
		coefPlan[1] * pozSursa[1] +
		coefPlan[2] * pozSursa[2] +
		coefPlan[3] * pozSursa[3];
	//prima coloanã
	mat[0][0] = temp - pozSursa[0] * coefPlan[0];
	mat[1][0] = 0.0f - pozSursa[0] * coefPlan[1];
	mat[2][0] = 0.0f - pozSursa[0] * coefPlan[2];
	mat[3][0] = 0.0f - pozSursa[0] * coefPlan[3];
	//a doua coloana
	mat[0][1] = 0.0f - pozSursa[1] * coefPlan[0];
	mat[1][1] = temp - pozSursa[1] * coefPlan[1];
	mat[2][1] = 0.0f - pozSursa[1] * coefPlan[2];
	mat[3][1] = 0.0f - pozSursa[1] * coefPlan[3];
	//a treia coloana
	mat[0][2] = 0.0f - pozSursa[2] * coefPlan[0];
	mat[1][2] = 0.0f - pozSursa[2] * coefPlan[1];
	mat[2][2] = temp - pozSursa[2] * coefPlan[2];
	mat[3][2] = 0.0f - pozSursa[2] * coefPlan[3];
	//a patra coloana
	mat[0][3] = 0.0f - pozSursa[3] * coefPlan[0];
	mat[1][3] = 0.0f - pozSursa[3] * coefPlan[1];
	mat[2][3] = 0.0f - pozSursa[3] * coefPlan[2];
	mat[3][3] = temp - pozSursa[3] * coefPlan[3];
}

void deseneazaSoare()
{
	glPushMatrix();
	glTranslatef(position[0], position[1] - 150, position[2]);
	glColor3f(1, 1, 0);
	glDisable(GL_LIGHTING);
	double radius = 10;
	auxSolidSphere(radius);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void deseneazaDrum(GLfloat  mat_ambientDrum[4], GLfloat  mat_diffuseDrum[4])
{
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientDrum);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseDrum);
	//glTranslatef(0, -170, 0);
	glScalef(500, 1, 500);
	glBegin(GL_QUADS);//four vertices
	glVertex3f(500.0f, -205.0f, -500.0f);
	glVertex3f(-500.0f, -205.0f, -500.0f);
	glVertex3f(-500.0f, -205.0f, 500.0f);
	glVertex3f(500.0f, -205.0f, 500.0f);
	glEnd();

	glPopMatrix();
}

void deseneazaScheletBicicleta(GLfloat  mat_ambientCuloareBicicleta[4], GLfloat  mat_diffuseCuloareBicicleta[4])
{
	{//c1
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientCuloareBicicleta);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseCuloareBicicleta);
		glTranslatef(0, 100, 0);
		glRotatef(90, 0, 0, 1);
		double radius = 3;
		double height = 100;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//c2
		glPushMatrix();
		glTranslatef(35, 0, 0);
		glRotatef(200, 0, 0, 1);
		double radius = 3;
		double height = 120;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{
		glPushMatrix();
		glTranslatef(100, 0, 0);
		glRotatef(ghidon, 0, 1, 0);
		glTranslatef(-100, 0, 0);
		{//cilindru fata vertical1
			glPushMatrix();
			glTranslatef(105, 90, 0);
			glRotatef(200, 0, 0, 1);
			double radius = 3;
			double height = 40;
			auxSolidCylinder(radius, height);
			glPopMatrix();
		}
		{//cilindru fata orizontal
			glPushMatrix();
			glTranslatef(105, 90, 10);
			glRotatef(200, 0, 0, 1);
			glRotatef(90, 1, 0, 0);
			double radius = 2;
			double height = 20;
			auxSolidCylinder(radius, height);
			glPopMatrix();
		}
		{//cilindru fata vertical, conexiune cu roata dreapta
			glPushMatrix();
			glTranslatef(105, 90, 10);
			glRotatef(20, 0, 0, 1);
			glRotatef(3, 1, 0, 0);
			double radius = 2;
			double height = 100;
			auxSolidCylinder(radius, height);
			glPopMatrix();
		}
		{//cilindru fata vertical, conexiune cu roata stanga
			glPushMatrix();
			glTranslatef(105, 90, -10);
			glRotatef(20, 0, 0, 1);
			glRotatef(3, -1, 0, 0);
			double radius = 2;
			double height = 100;
			auxSolidCylinder(radius, height);
			glPopMatrix();
		}
		{//cilindru fata orizontal, conexiune roata cu ceilalti cilindrii 
			glPushMatrix();
			glTranslatef(135, 0, 5);
			glRotatef(200, 0, 0, 1);
			glRotatef(90, 1, 0, 0);
			double radius = 2;
			double height = 10;
			auxSolidCylinder(radius, height);
			glPopMatrix();
		}
		glPopMatrix();
	}

	{//cilindru spate vertical1
		glPushMatrix();
		glTranslatef(-15, 80, 0);
		glRotatef(215, 0, 0, -1);
		double radius = 3;
		double height = 25;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate orizontal sus
		glPushMatrix();
		glTranslatef(-15, 80, 10);
		glRotatef(200, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		double radius = 2;
		double height = 20;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate vertical, conexiune cu roata dreapta
		glPushMatrix();
		glTranslatef(-15, 80, 10);
		glRotatef(35, 0, 0, -1);
		glRotatef(3, 1, 0, 0);
		double radius = 2;
		double height = 100;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate vertical, conexiune cu roata stanga
		glPushMatrix();
		glTranslatef(-15, 80, -10);
		glRotatef(35, 0, 0, -1);
		glRotatef(3, -1, 0, 0);
		double radius = 2;
		double height = 100;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}


	{//cilindru spate orizontal jos
		glPushMatrix();
		glTranslatef(10, 0, 0);
		glRotatef(90, 0, 0, 1);
		double radius = 3;
		double height = 25;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate orizontal jos 2
		glPushMatrix();
		glTranslatef(10, 0, 10);
		glRotatef(90, 1, 0, 0);
		double radius = 2;
		double height = 20;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate orizontal jos 3
		glPushMatrix();
		glTranslatef(-70, 0, 5);
		glRotatef(90, 0, 0, 1);
		//	glRotatef(20, 0, 0, 1);
		glRotatef(3, -1, 0, 0);
		double radius = 2;
		double height = 80;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate orizontal jos 4
		glPushMatrix();
		glTranslatef(-70, 0, -5);
		glRotatef(90, 0, 0, 1);
		//	glRotatef(20, 0, 0, 1);
		glRotatef(3, 1, 0, 0);
		double radius = 2;
		double height = 80;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//cilindru spate orizontal jos 4
		glPushMatrix();
		glTranslatef(-70, 0, 5);
		glRotatef(200, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		double radius = 2;
		double height = 10;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//c6
		glPushMatrix();
		glTranslatef(30, 0, 0);
		glRotatef(215, 0, 0, -1);
		double radius = 3;
		double height = 120;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}
}

void deseneazaRoti(GLfloat  mat_ambientBlack[4], GLfloat  mat_diffuseBlack[4])
{
	{
		glPushMatrix();

		glTranslatef(100, 0, 0);
		glRotatef(ghidon, 0, 1, 0);
		glTranslatef(-100, 0, 0);

		{//roata1
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
			glTranslatef(135, 0, 0);
			glRotatef(roata, 0, 0, -1);
			gluQuadricDrawStyle(quadric, GLU_LINE);
			double innerRadius = 0, outerRadius = 70;
			int slices = 15, loops = 1;
			gluDisk(quadric, innerRadius, outerRadius, slices, loops);
			glPopMatrix();
		}
		{//cauciuc1
			glPushMatrix();
			glTranslatef(135, 0, 0);
			glRotatef(roata, 0, 0, -1);
			glScaled(1, 1, 1);
			double innerRadius = 5;
			double outerRadius = 70;
			auxSolidTorus(innerRadius, outerRadius);
			glPopMatrix();
		}
		glPopMatrix();
	}
	{//roata2
		glPushMatrix();
		glTranslatef(-70, 0, 0);
		glRotatef(roata, 0, 0, -1);
		gluQuadricDrawStyle(quadric, GLU_LINE);
		double innerRadius = 0, outerRadius = 70;
		int slices = 15, loops = 1;
		gluDisk(quadric, innerRadius, outerRadius, slices, loops);
		glPopMatrix();
	}
	{//cauciuc2
		glPushMatrix();
		glTranslatef(-70, 0, 0);
		glRotatef(roata, 0, 0, -1);
		double innerRadius = 5;
		double outerRadius = 70;
		auxSolidTorus(innerRadius, outerRadius);
		glPopMatrix();
	}
}

void deseneazaSaua(GLfloat  mat_ambientBlack[4], GLfloat  mat_diffuseBlack[4])
{
	{//saua
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(0, 115, 0);
		glRotatef(90, 0, 0, 1);
		glRotatef(90, 0, 1, 0);
		glScaled(0.7, 1, 1);
		double innerRadius = 7;
		double outerRadius = 10;
		auxSolidTorus(innerRadius, outerRadius);
		glPopMatrix();
	}
}

void deseneazaGhidon(GLfloat  mat_ambientWhite[4], GLfloat  mat_diffuseWhite[4], GLfloat  mat_ambientBlack[4], GLfloat  mat_diffuseBlack[4])
{
	//ghidon:
	{//cilindru fata pt ghidon
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientWhite);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseWhite);
		glTranslatef(91.6, 127, 0);
		glRotatef(200, 0, 0, 1);
		GLdouble radius = 3;
		GLdouble height = 5;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//box fata pt ghidon 
		glPushMatrix();
		glTranslatef(98, 130, 0);
		glRotatef(15, 0, 0, 1);
		double width = 20;
		double height = 5;
		double depth = 5;
		auxSolidBox(width, height, depth);
		glPopMatrix();
	}

	{//box2 fata orizontal
		glPushMatrix();
		glTranslatef(105, 130, 0);
		glRotatef(15, 0, 0, 1);
		glRotatef(90, 0, 1, 0);
		double width = 40;
		double height = 5;
		double depth = 5;
		auxSolidBox(width, height, depth);
		glPopMatrix();
	}
	{//maner ghidon dreapta
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(105, 130, 38);
		glRotatef(200, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		GLdouble radius = 4;
		GLdouble height = 20;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}
	{//maner ghidon stanga
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(105, 130, -18);
		glRotatef(200, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		GLdouble radius = 4;
		GLdouble height = 20;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}
	{//dop dreapta
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(105, 130, 39);
		gluQuadricDrawStyle(quadric, GLU_FILL);
		double innerRadius = 0, outerRadius = 4;
		int slices = 15, loops = 3;
		gluDisk(quadric, innerRadius, outerRadius, slices, loops);
		glPopMatrix();
	}

	{//dop stanga
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(105, 130, -37);
		gluQuadricDrawStyle(quadric, GLU_FILL);
		double innerRadius = 0, outerRadius = 4;
		int slices = 15, loops = 3;
		gluDisk(quadric, innerRadius, outerRadius, slices, loops);
		glPopMatrix();
	}
}

void deseneazaSoneriesiFar(GLfloat  mat_ambient3[4], GLfloat  mat_diffuse3[4], GLfloat  mat_ambientBlack[4], GLfloat  mat_diffuseBlack[4], GLfloat mat_ambientLight[4], GLfloat mat_diffuseLight[4])
{
	//partea de sonerie
	{//juma de sfera
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		glTranslatef(104, 134, 10);
		glRotatef(15, 0, 0, 1);
		GLdouble eqn0[4] = { 0, 1, 0.0, 0 };//taie partea de jos
		glClipPlane(GL_CLIP_PLANE0, eqn0);
		glEnable(GL_CLIP_PLANE0);
		glRotatef(90, 0, 0, 1);
		double radius = 5;
		auxSolidSphere(radius);
		glDisable(GL_CLIP_PLANE0);
		glPopMatrix();
	}
	{//bat sonerie
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(104, 134, 10);
		glRotatef(15, 0, 0, 1);
		double radius = 1;
		double height = 5;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{//sfera mica
		glPushMatrix();
		glTranslatef(103, 138, 10);
		glRotatef(15, 0, 0, 1);
		glRotatef(90, 0, 0, 1);
		double radius = 1.5;
		auxSolidSphere(radius);
		glPopMatrix();
	}

	//parte de far:
	{//far
		glPushMatrix();
		glTranslatef(112, 133, 0);
		glRotatef(15, 0, 0, 1);
		glRotatef(90, 0, 0, 1);
		GLdouble eqn0[4] = { 0, 1, 0.0, 0 };//taie partea de jos
		glClipPlane(GL_CLIP_PLANE0, eqn0);
		glEnable(GL_CLIP_PLANE0);
		glRotatef(90, 0, 0, 1);
		double radius = 5;
		auxSolidSphere(radius);
		glDisable(GL_CLIP_PLANE0);
		glPopMatrix();
	}

	{//sfera lumina far
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientLight);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseLight);
		glTranslatef(113, 133, 0);
		glRotatef(15, 0, 0, 1);
		glRotatef(90, 0, 0, 1);
		GLdouble eqn0[4] = { 0, 1, 0.0, 0 };//taie partea de jos
		glClipPlane(GL_CLIP_PLANE0, eqn0);
		glEnable(GL_CLIP_PLANE0);
		glRotatef(90, 0, 0, 1);
		double radius = 4.5;
		auxSolidSphere(radius);
		glDisable(GL_CLIP_PLANE0);
		glPopMatrix();
	}
}

void deseneazaAnsambluPedale(GLfloat  mat_ambient3[4], GLfloat  mat_diffuse3[4], GLfloat  mat_ambientBlack[4], GLfloat  mat_diffuseBlack[4])
{
	//partea de pedale:
	{//cerc1
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		glTranslatef(30, 0, 5);
		glRotatef(roata, 0, 0, -1);
		gluQuadricDrawStyle(quadric, GLU_LINE);
		double innerRadius = 5, outerRadius = 13;
		int slices = 50, loops = 50;
		gluDisk(quadric, innerRadius, outerRadius, slices, loops);
		glPopMatrix();
	}

	{//cilindru pedale1 orizontal
		glPushMatrix();
		glTranslatef(30, 0, 15);
		glRotatef(200, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		double radius = 2;
		double height = 30;
		auxSolidCylinder(radius, height);
		glPopMatrix();
	}

	{
		glPushMatrix();
		{//cilindru pedale1 orizontal
			glPushMatrix();
			glTranslatef(30, 0, 15);
			glRotatef(20, 0, 0, 1);
			glRotatef(3, 1, 0, 0);
			glRotatef(roata, 0, 0, -1);
			double cylinderRadius = 2;
			double cylinderHeight = 20;
			auxSolidCylinder(cylinderRadius, cylinderHeight);
			//pedala1
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);

			glTranslatef(8, -17, 2);
			glRotatef(90, 0, 1, 0);
			double width = 10;
			double height = 5;
			double depth = 20;
			auxSolidBox(width, height, depth);
			glPopMatrix();
		}
		glPopMatrix();
	}


	{//cilindru pedale2 orizontal
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		glTranslatef(30, 0, -15);
		glRotatef(200, 0, 0, 1);
		glRotatef(3, -1, 0, 0);
		glRotatef(roata, 0, 0, -1);
		double cylinderRadius = 2;
		double cylinderHeight = 20;
		auxSolidCylinder(cylinderRadius, cylinderHeight);
		//pedala2
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);

		glTranslatef(8, -17, -2);
		glRotatef(90, 0, 1, 0);
		double width = 10;
		double height = 5;
		double depth = 20;
		auxSolidBox(width, height, depth);
		glPopMatrix();
	}

	{//cerc2 mai mic
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		glTranslatef(-70, 0, 5);
		glRotatef(roata, 0, 0, -1);
		gluQuadricDrawStyle(quadric, GLU_LINE);
		double innerRadius = 5, outerRadius = 10;
		int slices = 50, loops = 50;
		gluDisk(quadric, innerRadius, outerRadius, slices, loops);
		glPopMatrix();
	}
	{//lant
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambientBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseBlack);
		glTranslatef(-18.5, 0, 5);
		glScaled(3.2, 1, 1);
		double innerRadius = 0.3;
		double outerRadius = 20;
		auxSolidTorus(innerRadius, outerRadius);
		glPopMatrix();
	}
}

#pragma endregion

void CALLBACK display(void)
{
	quadric = gluNewQuadric();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region culoriLumina

	//sursa 0 este la infinit pe directia y=3, z=2

	GLfloat mat_ambientCuloareBicicleta[] = { 0.196078, 0.803921, 0.196078, 1.0 };//coeficient de reflexie ambientala
	GLfloat mat_diffuseCuloareBicicleta[] = { 0.196078, 0.803921, 0.196078, 1.0 };//coeficient de reflexie difuza

	GLfloat mat_ambientDrum[] = { 0.639, 0.702, 0.765 };//coeficient de reflexie ambientala
	GLfloat mat_diffuseDrum[] = { 0.639, 0.702, 0.765 };//coeficient de reflexie difuza

	GLfloat mat_ambientBlack[] = { 0.3, 0.3, 0.3, 1.0 };//coeficient de reflexie ambientala
	GLfloat mat_diffuseBlack[] = { 0.3, 0.3, 0.3, 1.0 };//coeficient de reflexie difuza

	GLfloat mat_ambientLight[] = { 1, 1, 0.5019, 1.0 };//coeficient de reflexie ambientala
	GLfloat mat_diffuseLight[] = { 1, 1, 0.5019, 1.0 };//coeficient de reflexie difuza

	GLfloat mat_ambientWhite[] = { 1, 1, 1, 1.0 };//coeficient de reflexie ambientala
	GLfloat mat_diffuseWhite[] = { 1, 1, 1, 0 };//coeficient de reflexie difuza

	GLfloat mat_ambient3[] = { 0.4117, 0.9019, 0.9882, 1.0 };//coeficient de reflexie ambientala
	GLfloat mat_diffuse3[] = { 0.4117, 0.9019, 0.9882, 1.0 };//coeficient de reflexie difuza

	GLfloat mat_ambient4[] = { 0.8, 0.4, 0.2, 1.0 };//coeficient de reflexie ambientala
	GLfloat mat_diffuse4[] = { 0.8, 0.4, 0.2, 1.0 };//coeficient de reflexie difuza

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };//coeficient de reflexie speculara
	GLfloat no_shininess[] = { 0.0 };//exponentul de reflexie speculara
	GLfloat high_shininess[] = { 100.0 };//exponentul de reflexie speculara
#pragma endregion


	//umbre
	GLfloat matUmbra[4][4];
	GLfloat puncte[3][3] = { {-50.0f, -204.0f, -50.0f},{-50.0, -204.0f, 50.0},{50.0f, -204.0f, 50.0f} };

	glPushMatrix();
	{
		glRotatef(15, 1, 0, 0);
		glRotatef(25, 0, 1, 0);

		//partea de zoom si rotire
		if (zoom <= 1.7) {
			glScalef(zoom, zoom, zoom);
			glTranslatef(0, zoom * 40, 0.0);
		}
		else {
			glScalef(1.7, 1.7, 1.7);
			glTranslatef(0, 1.7 * 40, 0.0);
		}
		glRotatef(zoom * 30, 0, 1, 0);

		MatriceUmbra(puncte, position, matUmbra);

		{//soare
			glPushMatrix();
			glTranslatef(position[2], 0, 0);
			glLightfv(GL_LIGHT0, GL_POSITION, position);
			deseneazaSoare();
			glPopMatrix();
		}

		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //se aplica la toate obiectele    glMaterialfv function assigns values to material parameters
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess); // se aplica la toate obiectele

		deseneazaDrum(mat_ambientDrum, mat_diffuseDrum);

		glTranslatef(0, -132, 0.0);//ca sa puna bicicleta mai jos
		glRotatef(unghi, 0, 1, 0);
		glTranslatef(x, y, 0.0);
		glTranslatef(roata, 0, 0.0);

		deseneazaScheletBicicleta(mat_ambientCuloareBicicleta, mat_diffuseCuloareBicicleta);
		deseneazaRoti(mat_ambientBlack, mat_diffuseBlack);
		deseneazaSaua(mat_ambientBlack, mat_diffuseBlack);
		{
			glPushMatrix();
			glTranslatef(100, 0, 0);
			glRotatef(ghidon, 0, 1, 0);
			glTranslatef(-100, 0, 0);
			deseneazaGhidon(mat_ambientWhite, mat_diffuseWhite, mat_ambientBlack, mat_diffuseBlack);
			deseneazaSoneriesiFar(mat_ambient3, mat_diffuse3, mat_ambientBlack, mat_diffuseBlack, mat_ambientLight, mat_diffuseLight);
			glPopMatrix();
		}
		deseneazaAnsambluPedale(mat_ambient3, mat_diffuse3, mat_ambientBlack, mat_diffuseBlack);


		//	umbra:
		{
			glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glTranslatef(0, 132, 20);

			glMultMatrixf((GLfloat*)matUmbra);
			glColor3f(0.2, 0.2, 0.2);
			deseneazaScheletBicicleta(mat_ambientCuloareBicicleta, mat_diffuseCuloareBicicleta);
			deseneazaRoti(mat_ambientBlack, mat_diffuseBlack);
			deseneazaSaua(mat_ambientBlack, mat_diffuseBlack);
			{
				glPushMatrix();
				glTranslatef(100, 0, 0);
				glRotatef(ghidon, 0, 1, 0);
				glTranslatef(-100, 0, 0);
				deseneazaGhidon(mat_ambientWhite, mat_diffuseWhite, mat_ambientBlack, mat_diffuseBlack);
				glPopMatrix();
			}
			deseneazaAnsambluPedale(mat_ambient3, mat_diffuse3, mat_ambientBlack, mat_diffuseBlack);
			glPopMatrix();
			glEnable(GL_LIGHTING);
			glPopAttrib();
			//sfarsit parte de umbra
		}
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);
	glFlush();
}


void CALLBACK myReshape(GLsizei w, GLsizei h)
{
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat ortho = 260;//=180 // = 400 // sau=26 for super zoom in
	if (w <= h)
		glOrtho(-ortho, ortho, -ortho * (GLfloat)h / (GLfloat)w,
			ortho * (GLfloat)h / (GLfloat)w, -700.0, 700.0);
	glOrtho(-ortho * (GLfloat)w / (GLfloat)h,
		ortho * (GLfloat)w / (GLfloat)h, -ortho, ortho, -700.0, 700.0);

	glMatrixMode(GL_MODELVIEW);
	//The params for glOrtho are left, right, bottom, top, near, far
}

int main(int argc, char** argv)
{
	auxInitDisplayMode(AUX_SINGLE | AUX_RGB | AUX_DEPTH);
	auxInitPosition(10, 10, 1900, 1000);
	auxInitWindow("Bicicleta");
	myinit();

#pragma region keybinds
	auxKeyFunc(AUX_LEFT, MutaStanga);
	auxKeyFunc(AUX_RIGHT, MutaDreapta);
	auxKeyFunc(AUX_UP, MutaSus);
	auxKeyFunc(AUX_DOWN, MutaJos);

	auxKeyFunc(AUX_S, RotDreaptaLumina);
	auxKeyFunc(AUX_s, RotDreaptaLumina);

	auxKeyFunc(AUX_W, RotStangaLumina);
	auxKeyFunc(AUX_w, RotStangaLumina);

	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, RotStanga);
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, RotDreapta);

	auxKeyFunc(AUX_a, roataSpate);
	auxKeyFunc(AUX_A, roataSpate);

	auxKeyFunc(AUX_d, roataFata);
	auxKeyFunc(AUX_D, roataFata);

	auxKeyFunc(AUX_i, zoomIn);
	auxKeyFunc(AUX_I, zoomIn);

	auxKeyFunc(AUX_o, zoomOut);
	auxKeyFunc(AUX_O, zoomOut);

	auxKeyFunc(AUX_z, rotGhidonStanga);
	auxKeyFunc(AUX_Z, rotGhidonStanga);

	auxKeyFunc(AUX_x, rotGhidonDreapta);
	auxKeyFunc(AUX_X, rotGhidonDreapta);
#pragma endregion

	auxReshapeFunc(myReshape);
	auxMainLoop(display);
	return(0);
}