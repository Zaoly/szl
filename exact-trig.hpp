#pragma once

#include <cmath>

namespace zaoly
{
#ifndef _ZAOLY_PI_
#define _ZAOLY_PI_
	const double PI = 3.141592653589793;
#endif

	double exact_sin(double x_divided_by_pi)
	{
		double new_x_divided_by_pi = 0.0, temp_result = 0.0;
		bool minus = false, another = false;
		new_x_divided_by_pi = x_divided_by_pi - floor(x_divided_by_pi / 2.0) * 2.0; // sin(x) = sin(x - 2 * PI * k), where k is any integer, now 0 <= x < 2 * PI
		if (new_x_divided_by_pi >= 1.0)
		{
			minus = true;
			new_x_divided_by_pi -= 1.0; // sin(x) = -sin(x - PI), now 0 <= x < PI
		}
		if (new_x_divided_by_pi > 0.5)
			new_x_divided_by_pi = 1.0 - new_x_divided_by_pi; // sin(x) = sin(PI - x), now 0 <= x <= PI / 2
		if (new_x_divided_by_pi > 0.25)
		{
			another = true;
			new_x_divided_by_pi = 0.5 - new_x_divided_by_pi; // sin(x) = cos(PI / 2 - x), now 0 <= x <= PI / 4
		}
		if (another) // sin turned into cos
			temp_result = cos(new_x_divided_by_pi * PI);
		else
			temp_result = sin(new_x_divided_by_pi * PI);
		if (minus && temp_result != 0.0)
			temp_result = -temp_result;
		return temp_result;
	}

	double exact_cos(double x_divided_by_pi)
	{
		double new_x_divided_by_pi = 0.0, temp_result = 0.0;
		bool minus = false, another = false;
		new_x_divided_by_pi = x_divided_by_pi - floor(x_divided_by_pi / 2.0) * 2.0; // cos(x) = cos(x - 2 * PI * k), where k is any integer, now 0 <= x < 2 * PI
		if (new_x_divided_by_pi >= 1.0)
			new_x_divided_by_pi = 2.0 - new_x_divided_by_pi; // cos(x) = cos(2 * PI - x), now 0 <= x < PI
		if (new_x_divided_by_pi > 0.5)
		{
			minus = true;
			new_x_divided_by_pi = 1.0 - new_x_divided_by_pi; // cos(x) = -cos(PI - x), now 0 <= x <= PI / 2
		}
		if (new_x_divided_by_pi > 0.25)
		{
			another = true;
			new_x_divided_by_pi = 0.5 - new_x_divided_by_pi; // cos(x) = sin(PI / 2 - x), now 0 <= x <= PI / 4
		}
		if (another) // cos turned into sin
			temp_result = sin(new_x_divided_by_pi * PI);
		else
			temp_result = cos(new_x_divided_by_pi * PI);
		if (minus && temp_result != 0.0)
			temp_result = -temp_result;
		return temp_result;
	}

	double exact_tan(double x_divided_by_pi)
	{
		double new_x_divided_by_pi = 0.0, temp_result = 0.0;
		bool minus = false, reciprocal = false;
		new_x_divided_by_pi = x_divided_by_pi - floor(x_divided_by_pi); // tan(x) = tan(x - PI * k), where k is any integer, now 0 <= x < PI
		if (new_x_divided_by_pi > 0.5)
		{
			minus = true;
			new_x_divided_by_pi = 1.0 - new_x_divided_by_pi; // tan(x) = -tan(PI - x), now 0 <= x <= PI / 2
		}
		if (new_x_divided_by_pi > 0.25)
		{
			reciprocal = true;
			new_x_divided_by_pi = 0.5 - new_x_divided_by_pi; // tan(x) = 1 / tan(PI / 2 - x), now 0 <= x <= PI / 4
		}
		temp_result = tan(new_x_divided_by_pi * PI);
		if (minus && temp_result != 0.0)
			temp_result = -temp_result;
		if (reciprocal)
			temp_result = 1.0 / temp_result;
		return temp_result;
	}

	double exact_cot(double x_divided_by_pi)
	{
		double temp_result = exact_tan(x_divided_by_pi + 0.5); // cot(x) = tan(x + PI / 2)
		if (temp_result == 0.0)
			return 0.0;
		else
			return -temp_result;
	}

