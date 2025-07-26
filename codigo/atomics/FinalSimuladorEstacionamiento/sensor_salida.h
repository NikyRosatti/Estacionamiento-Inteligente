// FLAGS:-std=c++0x
// CPP:us3/sensor_salida.cpp
#if !defined sensor_salida_h
#define sensor_salida_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <queue>
#include <limits>
#include <utility>
#include "fstream"

class sensor_salida : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	double pat; // Patente corriente

	double latencia; // delay de lectura del sensor.

	bool bloqueado;	 // Flag para saber si el sensor está bloqueado
	bool procesando; // Flag para saber si el sensor está procesando un auto

	std::queue<double> cola; // Cola de autos con disciplina FIFO

	std::ofstream csv; // Variable para manejar mejor los logs

	// Puertos de entrada
	Port VEHICULO_QUIERE_SALIR;
	Port VOLVER_A_SENSAR;
	// Puerto de salida
	Port SOLICITAR_SALIDA;

	static double infinito; // Representación para el infinito

public:
	sensor_salida(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};
#endif
