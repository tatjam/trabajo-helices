#include <sciplot/sciplot.hpp>
#include "DatosPerfiles.h"
#include "Helice.h"
#include "Util.h"

using namespace sciplot;


int main(int argc, char** argv)
{
	DatosPerfiles datos("./res/drag_naca2411.txt", "./res/lift_naca2411.txt");
	datos.graficar("resultados/lift.pdf", "resultados/drag.pdf");

	Helice helice(0.2, 1.2, 20 * M_DEG_TO_RAD, 5 * M_DEG_TO_RAD, 2500, 0.07, 8, &datos);
	helice.grafico_x_a(5.0, "resultados/a5.pdf");
	helice.grafico_x_a(10.0, "resultados/a10.pdf");
	helice.grafico_x_a(30.0, "resultados/a30.pdf");
	helice.grafico_x_a(50.0, "resultados/a50.pdf");
	// Se ajustaron hasta que no se producian errores NaN
	helice.grafico_v_T(5.0, 49.5, "resultados/vT.pdf", 1000);
	helice.grafico_x_ct({5.0, 10.0, 20.0, 30.0, 40.0, 50.0}, "resultados/ct.pdf");

}