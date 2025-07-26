// CPP:us3/guardar_en_disco.cpp
#if !defined guardar_en_disco_h
#define guardar_en_disco_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "string.h"

class guardar_en_disco : public Simulator
{
	double Sigma;
	long int FOutput;

public:
	guardar_en_disco(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};
#endif