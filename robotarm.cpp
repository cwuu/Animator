// The sample model.  You should build a file
// very similar to this for when you make your model.

#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#pragma warning(disable : 4786)

#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <cstring>
#include <FL/glut.h>
#include "modelerglobals.h"
#include "particleSystem.h"
#include "metaball.h"
#include "mat.h"
#include "vec.h"
using namespace std;


// To make a SampleModel, we inherit off of ModelerView
class SampleModel : public ModelerView
{
public:
	SampleModel(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label) { }

	virtual void draw();
	int feet_angle = 0;
	bool feet_dir = true; // false= negative, true = positive
	bool heart_beat = false;
	int beat_count = 0;
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

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel
void SampleModel::draw()
{
	//the light



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
	//SpawnParticles(CameraMatrix, VAL(PARTICLE_NUM));
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
				glTranslated(0, 0, 1.3);
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
			//draw upper leg
			glRotated(-270, 1.0, 0.0, 0.0);
			glRotated(VAL(LEFT_LEG), 1.0, 0.0, 0.0);
			if (VAL(TEXTURE) == 1)
				drawTextureCylinder(1.2, 0.4, 0.3, "Image/Red.bmp");
			else
				drawCylinder(1.2, 0.4, 0.3);//upper leg

			if (VAL(LEVEL_OF_DETAILS) >= 3)
			{
				//draw joint
				setDiffuseColor(COLOR_GRAY);
				glTranslated(0, 0, 1.3);
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
					if (VAL(TEXTURE) == 1)
						drawTextureCylinder(1.2, 0.3, 0.4, "Image/Red.bmp");
					else
						drawCylinder(1.2, 0.3, 0.4);//lower leg

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
	controls[INVERSE_KINEMATICS] = ModelerControl("Inverse Kinematics?", 0, 1, 1, 0);
	controls[CONSTRAINT_X] = ModelerControl("Constraint 1 X", -5, 5, 0.1, 0);
	controls[CONSTRAINT_Y] = ModelerControl("Constraint 1 Y", -5, 5, 0.1, 0);
	controls[CONSTRAINT_Z] = ModelerControl("Constraint 1 Z", -5, 5, 0.1, 0);
	controls[METABALL] = ModelerControl("metaball", 0, 1, 1, 0);
	controls[PARTICLE_NUM] = ModelerControl("Number of particel", 0, 50, 1, 5);

	// You should create a ParticleSystem object ps here and then
	// call ModelerApplication::Instance()->SetParticleSystem(ps)
	// to hook it up to the animator interface.

	ParticleSystem *ps = new ParticleSystem();
	ModelerApplication::Instance()->SetParticleSystem(ps);

	ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);
	return ModelerApplication::Instance()->Run();
}
