#pragma once
/*#ifndef _SUBDIVISIONSURFACE_H_
#define _SUBDIVISIONSURFACE_H_


#include "vec.h"
#include <vector>
#include <math.h>
#include <FL/gl.h>


#define PI 3.14159265359

using namespace std;
class Triface;
class Vertex {
public:
	Vec3f position;
	vector<Vertex*> neighbours;
	float alpha() 
	{
		int n = neighbours.size();
		float betaN = 5 / 4.0f - pow(3 + 2 * cosf(2 * PI / n), 2) / 32.0f;
		return n * (1 - betaN) / betaN;
	}

	Vec3f normal;


	static vector<float> averageMask;

	Vertex(Vec3f pos) { position = pos; }

	void updateNeighbour(Vertex* origin, Vertex* newOne)
	{
		for (int i = 0; i < neighbours.size(); i++) 
		{
			if (neighbours[i] == origin) 
			{
				neighbours.at(i) = newOne;
				return;
			}
			if (neighbours[i] == newOne)
				return;
		}
	}
	void addNormal(Vec3f& normal) { this->normal += normal;}
	Vec3f getNormal() 
	{
		normal.normalize();
		return normal;
	}
	void addNeightBour(Vertex* neighbour) { neighbours.push_back(neighbour);}

	void average() 
	{
		float alphaN = alpha();
		int n = neighbours.size();
		Vec3f sum;
		for (int i = 0; i < n; i++) 
		{
			float factor = 1;
			if (averageMask.size() > i)
				factor = averageMask[i];
			sum = sum + neighbours[i]->position * factor;
		}
		sum += alphaN * position;
		position = sum / (alphaN + n);
	}

	Vec3f& getPosition() { return position; }
};

class Edge {
public:
	Edge* subEdges[2];
	Vertex* midPoint;
	Vertex* vertices[2];
	Triface* faces[2];
	bool hasBeenAdded;
	Edge(Vertex* v1, Vertex* v2)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		subEdges[0] == NULL;
		subEdges[1] == NULL;
		midPoint = NULL;
		faces[0] = NULL;
		faces[1] = NULL;
		hasBeenAdded = false;

	}

	void setNeighbour() 
	{
		vertices[0]->addNeightBour(vertices[1]);
		vertices[1]->addNeightBour(vertices[0]);
	}

	void addMidPoint() 
	{
		if (midPoint != NULL)
			return;
		midPoint = new Vertex((vertices[0]->getPosition() + vertices[1]->getPosition()) / 2.0f);
		subEdges[0] = new Edge(vertices[0], midPoint);
		subEdges[1] = new Edge(midPoint, vertices[1]);
		midPoint->addNeightBour(vertices[0]);
		midPoint->addNeightBour(vertices[1]);
	}

	void addFaces(Triface* face) 
	{
		if (faces[0] == NULL)
			faces[0] = face;
		if (faces[1] == NULL)
			faces[1] = face;
	}

	bool has(Vertex* v) 
	{
		if (vertices[0] == v)
			return true;
		if (vertices[1] == v)
			return true;
		return false;
	}

	Edge** getSubEdges() { return subEdges;}

	Edge* getSubEdge(Vertex* v) 
	{
		if (midPoint == NULL)
			return NULL;
		if (subEdges[0]->has(v))
			return subEdges[0];
		if (subEdges[1]->has(v))
			return subEdges[1];
		return NULL;
	}

	Vertex* getMidPoint() {return midPoint;}

	Vertex* getAnotherVertex(Vertex* v)
	{
		if (vertices[0] == v)
			return vertices[1];
		if (vertices[1] == v)
			return vertices[0];
		return NULL;
	}

	void addToThis(vector<Edge*>& list) 
	{
		if (hasBeenAdded)
			return;
		hasBeenAdded = true;
		list.push_back(this);
	}

	Vertex* get(int n) { return vertices[n]; }


};

class Triface {
public:
	Edge* edges[3];
	Triface* subfaces[4];
	vector<Edge*> newEdges;
	Triface* getSubface(Vertex* v) 
	{
		Edge* subEdge[2];
		int index = 0;
		for (int i = 0; i < 3; i++) 
		{
			Edge* cur = edges[i]->getSubEdge(v);
			if (cur != NULL) 
			{
				subEdge[index] = cur;
				index++;
			}
		}

		Edge* newEdge = new Edge(subEdge[0]->getAnotherVertex(v), subEdge[1]->getAnotherVertex(v));
		newEdges.push_back(newEdge);
     	Triface* t = new Triface(subEdge[0], subEdge[1], newEdge);
		return t;
	}


	Vertex** getVertices() 
	{
		Vertex** vertices = new Vertex*[3];
		vertices[0] = edges[0]->get(0);
		vertices[1] = edges[0]->get(1);
		vertices[2] = edges[1]->getAnotherVertex(vertices[0]);
		if (vertices[2] == NULL)
			vertices[2] = edges[2]->getAnotherVertex(vertices[0]);
		return vertices;
	}

	void TriNeighbour(Vertex* v1, Vertex* v2, Vertex* v3) 
	{
	
		v1->addNeightBour(v2);
		v1->addNeightBour(v3);

		v2->addNeightBour(v1);
		v2->addNeightBour(v3);

		v3->addNeightBour(v1);
		v3->addNeightBour(v2);
	}

	Triface(Edge* edge1, Edge* edge2, Edge* edge3) 
	{

		edges[0] = edge1;
		edge1->addFaces(this);
		edges[1] = edge2;
		edge2->addFaces(this);
		edges[2] = edge3;
		edge3->addFaces(this);
	}

	void split() 
	{
		for (int i = 0; i < 3; i++)
			edges[i]->addMidPoint();

		Vertex** vertices = getVertices();

		for (int i = 0; i < 3; i++) 
			subfaces[i] = getSubface(vertices[i]);		
		subfaces[3] = new Triface(newEdges[0], newEdges[1], newEdges[2]);
		TriNeighbour(edges[0]->getMidPoint(), edges[1]->getMidPoint(), edges[2]->getMidPoint());
	}

	void draw() 
	{
		Vertex** vertices = getVertices();


		glBegin(GL_TRIANGLES);
		glNormal3fv(vertices[0]->getNormal().getPointer());
		glVertex3fv(vertices[0]->getPosition().getPointer());

		glNormal3fv(vertices[1]->getNormal().getPointer());
		glVertex3fv(vertices[1]->getPosition().getPointer());

		glNormal3fv(vertices[2]->getNormal().getPointer());
		glVertex3fv(vertices[2]->getPosition().getPointer());
		glEnd();
	}


};

class Diamond {
public:
	vector<Triface*> faces;
	vector<Vertex*>vertices;
	vector<Edge*> edges;
	Vec3f center;

	Diamond(Vec3f* positions) {
		//#1 ~ #4: edges
		//#0 & # 5: center point
		for (int i = 0; i < 6; i++) 
		{
			vertices.push_back(new Vertex(positions[i]));
			if (i != 0 && i != 5)
				center = center + positions[i];

		}

		center = (center + positions[1] + positions[2] + positions[3] + positions[4])/4.0;

		//create 12 edges
		Edge* e1 = new Edge(vertices[0], vertices[1]);
		e1->setNeighbour();
		edges.push_back(e1);
		Edge* e2 = new Edge(vertices[0], vertices[2]);
		e2->setNeighbour();
		edges.push_back(e2);
		Edge* e3 = new Edge(vertices[0], vertices[3]);
		e3->setNeighbour();
		edges.push_back(e3);
		Edge* e4 = new Edge(vertices[0], vertices[4]);
		e4->setNeighbour();
		edges.push_back(e4);
		Edge* e5 = new Edge(vertices[5], vertices[1]);
		e5->setNeighbour();
		edges.push_back(e5);
		Edge* e6 = new Edge(vertices[5], vertices[2]);
		e6->setNeighbour();
		edges.push_back(e6);
		Edge* e7 = new Edge(vertices[5], vertices[3]);
		e7->setNeighbour();
		edges.push_back(e7);
		Edge* e8 = new Edge(vertices[5], vertices[4]);
		e8->setNeighbour();
		edges.push_back(e8);
		Edge* e9 = new Edge(vertices[1], vertices[2]);
		e9->setNeighbour();
		edges.push_back(e9);
		Edge* e10 = new Edge(vertices[2], vertices[3]);
		e10->setNeighbour();
		edges.push_back(e10);
		Edge* e11 = new Edge(vertices[3], vertices[4]);
		e11->setNeighbour();
		edges.push_back(e11);
		Edge* e12 = new Edge(vertices[4], vertices[1]);
		e12->setNeighbour();
		edges.push_back(e12);
	
		//create 8 faces
		faces.push_back(new Triface(edges[1], edges[2], edges[0]));
		faces.push_back(new Triface(edges[1], edges[4], edges[0]));
		faces.push_back(new Triface(edges[3], edges[2], edges[0]));
		faces.push_back(new Triface(edges[3], edges[4], edges[0]));
		faces.push_back(new Triface(edges[1], edges[2], edges[5]));
		faces.push_back(new Triface(edges[1], edges[4], edges[5]));
		faces.push_back(new Triface(edges[3], edges[2], edges[5]));
		faces.push_back(new Triface(edges[3], edges[4], edges[5]));

	}
	void draw() 
	{
		for (Triface* t : faces) 
		{
			t->draw();
		}
	}
	void split(int n)
	{
		for (int i = 0; i < n; i++) 
		{
			vector<Triface*> newFaces;
			vector<Edge*> newEdges;
			for (Triface* t : faces) 
			{
				t->split();
				for (int i = 0; i < 4; i++)
				{
					newFaces.push_back(t->subfaces[i]);
				}
			}
			
			for (Edge* e : edges) 
			{
				e->vertices[0]->updateNeighbour(vertices[1], e->midPoint);
				e->vertices[1]->updateNeighbour(vertices[0], e->midPoint);
				newEdges.push_back(e->subEdges[0]);
				newEdges.push_back(e->subEdges[1]);
			}
			//averaging old vertices
			for (Vertex* v : vertices)
				v->average();

			for (Edge* e : edges)
				vertices.push_back(e->midPoint);

			//delete  old faces and edges
			for (Triface* t : faces)
				delete t;
			for (Edge* e : edges)
				delete e;
			faces = newFaces;
			edges = newEdges;
		}

		for (Triface* t : faces)
		{
			Vertex** vertices = t->getVertices();
			Vec3f v1 = vertices[0]->getPosition() - vertices[1]->getPosition();
			Vec3f v2 = vertices[1]->getPosition() - vertices[2]->getPosition();
			Vec3f normal = v1 ^ v2;
			normal.normalize();
			Vec3f backDir = center - vertices[0]->getPosition();
			if (normal * backDir > 0)
				normal = -normal;
			for (int i = 0; i < 3; i++)
				vertices[i]->addNormal(normal);
		}
		
	}
};
#endif
*/

