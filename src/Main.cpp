#include <glm/glm.hpp>
#include <sciplot/sciplot.hpp>
#include "DatosPerfiles.h"
using namespace sciplot;

int main(int argc, char** argv)
{
	DatosPerfiles datos("./res/drag_naca2411.txt", "./res/lift_naca2411.txt");
	datos.graficar("resultados/lift.pdf", "resultados/drag.pdf");
}