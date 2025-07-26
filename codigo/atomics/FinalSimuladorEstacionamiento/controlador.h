// CPP:us3/controlador.cpp
#if !defined controlador_h
#define controlador_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "fstream"

#include <limits>
#include <utility>
#include <vector>

class controlador : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	double pat;			// Patente corriente
	double pat_entrada; // Patente entrante
	double cant_autos;	// Cantidad de autos estacionados
	double capacidad_max_estacionamiento; //cantidad autos en el estacionamiento

	Port puerto;				  // Último puerto ejecutado del controlador
	bool pasar_a_barrera_entrada; // Flag de que un auto puede pasar a la barrera de entrada
	bool esperando_entrar;		  // Flag para saber si existe un auto esperando para entrar

	double tiempo_max_rta_controlador; // Tiempo máximo de respuesta del controlador para rechazar o admitir

	// Puertos de entrada
	Port VEHICULO_HA_INGRESADO;
	Port DETECTAR_VEHICULO;
	Port SOLICITAR_SALIDA;
	Port VEHICULO_HA_SALIDO;
	Port RECHAZADOS;
	// Puertos de salida
	Port PERMITIR_ENTRADA_BARRERA_DIRECTO;
	Port PERMITIR_ENTRADA_ESTACIONAMIENTO_Y_VOLVER_SENSAR_ENTRADA;
	Port DENEGAR_ENTRADA;
	Port VOLVER_A_SENSAR_SALIDA;
	Port PERMITIR_SALIDA;
	Port PERMITIR_ENTRADA_BARRERA_DEMORA_Y_VOLVER_SENSAR_SALIDA;
	Port VOLVER_A_SENSAR_ENTRADA;

	std::ofstream csv;		// Variable para manejar mejor los logs
	std::ofstream csv_exit; // Variable para manejar mejor los logs

	static double infinito; // Representación para el infinito

public:
	controlador(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};
#endif
