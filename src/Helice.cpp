#include "Helice.h"

double interpolar(double a, double b, double t)
{
	return t * b + (1.0 - t) * a;
}

Helice::Helice(double r0, double r1, double t0, double t1, double rpm, double c, int n)
{
	this->r0 = r0; this->r1 = r1; this->t0 = t0; this->t1 = t1; this->rpm = rpm; this->c = c; this->n = n;

}

double Helice::get_r(double x)
{
	return interpolar(r0, r1, x);
}

double Helice::get_t(double x)
{
	return interpolar(t0, t1, x);
}
