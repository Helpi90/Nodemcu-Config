#include "osapi.h"
#include "user_config.h"
#include "user_interface.h"
#include "cJSON_math.h"

double ICACHE_FLASH_ATTR std_floor(double x)
{
	return (double)(x < 0.f ? (((int)x) - 1) : ((int)x));
}

/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1985-2004 AT&T Corp.                *
*        and it may only be used by you under license from         *
*                       AT&T Corp. ("AT&T")                        *
*         A copy of the Source Code Agreement is available         *
*                at the AT&T Internet web site URL                 *
*                                                                  *
*       http://www.research.att.com/sw/license/ast-open.html       *
*                                                                  *
*    If you have copied or used this software without agreeing     *
*        to the terms of the license you are infringing on         *
*           the license and copyright and are violating            *
*               AT&T's intellectual property rights.               *
*                                                                  *
*            Information and Software Systems Research             *
*                        AT&T Labs Research                        *
*                         Florham Park NJ                          *
*                                                                  *
*               Glenn Fowler <gsf@research.att.com>                *
*                David Korn <dgk@research.att.com>                 *
*                 Phong Vo <kpv@research.att.com>                  *
*                                                                  *
*******************************************************************/

/*
 * std_frexp/std_ldexp implementation
 */

static double pow2tab[] ICACHE_RODATA_ATTR = {
	(double)1,
	(double)2,
	(double)4,
	(double)8,
	(double)16,
	(double)32,
	(double)64,
	(double)128,
	(double)256,
	(double)512,
	(double)1024,
	(double)2048,
	(double)4096,
	(double)8192,
	(double)16384,
	(double)32768,
	(double)65536,
	(double)131072,
	(double)262144,
	(double)524288,
	(double)1048576,
	(double)2097152,
	(double)4194304,
	(double)8388608,
	(double)16777216,
	(double)33554432,
	(double)67108864,
	(double)134217728,
	(double)268435456,
	(double)536870912,
	(double)1073741824,
};

#define DBL_MAX_EXP     (sizeof(pow2tab)/sizeof(double) - 1)

#define std_pow2(i)     pow2tab[i]

extern double ICACHE_FLASH_ATTR std_frexp(double f, int *p)
{
	int k;
	int x;
	double g;

	/*
	 * normalize
	 */

	x = k = DBL_MAX_EXP / 2;
	if (f < 1) {
		g = 1.0L / f;
		for (;;) {
			k = (k + 1) / 2;
			if (g < std_pow2(x))
				x -= k;
			else if (k == 1 && g < std_pow2(x + 1))
				break;
			else
				x += k;
		}
		if (g == std_pow2(x))
			x--;
		x = -x;
	} else if (f > 1) {
		for (;;) {
			k = (k + 1) / 2;
			if (f > std_pow2(x))
				x += k;
			else if (k == 1 && f > std_pow2(x - 1))
				break;
			else
				x -= k;
		}
		if (f == std_pow2(x))
			x++;
	} else
		x = 1;
	*p = x;

	/*
	 * shift
	 */

	x = -x;
	if (x < 0)
		f /= std_pow2(-x);
	else if (x < DBL_MAX_EXP)
		f *= std_pow2(x);
	else
		f = (f * std_pow2(DBL_MAX_EXP - 1)) * std_pow2(x -
							       (DBL_MAX_EXP -
								1));
	return f;
}

extern double ICACHE_FLASH_ATTR std_ldexp(double f, int x)
{
	if (x < 0)
		f /= std_pow2(-x);
	else if (x < DBL_MAX_EXP)
		f *= std_pow2(x);
	else
		f = (f * std_pow2(DBL_MAX_EXP - 1)) * std_pow2(x -
							       (DBL_MAX_EXP -
								1));
	return f;
}

#define MAXEXP  2031		/* (MAX_EXP * 16) - 1           */
#define MINEXP  -2047		/* (MIN_EXP * 16) - 1           */
#define HUGE    MAXFLOAT

