#include <sciplot/sciplot.hpp>
#include "DatosPerfiles.h"
#include "Helice.h"
#include "Util.h"

using namespace sciplot;


int main(int argc, char** argv)
{
	DatosPerfiles datos("./res/drag_naca2411.txt", "./res/lift_naca2411.txt");
	datos.graficar("resultados/lift.pdf", "resultados/drag.pdf");

	Helice helice(0.2, 1.2, 20 * DEG_TO_RAD, 5 * DEG_TO_RAD, 2500, 0.07, 8, &datos);
	helice.get_F(0.5, 100.0);
}