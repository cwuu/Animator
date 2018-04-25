#pragma once

#ifndef _SUBDIVISION_H_
#define _SUBDIVISION_H_
#include "curveevaluator.h"

using namespace std;

class SubdivisionCurveEvaluator : public CurveEvaluator
{
public:
	SubdivisionCurveEvaluator() {
		Mask = 0.5;
	}
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts,
		std::vector<Point>& ptvEvaluatedCurvePts,
		const float& fAniLength,
		const bool& bWrap,
		const bool& bAdaptive) const;
	void setMask(float value) { Mask = value; }

private:
	float Mask;
};

#endif 