double a1[] ICACHE_RODATA_ATTR = {
	1.0,
	0.95760328069857365,
	0.91700404320467123,
	0.87812608018664974,
	0.84089641525371454,
	0.80524516597462716,
	0.77110541270397041,
	0.73841307296974966,
	0.70710678118654752,
	0.67712777346844637,
	0.64841977732550483,
	0.62092890603674203,
	0.59460355750136054,
	0.56939431737834583,
	0.54525386633262883,
	0.52213689121370692,
	0.50000000000000000
};

double a2[] ICACHE_RODATA_ATTR = {
	0.24114209503420288E-17,
	0.92291566937243079E-18,
	-0.15241915231122319E-17,
	-0.35421849765286817E-17,
	-0.31286215245415074E-17,
	-0.44654376565694490E-17,
	0.29306999570789681E-17,
	0.11260851040933474E-17
};

double p1 ICACHE_RODATA_ATTR = 0.833333333333332114e-1;
double p2 ICACHE_RODATA_ATTR = 0.125000000005037992e-1;
double p3 ICACHE_RODATA_ATTR = 0.223214212859242590e-2;
double p4 ICACHE_RODATA_ATTR = 0.434457756721631196e-3;
double q1 ICACHE_RODATA_ATTR = 0.693147180559945296e0;
double q2 ICACHE_RODATA_ATTR = 0.240226506959095371e0;
double q3 ICACHE_RODATA_ATTR = 0.555041086640855953e-1;
double q4 ICACHE_RODATA_ATTR = 0.961812905951724170e-2;
double q5 ICACHE_RODATA_ATTR = 0.133335413135857847e-2;
double q6 ICACHE_RODATA_ATTR = 0.154002904409897646e-3;
double q7 ICACHE_RODATA_ATTR = 0.149288526805956082e-4;
double k ICACHE_RODATA_ATTR = 0.442695040888963407;

double ICACHE_FLASH_ATTR std_pow(double x, double y)
{
	double std_frexp(), g, std_ldexp(), r, u1, u2, v, w, w1, w2, y1, y2, z;
	int iw1, m, p;

	if (y == 0.0)
		return (1.0);
	if (x <= 0.0) {
		if (x == 0.0) {
			if (y > 0.0)
				return (x);
			//cmemsg(FP_POWO, &y);
			//return(HUGE);
		} else {
			//cmemsg(FP_POWN, &x);
			x = -x;
		}
	}
	g = std_frexp(x, &m);
	p = 0;
	if (g <= a1[8])
		p = 8;
	if (g <= a1[p + 4])
		p += 4;
	if (g <= a1[p + 2])
		p += 2;
	p++;
	z = ((g - a1[p]) - a2[p / 2]) / (g + a1[p]);
	z += z;
	v = z * z;
	r = (((p4 * v + p3) * v + p2) * v + p1) * v * z;
	r += k * r;
	u2 = (r + z * k) + z;
	u1 = 0.0625 * (double)(16 * m - p);
	y1 = 0.0625 * (double)((int)(16.0 * y));
	y2 = y - y1;
	w = u2 * y + u1 * y2;
	w1 = 0.0625 * (double)((int)(16.0 * w));
	w2 = w - w1;
	w = w1 + u1 * y1;
	w1 = 0.0625 * (double)((int)(16.0 * w));
	w2 += (w - w1);
	w = 0.0625 * (double)((int)(16.0 * w2));
	iw1 = 16.0 * (w1 + w);
	w2 -= w;
	while (w2 > 0.0) {
		iw1++;
		w2 -= 0.0625;
	}
	if (iw1 > MAXEXP) {
		//cmemsg(FP_POWO, &y);
		return (HUGE);
	}
	if (iw1 < MINEXP) {
		//cmemsg(FP_POWU, &y);
		return (0.0);
	}
	m = iw1 / 16;
	if (iw1 >= 0)
		m++;
	p = 16 * m - iw1;
	z = ((((((q7 * w2 + q6) * w2 + q5) * w2 + q4) * w2 + q3) * w2 +
	      q2) * w2 + q1) * w2;
	z = a1[p] + a1[p] * z;
	return (std_ldexp(z, m));
}
