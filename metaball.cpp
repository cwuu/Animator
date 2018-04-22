#include "MetaBall.h"
#include "modelerdraw.h"
#include "mat.h"

MetaBall::MetaBall(float mult)
{
	initialized = false;
	ModelerDrawState *mds = ModelerDrawState::Instance();
	int quality;

	if (mds->m_rayFile) { return; }

//	_setupOpenGl();

	switch (mds->m_quality)
	{
	case HIGH:
		quality = 120 * mult; break;
	case MEDIUM:
		quality = 88 * mult; break;
	case LOW:
		quality = 56 * mult; break;
	case POOR:
		quality = 24 * mult; break;
	}

	m_marchingCube = new MarchingCube(quality);
	initialized = true;
}

void MetaBall::cal()
{
	for (int i = 0; i < m_marchingCube->numVertices; ++i)
	{
		m_marchingCube->m_vertices[i].value = 0.0f;
		m_marchingCube->m_vertices[i].normal.zeroElements();
	}

	Vec3f ballToPoint;
	float squaredRadius;
	Vec3f ballPosition;
	float normalScale;

	for (std::vector<Ball>::iterator it = m_metaBalls.begin(); it != m_metaBalls.end(); it++)
	{
		squaredRadius = it->squaredRadius;
		ballPosition = it->position;

		for (int j = 0; j < m_marchingCube->numVertices; j++) {
			ballToPoint = m_marchingCube->m_vertices[j].position - ballPosition;

			float squaredDistance = ballToPoint.length2();

			if (squaredDistance == 0.0f)
				squaredDistance = 0.0001f;

			m_marchingCube->m_vertices[j].value += squaredRadius / squaredDistance;
			normalScale = squaredRadius / (squaredDistance*squaredDistance);

			m_marchingCube->m_vertices[j].normal += ballToPoint*normalScale;
		}
	}
}

void MetaBall::draw(float threshold)
{
	cal();
	m_marchingCube->drawSurface(threshold);
}

void MetaBall::addBallAbs(Vec3f center, float radius)
{
	m_metaBalls.push_back(Ball(center, radius));
}

void MetaBall::addBallRel(float x, float y, float z, float radius)
{
	Vec3f center(x, y, z);
	Mat4f modelMtx;
	glGetFloatv(GL_MODELVIEW_MATRIX, modelMtx[0]);
	Vec3f abs = modelMtx * center;
	addBallAbs(abs, radius);
}

MetaBall::~MetaBall()
{
	delete m_marchingCube;
}