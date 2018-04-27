// The sample model.  You should build a file
// very similar to this for when you make your model.

#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#pragma warning(disable : 4786)
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "texture.h"
#include <FL/gl.h>
#include <cstring>
#include <FL/glut.h>
#include "modelerglobals.h"
#include "particleSystem.h"
#include "metaball.h"
#include "mat.h"
#include "vec.h"
#include "bitmap.h"
#include "subdivisionsurface.h"
#include "IK\ik.h"
#include <vector>
using namespace std;


// To make a SampleModel, we inherit off of ModelerView
vector<float> Vertex::averageMask;
class SampleModel : public ModelerView
{
public:
	SampleModel(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label) {
		ik_point = Vec3f(0, 0, 0);
		rightLeg = new InverseKinematics(ik_point, 1.2, 1.2);

	};

	virtual void draw();
	int feet_angle = 0;
	Vec3f ik_point;
	bool feet_dir = true; // false= negative, true = positive
	bool heart_beat = false;
	int beat_count = 0;
	//int it = 0;
	Diamond* diamond;
	InverseKinematics* rightLeg;
};




void SpawnParticles(Mat4d CameraMatrix, int num)
{
	Mat4d WorldMatrix = CameraMatrix.inverse() * getModelViewMatrix();
	Vec4d position = WorldMatrix *  Vec4d(0.5, 1, -2, 1);
	ParticleSystem *ps = ModelerApplication::Instance()->GetParticleSystem();
	//ps->SpawnParticles(Vec3d(position[0], position[1], position[2]), num);
	Vec3d pos = Vec3d(position[0], position[1], position[2]);
	map<double, std::vector<Particle>>::iterator it = ps->bakeContainer.find(ps->currentT + ps->bake_fps);
	bool BakeExisted = (it != ps->bakeContainer.end());

	if (ps->isSimulate() == true)
	{
		if (BakeExisted == false)
		{
			for (int i = 0; i < num; ++i)
			{
				double mass = rand() % 5 + 0.2;
				Particle p = Particle(pos, mass);
				double F = rand() % 10 / 10.0 + 0.2;
				double theta = rand() % 360 / 57.3;

				double zSpeed = -(rand() % 10 / 10.0 + 5);
				// double ySpeed = cos(theta) * F;
				// double xSpeed = sin(theta) * F;
				double ySpeed = 0;
				double xSpeed = -(rand() % 10 / 10.0) + 0.5;
				p.setVelocity(Vec3d(xSpeed, ySpeed, zSpeed));
				ps->particles.push_back(p);
			}
		}
	}
}

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSampleModel(int x, int y, int w, int h, char *label)
{
	return new SampleModel(x, y, w, h, label);
}