#ifndef _SUBDIVISIONSURFACE_H_
#define _SUBDIVISIONSURFACE_H_

#define PI 3.14159265359
#include "vec.h"
#include <vector>
#include <math.h>
#include <FL/gl.h>

using namespace std;
class Triface;
class Vertex {
private:
	Vec3f position;
	vector<Vertex*> neighbours;
	float alpha() {
		int n = neighbours.size();
		float betaN = 5 / 4.0f - pow(3 + 2 * cosf(2 * PI / n), 2) / 32.0f;
		return n * (1 - betaN) / betaN;
	}
	Vec3f normal;
public:
	static vector<float> averageMask;

	Vertex(Vec3f pos) { position = pos; }

	void updateNeighbour(Vertex* origin, Vertex* newOne) {
		for (int i = 0; i < neighbours.size(); i++) {
			if (neighbours[i] == origin) {
				neighbours.at(i) = newOne;
				//cout << "new!:" << neighbours[i]->getPosition() << "old:" << origin->getPosition() << endl;
				return;
			}
			if (neighbours[i] == newOne)//has been already changed
				return;
		}
	}
	void addNormal(Vec3f& normal) {
		this->normal += normal;
	}
	Vec3f getNormal() {
		normal.normalize();
		return normal;
	}
	void addNeightBour(Vertex* neighbour) {
		neighbours.push_back(neighbour);
	}

