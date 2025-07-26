//FLAGS:-std=c++0x
// CPP:us3/gen_aleatorio.cpp
#if !defined gen_aleatorio_h
#define gen_aleatorio_h

#include "simulator.h"
#include "event.h"

#include "random"
#include "cstdarg"

class gen_aleatorio : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta
	
	double pat;	// Patente corriente generada

	// Motor y distribución STL
	std::mt19937 engine;
	std::exponential_distribution<double> exp_dist;

	// Puerto de salida
	Port PATENTE_VEHICULO;

public:
	gen_aleatorio(const char *n) : Simulator(n) {};
	void init(double t, ...);
	double ta(double t);
	void dint(double t);
	void dext(Event x, double t);
	Event lambda(double);
	void exit();
};
#endif
