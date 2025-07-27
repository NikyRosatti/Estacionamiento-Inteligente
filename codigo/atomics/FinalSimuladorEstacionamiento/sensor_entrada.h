// FLAGS:-std=c++0x
// CPP:us3/sensor_entrada.cpp
#if !defined sensor_entrada_h
#define sensor_entrada_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <queue>
#include <limits>
#include "fstream"

class sensor_entrada : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	double pat; // Patente corriente

	double latencia; // delay de lectura del sensor.

	bool bloqueado;		 // Flag para saber si el sensor está bloqueado
	bool procesando;	 // Flag para saber si el sensor está procesando un auto
	bool fin_simulacion; // Flag para saber si termino la simulacion

	double TIEMPO_MAX_ENTRADA_VEHICULOS;	// Constante que indica cuánto tiempo tienen los autos permitido para entrar

	std::queue<double> cola; // Cola de autos con disciplina FIFO

	std::ofstream csv;		// Variable para manejar mejor los logs
	std::ofstream csv_exit; // Variable para manejar mejor los logs

	// Puertos de entrada
	Port PATENTE_VEHICULO;
	Port VOLVER_A_SENSAR;
	// Puerto de salida
	Port DETECTAR_VEHICULO;

	static double infinito; // Representación para el infinito

public:
	sensor_entrada(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};
#endif
