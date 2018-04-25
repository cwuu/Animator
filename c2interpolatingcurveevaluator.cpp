#include "c2interpolatingcurve.h"
#include "mat.h"
#include "vec.h"
#include "modelerapp.h"

#define SAMPLE 150


void C2InterpolatingEvaluator::evaluateCurve(const std::vector<Point>& controlPoints,
	std::vector<Point>& evaluatedPoints,
	const float& animationLength,
	const bool& bWrap,
	const bool& bAdaptive) const
{
	evaluatedPoints.clear();

	vector<Point> curve;
	curve.insert(curve.end(), controlPoints.begin(), controlPoints.end());
	curve.push_back(Point(curve[0].x + animationLength, curve[0].y));


	//think of each cubic segment as a hermite curve, for which we get to set the position and deriavative of the endpoints
	//solve the derivative and find D
	//input D to get the bezier control points and draw the final spline

	vector<float> D(curve.size(), 0.0);

	int N = controlPoints.size();
	if (bWrap == false)
		N = N - 1;


	vector<float> coef(curve.size(), 0.0);
	vector<float> delta(curve.size(), 0.0);

	//CONSTRUCT COEFFICIENT
	coef[0] = 0.5f;

	for (int i = 1; i < N; ++i)
		coef[i] = 1.0f / (4.0f - coef[i - 1]);

	coef[N] = 1.0 / (2.0f - coef[N - 1 - 1]);

	//CONSTRUCT PI-PI-1
	delta[0] = 1.5 * (curve[1].y - curve[0].y);
	for (int i = 1; i < N; i++)
		delta[i] = coef[i] * (3 * (curve[i + 1].y - curve[i - 1].y) - delta[i - 1]);

	delta[N] = coef[N] * (3 * (curve[N].y - curve[N - 1].y) - delta[N]);

	//CONSTRUXT D0-DM
	D[N] = delta[N];
	for (int i = N - 1; i >= 0; i--)
		D[i] = delta[i] - coef[i] * D[i + 1];


	for (int i = 0; i < N; i++)
	{
		Mat4d M
		(2.0, -2.0, 1.0, 1.0,
			-3.0, 3.0, -2.0, -1.0,
			0.0, 0.0, 1.0, 0.0,
			1.0, 0.0, 0.0, 0.0);

		Vec4d vec = M * Vec4d(
			curve[i].y,
			curve[i+1].y,
			D[i],
			D[i+1]
		);

		for (int j = 0; j < SAMPLE; ++j)
		{
			double interval = j / (double)SAMPLE;
			Vec4d T(interval*interval*interval, interval*interval, interval, 1);
			float length_x = curve[i+1].x - curve[i].x;
			if (length_x < 0)
				length_x = length_x + animationLength;
			float py = T*vec;
			float px = curve[i].x + interval * length_x;
			px = fmod(px, animationLength);
			evaluatedPoints.push_back(Point(px, py));
		}
	}
	

	if (bWrap == false)
	{
		evaluatedPoints.push_back(Point(0, controlPoints[0].y));
		evaluatedPoints.push_back(Point(animationLength, evaluatedPoints.back().y));
	}
}



