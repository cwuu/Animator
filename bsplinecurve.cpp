#include "bsplinecurve.h"
#include "beziercurve.h"
#include "mat.h"
#include "vec.h"

#define SAMPLE 15

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,std::vector<Point>& ptvEvaluatedCurvePts,const float& fAniLength,const bool& bWrap,const bool& bAdaptive, const double& dTension) const
{
	
	ptvEvaluatedCurvePts.clear();
	std::vector<Point> curve;

	if (bWrap == true) 
	{
		curve.push_back(Point((ptvCtrlPts.end() - 2)->x - fAniLength, (ptvCtrlPts.end() - 2)->y));
		curve.push_back(Point((ptvCtrlPts.end() - 1)->x - fAniLength, (ptvCtrlPts.end() - 1)->y));
		curve.insert(curve.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		curve.push_back(Point((ptvCtrlPts.begin())->x + fAniLength, (ptvCtrlPts.begin())->y));
		curve.push_back(Point((ptvCtrlPts.begin() + 1)->x + fAniLength, (ptvCtrlPts.begin() + 1)->y));
	}
	else 
	{
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
		curve.push_back(ptvCtrlPts[0]);
		curve.push_back(ptvCtrlPts[0]);
		curve.insert(curve.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		curve.push_back(ptvCtrlPts.back());
		curve.push_back(ptvCtrlPts.back());
	}
	


	/*
	displaybspline
	for (i=0 to n-3)
		convert point to bezier control point
		displaybezier
	*/
	BezierCurveEvaluator beziercurve;
	const Mat4d M(
		1 / 6.0, 4 / 6.0, 1 / 6.0, 0,
		0, 4 / 6.0, 2 / 6.0, 0,
		0, 2 / 6.0, 4 / 6.0, 0,
		0, 1 / 6.0, 4 / 6.0, 1 / 6.0) ;

	for (int i = 0; i + 3 < curve.size(); ++i)
	{
		Vec4d bezier_x(curve[i].x, curve[i + 1].x, curve[i + 2].x, curve[i + 3].x);
		Vec4d bezier_y(curve[i].y, curve[i + 1].y, curve[i + 2].y, curve[i + 3].y);
		Vec4d convert_x = M * bezier_x;
		Vec4d convert_y = M * bezier_y;
		std::vector<Point> combine;

		for (int j = 0; j < 4; ++j)
			combine.push_back(Point(convert_x[j], convert_y[j]));

		std::vector<Point> epts;
		beziercurve.evaluateCurve(combine, epts, fAniLength, false, bAdaptive,dTension);
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.end(), epts.begin(), epts.end());


	}

}