	void average() {
		float alphaN = alpha();
		int n = neighbours.size();
		Vec3f sum;
		for (int i = 0; i < n; i++) {
			float factor = 1;
			if (averageMask.size() > i)
				factor = averageMask[i];
			sum += neighbours[i]->position * factor;
		}
		sum += alphaN * position;
		position = sum / (alphaN + n);
	}

	Vec3f& getPosition() {
		return position;
	}
};

class Edge {
private:
	Edge* subEdges[2];
	Vertex* midPoint;
	Vertex* vertices[2];
	Triface* faces[2];
	bool hasBeenAdded;
public:
	Edge(Vertex* v1, Vertex* v2) {
		vertices[0] = v1;
		vertices[1] = v2;
		subEdges[0] == NULL;
		subEdges[1] == NULL;
		midPoint = NULL;
		faces[0] = NULL;
		faces[1] = NULL;
		hasBeenAdded = false;

		//v1 and v2 are now new neighbours 
	}

	void setNeighbour() {
		vertices[0]->addNeightBour(vertices[1]);
		vertices[1]->addNeightBour(vertices[0]);
	}

	void addMidPoint() {
		if (midPoint != NULL)
			return;
		midPoint = new Vertex((vertices[0]->getPosition() + vertices[1]->getPosition()) / 2.0f);
		subEdges[0] = new Edge(vertices[0], midPoint);
		subEdges[1] = new Edge(midPoint, vertices[1]);
		midPoint->addNeightBour(vertices[0]);
		midPoint->addNeightBour(vertices[1]);
	}

