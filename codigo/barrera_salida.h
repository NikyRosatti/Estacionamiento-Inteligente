// CPP:us3/barrera_salida.cpp
#if !defined barrera_salida_h
#define barrera_salida_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <random>

class barrera_salida : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	double pat; // Patente actual procesada por el dev

	double tiempo_apertura;
	double tiempo_cierre;

	std::mt19937 engine_;						  // Variable motor para la distribución uniforme
	std::uniform_real_distribution<double> dist_; // Variable de la distribución uniforme

	// Puerto de entrada
	Port PERMITIR_SALIDA;
	// Puerto de salida
	Port VEHICULO_HA_SALIDO;

	static double infinito; // Representación para el infinito

public:
	barrera_salida(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};
#endif
