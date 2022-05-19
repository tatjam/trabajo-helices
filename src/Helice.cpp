#include "Helice.h"
#include "Util.h"
#include <iostream>


double interpolar(double a, double b, double t)
{
	return t * b + (1.0 - t) * a;
}

Helice::Helice(double r0, double r1, double t0, double t1, double rpm, double c, int n,
			   DatosPerfiles *perf)
{
	this->r0 = r0;
	this->r1 = r1;
	this->t0 = t0;
	this->t1 = t1;
	this->rpm = rpm;
	this->c = c;
	this->n = n;
	this->perf = perf;
}

double Helice::get_r(double x)
{
	return interpolar(r0, r1, x);
}

double Helice::get_t(double x)
{
	return interpolar(t0, t1, x);
}

Eigen::Vector2d Helice::get_F(double xp, double v0)
{
	Eigen::Vector2d out;

	// La velocidad del aire incidente
	Eigen::Vector2d vin;
	vin[0] = v0;
	// Velocidad incidente por la rotacion en m/s (Factor de conversion)
	vin[1] = -get_r(xp) * 2.0 * PI * (1.0 / 60.0);
	// Conociendo la velocidad incidente y la torsion podemos obtener el angulo de ataque
	double alpha_v = atan(vin[1] / vin[0]);
	double alpha = alpha_v + get_t(xp);

	// Obtenemos el dato de perfiles apropiado para alpha
	int alpha_inf = -1, alpha_sup = -1;
	for (int i = 0; i < perf->alphas.size() - 1; i++)
	{
		if (perf->alphas[i] < alpha && perf->alphas[i + 1] >= alpha)
		{
			alpha_inf = i;
			alpha_sup = i + 1;
			break;
		}
	}

	double alpha_prg = (perf->alphas[alpha_sup] - alpha) /
			(perf->alphas[alpha_sup] - perf->alphas[alpha_inf]);

	// Y el dato apropiado para Reynolds
	double Re = (RHO * vin.norm() * c) / MU;
	int re_inf = -1, re_sup = -1;
	for (int i = 0; i < perf->reynolds.size() - 1; i++)
	{
		if(perf->reynolds[i] < Re && perf->reynolds[i + 1] >= Re)
		{
			re_inf = i;
			re_sup = i + 1;
			break;
		}
	}

	double Re_prg = (perf->reynolds[re_sup] - Re) /
					   (perf->reynolds[re_sup] - perf->reynolds[re_inf]);

	if (alpha_inf == -1)
	{
		std::cerr << "No se pudo encontrar un perfil para alpha = " << alpha << std::endl;
	}

	if(re_inf == -1)
	{
		std::cerr << "No se pudo encontrar un perfil para Re = " << Re << std::endl;
	}

	// Interpolacion bilinear en las dimensiones Re-alpha
	// Es similar a una textura 2D, se toman 4 muestras:
	double lifts[4];
	lifts[0] = perf->datos_lift[re_inf][alpha_inf];
	lifts[1] = perf->datos_lift[re_inf][alpha_sup];
	lifts[2] = perf->datos_lift[re_sup][alpha_inf];
	lifts[3] = perf->datos_lift[re_sup][alpha_sup];
	double drags[4];
	drags[0] = perf->datos_drag[re_inf][alpha_inf];
	drags[1] = perf->datos_drag[re_inf][alpha_sup];
	drags[2] = perf->datos_drag[re_sup][alpha_inf];
	drags[3] = perf->datos_drag[re_sup][alpha_sup];

	// primera interpolacion
	double lift_re_inf = interpolar(lifts[0], lifts[1], alpha_prg);
	double lift_re_sup = interpolar(lifts[1], lifts[2], alpha_prg);
	double drag_re_inf = interpolar(drags[0], drags[1], alpha_prg);
	double drag_re_sup = interpolar(drags[1], drags[2], alpha_prg);

	// segunda interpolacion, obtenemos los coeficientes cl y cd
	double lift = interpolar(lift_re_inf, lift_re_sup, Re_prg);
	double drag = interpolar(drag_re_inf, drag_re_sup, Re_prg);

	return out;
}
