// CPP:us3/barrera_entrada.cpp
#if !defined barrera_entrada_h
#define barrera_entrada_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <random>

class barrera_entrada : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	double pat;		// Patente actual procesada por el dev
	bool rechazado; // Flag de auto rechazado

	double tiempo_apertura;	// Tiempo que tarda en abrirse la barrera
	double tiempo_cierre;	// Tiempo que tarda en cerrarse la barrera
	double tiempo_retiro;	// Tiempo que tarda en retirarse un vehículo rechazado de la barrera

	std::mt19937 engine_;						  // Variable motor para la distribución uniforme
	std::uniform_real_distribution<double> dist_; // Variable de la distribución uniforme

	// Puertos de entrada
	Port PERMITIR_ENTRADA;
	Port DENEGAR_ENTRADA;
	// Puertos de salida
	Port VEHICULO_HA_INGRESADO;
	Port VEHICULO_RECHAZADO_SE_HA_RETIRADO;

	static double infinito; // Representación para el infinito

public:
	barrera_entrada(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};
#endif