	void addFaces(Triface* face) {
		if (faces[0] == NULL)
			faces[0] = face;
		if (faces[1] == NULL)
			faces[1] = face;
	}

	bool has(Vertex* v) {
		if (vertices[0] == v)
			return true;
		if (vertices[1] == v)
			return true;
		return false;
	}

	Edge** getSubEdges() {
		return subEdges;
	}

	Edge* getSubEdge(Vertex* v) {//get the sub edge containing this vertex
		if (midPoint == NULL)//no subedge
			return NULL;
		if (subEdges[0]->has(v))
			return subEdges[0];
		if (subEdges[1]->has(v))
			return subEdges[1];
		return NULL;
	}

	Vertex* getMidPoint() {
		return midPoint;
	}

	Vertex* getAnotherVertex(Vertex* v) {
		if (vertices[0] == v)
			return vertices[1];
		if (vertices[1] == v)
			return vertices[0];
		return NULL;
	}

	void addToThis(vector<Edge*>& list) {
		if (hasBeenAdded)
			return;
		hasBeenAdded = true;
		list.push_back(this);
	}

	void addMidPointToThis(vector<Vertex*>& list) {
		list.push_back(midPoint);
	}

	Vertex* get(int n) {
		return vertices[n];
	}

	void updateNeighbour() {
		vertices[0]->updateNeighbour(vertices[1], midPoint);
		vertices[1]->updateNeighbour(vertices[0], midPoint);
	}

	void addSubEdgeToThis(vector<Edge*>& list) {
		list.push_back(subEdges[0]);
		list.push_back(subEdges[1]);
	}
};

class Triface {
private:
	Edge* edges[3];
	Triface* subfaces[4];
	vector<Edge*> newEdges;
	Triface* getSubface(Vertex* v) {//create a surface containing this vertex
		Edge* subEdge[2];
		int index = 0;
		for (int i = 0; i < 3; i++) {
			Edge* cur = edges[i]->getSubEdge(v);
			if (cur != NULL) {
				subEdge[index] = cur;
				index++;
			}
		}
		//the index should be 2 now
		Edge* newEdge = new Edge(subEdge[0]->getAnotherVertex(v), subEdge[1]->getAnotherVertex(v));
		newEdges.push_back(newEdge);
		Triface* t = new Triface(subEdge[0], subEdge[1], newEdge);
		//cout << "subEdge1:" << subEdge[0]->get(0)->getPosition() << subEdge[0]->get(1)->getPosition() << endl;
		//cout << "subEdge2:" << subEdge[1]->get(0)->getPosition() << subEdge[1]->get(1)->getPosition() << endl;
		//cout << "newEdge:" << newEdge->get(0)->getPosition() << newEdge->get(1)->getPosition() << endl;
		//cout << endl;
		return t;
	}

	Vertex** getVertices() {
		Vertex** vertices = new Vertex*[3];
		vertices[0] = edges[0]->get(0);
		vertices[1] = edges[0]->get(1);
		vertices[2] = edges[1]->getAnotherVertex(vertices[0]);
		if (vertices[2] == NULL)
			vertices[2] = edges[2]->getAnotherVertex(vertices[0]);
		return vertices;
	}

	void TriNeighbour(Vertex* v1, Vertex* v2, Vertex* v3) {
		v1->addNeightBour(v2);
		v1->addNeightBour(v3);

		v2->addNeightBour(v1);
		v2->addNeightBour(v3);

		v3->addNeightBour(v1);
		v3->addNeightBour(v2);
	}
public:
	Triface(Edge* edge1, Edge* edge2, Edge* edge3) {
		edges[0] = edge1;
		edge1->addFaces(this);
		edges[1] = edge2;
		edge2->addFaces(this);
		edges[2] = edge3;
		edge3->addFaces(this);
	}

