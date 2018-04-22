
//	resource:http://www.paulsprojects.net/NewBSDLicense.txt)


#include "Marching_Cube.h"

MarchingCube::MarchingCube(int numGrid)
{
	// because the property of square
	numVertices = (numGrid + 1) * (numGrid + 1) * (numGrid + 1);
	m_vertices = new mcVertex[numVertices];

	// because the property of cube (rubbish comment)
	numCubes = numGrid * numGrid * numGrid;
	m_cubes = new mcCube[numCubes];

	int cnt = 0;
	for (int i = 0; i < numGrid + 1; ++i)
	{
		for (int j = 0; j < numGrid + 1; ++j)
		{
			for (int k = 0; k < numGrid + 1; ++k)
			{
				m_vertices[cnt].position = Vec3f(
					(i*20.0f) / (numGrid)-10.0f,
					(j*20.0f) / (numGrid)-10.0f,
					(k*20.0f) / (numGrid)-10.0f);
				cnt++;
			}
		}
	}

	cnt = 0;
	for (int i = 0; i < numGrid; ++i)
	{
		for (int j = 0; j < numGrid; ++j)
		{
			for (int k = 0; k < numGrid; ++k)
			{
				// May not so friendly in the first see, however, just list them by hand, suppose num Grid is 2
				// 0, 1, 3, 4, 9, 10, 12, 13, just the first cube
				m_cubes[cnt].cbVertices[0] = &m_vertices[(i*(numGrid + 1) + j)*(numGrid + 1) + k];
				m_cubes[cnt].cbVertices[1] = &m_vertices[(i*(numGrid + 1) + j)*(numGrid + 1) + k + 1];
				m_cubes[cnt].cbVertices[2] = &m_vertices[(i*(numGrid + 1) + (j + 1))*(numGrid + 1) + k + 1];
				m_cubes[cnt].cbVertices[3] = &m_vertices[(i*(numGrid + 1) + (j + 1))*(numGrid + 1) + k];
				m_cubes[cnt].cbVertices[4] = &m_vertices[((i + 1)*(numGrid + 1) + j)*(numGrid + 1) + k];
				m_cubes[cnt].cbVertices[5] = &m_vertices[((i + 1)*(numGrid + 1) + j)*(numGrid + 1) + k + 1];
				m_cubes[cnt].cbVertices[6] = &m_vertices[((i + 1)*(numGrid + 1) + (j + 1))*(numGrid + 1) + k + 1];
				m_cubes[cnt].cbVertices[7] = &m_vertices[((i + 1)*(numGrid + 1) + (j + 1))*(numGrid + 1) + k];
				cnt++;
			}
		}
	}

}

void MarchingCube::drawSurface(float threshold)
{
	numSurface = 0;
	static sfVertex edgeVertices[12];
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < numCubes; ++i)
		{
			unsigned char cubeIndex = 0;
			if (m_cubes[i].cbVertices[0]->value < threshold) cubeIndex |= 1;
			if (m_cubes[i].cbVertices[1]->value < threshold) cubeIndex |= 2;
			if (m_cubes[i].cbVertices[2]->value < threshold) cubeIndex |= 4;
			if (m_cubes[i].cbVertices[3]->value < threshold) cubeIndex |= 8;
			if (m_cubes[i].cbVertices[4]->value < threshold) cubeIndex |= 16;
			if (m_cubes[i].cbVertices[5]->value < threshold) cubeIndex |= 32;
			if (m_cubes[i].cbVertices[6]->value < threshold) cubeIndex |= 64;
			if (m_cubes[i].cbVertices[7]->value < threshold) cubeIndex |= 128;

			int usedEdge = edgeTable[cubeIndex];

			if (usedEdge == 0 || usedEdge == 255) continue;

			for (int currentEdge = 0; currentEdge < 12; ++currentEdge)
			{
				if (usedEdge & (1 << currentEdge))
				{
					mcVertex* v1 = m_cubes[i].cbVertices[verticesAtEndsOfEdges[currentEdge * 2]];
					mcVertex* v2 = m_cubes[i].cbVertices[verticesAtEndsOfEdges[currentEdge * 2 + 1]];

					float delta = (threshold - v1->value) / (v2->value - v1->value);
					//edgeVertices[currentEdge].position=v1->position + delta*(v2->position - v1->position);
					edgeVertices[currentEdge].position[0] = v1->position[0] + delta*(v2->position[0] - v1->position[0]);
					edgeVertices[currentEdge].position[1] = v1->position[1] + delta*(v2->position[1] - v1->position[1]);
					edgeVertices[currentEdge].position[2] = v1->position[2] + delta*(v2->position[2] - v1->position[2]);
					//edgeVertices[currentEdge].normal=v1->normal + delta*(v2->normal - v1->normal);
					edgeVertices[currentEdge].normal[0] = v1->normal[0] + delta*(v2->normal[0] - v1->normal[0]);
					edgeVertices[currentEdge].normal[1] = v1->normal[1] + delta*(v2->normal[1] - v1->normal[1]);
					edgeVertices[currentEdge].normal[2] = v1->normal[2] + delta*(v2->normal[2] - v1->normal[2]);
				}
			}

			for (int k = 0; triTable[cubeIndex][k] != -1; k += 3)
			{
				glNormal3fv(edgeVertices[triTable[cubeIndex][k + 0]].normal.getPointer());
				glVertex3fv(edgeVertices[triTable[cubeIndex][k + 0]].position.getPointer());

				glNormal3fv(edgeVertices[triTable[cubeIndex][k + 2]].normal.getPointer());
				glVertex3fv(edgeVertices[triTable[cubeIndex][k + 2]].position.getPointer());

				glNormal3fv(edgeVertices[triTable[cubeIndex][k + 1]].normal.getPointer());
				glVertex3fv(edgeVertices[triTable[cubeIndex][k + 1]].position.getPointer());

				numSurface++;
			}

		}

	}
	glEnd();
}

MarchingCube::~MarchingCube()
{
	if (m_vertices) delete[] m_vertices;
	m_vertices = NULL;
	numVertices = 0;
	if (m_cubes) delete[] m_cubes;
	m_cubes = NULL;
	numCubes = 0;
}