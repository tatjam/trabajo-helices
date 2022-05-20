#include "DatosPerfiles.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sciplot/sciplot.hpp>
#include "Util.h"

DatosPerfiles::DatosPerfiles(const std::string &path_drag, const std::string &path_lift)
{
	for (int i = 0; i <= 100; i++)
	{
		alphas[i] = M_DEG_TO_RAD * (-25.0 + 0.5 * i);
	}

	std::fstream drag;
	drag.open(path_drag, std::ios::in);
	if (!drag.is_open())
	{
		std::cerr << "No se pudo abrir el fichero de drag. Comprobar directorio de trabajo" << std::endl;
		return;
	}
	carga_datos(datos_drag, drag);
	drag.close();

	std::fstream lift;
	lift.open(path_lift, std::ios::in);
	if (!lift.is_open())
	{
		std::cerr << "No se pudo abrir el fichero de lift. Comprobar directorio de trabajo" << std::endl;
		return;
	}
	carga_datos(datos_lift, lift);
	lift.close();

	std::cout << "Leidos los datos con exito" << std::endl;
}

void DatosPerfiles::carga_datos(DatosPerfiles::array_datos& objetivo, std::fstream& f)
{
	std::string tp;

	int reynolds = 0;
	int alpha = 0;
	while(getline(f, tp))
	{
		std::stringstream ss(tp);
		// Separamos por los tabuladores
		std::string valor_str;
		while(getline(ss, valor_str, '\t'))
		{
			// Remplazamos la coma por punto
			for(size_t i = 0; i < valor_str.size(); i++)
			{
				if(valor_str[i] == ',')
					valor_str[i] = '.';
			}
			double valor = strtod(valor_str.c_str(), nullptr);
			objetivo[reynolds][alpha] = valor;
			reynolds++;
		}
		reynolds = 0;
		alpha++;
	}

}

void DatosPerfiles::graficar(const std::string &path_salida_lift, const std::string &path_salida_drag)
{
	sciplot::Plot lift, drag;
	lift.palette(PALETA);
	drag.palette(PALETA);

	lift.fontSize(6);
	drag.fontSize(6);

	lift.xlabel("α");
	drag.xlabel("α");

	lift.ylabel("c_l");
	drag.ylabel("c_d");

	lift.xrange("*", "*");
	drag.xrange("*", "*");

	lift.yrange("*", "*");
	drag.yrange("*", "*");

	lift.legend().atTopLeft();
	drag.legend().atTop();
	lift.grid().show();
	drag.grid().show();



	for(size_t i = 0; i < reynolds.size(); i++)
	{
		int valor_re = reynolds[i];
		std::string titulo_lift = "c_l (" + std::to_string(valor_re) + ")";
		std::string titulo_drag = "c_d (" + std::to_string(valor_re) + ")";
		lift.drawBrokenCurve(alphas, datos_lift[i])
			.label(titulo_lift);
		drag.drawBrokenCurve(alphas, datos_drag[i])
				.label(titulo_drag);
	}


	lift.xtics().rotateBy(-90.0);
	lift.xtics().fontSize(4);
	drag.xtics().rotateBy(-90.0);
	drag.xtics().fontSize(4);
	lift.save(path_salida_lift);
	drag.save(path_salida_drag);
}

