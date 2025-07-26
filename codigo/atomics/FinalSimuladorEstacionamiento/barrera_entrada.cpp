#include "barrera_entrada.h"

/** @brief Representa un valor infinito para el tipo double. */
double barrera_entrada::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa la barrera de entrada con tiempos y estados por defecto.
 *
 * Se establecen los valores iniciales de sigma, flag de auto rechazado,
 * patente inicial.
 * Inicializa la semilla, motor y distribucion relacionados a la distribución
 * uniforme según los parámetros dados desde el editor.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void barrera_entrada::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    double seed = va_arg(parameters, double); // Semilla
    double a = va_arg(parameters, double);    // Rango inicial de la uniforme
    double b = va_arg(parameters, double);    // Rango final de la uniforme
    tiempo_apertura = va_arg(parameters, double); // Tiempo de apertura de barrera
    tiempo_cierre = va_arg(parameters, double); // Tiempo de cierre de barrera
    tiempo_retiro = va_arg(parameters, double); // Tiempo de retiro de un auto al ser rechazado
    va_end(parameters);

    pat = 0;
    rechazado = false;

    engine_.seed(static_cast<unsigned int>(seed));
    dist_ = std::uniform_real_distribution<double>(a, b);

    // Puertos de entrada
	PERMITIR_ENTRADA = 0;
	DENEGAR_ENTRADA = 1;
	// Puertos de salida
	VEHICULO_HA_INGRESADO = 0;
	VEHICULO_RECHAZADO_SE_HA_RETIRADO = 1;

    sigma = infinito;
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double barrera_entrada::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna de la barrera de entrada.
 *
 * Se setea el flag de rechazado en falso, una patente nula y sigma en infinito.
 * @param t Tiempo en el que ocurre el evento interno.
 */
void barrera_entrada::dint(double t)
{
    rechazado = false;
    pat = 0;
    sigma = infinito;
}

/**
 * @brief Maneja eventos externos entrantes a la barrera de entrada.
 *
 * Actualiza el estado según el puerto del evento:
 * - Puerto 0: un auto fue aceptado por el controlador.
 * - Puerto 1: un auto fue rechazado por el controlador.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void barrera_entrada::dext(Event x, double t)
{
    if (x.port == PERMITIR_ENTRADA)
    {
        sigma = tiempo_apertura + dist_(engine_) + tiempo_cierre;
        rechazado = false;
        pat = x.getDouble();
    }
    else if (x.port == DENEGAR_ENTRADA)
    {
        // Tiempo de espera (2 seg) para que un auto rechazado se retire del sistema.
        sigma = tiempo_retiro;
        rechazado = true;
        pat = x.getDouble();
    }
}

/**
 * @brief Genera eventos de salida desde la barrera de entrada a otros componentes.
 *
 * Devuelve la patente que utilizó por último la barrera.
 * Por el puerto cero devuelve patentes que fueron admitidas y por el puerto uno
 * devuelve las patentes que fueron rechazadas.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event barrera_entrada::lambda(double t)
{
    double *patente = new double(pat);
    if (!rechazado)
    {
        return Event(patente, VEHICULO_HA_INGRESADO); // admitidos
    }
    else
    {
        return Event(patente, VEHICULO_RECHAZADO_SE_HA_RETIRADO); // rechazados
    }
}

/**
 * @brief Finaliza la barrera de entrada al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void barrera_entrada::exit()
{
    // Código ejecutado al final de la simulación.
}
