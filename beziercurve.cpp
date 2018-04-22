#include "beziercurve.h"
#include "mat.h"
#include "vec.h"

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, std::vector<Point>& ptvEvaluatedCurvePts, const float& fAniLength, const bool& bWrap) const
{
	/*
	wrapping:
	case1:node is on the generated curve
	case2: if it isnt, then use ;omear omterpolation
	*/

	ptvEvaluatedCurvePts.clear();
	vector<Point> curve;
	curve.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap == true)
		curve.push_back(Point(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y));


}