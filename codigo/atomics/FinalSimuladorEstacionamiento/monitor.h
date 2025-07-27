// FLAGS:-std=c++0x
// CPP:us3/monitor.cpp
#if !defined monitor_h
#define monitor_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "fstream"

#include "vector"
#include "functional"
#include "limits"

class monitor : public Simulator
{
	double sigma; // Variable del tiempo para la definición del ta

	std::vector<double> tiempos_entrada; // Lista de patentes con su tiempo de entrada al estacionamiento
	std::vector<double> tiempos_salida;	 // Lista de patentes con su tiempo de salida del estacionamiento

	std::vector<double> autos_entrantes; // Lista de patentes que entran
	std::vector<double> autos_salientes; // Lista de patentes que salen

	std::vector<std::pair<double, bool>> lista_par_patentes_estado_sistema; // Lista de pares: patente estado(admitida, rechazada) que pasaron por el sistema

	double K_ESTACIONAMENTO; // constante con la cantidad maxima de autos

	double total_intentos;	// cantidad de autos totales admitidos + rechazados que quisieron estacionar
	double rechazados;		// cantidad de autos rechazados
	double total_admitidos; // cantidad de autos admitidos
	double total_salidos;	// cantidad de autos que salieron del estacionamiento

	double tiempo_ultimo_evento; // tiempo del ultimo evento (entrante)
	double ocupacion_actual;	 // ocupacion actual del estacionamiento

	double suma_tiempos_permanencia; // constructor del estadistico de promedio_permanencia
	double acumulado_ocupacion;		 // constructor del estadistico de ocupacion_promedio
									 // representa la integral en el tiempo de la ocupacion instantanea del estacionamiento

	// los tres estadisticos
	double promedio_permanencia; // Sumatoria(tiempos_de_permanencia) / cantidad_de_autos_que_salieron
	double tasa_rechazo;		 // cantidad_de_rechazados / cantidad_de_intentos_de_entrada
	double ocupacion_promedio;	 // area_bajo_la_curva_de_ocupacion / tiempo_total_simulacion

	bool propiedad_safety;	   // Los vehiculos que estacionan no pueden superar la cantidad maxima permitida de autos
	bool propiedad_liveness_1; // Todo vehiculo ingresado debe salir
	// bool propiedad_liveness_2;	// Todo vehiculo ingresado no puede estar mas del 1% del tiempo que estuvo estacionado para salir de la zona del estacionamiento

	std::ofstream csv; // Variable para manejar mejor los logs

	Port ultimo_puerto;

	// Puertos de entrada
	Port BARRERA_ENTRADA_ADMITIDOS;
	Port BARRERA_ENTRADA_RECHAZADOS;
	Port BARRERA_SALIDA_EGRESO_ABSOLUTO;
	// Puertos de salida
	Port TIEMPO_PROMEDIO_PERMANENCIA;
	Port TASA_DE_RECHAZO;
	Port OCUPACION_PROMEDIO;

	static double infinito; // Representación para el infinito

public:
	monitor(const char *n) : Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
	void actualizarPropiedadLiveness1(double t, double patente);
};

#endif
