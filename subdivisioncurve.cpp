#include "subdivisioncurve.h"
#include "vec.h"


void SubdivisionCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap,
	const bool& bAdaptive) const {

	vector<Point> curve;
	curve.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());

	if (bWrap) 
	{
		curve.insert(curve.begin(), Point(0, 0));
		curve.push_back(Point(fAniLength, 0));
	}

	
	for(int k=0;k<10;++k)
	{
		if (curve.size() == 2)
			break;

		//chaikiin's algorithm: split -> average
		vector<Point> split;
		split.push_back(curve[0]);

		for (int i = 0; i < curve.size() - 1; i++) 
		{
			float midx = (curve[i].x + curve[i + 1].x) / 2;
			float midy = (curve[i].y + curve[i + 1].y) / 2;
			split.push_back(Point(midx,midy));
			split.push_back(curve[i + 1]);
		}

		vector<Point> avg;
		avg.push_back(split[0]);

		for (int i = 0; i < split.size() - 1; i++)
		{
			float x = split[i].x + (split[i + 1].x - split[i].x) * Mask;
			float y = split[i].y + (split[i + 1].y - split[i].y) * Mask;
			avg.push_back(Point(x, y));
		}
		split.push_back(split.back());
		curve = avg;

	}

	ptvEvaluatedCurvePts.clear();
	ptvEvaluatedCurvePts.assign(curve.begin(), curve.end());
	         
	if (!bWrap)
	{
		ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0, curve[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, curve.back().y));
	}
}
