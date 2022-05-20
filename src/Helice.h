#pragma once
#include <utility>
#include "Eigen/Dense"
#include "DatosPerfiles.h"

class Helice
{
private:

	double r0, r1, t0, t1, rpm, c;
	int n;
	DatosPerfiles* perf;

public:

	double get_r(double x);
	double get_t(double x);

	// Obtiene la fuerza puntual de un segmento,
	// dada la velocidad de vuelo. Utilizamos los ejes
	// definidos en el escrito, x -> dir vuelo, y -> dir de rotacion
	// xp se refiere al progreso en el ala, no al radio
	// Seria una fuerza por unidad de longitud
	Eigen::Vector2d get_F(double xp, double v0);
	// Obtiene el angulo de ataque debido a la velocidad
	// radial y de vuelo en un perfil de torsion 0
	double get_alpha_v(double xp, double v0);
	Eigen::Vector2d get_vin(double xp, double v0);

	// Obtiene el empuje neto total a una velocidad v0 usando intervalos
	double get_T(int intervalos, double v0);

	void grafico_v_T(double v0, double v1, const std::string& fichero, int puntos = 100);
	void grafico_x_a(double v0, const std::string& fichero, int puntos = 100);
	void grafico_x_ct(std::vector<double> v0, const std::string& fichero, int puntos = 100);

	// r0 -> Radio encastre
	// r1 -> Radio punta
	// t0 -> Torsion encastre
	// t1 -> Torsion punta
	// rpm -> Velocidad de giro en revoluciones por minuto
	// c -> Cuerda
	// n -> Numero de alabes (multiplicador)
	// Leyes lineales para radio y torsion
	// Unidades del S.I excepto rpm, angulos en radianes!
	Helice(double r0, double r1, double t0, double t1, double rpm, double c, int n,
		DatosPerfiles* perf);

};


