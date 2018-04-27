#pragma once
#ifndef INVERSE_KINEMATICS
#define INVERSE_KINEMATICS
#include "../vec.h"

class InverseKinematics {//using ccd method
private:
	Vec3f endPoint;
	Vec3f arm1;//the direction vector of arm1


	Vec3f arm2;//the dirction vector of arm2

	Vec3f effector;

	float arm1Length;//the length of the arm between end point and joint
	float arm2Length;//the length of the arm between joint and effector

	const float THRESHOLD = 1;
	bool enableConstraint = false;
	float constraint1;//store the constraint angle in sine, used to calculate vector
	float constraint2;

	bool largerThan901;//whether constraint1 is bigger than 90 
	bool largerThan902;//whether constraint2 is bigger than 90

public:
	Vec3f joint;
	InverseKinematics(Vec3f end, float armLength1, float armLength2);

	Vec4f getResult(Vec3f& destination);

	//giving normal vector arm1 and arm2,return the rotation angle
	//rotation sequence: 
	//arm1: rotate about y ,then about x
	//arm2: rotate about x of arm1 coords
	Vec4f calculateAngle();
	void setConstraint(bool value);
	void setConstraint1(float value);
	void setConstraint2(float value);
	void reset();
};

#endif // !INVERSE_KINEMATICS