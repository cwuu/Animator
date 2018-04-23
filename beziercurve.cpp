#include "beziercurve.h"
#include "mat.h"
#include "vec.h"

#define SAMPLE 150

void BezierCurveEvaluator::displayBezier(std::vector<Point>& ptvEvaluatedCurvePts, const Vec4d& px, const Vec4d& py) const
{
	/*
	display(v0,v1,v2,v3)
	if(flatenough(v0,v1,v2,v3))
	line(v0,v3)
	else
	subdivided(v)=>L,R
	display(L0,L1,L2,L3)
	display(R0,R1,R2,R3)

	*/

	/*
	flatEnough = (|V0-V1|+|V1-V2|+|V2-V3|)/|V0-V3|<1+Episolon
	*/

	Point V[4];
	for (int i = 0; i < 4; ++i)
		V[i] = Point(px[i], py[i]);

	bool flatEnough = ((V[0].distance(V[1]) + V[1].distance(V[2]) + V[2].distance(V[3])) / V[0].distance(V[3]) < (1 + CurveEvaluator::s_fFlatnessEpsilon));

	if (flatEnough)
	{
		ptvEvaluatedCurvePts.push_back(V[0]);
		ptvEvaluatedCurvePts.push_back(V[3]);

	}
	else
	{
		//17-CURVE-DETAIL P.10
		Point V_[3];
		V_[0] = Point((V[0].x + V[1].x) / 2, (V[0].y + V[1].y) / 2);
		V_[1] = Point((V[1].x + V[2].x) / 2, (V[1].y + V[2].y) / 2);
		V_[2] = Point((V[2].x + V[3].x) / 2, (V[2].y + V[3].y) / 2);
		Point V__[2];
		V__[0] = Point((V_[0].x + V_[1].x) / 2, (V_[0].y + V_[1].y) / 2);
		V__[1] = Point((V_[1].x + V_[2].x) / 2, (V_[1].y + V_[2].y) / 2);
		Point Q((V__[0].x + V__[1].x) / 2, (V__[0].y + V__[1].y) / 2);
		displayBezier(ptvEvaluatedCurvePts, Vec4d(V[0].x, V_[0].x, V__[0].x, Q.x), Vec4d(V[0].y, V_[0].y, V__[0].y, Q.y));
		displayBezier(ptvEvaluatedCurvePts, Vec4d(Q.x, V__[1].x, V_[2].x, V[3].x), Vec4d(Q.y, V__[1].y, V_[2].y, V[3].y));
	}
}
void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, std::vector<Point>& ptvEvaluatedCurvePts, const float& fAniLength, const bool& bWrap, const bool& bAdaptive) const
{
	/*
	wrapping: (similar to the concept in linecurvewvaluator)
	case1:node is on the generated curve
	case2: if it isnt, then use ;omear interpolation
	*/

	ptvEvaluatedCurvePts.clear();
	std::vector<Point> curve;
	curve.assign(ptvCtrlPts.begin(), ptvCtrlPts.end()); //possilby get nothing

	if (bWrap == true)
		curve.push_back(Point(ptvCtrlPts[0].x + fAniLength, ptvCtrlPts[0].y)); //make the beginning link to the end

	//ptvEvaluatedCurvePts.clear();
	//ptvEvaluatedCurvePts.push_back(*(ptvCtrlPts.begin()));

	int i = 0;
	bool Iswrapped = false;


	for (i = 0; i + 3 < curve.size(); i = i + 3)
	{

		//get v0 v1 v2 v3
		ptvEvaluatedCurvePts.push_back(curve[i]); // starting point
		ptvEvaluatedCurvePts.push_back(curve[i + 3]); //ending point
		const Vec4d px(curve[i].x, curve[i + 1].x, curve[i + 2].x, curve[i + 3].x);
		const Vec4d py(curve[i].y, curve[i + 1].y, curve[i + 2].y, curve[i + 3].y);


		if (bAdaptive == false)
		{
			for (int i = 0; i < SAMPLE; i++)
			{

				const Mat4d M
				(-1, 3, -3, 1,
					3, -6, 3, 0,
					-3, 3, 0, 0,
					1, 0, 0, 0);

				const double interval = i / (double)SAMPLE;
				Vec4d T(interval* interval* interval, interval* interval, interval, 1);
				Point bezier_point(T*M*px, T*M*py); //weighted bezier curve

				// constraint the maximum time that a curve is defined
				if (bWrap == true && bezier_point.x > fAniLength)
				{
					const float x_mod = fmod(bezier_point.x, fAniLength);
					//deal with the ending point
					if (Iswrapped == false)
					{
						const Point prev_point(ptvEvaluatedCurvePts.back());
						const float x_neg1 = prev_point.x - fAniLength;
						ptvEvaluatedCurvePts.push_back(Point(x_neg1, prev_point.y));
						ptvEvaluatedCurvePts.push_back(bezier_point);
						Iswrapped = true;
					}
					bezier_point.x = x_mod;
				}
				ptvEvaluatedCurvePts.push_back(bezier_point);
			}

		}
		else
			displayBezier(ptvEvaluatedCurvePts, px, py);
	}



	// show how many points are used for generating the curve

	printf("Points used:%d\n", ptvEvaluatedCurvePts.size());

	//pick up the rest point
	while (i < ptvCtrlPts.size())
	{
		ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i]);
		++i;
	}

	if (bWrap) 
	{
		if (Iswrapped == false)
		{
			const float percent = 1.0f - ptvCtrlPts.front().x / ((ptvCtrlPts.front().x + fAniLength) - ptvCtrlPts.back().x);
			const float y = ptvCtrlPts.back().y + percent*(ptvCtrlPts[0].y - ptvCtrlPts.back().y);
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, y));
			ptvEvaluatedCurvePts.push_back(Point(0, y));
		}
	}
	else 
	{
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts.back().y));
	}


}