void SampleModel::draw()
{
	
	Vec4f ik_result = Vec4f(0.f, 0.f, 0.f, 0.f); 
	if (VAL(ACCUMULATION_BUFFER)) {
		glAccum(GL_RETURN, 1.0f);
		glClear(GL_ACCUM_BUFFER_BIT);
	}
	/*if (ModelerApplication::Instance()->m_animating == true)
	{
		beat_count++;
		if (beat_count == 10)
		{
			beat_count = 0;
			heart_beat = !heart_beat;
		}

		if (VAL(MOOD) == false)
		{

			if (feet_angle == 45 || feet_angle == 0)
				feet_dir = !feet_dir;

			if (feet_dir == true)
				feet_angle -= 1;
			else
				feet_angle += 1;
		}

	}
	*/
	
	
	if (VAL(MOOD) == true)
	{

		//ANION();
		SETVAL(NOSE, 1.5);
		SETVAL(NERVOUS, 1);
		SETVAL(RIGHT_ARM, 35);
		SETVAL(LEFT_ARM, -39);
		SETVAL(RIGHT_LEG, -58);
		SETVAL(LEFT_LEG, 22);
		SETVAL(RIGHT_KNEE, 48);
		SETVAL(LEFT_KNEE, 53);
		SETVAL(FEET_ROTATE, 23);

	}


	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	Mat4d CameraMatrix = getModelViewMatrix();

	GLfloat lightPosition0[] = { VAL(LIGHT_X), VAL(LIGHT_Y), VAL(LIGHT_Z), 0.0f };
	GLfloat lightDiffuse0[] = { VAL(LIGHT_R), VAL(LIGHT_G), VAL(LIGHT_B) };
	GLfloat lightPosition1[] = { VAL(LIGHT1_X), VAL(LIGHT1_Y), VAL(LIGHT1_Z), 0.0f };
	GLfloat lightDiffuse1[] = { VAL(LIGHT1_R), VAL(LIGHT1_G), VAL(LIGHT1_B) };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
	

	Vec3f p[6];
	p[0] = Vec3f(0, 2, 0);

	p[1] = Vec3f(1, 1, 1);
	p[2] = Vec3f(1, 1, -1);
	p[3] = Vec3f(-1, 1, -1);
	p[4] = Vec3f(-1, 1, 1);

	p[5] = Vec3f(0, 0, 0);


	diamond = new Diamond(p);
	for (int i = 0; i < 4; i++)
	{
		float k = 1.5;
		if (i % 2 == 0)
			k = 0.5;
		Vertex::averageMask.push_back(1.0f * k);
	}

	int num_split=3;
	//cin >> num_split;
	
	diamond->split(num_split);
	
	if (VAL(L_SYSTEM))
	{
		glPushMatrix();
		//glRotated(180, 1.0, 0.0, 0.0);
		glTranslated(VAL(XPOS), VAL(YPOS) - 5, VAL(ZPOS));
		drawLSystem(VAL(L_SYSTEM_ITERATION));
		glPopMatrix();

	}
	else if (VAL(INDIVIDUAL_LOOK))
	{
		//draw body
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(COLOR_DARK_RED);
		glPushMatrix();
		glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
		glPushMatrix();
		glTranslated(0.0, -0.1, 0.5);
		glScaled(0.75, 0.8, 0.6);

		glutSolidDodecahedron();
		glPopMatrix();

		glTranslated(-1, -1, 0);

		//draw heart

		glPushMatrix();
		if (heart_beat == true)
			setDiffuseColor(COLOR_DARK_RED);
		else
			setDiffuseColor(COLOR_YELLOW);
		glTranslated(1.2, 1.5, 1.2);
		glRotated(70, 1.0, 0.0, 0.0);

		drawTriangularPolygon(0.5, 0.5, 0.2, 60);
		glPopMatrix();


		//draw paint
		setDiffuseColor(COLOR_YELLOW);
		glTranslated(0, -0.5, 0);
		drawBox(2, 0.5, 1);
		glRotated(-90, 1.0, 0.0, 0.0);
		glTranslated(1, -0.6, -0.3);
		drawCylinder(0.3, 0.3, 0.4);

		glPopMatrix();

		//draw head (neck->head->nose)
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{

			setDiffuseColor(COLOR_YELLOW);
			glPushMatrix();
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glTranslated(0, 0.1, 0.5);
			//draw neck
			glPushMatrix();
			glRotated(VAL(ROTATE), 0.0, 1.0, 0.0);
			glRotated(-90, 1.0, 0.0, 0.0);
			drawCylinder(VAL(HEIGHT) + 0.1, 0.4, 0.3);//NECK

			//draw head
			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				setDiffuseColor(COLOR_DARK_RED);
				glTranslated(0.0, 0.0, VAL(HEIGHT));

				glPushMatrix();
				glTranslated(0.0, 0.0, 0.6);
				glScaled(0.4, 0.4, 0.4);

				glutSolidDodecahedron();
				glPopMatrix();

				//Particle System
				//SpawnParticles(CameraMatrix, VAL(PARTICLE_NUM));

				//draw nose
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					setDiffuseColor(COLOR_YELLOW);
					glTranslated(0.0, 0.0, 0.6);
					glRotated(90, 1.0, 0.0, 0.0);
					drawCylinder(0.9 + VAL(NOSE), 0.1, 0.1);//NOSE

				}

			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw right arm (shoulder->joint->arm)

		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{

			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glTranslated(-1.5, 0.2, 0.5);
			glPushMatrix();
			//draw shoulder
			glRotated(VAL(RIGHT_ARM), 1.0, 0.0, 0.0);
			glRotated(-90, 1.0, 0.0, 0.0); 

			glPushMatrix();
			glTranslated(0.0, 0.0, 0.2);
			glScaled(0.2, 0.2, 0.2);

			glutSolidDodecahedron();
			glPopMatrix();


			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_YELLOW);
				glTranslated(0, 0, -0.2);
				drawCylinder(0.2, 0.15, 0.2);//joint

				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					
						//draw arm
						setDiffuseColor(COLOR_DARK_RED);
						glTranslated(0, 0, -0.7);

						glPushMatrix();

						glScaled(0.2, 0.3, 0.5);

						glutSolidDodecahedron();
						glPopMatrix();

						
						if (VAL(HAND_WEAPON) == 1)
						{
							setDiffuseColor(COLOR_YELLOW);
							glRotated(-90, 1.0, 0.0, 0.0);
							drawTorus(0.3, 0.1);
							glTranslated(0, 0.25, 0.0);
							drawTorus(0.3, 0.1);
							glTranslated(0, 0.25, 0.0);
							drawTorus(0.3, 0.1);
							glTranslated(0, 0.5, 0.0);
							glRotated(90, 1.0, 0.0, 0.0);
							drawCylinder(0.4, 0, 0.3);
						}
					
				}
			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw left arm (shoulder->joint->arm)
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{
			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();

			glTranslated(1.5, 0.2, 0.5);
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glPushMatrix();
			//draw shoulder
			glRotated(VAL(LEFT_ARM), 1.0, 0.0, 0.0);
			glRotated(-90, 1.0, 0.0, 0.0);
			glPushMatrix();
			glTranslated(0.0, 0.0, 0.2);
			glScaled(0.2, 0.2, 0.2);

			glutSolidDodecahedron();
			glPopMatrix();

			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_YELLOW);
				glTranslated(0, 0, -0.2);
				drawCylinder(0.2, 0.15, 0.2);//joint
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					//draw arm
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, -0.7);

					glPushMatrix();

					glScaled(0.2, 0.3, 0.5);

					glutSolidDodecahedron();
					glPopMatrix();
				}
			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw right leg (joint->upper leg->joint->lower leg->foot)
		//implement ik here 
		
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{
			
				setDiffuseColor(COLOR_DARK_RED);
				glPushMatrix();
				glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
				glTranslated(-0.8, -1.6, 0.5);
				glPushMatrix();


				//draw upper leg
				
					rightLeg->reset();

					glRotated(-270, 1.0, 0.0, 0.0);
					glRotated(VAL(RIGHT_LEG), 1.0, 0.0, 0.0);

					glPushMatrix();
					glTranslated(0.0, 0.0, 0.5);
					glScaled(0.4, 0.3, 0.5);

					glutSolidDodecahedron();
					glPopMatrix();
				
			

			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_YELLOW);
				glTranslated(0, 0, 1.3);
			
				glRotated(VAL(RIGHT_KNEE), 1.0, 0.0, 0.0);
				drawSphere(0.3);
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					//draw lower leg
					
						
					
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, 0.2);
					glPushMatrix();
					glTranslated(0.0, 0.0, 0.5);
					glScaled(0.25, 0.3, 0.5);

					glutSolidDodecahedron();
					glPopMatrix();

					if (VAL(LEVEL_OF_DETAILS) >= 5)
					{
						//draw foot		
						setDiffuseColor(COLOR_YELLOW);
						glRotated(VAL(FEET_ROTATE) + feet_angle, 0.0, 0.0, 1.0);
						glTranslated(-0.35, -0.4, 1.2);

						drawBox(0.75, 1.5, 0.3);//foot
					}
				}
			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw left leg (joint->upper leg->joint->lower leg->foot)
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{
			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glTranslated(0.8, -1.6, 0.5);
			glPushMatrix();
			//draw upper leg
			glRotated(-270, 1.0, 0.0, 0.0);
			glRotated(VAL(LEFT_LEG), 1.0, 0.0, 0.0);
			glPushMatrix();
			glTranslated(0.0, 0.0, 0.5);
			glScaled(0.4, 0.3, 0.5);

			glutSolidDodecahedron();
			glPopMatrix();

			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_YELLOW);
				glTranslated(0, 0, 1.3);
				glRotated(VAL(LEFT_KNEE), 1.0, 0.0, 0.0);
				drawSphere(0.3);
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					//draw lower leg
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, 0.2);
					glPushMatrix();
					glTranslated(0.0, 0.0, 0.5);
					glScaled(0.25, 0.3, 0.5);

					glutSolidDodecahedron();
					glPopMatrix();

					if (VAL(LEVEL_OF_DETAILS) >= 5)
					{
						//draw foot		
						setDiffuseColor(COLOR_YELLOW);
						glRotated(-(VAL(FEET_ROTATE) + feet_angle), 0.0, 0.0, 1.0);
						glTranslated(-0.35, -0.4, 1.2);

						drawBox(0.75, 1.5, 0.3);//foot
					}
				}
			}
			glPopMatrix();
			glPopMatrix();

		}
	}
	else
	{
		
		// height field 
		if (VAL(HEIGHT_FIELD)) {
			//TODO
			//int height_field_range = 10; //unit of field 
			int height_field_height, height_field_width;
			glPushMatrix();
			
			
			//save the intensity value in Vertices and the destination color in Colors
			std::vector<Vec3f> Vertices;
			std::vector<Vec3f> Colors;
			unsigned char *heightField = readBMP("skybox/cliffBottom.bmp", height_field_height, height_field_width);
			for (int j = 0; j < height_field_height; j++) {
				for (int i = 0; i < height_field_width; i++) {
					unsigned char* pixel = heightField + (i + j * height_field_width) * 3;
					Vec3f color = Vec3f(0.0f, 0.0f, 0.0f);
					if(pixel !=nullptr)
						color = Vec3f(float((int)*pixel) / 255.0, float((int)*(pixel + 1)) / 255.0, float((int)*(pixel + 2)) / 255.0);
					 
					Colors.push_back(color);
					double intensity = 0.299*(color[0]) + 0.587*color[1] + 0.114*color[2];
					Vec3f newVertix = Vec3f(i, j, intensity*15.0);
					
					Vertices.push_back(newVertix);
				}
			}

			//draw height field 
			for (int j = 0; j < height_field_height-1; j++) {
				for (int i = 0; i < height_field_width-1; i++) {
					// the vertices index for triangle
					int v00 = i + j * height_field_width; 
					int v01 = i + j *height_field_width + 1;
					int v10 = i + (j+1) * height_field_width;
					int v11 = i + 1 + (j + 1)* height_field_width;
					// draw triangle (0): v00.v01,v11
					setDiffuseColor((Colors[v00][0]+Colors[v01][0]+Colors[v11][0])/3.0f,
						(Colors[v00][1]+Colors[v01][1]+Colors[v11][1]) / 3.0f, 
						(Colors[v00][1] + Colors[v01][1] + Colors[v11][1])/3.0f);
					
					drawTriangle(Vertices[v00][0]-10, Vertices[v00][2]-12, Vertices[v00][1] - 10,
						Vertices[v01][0] - 10, Vertices[v01][2]-12, Vertices[v01][1] - 10,
						Vertices[v11][0] -10, Vertices[v11][2] - 12, Vertices[v11][1] - 10
						);
					//draw triangle(1): v00, v11, v10
					setDiffuseColor((Colors[v00][0] + Colors[v10][0] + Colors[v11][0]) / 3.0f,
						(Colors[v00][1] + Colors[v10][1] + Colors[v11][1]) / 3.0f,
						(Colors[v00][1] + Colors[v10][1] + Colors[v11][1]) / 3.0f);
					
					drawTriangle(Vertices[v00][0] - 10, Vertices[v00][2] - 12, Vertices[v00][1] - 10,
						Vertices[v10][0] - 10, Vertices[v10][2] - 12, Vertices[v10][1] - 10,
						Vertices[v11][0] - 10, Vertices[v11][2] - 12, Vertices[v11][1] - 10
						);

				}
			}
			glPopMatrix();

		}
		// skybox for the background
		if(VAL(SKYBOX) == 1 ){
			//printf("skybox");
			// Store the current matrix
			glPushMatrix();

			// Reset and transform the matrix.

			//glLoadIdentity();
			//glTranslated(0, 1, 0);
			glScaled(80, 80, 80);
			//camera->gluLookAt(
			//	0, 0, 0,
			//	camera->x(), camera->y(), camera->z(),
			//	0, 1, 0);
			//
			// Enable/Disable features
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			// Just in case we set all vertices to white.
			//glColor4f(1, 1, 1, 1);
			setAmbientColor(.1f, .1f, 0.1f);
			setDiffuseColor(COLOR_WHITE);
			//generate texture
			Texture skybox; 
			//skybox.loadBMP_custom("skybox/cliffFront.bmp");
			
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			// Render the front quad
			glBindTexture(GL_TEXTURE_2D, skybox.loadBMP_custom("skybox/cliffFront.bmp"));
			//skybox.loadBMP_custom("skybox/cliffFront.bmp");
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, -0.5f);
			glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
			glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
			glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, -0.5f);
			glEnd();

			// Render the left quad
			glBindTexture(GL_TEXTURE_2D, skybox.loadBMP_custom("skybox/cliffLeft.bmp"));
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, 0.5f);
			glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
			glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
			glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, 0.5f);
			glEnd();

			// Render the back quad
			glBindTexture(GL_TEXTURE_2D, skybox.loadBMP_custom("skybox/cliffBack.bmp"));
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
			glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, 0.5f);
			glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, 0.5f);
			glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, 0.5f);

			glEnd();

			// Render the right quad
			glBindTexture(GL_TEXTURE_2D, skybox.loadBMP_custom("skybox/cliffRight.bmp"));
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
			glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
			glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
			glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
			glEnd();

			// Render the top quad
			glBindTexture(GL_TEXTURE_2D,skybox.loadBMP_custom("skybox/cliffTop.bmp"));
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, -0.5f);
			glTexCoord2f(0, 0); glVertex3f(-0.5f, 0.5f, 0.5f);
			glTexCoord2f(1, 0); glVertex3f(0.5f, 0.5f, 0.5f);
			glTexCoord2f(1, 1); glVertex3f(0.5f, 0.5f, -0.5f);
			glEnd();

			// Render the bottom quad
			glBindTexture(GL_TEXTURE_2D, skybox.loadBMP_custom("skybox/cliffBottom.bmp"));
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
			glTexCoord2f(0, 1); glVertex3f(-0.5f, -0.5f, 0.5f);
			glTexCoord2f(1, 1); glVertex3f(0.5f, -0.5f, 0.5f);
			glTexCoord2f(1, 0); glVertex3f(0.5f, -0.5f, -0.5f);
			glEnd();

			// Restore enable bits and matrix
			glPopAttrib();
			glPopMatrix();
			
		}

		
		// the box indicating where the index is 
		//glTranslated(0, -4.3, 0);
		//Vec3f destination = Vec3f(VAL(IK_LEG_X) - 0.5, VAL(IK_LEG_Y) - 4.1, VAL(IK_LEG_Z));
		glTranslated(VAL(IK_LEG_X) + 0.5, VAL(IK_LEG_Y) - 4.0, VAL(IK_LEG_Z));
		setDiffuseColor(COLOR_GREEN);
		drawBox(0.25, 0.25, 0.25);
		glTranslated(-VAL(IK_LEG_X) - 0.5, -VAL(IK_LEG_Y) + 4.0, -VAL(IK_LEG_Z));
		//draw body
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(COLOR_DARK_RED);
		glPushMatrix();
		glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

		glTranslated(-1, -1, 0);


		if (VAL(TEXTURE) == 1)
			drawTextureBox(2, 2, 1, "Image/Red.bmp");
		else
			drawBox(2, 2, 1);
		//draw heart
		glPushMatrix();
		glTranslated(0, 2, 3);
		SpawnParticles(CameraMatrix, VAL(PARTICLE_NUM));
		glPopMatrix();
		glPushMatrix();
		if (heart_beat == true)
			setDiffuseColor(COLOR_DARK_RED);
		else
			setDiffuseColor(COLOR_YELLOW);
		glTranslated(1.2, 1.5, 1);
		glRotated(90, 1.0, 0.0, 0.0);
		drawTriangularPolygon(0.5, 0.5, 0.2, 60);
		
		glPopMatrix();

		

		//draw paint
		setDiffuseColor(COLOR_GRAY);
		glTranslated(0, -0.5, 0);
		drawBox(2, 0.5, 1);
		glRotated(-90, 1.0, 0.0, 0.0);
		glTranslated(1, -0.6, -0.3);
		drawCylinder(0.3, 0.3, 0.4);

		glPopMatrix();

		//draw head (neck->head->nose,eyes,mouth)
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{

			setDiffuseColor(COLOR_GRAY);
			glPushMatrix();
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glTranslated(0, 0.1, 0.5);
			//draw neck
			if (VAL(METABALL) == false)
			{
				glPushMatrix();
				glRotated(VAL(ROTATE), 0.0, 1.0, 0.0);
				glRotated(-90, 1.0, 0.0, 0.0);
				drawCylinder(VAL(HEIGHT), 0.4, 0.4);//NECK
			}
			else
			{
				glPushMatrix();
				glRotated(VAL(ROTATE), 0.0, 1.0, 0.0);
				glRotated(-90, 1.0, 0.0, 0.0);
			}
			//draw head
			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				setDiffuseColor(COLOR_DARK_RED);
				glTranslated(0.0, 0.0, VAL(HEIGHT));




				if (VAL(TEXTURE) == 1)
					drawTextureCylinder(0.7, 0.6, 0.5, "Image/Red.bmp");//head
				else
					drawCylinder(0.7, 0.6, 0.5);//head

				//draw crown
				glPushMatrix();
				glRotated(-90, 1.0, 0.0, 0.0);
				glTranslated(0.0, -1.9, 0.0);
				setDiffuseColor(COLOR_GREEN);
				diamond->draw();
				glPopMatrix();
				//draw nose
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					setDiffuseColor(COLOR_GRAY);
					glTranslated(0.0, 0.0, 0.3);
					glRotated(90, 1.0, 0.0, 0.0);
					drawCylinder(0.8 + VAL(NOSE), 0.1, 0.1);//NOSE
					glTranslated(0.0, 0.0, 0.8 + VAL(NOSE));
					drawSphere(0.1);
				}

				//draw eyes
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					setDiffuseColor(COLOR_WHITE);
					glTranslated(0.2, 0.2, -0.3 - VAL(NOSE));
					drawSphere(0.08);
					glTranslated(-0.4, 0.0, 0.0);
					drawSphere(0.08);

				}

				//draw mouth
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					setDiffuseColor(COLOR_WHITE);
					glTranslated(0.06, -0.32, 0.06);
					glRotated(90, 1.0, 0.0, 0.0);
					if (VAL(NERVOUS) == true)
					{
						glRotated(180, 0.0, 1.0, 0.0);
						glTranslated(-0.3, 0.0, -0.27);
					}

					drawTriangularPolygon(0.3, 0.3, 0.05, 60);

				}
				if (VAL(METABALL) == true)
				{
					setDiffuseColor(COLOR_DARK_RED);
					MetaBall* m_metaBall = new MetaBall(1.0f);
					m_metaBall->addBallRel(0.2, -0.8, -1.5, 2.0);
					m_metaBall->addBallRel(0.2, 0.12, -1.5, 2.0);
					m_metaBall->draw(15);
					delete m_metaBall;
				}



			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw right arm (shoulder->joint->arm)

		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{

			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glTranslated(-1.5, 0.2, 0.5);
			glPushMatrix();
			//draw shoulder
			glRotated(VAL(RIGHT_ARM), 1.0, 0.0, 0.0);
			glRotated(-90, 1.0, 0.0, 0.0);

			if (VAL(METABALL) == false)
			{
				if (VAL(TEXTURE) == 1)
					drawTextureCylinder(0.6, 0.3, 0.2, "Image/Red.bmp");
				else
					drawCylinder(0.6, 0.3, 0.2);//shoulder
			}



			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				if (VAL(METABALL) == false)
				{
					//draw joint
					setDiffuseColor(COLOR_GRAY);
					glTranslated(0, 0, -0.2);
					drawCylinder(0.2, 0.2, 0.2);//joint
				}
				else
				{
					setDiffuseColor(COLOR_GRAY);
					glTranslated(0, 0, -0.2);
				}

				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{

					//draw arm
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, -1.2);
					if (VAL(TEXTURE) == 1)
						drawTextureCylinder(1.2, 0.3, 0.3, "Image/Red.bmp");
					else
						drawCylinder(1.2, 0.3, 0.3);//arm
					if (VAL(HAND_WEAPON) == 1)
					{
						setDiffuseColor(COLOR_YELLOW);
						glRotated(-90, 1.0, 0.0, 0.0);
						drawTorus(0.3, 0.1);
						glTranslated(0, 0.25, 0.0);
						drawTorus(0.3, 0.1);
						glTranslated(0, 0.25, 0.0);
						drawTorus(0.3, 0.1);
						glTranslated(0, 0.5, 0.0);
						glRotated(90, 1.0, 0.0, 0.0);
						drawCylinder(0.4, 0, 0.3);
					}

				}

				if (VAL(METABALL) == true)
				{
					setDiffuseColor(COLOR_DARK_RED);
					MetaBall* m_metaBall = new MetaBall(1.0f);
					m_metaBall->addBallRel(0.0, 0.9, 0.12, 0.4);
					m_metaBall->addBallRel(0.0, 1.6, 0.12, 0.5);
					m_metaBall->draw(6);
					delete m_metaBall;
				}

			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw left arm (shoulder->joint->arm)
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{
			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();

			glTranslated(1.5, 0.2, 0.5);
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glPushMatrix();
			//draw shoulder
			glRotated(VAL(LEFT_ARM), 1.0, 0.0, 0.0);
			glRotated(-90, 1.0, 0.0, 0.0);
			if (VAL(METABALL) == false)
			{
				if (VAL(TEXTURE) == 1)
					drawTextureCylinder(0.6, 0.3, 0.2, "Image/Red.bmp");
				else
					drawCylinder(0.6, 0.3, 0.2);//shoulder
			}
			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_GRAY);
				glTranslated(0, 0, -0.2);
				if (VAL(METABALL) == false)
				{
					drawCylinder(0.2, 0.2, 0.2);//joint
				}
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					//draw arm
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, -1.2);

					if (VAL(TEXTURE) == 1)
						drawTextureCylinder(1.2, 0.3, 0.32, "Image/Red.bmp");
					else
						drawCylinder(1.2, 0.3, 0.3);//arm
				}
				if (VAL(METABALL) == true)
				{
					setDiffuseColor(COLOR_DARK_RED);
					MetaBall* m_metaBall = new MetaBall(1.0f);
					m_metaBall->addBallRel(0.0, 0.9, 0.12, 0.4);
					m_metaBall->addBallRel(0.0, 1.6, 0.12, 0.5);
					m_metaBall->draw(6);
					delete m_metaBall;
				}
			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw right leg (joint->upper leg->joint->lower leg->foot)
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{
			
			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();
			
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			
			glTranslated(-0.8, -1.6, 0.5);
			glPushMatrix();
			
			//draw upper leg
			
				glRotated(-270, 1.0, 0.0, 0.0);
				glRotated(VAL(RIGHT_LEG), 1.0, 0.0, 0.0);
				if (VAL(TEXTURE) == 1)
					drawTextureCylinder(1.2, 0.4, 0.3, "Image/Red.bmp");
				else
					drawCylinder(1.2, 0.4, 0.3);//upper leg
			
			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_GRAY);
				glTranslated(0, 0, 1.3); //1.3 is y
				glRotated(VAL(RIGHT_KNEE), 1.0, 0.0, 0.0);
				if (VAL(METABALL) == false)
				{
					drawSphere(0.3);
				}
				else
				{
					if (VAL(METABALL) == true)
					{
						setDiffuseColor(COLOR_DARK_RED);
						MetaBall* m_metaBall = new MetaBall(1.0f);
						m_metaBall->addBallRel(0.0, -0.3, 0.12, 0.4);
						m_metaBall->addBallRel(0.0, 0.3, 0.12, 0.5);
						m_metaBall->draw(7);
						delete m_metaBall;
					}
				}
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					//draw lower leg
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, 0.2);
					if (VAL(TEXTURE) == 1)
						drawTextureCylinder(1.2, 0.3, 0.4, "Image/Red.bmp");
					else
						drawCylinder(1.2, 0.3, 0.4);//lower leg

					if (VAL(LEVEL_OF_DETAILS) >= 5)
					{
						//draw foot		
						setDiffuseColor(COLOR_GRAY);
						glRotated(VAL(FEET_ROTATE) + feet_angle, 0.0, 0.0, 1.0);
						glTranslated(-0.35, -0.4, 1.2);

						drawBox(0.75, 1.5, 0.3);//foot
					}
				}
			}
			glPopMatrix();
			glPopMatrix();
		}

		//draw left leg (joint->upper leg->joint->lower leg->foot)
	
		if (VAL(LEVEL_OF_DETAILS) >= 2)
		{
			
			setDiffuseColor(COLOR_DARK_RED);
			glPushMatrix();
			glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
			glTranslated(0.8, -1.6, 0.5);
			glPushMatrix();
			//glRotated(-270, 1.0, 0.0, 0.0);
			//draw upper leg
			if (VAL(ENABLE_IK)) {
				// if the ik is enabled, calculate the rotation angle accordingly 
				

				Vec3f destination = Vec3f(VAL(IK_LEG_X) +1.0, VAL(IK_LEG_Y) , VAL(IK_LEG_Z));
				ik_result = rightLeg->getResult(destination);
				glRotated(180, 1.0, 0.0, 0.0);
				glRotated(ik_result[1], 0, 1, 0);
				glRotated(ik_result[0], 1, 0, 0);
				drawCylinder(1.2, 0.4, 0.3);//upper leg

			}
			else {
				rightLeg->reset();
				glRotated(-270, 1.0, 0.0, 0.0);
				glRotated(VAL(LEFT_LEG), 1.0, 0.0, 0.0);
				if (VAL(TEXTURE) == 1)
					drawTextureCylinder(1.2, 0.4, 0.3, "Image/Red.bmp");
				else
					drawCylinder(1.2, 0.4, 0.3);//upper leg
			}
			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				setDiffuseColor(COLOR_GRAY);
				glTranslated(0, 0, 1.3);
				
				//draw joint
				
				glRotated(VAL(LEFT_KNEE), 1.0, 0.0, 0.0);
				if (VAL(METABALL) == false)
				{
					drawSphere(0.3);
				}

				else
				{
					if (VAL(METABALL) == true)
					{
						setDiffuseColor(COLOR_DARK_RED);
						MetaBall* m_metaBall = new MetaBall(1.0f);
						m_metaBall->addBallRel(0.0, -0.3, 0.12, 0.4);
						m_metaBall->addBallRel(0.0, 0.3, 0.12, 0.5);
						m_metaBall->draw(7);
						delete m_metaBall;
					}
				}
				if (VAL(LEVEL_OF_DETAILS) >= 4)
				{
					//draw lower leg
					setDiffuseColor(COLOR_DARK_RED);
					glTranslated(0, 0, 0.2);
					
					if (VAL(ENABLE_IK)) {
						
						glRotated(ik_result[3], 0, 1, 0);
						glRotated(ik_result[2], 1, 0, 0);
						drawCylinder(1.2, 0.3, 0.4);
						//printf("lower leg angle result, (%d,%d)", ik_result[3], ik_result[2]);
					}
					else {
						if (VAL(TEXTURE) == 1)
							drawTextureCylinder(1.2, 0.3, 0.4, "Image/Red.bmp");
						else
							drawCylinder(1.2, 0.3, 0.4);//lower leg
					}
						if (VAL(LEVEL_OF_DETAILS) >= 5)
						{
							//draw foot		
							setDiffuseColor(COLOR_GRAY);
							glRotated(-(VAL(FEET_ROTATE) + feet_angle), 0.0, 0.0, 1.0);
							glTranslated(-0.35, -0.4, 1.2);

							drawBox(0.75, 1.5, 0.3);//foot
						}
					
				}
			}
			glPopMatrix();
			glPopMatrix();

		}

	}
	if (VAL(ACCUMULATION_BUFFER)) {
		glAccum(GL_ACCUM, 1.0);
		//	glDrawBuffer(GL_FRONT);
		//glAccum(GL_RETURN, 1.0);
		endDraw();
		//glAccum(GL_RETURN, 1.0);
		//glDrawBuffer(GL_BACK);
		//glClear(GL_ACCUM_BUFFER_BIT);
	}
	else
		endDraw();
	
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	controls[LIGHT_X] = ModelerControl("Light0 X", -5, 5, 0.1, 4.0);
	controls[LIGHT_Y] = ModelerControl("Light0 Y", -5, 5, 0.1, 2.0);
	controls[LIGHT_Z] = ModelerControl("Light0 Z", -5, 5, 0.1, -4.0);
	controls[LIGHT_R] = ModelerControl("Light0 R", 0.0f, 1.0f, 0.1f, 1.0f);
	controls[LIGHT_G] = ModelerControl("Light0 G", 0.0f, 1.0f, 0.1f, 1.0f);
	controls[LIGHT_B] = ModelerControl("Light0 B", 0.0f, 1.0f, 0.1f, 1.0f);
	controls[LIGHT1_X] = ModelerControl("Light1 X", -5.0f, 5.0f, 0.1f, -2.0f);
	controls[LIGHT1_Y] = ModelerControl("Light1 Y", -5.0f, 5.0f, 0.1f, 1.0f);
	controls[LIGHT1_Z] = ModelerControl("Light1 Z", -5.0f, 5.0f, 0.1f, 5.0f);
	controls[LIGHT1_R] = ModelerControl("Light1 R", 0.0f, 1.0f, 0.1f, 1.0f);
	controls[LIGHT1_G] = ModelerControl("Light1 G", 0.0f, 1.0f, 0.1f, 1.0f);
	controls[LIGHT1_B] = ModelerControl("Light1 B", 0.0f, 1.0f, 0.1f, 1.0f);
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
	controls[HEIGHT] = ModelerControl("Neck Height", 1, 2.5, 0.1f, 1);
	controls[ROTATE] = ModelerControl("Head Rotate", -135, 135, 1, 0);
	controls[NERVOUS] = ModelerControl("Nervous", 0, 1, 1, 0);
	controls[NOSE] = ModelerControl("Nose Length", 0.0, 2, 0.1f, 0);
	controls[RIGHT_ARM] = ModelerControl("Right Arm", -90, 90, 1, 0);
	controls[LEFT_ARM] = ModelerControl("Left Arm", -90, 90, 1, 0);
	controls[RIGHT_LEG] = ModelerControl("Right Leg", -90, 90, 1, 0);
	controls[LEFT_LEG] = ModelerControl("Left Leg", -90, 90, 1, 0);
	controls[RIGHT_KNEE] = ModelerControl("Right Knee", 0, 90, 1, 0);
	controls[LEFT_KNEE] = ModelerControl("Left Knee", 0, 90, 1, 0);
	controls[FEET_ROTATE] = ModelerControl("Feet Rotate", 0, 45, 1, 0);
	controls[LEVEL_OF_DETAILS] = ModelerControl("Level of Details", 1, 5, 1, 5);
	controls[HAND_WEAPON] = ModelerControl("Hand Weapon?", 0, 1, 1, 0);
	controls[TEXTURE] = ModelerControl("Texture?", 0, 1, 1, 0);
	controls[L_SYSTEM] = ModelerControl("Display L System", 0, 1, 1, 0);
	controls[L_SYSTEM_ITERATION] = ModelerControl("L system Iteration", 1, 5, 1, 1);
	controls[FRAME_ALL] = ModelerControl("Frame All", 0, 1, 1, 0);
	controls[INDIVIDUAL_LOOK] = ModelerControl("Individual Look", 0, 1, 1, 0);
	controls[MOOD] = ModelerControl("Lying Mode", 0, 1, 1, 0);
	//controls[INVERSE_KINEMATICS] = ModelerControl("Inverse Kinematics?", 0, 1, 1, 0);
	controls[CONSTRAINT_X] = ModelerControl("Constraint 1 X", -5, 5, 0.1, 0);
	controls[CONSTRAINT_Y] = ModelerControl("Constraint 1 Y", -5, 5, 0.1, 0);
	controls[CONSTRAINT_Z] = ModelerControl("Constraint 1 Z", -5, 5, 0.1, 0);
	controls[METABALL] = ModelerControl("metaball", 0, 1, 1, 0);
	controls[SKYBOX] = ModelerControl("Skybox", 0, 1, 1, 0);
	controls[HEIGHT_FIELD] = ModelerControl("Height Field", 0, 1, 1, 0);
	controls[PARTICLE_NUM] = ModelerControl("Number of particel", 0, 50, 1, 5);
	controls[ACCUMULATION_BUFFER] = ModelerControl("accumulation buffer", 0, 1, 1, 0);
	controls[ENABLE_IK] = ModelerControl("Inverse Kinematics", 0, 1, 1, 0);
	controls[IK_LEG_X] = ModelerControl("IK arm X-axis", 0, 2, 0.1f, 0);
	controls[IK_LEG_Y] = ModelerControl("IK arm Y-axis", -1, 1, 0.1f, 0);
	controls[IK_LEG_Z] = ModelerControl("IK arm Z-axis", -2, 2, 0.1f, 0);

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.

	ParticleSystem *ps = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(ps);

	ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);
	return ModelerApplication::Instance()->Run();
}
