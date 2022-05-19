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
	Eigen::Vector2d get_F(double xp, double v0);

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


