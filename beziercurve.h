#pragma once
#ifndef _BEZIERCURVE_H_
#define _BEZIERCURVE_H_

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"
#include "vec.h"

//using namespace std;

class BezierCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap) const;

	void displayBezier(std::vector<Point>& ptvEvaluatedCurvePts, const Vec4d& px, const Vec4d& py);
};

#endif 