#include "ik.h"
#include "../mat.h"
#include <cstdlib>
#include <iostream>
#include <math.h>
using namespace std;


/* inverse kinematics using CCD methods*/
InverseKinematics::InverseKinematics(Vec3f end, float armLength1, float armLength2) {
	endPoint = end;//set end point location

				   //set arm length
	arm1Length = armLength1;
	arm2Length = armLength2;

	//set arm direction vector
	arm1[0] = 0; arm1[1] = -1; arm1[2] = 0;
	arm2[0] = 0; arm2[1] = -1; arm2[2] = 0;

	//set joint location and effector location
	joint = endPoint; joint[1] -= armLength1;
	effector = joint; effector[1] -= armLength2;
	//cout << "initial effector:(" << effector[0] << "," << effector[1] << "," << effector[2] << ")" << endl;
}

Vec4f InverseKinematics::getResult(Vec3f& destination) {
	//cout << destination[0] << "," << destination[1] << "," << destination[2] << endl;
	Vec3f yaxes(0, 1, 0);
	if (enableConstraint&&destination[2] == 0)
		destination[2] == -0.01;
	if (enableConstraint&&destination[0] > -0.7&&destination[2] < 0)
		destination[0] = -0.7;
	for (int time = 0; time < 100; time++) {
		//1.calculate direction of joint and destination
		effector = destination;
		Vec3f dir0 = destination - joint;
		if (dir0 * arm2 == 0) {
			Vec3f delta(0.1, 0.1, 0.1);
			joint = joint + delta;
		}
		arm2 = destination - joint;
		arm2.normalize();
		//2.move the arm2 long the direction
		joint = destination - arm2Length * arm2;
		//3.calculate direction of endpoint and joint
		arm1 = joint - endPoint;
		arm1.normalize();

		Vec3f fakeEndPoint = joint - arm1Length * arm1;
		//5.move whole things back to end point without changing the angle
		Vec3f dir = fakeEndPoint - endPoint;
		joint -= dir;
		effector -= dir;
		//6.calculate the distance between effector and destination and check whether distance smaller than threshold,if yes,done
		float currentDistance = dir.length();
		if (currentDistance < THRESHOLD) {
			//cout << "effector:(" << effector[0] << "," << effector[1] << "," << effector[2] << ")" << endl;
			//cout << "destination:"<<destination[0] << "," << destination[1] << "," << destination[2] << endl;
			return calculateAngle();
		}
		//7.if 6 doesn`t satisfied,check angle constraints and processing time constraints, if satisfied ,go to step1
		//if no, return

	}

	return calculateAngle();
}

Vec4f InverseKinematics::calculateAngle() {
	//cout << "(" << effector[0] << "," << effector[1] << "," << effector[2] << ")" << endl;
	Vec4f result;
	float pi = 3.1415926;

	Vec3f yaxes(0, 1, 0);
	Vec3f zaxes(0, 0, 1);

	Vec3f axeVec = (arm1 * yaxes) * yaxes; //the projection of arm1 on y axes

	Vec3f rotationVec = arm1 - axeVec;
	//Vec3f rotationPlaneVec
	//cout <<"rotation vector:"<< rotationVec[0] << "," << rotationVec[1] << "," << rotationVec[2] << endl;

	Vec3f planeVec1(0, axeVec[1], rotationVec.length());

	if (axeVec.length() == 0)
		result[0] = -90;
	else
		result[0] = -(acosf(planeVec1 * (-yaxes)) / pi) * 180;

	if (rotationVec.length() == 0)
		result[1] = 0;
	else
		result[1] = (acosf((zaxes * rotationVec) / rotationVec.length()) / pi) * 180;

	if (arm1[0] < 0)
		result[1] = -result[1];
	//if (arm1[2] < 0)
	//result[0] = -result[0];

	axeVec = (arm2 * yaxes) * yaxes;
	rotationVec = arm2 - axeVec;
	Vec3f planeVec2(0, axeVec[1], rotationVec.length());

	if (rotationVec.length() == 0)
		result[3] = 0;
	else
		result[3] = (acosf((zaxes * rotationVec) / rotationVec.length()) / pi) * 180;
	//cout << result[3] << endl;
	if (axeVec.length() == 0)
		result[2] = -90;
	else
		result[2] = -(acosf(planeVec2 * (-yaxes)) / pi) * 180;

	if (arm2[0] < 0)
		result[3] = -result[3];
	//if (arm2[2] < 0)
	//result[2] = -result[2];


	return result;
}

void InverseKinematics::setConstraint(bool value) {
	enableConstraint = value;
}

void InverseKinematics::setConstraint1(float value) {
	float radian = value / 180 * 3.1415926;
	constraint1 = sinf(radian);
	if (value > 90)
		largerThan901 = true;
	else
		largerThan901 = false;
}

void InverseKinematics::setConstraint2(float value) {
	constraint2 = value / 180 * 3.1415926;
}

void InverseKinematics::reset() {
	arm1.zeroElements();
	arm1[1] = -1;
	arm2.zeroElements();
	arm2[1] = -1;
	joint = endPoint; joint[1] -= arm1Length;
	effector = joint; effector[1] -= arm1Length;
}