	void split() {
		for (int i = 0; i < 3; i++)
			edges[i]->addMidPoint();

		Vertex** vertices = getVertices();

		//add 3 faces 
		for (int i = 0; i < 3; i++) {
			subfaces[i] = getSubface(vertices[i]);
		}
		//add the center face
		subfaces[3] = new Triface(newEdges[0], newEdges[1], newEdges[2]);

		//the three new points are neighbours of each other
		TriNeighbour(edges[0]->getMidPoint(), edges[1]->getMidPoint(), edges[2]->getMidPoint());
	}

	void addSubfaceToThis(vector<Triface*>& list) {
		for (int i = 0; i < 4; i++) {
			list.push_back(subfaces[i]);
		}

	}

	void draw() {
		Vertex** vertices = getVertices();


		glBegin(GL_TRIANGLES);
		glNormal3fv(vertices[0]->getNormal().getPointer());
		glVertex3fv(vertices[0]->getPosition().getPointer());

		glNormal3fv(vertices[1]->getNormal().getPointer());
		glVertex3fv(vertices[1]->getPosition().getPointer());

		glNormal3fv(vertices[2]->getNormal().getPointer());
		glVertex3fv(vertices[2]->getPosition().getPointer());
		glEnd();
	}

	void computeNormal(Vec3f& backPoint) {
		Vertex** vertices = getVertices();
		Vec3f v1 = vertices[0]->getPosition() - vertices[1]->getPosition();
		Vec3f v2 = vertices[1]->getPosition() - vertices[2]->getPosition();
		Vec3f normal = v1 ^ v2;
		normal.normalize();
		Vec3f backDir = backPoint - vertices[0]->getPosition();
		if (normal * backDir > 0)
			normal = -normal;

		for (int i = 0; i < 3; i++)
			vertices[i]->addNormal(normal);
	}
};

class Diamond {
private:
	vector<Triface*> faces;
	vector<Vertex*>vertices;
	vector<Edge*> edges;
	Vec3f center;//the center of the diamond,used to calculate the right direction of the normal
public:
	Diamond(Vec3f* positions) {
		//#0 & #5: position of vertex points
		//#1 ~ # 4: position of edge points	
		for (int i = 0; i < 6; i++) {
			vertices.push_back(new Vertex(positions[i]));
			if (i != 0 && i != 5)
				center += positions[i];
			//cout << "vertex:" << vertices[i]->getPosition() << endl;
		}

		center /= 4.0f;
		//#0 ~ #3: upper pyramid edge
		//#4 ~ #7: middle edge
		//#8 ~ #11: lower pyramid edge
		int k = 0;
		for (int i = 1; i < 5; i++) {//1,2,3,4
			Edge* e = new Edge(vertices[0], vertices[i]);
			e->setNeighbour();
			edges.push_back(e);
		}
		for (int i = 1; i < 5; i++) {
			int next = i + 1;
			if (next > 4)//loop back
				next = 1;
			Edge* e = new Edge(vertices[i], vertices[next]);
			e->setNeighbour();
			edges.push_back(e);
		}
		for (int i = 1; i < 5; i++) {
			Edge* e = new Edge(vertices[5], vertices[i]);
			e->setNeighbour();
			edges.push_back(e);
		}

		//add faces
		for (int i = 0; i < 4; i++) {
			faces.push_back(new Triface(edges[i], edges[(i + 1) % 4], edges[i + 4]));
		}
		for (int i = 8; i < 12; i++) {
			faces.push_back(new Triface(edges[i], edges[(i - 7) % 4 + 8], edges[i - 4]));
		}
	}
	void draw() {
		for (Triface* t : faces) {
			t->draw();
		}
	}
	void split(int n) {
		for (int i = 0; i < n; i++) {
			vector<Triface*> newFaces;
			vector<Edge*> newEdges;
			for (Triface* t : faces) {
				t->split();
				t->addSubfaceToThis(newFaces);
			}
			//update the neighbour and the edges
			for (Edge* e : edges) {
				e->updateNeighbour();
				e->addSubEdgeToThis(newEdges);
			}
			//averaging the old vertices
			for (Vertex* v : vertices)
				v->average();

			for (Edge* e : edges)
				e->addMidPointToThis(vertices);
			//delete the old faces and edges
			for (Triface* t : faces)
				delete t;
			for (Edge* e : edges)
				delete e;
			faces = newFaces;
			edges = newEdges;
		}

		for (Triface* t : faces)
			t->computeNormal(center);

	}
};
#endif