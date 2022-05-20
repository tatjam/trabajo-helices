#include "Helice.h"
#include "Util.h"
#include <iostream>
#include <sciplot/sciplot.hpp>


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

	// Conociendo la velocidad incidente y la torsion podemos obtener el angulo de ataque
	auto vin = get_vin(xp, v0);
	double alpha_v = get_alpha_v(xp, v0);
	double alpha_t = get_t(xp);
	double alpha = alpha_v + alpha_t;

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

	// Y el dato apropiado para Reynolds
	double Re = (M_RHO * vin.norm() * c) / M_MU;
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

	if (alpha_inf == -1)
	{
		std::cerr << "No se pudo encontrar un perfil para alpha = " << alpha << std::endl;
		return out;
	}

	if(re_inf == -1)
	{
		std::cerr << "No se pudo encontrar un perfil para Re = " << Re << std::endl;
		return out;
	}

	double alpha_prg = (perf->alphas[alpha_sup] - alpha) /
					   (perf->alphas[alpha_sup] - perf->alphas[alpha_inf]);


	double Re_prg = (perf->reynolds[re_sup] - Re) /
					(perf->reynolds[re_sup] - perf->reynolds[re_inf]);


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
	double cl = interpolar(lift_re_inf, lift_re_sup, Re_prg);
	double cd = interpolar(drag_re_inf, drag_re_sup, Re_prg);

	// Ahora simplemente podemos obtener las fuerzas
	// OJO: es una fuerza por unidad de longitud!
	double l = 0.5 * M_RHO * vin.norm() * vin.norm() * c * cl;
	double d = 0.5 * M_RHO * vin.norm() * vin.norm() * c * cd;

	if(isnan(l) || isnan(d))
	{
		std::cerr << "Uno de los valores es NaN" << std::endl;
		return out;
	}
	// Y obtener las resultantes en los ejes apropiados, teniendo en cuenta que se producen
	// respecto al eje del ala, es decir, respecto a la torsion

	// Eje x -> Dir de vuelo (thrust)
	out[0] = l * cos(alpha_t) - d * sin(alpha_t);
	// Eje y -> Dir de rotacion (axial)
	out[1] = -l * sin(alpha_t) - d * cos(alpha_t);

	return out;
}

double Helice::get_T(int intervalos, double v0)
{
	double Ttot = 0.0;
	double prev_r = get_r(0.0);
	for(int i = 0; i < intervalos; i++)
	{
		double x = (double)(i + 1) / (double)(intervalos);
		double dr = get_r(x) - prev_r;
		prev_r = get_r(x);
		auto F = get_F(x, v0);
		// Muy importante multiplicar por el diferencial de r
		Ttot += F[0] * dr;
	}
	return Ttot * n / 1000.0;
}

void Helice::grafico_v_T(double v0, double v1, const std::string& fichero, int puntos)
{
	sciplot::Plot plot;
	plot.palette(PALETA);

	std::vector<double> x;
	std::vector<double> y;

	std::array<double, 1> zero_x = {0.0};
	std::array<double, 1> zero_y = {0.0};

	for(int i = 0; i < puntos; i++)
	{
		double iprg = (double)i / (double)(puntos - 1);
		double v = interpolar(v0, v1, iprg);
		double T = get_T(500, v);

		if(T <= 0.01 && T > -0.01)
		{
			// NOTA: es tan solo aproximado, pero por el redondeo no importa
			zero_x[0] = v;
		}
		x.push_back(v);
		y.push_back(T);
	}

	plot.drawCurve(x, y).label("T");
	std::string titulo = "T=0 v_0=" + std::to_string((int)zero_x[0]) + "m/s";
	plot.drawPoints(zero_x, zero_y).label(titulo);
	plot.xlabel("v_0 (m/s)");
	plot.ylabel("T (kN)");
	plot.grid().show();

	plot.save(fichero);
}

void Helice::grafico_x_a(double v0, const std::string &fichero, int puntos)
{
	sciplot::Plot plot;
	plot.palette(PALETA);

	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> y2;

	for(int i = 0; i < puntos; i++)
	{
		double iprg = (double)i / (double)(puntos - 1);
		double a = get_alpha_v(iprg, v0);
		double a2 = a + get_t(iprg);
		double r = get_r(iprg);
		x.push_back(r);
		y.push_back(a);
		y2.push_back(a2);
	}

	plot.drawCurve(x, y).label("α_v");
	plot.drawCurve(x, y2).label("α");
	plot.xlabel("r");
	plot.ylabel("rad");
	plot.grid().show();
	plot.legend().atBottomRight();

	plot.save(fichero);
}

double Helice::get_alpha_v(double xp, double v0)
{
	auto vin = get_vin(xp, v0);
	return atan(vin[0] / vin[1]);
}

Eigen::Vector2d Helice::get_vin(double xp, double v0)
{
	// La velocidad del aire incidente
	Eigen::Vector2d vin;
	vin[0] = v0;
	// Velocidad incidente por la rotacion en m/s (Factor de conversion)
	vin[1] = -get_r(xp) * 2.0 * M_PI * (1.0 / 60.0) * rpm;
	return vin;
}

void Helice::grafico_x_ct(std::vector<double> v0, const std::string &fichero, int puntos)
{
	sciplot::Plot plot;
	plot.palette(PALETA);

	std::vector<double> x;
	std::vector<std::vector<double>> y;
	y.resize(v0.size());

	for(int i = 0; i < puntos; i++)
	{
		double iprg = (double)i / (double)(puntos - 1);
		double r = get_r(iprg);
		x.push_back(r);
		for(int j = 0; j < v0.size(); j++)
		{
			auto F = get_F(iprg, v0[j]);
			double vin = get_vin(iprg, v0[j]).norm();
			double cT = F[0] / (0.5 * M_RHO * vin * vin * c);
			y[j].push_back(cT);
		}
	}

	for(int j = 0; j < v0.size(); j++)
	{
		if(v0[j] == 30.0)
		{
			plot.drawCurve(x, y[j]).label("c_T(" + std::to_string((int) v0[j]) + " m/s)").lineWidth(3);
		}
		else
		{
			plot.drawCurve(x, y[j]).label("c_T(" + std::to_string((int) v0[j]) + " m/s)").lineWidth(1);
		}
	}

	plot.xlabel("r");
	plot.ylabel("c_T");
	plot.grid().show();
	plot.legend().atBottomRight().fontSize(8);

	plot.save(fichero);
}
