// FLAGS:-std=c++0x
// CPP:us3/estacionamiento.cpp
#if !defined estacionamiento_h
#define estacionamiento_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "fstream"

#include <queue>
#include <functional> // para std::greater
#include <vector>	  // para std::vector
#include <utility>	  // para std::pair
#include <limits>
#include <random>

class estacionamiento : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	// cola de prioridad de menor a mayor con clave la segunda componente
	struct CompareSecondMin
	{
		bool operator()(const std::pair<double, double> &a, const std::pair<double, double> &b) const
		{
			return a.second > b.second;
		}
	};
	std::priority_queue<
		std::pair<double, double>,
		std::vector<std::pair<double, double>>,
		CompareSecondMin>
		cola_prioridad;

	std::mt19937 engine_;						  // Variable motor para la distribución uniforme
	std::uniform_real_distribution<double> dist_; // Variable de la distribución uniforme

	std::ofstream csv; // Variable para manejar mejor los logs
	std::ofstream csv_exit; // Variable para manejar mejor los logs

	// Puerto de entrada
	Port PERMITIR_ENTRADA;
	// Puerto de salida
	Port VEHICULO_QUIERE_SALIR;

	static double infinito; // Representación para el infinito

public:
	estacionamiento(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
	void actualizarTiempos(double t);
};

#endif
