#pragma once
#ifndef _C2INTERPOLATING_H_
#define _C2INTERPOLATING_H_
#include "curveevaluator.h"

class C2InterpolatingEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& controlPoints,
		std::vector<Point>& evaluatedPoints,
		const float& animationLength,
		const bool& bWrap,
		const bool& bAdaptive,
		const double& dTension) const ;

};

#endif

