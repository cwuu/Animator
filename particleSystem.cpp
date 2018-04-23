#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "modelerdraw.h"
#include <ctime>

#define ParticleSize 3
#define gravity 9.8
#define K 0.25 //todo: find realistic k

void Particle::update(double timeStep)
{
	velocity = velocity + netForce / mass * timeStep;
	position = position + velocity * timeStep;
	Vec3d force = this->getNetForce() - K * this->getVelocity();
	force = force + Vec3d(gravity, gravity, gravity)* this->getMass();
	setNetForce(force);

}
void Particle::draw()
{
	setDiffuseColor(0, 0, 1); 
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
			for (int i = 0; i < particles.size(); ++i)
				particles[i].update(bake_fps);

			bakeParticles(t);
		}
		else
			particles = bakeContainer[t];

	}
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





