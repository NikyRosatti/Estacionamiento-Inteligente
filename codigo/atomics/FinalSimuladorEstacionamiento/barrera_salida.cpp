#include "barrera_salida.h"

/** @brief Representa un valor infinito para el tipo double. */
double barrera_salida::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa la barrera de salida con tiempos y estados por defecto.
 *
 * Se establecen los valores iniciales de sigma y patente inicial.
 * Inicializa la semilla, motor y distribucion relacionados a la distribución
 * uniforme según los parámetros dados desde el editor.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void barrera_salida::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    double seed = va_arg(parameters, double); // Semilla
    double a = va_arg(parameters, double);    // Rango inicial de la uniforme
    double b = va_arg(parameters, double);    // Rango final de la uniforme
    tiempo_apertura = va_arg(parameters, double); // Tiempo de apertura de barrera
    tiempo_cierre = va_arg(parameters, double); // Tiempo de cierre de barrera
    va_end(parameters);

    pat = 0;

    engine_.seed(static_cast<unsigned int>(seed));
    dist_ = std::uniform_real_distribution<double>(a, b);

    // Puerto de entrada
	PERMITIR_SALIDA = 0;
	// Puerto de salida
	VEHICULO_HA_SALIDO = 0;

    sigma = infinito;
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double barrera_salida::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna de la barrera de salida.
 *
 * Resetea la barrera: limpia patente actual y el sigma en infinito.
 * @param t Tiempo en el que ocurre el evento interno.
 */
void barrera_salida::dint(double t)
{
    pat = 0;
    sigma = infinito;
}

/**
 * @brief Maneja eventos externos entrantes a la barrera de salida.
 *
 * Todo evento externo que llega a la barrera de salida indica que un auto está
 * atravesando la barrera, entonces este dev actúa como un retardante según
 * un tiempo uniforme.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void barrera_salida::dext(Event x, double t)
{
    // Entrada por el único puerto: x.port == PERMITIR_SALIDA == 0
    sigma = tiempo_apertura + dist_(engine_) + tiempo_cierre;
    pat = x.getDouble();
}

/**
 * @brief Genera eventos de salida desde la barrera de salida a otros componentes.
 *
 * Devuelve la patente que utilizó por último la barrera.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event barrera_salida::lambda(double t)
{
    double *patente = new double(pat);
    return Event(patente, VEHICULO_HA_SALIDO);
}

/**
 * @brief Finaliza la barrera de salida al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void barrera_salida::exit()
{
    // Código ejecutado al final de la simulación.
}
