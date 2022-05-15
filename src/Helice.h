#pragma once

class Helice
{
private:

	double r0, r1, t0, t1, rpm, c;
	int n;

public:


	// r0 -> Radio encastre
	// r1 -> Radio punta
	// t0 -> Torsion encastre
	// t1 -> Torsion punta
	// rpm -> Velocidad de giro en revoluciones por minuto
	// c -> Cuerda
	// n -> Numero de alabes (multiplicador)
	// Leyes lineales para radio y torsion
	// Unidades del S.I excepto rpm
	Helice(double r0, double r1, double t0, double t1, double rpm, double c, int n);

};


