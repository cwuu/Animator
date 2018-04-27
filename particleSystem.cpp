#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "modelerdraw.h"
#include <ctime>
#include "modelerapp.h"
#include<iostream>
#define ParticleSize 7
#define gravity 9.8

#define   m_groundY -50

double K = 0.25;
void Particle::update(double timeStep)
{
	velocity = velocity + netForce / mass * timeStep;
	position = position + velocity * timeStep;
	Vec3d force = this->getNetForce() - K * this->getVelocity();
	force = force + Vec3d(0.0, -1.0, 0.0)* this->getMass()*gravity;
	setNetForce(force);

}
void Particle::bounce(double timeStep, Particle& p)
{
	//v1*=(m1-m2)/(m1+m2) * v1
	//v2*=(2m1)/(m1+m2) * v1
	//velocity = velocity + netForce / mass * timeStep;

	velocity = velocity*(mass - p.mass) / (mass + p.mass);
	p.velocity = velocity*(2 * mass) / (mass + p.mass);
	//cout << "velocity" << velocity << endl;
	//cout << "p.velocity" << p.velocity << endl;

	position = position + velocity * timeStep;
	Vec3d force = this->getNetForce() - K * this->getVelocity();
	force = force + Vec3d(0.0, -1.0, 0.0)* this->getMass()*gravity;
	setNetForce(force);

}
void Particle::draw()
{
	setDiffuseColor(1, 0, 0);
	glPushMatrix();
	glPointSize(ParticleSize);
	glBegin(GL_POINTS);
	glVertex3f(position[0], position[1], position[2]);
	glEnd();
	glPopMatrix();
	glPointSize(1);
}

/***************
* Constructors
***************/

ParticleSystem::ParticleSystem()
{
	// TODO
	srand(time(0));
}



/*************
* Destructor
*************/

ParticleSystem::~ParticleSystem()
{
	// TODO
	particles.clear();

}


/******************
* Simulation fxns
******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{

	// TODO

	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{

	// TODO

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{

	// TODO

	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{

	// TODO
	bake_fps = t - currentT; // bake sampling rate
	currentT = t;

	map<double, std::vector<Particle>>::iterator it = bakeContainer.find(t);
	bool BakeExisted = (it != bakeContainer.end());

	if (isSimulate() == true)
	{
		if (BakeExisted == false)
		{

			if (ModelerApplication::Instance()->bounce())
			{
				for (int i = 0; i < particles.size(); ++i)
				{
					for (int j = i + 1; j < particles.size(); ++j)
					{
						double dx = abs(particles[i].position[0] - particles[j].position[0]);
						double dy = abs(particles[i].position[1] - particles[j].position[1]);
						double dz = abs(particles[i].position[2] - particles[j].position[2]);
						double distance = sqrt(dx*dx + dy*dy + dz*dz);
						//cout << "velocity" << particles[i].velocity;
						if (distance < 14)
						{
							//particles[i].bounce(bake_fps, particles[j]);

							Particle temp(particles[i]);
							temp.velocity = (temp.velocity*(particles[i].mass - particles[j].mass) / (particles[i].mass - particles[j].mass)*10.0);
							//particles[j].velocity = (particles[i].velocity*(2 * particles[i].mass) / (particles[i].mass + particles[j].mass)*10.0);
							//cout << "     " << temp.velocity << endl;
							particles[i].update(bake_fps);
							particles[j].update(bake_fps);

						}
						else
						{

							particles[i].update(bake_fps);
							//cout << "     " << particles[i].velocity << endl;

						}
						cout << distance << endl;

					}
					//
					//particles[i].update(bake_fps);

				}

			}
			else if (ModelerApplication::Instance()->rb())
			{

				Vec3d boundary((double)ModelerApplication::Instance()->GetControlValue(XPOS),
					(double)ModelerApplication::Instance()->GetControlValue(YPOS),
					(double)ModelerApplication::Instance()->GetControlValue(ZPOS));

				for (int i = 0; i < particles.size(); ++i)
				{
					/*
					if (particles[i].position[2] < (-1.0 + boundary[2]) || particles[i].position[2] < (1.0 + boundary[2]))
					{
					cout << "ffff" << endl;
					particles[i].velocity[2] *= (-1);
					particles[i].update(bake_fps);
					}
					*/
					//cout << particles[i].position[2] << endl;
					if (particles[i].position[2] < 0)
					{
						cout << "ffff" << endl;
						//cout << particles[i].position[2] << endl;
						for (int i = 0; i < 30; i++)
						{
							particles[i].velocity[2] *= (-1);
							K = -K;
							particles[i].update(bake_fps);
						}

					}
					else
					{
						cout << "jjjjjj" << endl;
						particles[i].update(bake_fps);
					}
					particles[i].update(bake_fps);
					/*if (particles[i].position[2] < (-1000.0 + boundary[2]))
					{
					cout << "ffff" << endl;
					particles[i].velocity[2] *= (-1);
					particles[i].update(bake_fps);
					}*/

				}
			}
			else
			{
				for (int i = 0; i < particles.size(); ++i)
				{

					particles[i].update(bake_fps);
				}

			}
			bakeParticles(t);
		}
		else
			particles = bakeContainer[t];

	}


	//evolve
	//particle.position += particle.velocity * delta_t;

	/*else if (ModelerApplication::Instance()->flock()) {
	//steer towards the average of neighbors
	double radius = 1.5;
	Vec3d avgV;
	int count = 0;
	for (auto par : m_cache[frame]) {
	if ((par.position - particle.position).length2() < radius * radius) {
	++count;
	avgV += par.velocity;
	}
	}
	avgV /= count;
	particle.velocity = 0.5 * avgV + 0.5 * particle.velocity;
	}
	else {
	particle.velocity += m_gravity * delta_t;
	*/

}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{

	// TODO
	if (isSimulate())
	{
		for (int i = 0; i < particles.size(); ++i)
			particles[i].draw();
	}


}





/** Adds the current configuration of particles to
* your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(double t)
{

	// TODO
	// map (container) + pair (linked)
	bakeContainer.insert(std::pair<double, std::vector<Particle>>(t, particles));
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{

	// TODO
	bakeContainer.clear();
}


