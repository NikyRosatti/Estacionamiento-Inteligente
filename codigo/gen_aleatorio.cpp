#include "gen_aleatorio.h"

/**
 * @brief Inicializa el estado del generador aleatorio
 *
 * Setea sigma en un tiempo exponencial y la patente en 1.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void gen_aleatorio::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    double mean = va_arg(parameters, double);   // Leo la var mean
    double seed = va_arg(parameters, double);   // Leo la semilla
    va_end(parameters);

    engine.seed(seed);
    exp_dist = std::exponential_distribution<double>(1.0 / mean);

    pat = 1.0;

    // Puerto de salida
    PATENTE_VEHICULO = 0;

    sigma = exp_dist(engine);
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double gen_aleatorio::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna del generador aleatorio.
 *
 * Setea el sigma siguiendo un tiempo exponencial y suma en 1 a la patente.
 * 
 * @param t Tiempo en el que ocurre el evento interno.
 */
void gen_aleatorio::dint(double t)
{
    sigma = exp_dist(engine);
    pat ++;
}

/**
 * @brief Maneja eventos externos entrantes al generador aleatorio.
 *
 * Un generador aleatorio no recibe ningún evento externo, por lo tanto
 * esta función queda vacía.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void gen_aleatorio::dext(Event x, double t)
{
    // Al ser un generador aleatorio, no recibe ningún evento externo.
    // Este método queda vacío.
}

/**
 * @brief Genera eventos de salida desde el generador aleatorio a otros componentes.
 *
 * Devuelve la patente generada.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event gen_aleatorio::lambda(double t)
{
    double* output = new double(pat);
    return Event(output, PATENTE_VEHICULO);
}

/**
 * @brief Finaliza el generador aleatorio al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void gen_aleatorio::exit()
{
    // Código ejecutado al final de la simulación.
}
