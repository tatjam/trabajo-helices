#pragma once
#include <optional>
#include <string>
#include <array>

class DatosPerfiles
{
private:

	// array_datos[reynolds][alpha]
	using array_datos = std::array<std::array<double, 101>, 8>;
	void carga_datos(array_datos& objetivo, std::fstream& f);

public:

	// 8 numeros de reynolds * 101 angulos = 808 valores
	// Los valores NaN no estan presentes
	array_datos datos_lift;
	array_datos datos_drag;

	std::array<double, 8> reynolds = {40000, 80000, 160000, 360000, 700000, 1000000, 2000000, 5000000};
	// Alphas en radianes
	std::array<double, 101> alphas;

	// Carga los datos y genera alpha
	DatosPerfiles(const std::string& path_drag, const std::string& path_lift);
	void graficar(const std::string& path_salida_lift, const std::string& path_salida_drag);

};

