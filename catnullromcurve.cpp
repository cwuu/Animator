#include "catnullromcurve.h"
#include "mat.h"
#include "vec.h"

#define SAMPLE 150

void CatnullRomEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, std::vector<Point>& ptvEvaluatedCurvePts, const float& fAniLength, const bool& bWrap, const bool& bAdaptive, const double& dTension) const
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

		curve.push_back(ptvCtrlPts[0]);
		curve.push_back(ptvCtrlPts[0]);
		curve.insert(curve.end(), ptvCtrlPts.begin(), ptvCtrlPts.end());
		curve.push_back(ptvCtrlPts.back());
		curve.push_back(ptvCtrlPts.back());
	}


	for (int i = 0; i + 3 < curve.size(); ++i)
	{
		Vec4d px(curve[i].x, curve[i + 1].x, curve[i + 2].x, curve[i + 3].x);
		Vec4d py(curve[i].y, curve[i + 1].y, curve[i + 2].y, curve[i + 3].y);

		for (int i = 0; i < SAMPLE; i++)
		{


			const Mat4d M
			(-1*dTension/ 2.0, 3 * dTension/ 2.0, -3 * dTension/ 2.0, 1 * dTension / 2.0,
				2 * dTension / 2.0, -5 * dTension / 2.0, 4 * dTension / 2.0, -1 * dTension / 2.0,
				-1 * dTension / 2.0, 0, 1 * dTension / 2.0, 0,
				0, 2 * dTension / 2.0, 0, 0);
			
			const double interval = i / (double)SAMPLE;
			Vec4d T(interval* interval* interval, interval* interval, interval, 1);
			Point bezier_point(T*M*px, T*M*py); //weighted bezier curve
			//deal with the ending point
			if (bezier_point.x <= ptvEvaluatedCurvePts.back().x  && ptvEvaluatedCurvePts.empty() == false)
				continue;
			ptvEvaluatedCurvePts.push_back(bezier_point);
			
		}


	}

	if (bWrap == false)
	{
		ptvEvaluatedCurvePts.push_back(Point(0, ptvEvaluatedCurvePts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvEvaluatedCurvePts.back().y));
	}

}