	double exact_sec(double x_divided_by_pi)
	{
		return 1.0 / exact_cos(x_divided_by_pi); // sec(x) = 1 / cos(x)
	}

	double exact_csc(double x_divided_by_pi)
	{
		return 1.0 / exact_sin(x_divided_by_pi); // csc(x) = 1 / sin(x)
	}

	double sin_deg(double x_deg)
	{
		double new_x_deg = 0.0, temp_result = 0.0;
		bool minus = false, another = false;
		new_x_deg = x_deg - floor(x_deg / 360.0) * 360.0; // sin(x) = sin(x - 360 deg * k), where k is any integer, now 0 <= x < 360 deg
		if (new_x_deg >= 180.0)
		{
			minus = true;
			new_x_deg -= 180.0; // sin(x) = -sin(x - 180 deg), now 0 <= x < 180 deg
		}
		if (new_x_deg > 90.0)
			new_x_deg = 180.0 - new_x_deg; // sin(x) = sin(180 deg - x), now 0 <= x <= 90 deg
		if (new_x_deg > 45.0)
		{
			another = true;
			new_x_deg = 90.0 - new_x_deg; // sin(x) = cos(90 deg - x), now 0 <= x <= 45 deg
		}
		if (another)
			temp_result = cos(new_x_deg / 180.0 * PI); // sin turned into cos
		else
			if (new_x_deg == 30.0)
				temp_result = 0.5; // cos(30 deg) = 0.5
			else
				temp_result = sin(new_x_deg / 180.0 * PI);
		if (minus && temp_result != 0.0)
			temp_result = -temp_result;
		return temp_result;
	}

	double cos_deg(double x_deg)
	{
		double new_x_deg = 0.0, temp_result = 0.0;
		bool minus = false, another = false;
		new_x_deg = x_deg - floor(x_deg / 360.0) * 360.0; // cos(x) = cos(x - 360 deg * k), where k is any integer, now 0 <= x < 360 deg
		if (new_x_deg >= 180.0)
			new_x_deg = 360.0 - new_x_deg; // cos(x) = cos(180 deg - x), now 0 <= x < 180 deg
		if (new_x_deg > 90.0)
		{
			minus = true;
			new_x_deg = 180.0 - new_x_deg; // cos(x) = -cos(180 deg - x), now 0 <= x <= 90 deg
		}
		if (new_x_deg > 45.0)
		{
			another = true;
			new_x_deg = 90.0 - new_x_deg; // cos(x) = sin(90 deg - x), now 0 <= x <= 45 deg
		}
		if (another) // cos turned into sin
			if (new_x_deg == 30)
				temp_result = 0.5; // sin(30 deg) = 0.5
			else
				temp_result = sin(new_x_deg / 180.0 * PI);
		else
			temp_result = cos(new_x_deg / 180.0 * PI);
		if (minus && temp_result != 0.0)
			temp_result = -temp_result;
		return temp_result;
	}

	double tan_deg(double x_deg)
	{
		double new_x_deg = 0.0, temp_result = 0.0;
		bool minus = false, reciprocal = false;
		new_x_deg = x_deg - floor(x_deg / 180.0) * 180.0; // tan(x) = tan(x - 180 deg * k), where k is any integer, now 0 <= x < 180 deg
		if (new_x_deg > 90.0)
		{
			minus = true;
			new_x_deg = 180.0 - new_x_deg; // tan(x) = -tan(180 deg - x), now 0 <= x <= 90 deg
		}
		if (new_x_deg > 45.0)
		{
			reciprocal = true;
			new_x_deg = 90.0 - new_x_deg; // tan(x) = 1 / tan(90 deg - x), now 0 <= x <= 45 deg
		}
		temp_result = tan(new_x_deg / 180.0 * PI);
		if (minus && temp_result != 0.0)
			temp_result = -temp_result;
		if (reciprocal)
			temp_result = 1.0 / temp_result;
		return temp_result;
	}

	double cot_deg(double x_deg)
	{
		double temp_result = tan_deg(x_deg + 90.0); // cot(x) = tan(x + 90 deg)
		if (temp_result == 0.0)
			return 0.0;
		else
			return -temp_result;
	}

	double sec_deg(double x_deg)
	{
		return 1.0 / cos_deg(x_deg); // sec(x) = 1 / cos(x)
	}

	double csc_deg(double x_deg)
	{
		return 1.0 / sin_deg(x_deg); // csc(x) = 1 / sin(x)
	}
}
