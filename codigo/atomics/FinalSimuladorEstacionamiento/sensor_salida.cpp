#include "sensor_salida.h"

/** @brief Representa un valor infinito para el tipo double. */
double sensor_salida::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa el estado del sensor de salida
 *
 * Setea sigma en infinito y la condición de bloqueado y de procesando.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void sensor_salida::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    double valor = va_arg(parameters, double); // Inicialización del bool bloqueado
    latencia = va_arg(parameters, double); // latencia para detectar el vehiculo.
    va_end(parameters);

    bloqueado = (valor != 0.0);
    procesando = false;

    // Puertos de entrada
	VEHICULO_QUIERE_SALIR = 0;
	VOLVER_A_SENSAR = 1;
	// Puerto de salida
	SOLICITAR_SALIDA = 0;

    csv.open("CSV_SensorSalida_Cola.csv", std::ios::out | std::ios::trunc);

    sigma = infinito;
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double sensor_salida::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna del sensor de salida
 *
 * Al hacerse una transición interna, significa que mandó un auto al controlador pero
 * debe quedarse esperando a que el controlador le responda de nuevo para que vuelva a sensar.
 * Esto se hace así por que no existe un tiempo fijo en el cuál saber cuándo volver a sensar,
 * sino que depende de los tiempos del controlador.
 *
 * Entonces, setea el sigma en infinito y el flag de procesando en falso.
 *
 * @param t Tiempo en el que ocurre el evento interno.
 */
void sensor_salida::dint(double t)
{
    sigma = infinito;
    procesando = false;
}

/**
 * @brief Maneja eventos externos entrantes al sensor de salida.
 *
 * Se puede recibir el arribo de una patente, por parte del generador, por el puerto cero,
 * o una señal por parte del controlador de volver a sensar otro auto, por el puerto uno.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void sensor_salida::dext(Event x, double t)
{
    // Llega del estacionamiento que quiere salir
    if (x.port == VEHICULO_QUIERE_SALIR)
    {
        cola.push(x.getDouble()); // Ni bien llega lo encolamos
        csv << "Se encola: " << x.getDouble() << "\n";
        if (!bloqueado)
        {
            procesando = true;
            pat = cola.front();
            csv << "Patente procesada desde cola (puerto 0): " << pat << "\n";
            csv << "Sale en : " << t << "\n";
            cola.pop();
            bloqueado = true;
            sigma = latencia; // delay de 1
        }
        else
        {
            if (procesando)
            {
                // Está procesando uno
                sigma = sigma - e;
            }
            else
            {
                // No esta en el sensor pero no se puede detectar otro auto porque está bloqueado
                sigma = infinito;
            }
        }
    }
    // Llega una señal del controlador para desbloquearse (volver a sensar)
    else if (x.port == VOLVER_A_SENSAR)
    {
        bloqueado = false;
        if (cola.empty())
        {
            // No hay nadie en la cola del sensor, queda oscioso
            sigma = infinito;
        }
        else
        {
            // Hay cola en el sensor, procesa el primero
            pat = cola.front();
            cola.pop();
            csv << "Sale de cola desde el puerto 1(salida) : " << pat << "\n";
            csv << "A la hora: " << t << "\n";
            procesando = true;
            bloqueado = true;
            sigma = latencia;
        }
    }
}

/**
 * @brief Genera eventos de salida desde el sensor de salida a otros componentes.
 *
 * Devuelve la patente corriente que le toca salir.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event sensor_salida::lambda(double t)
{
    double *patente = new double(pat);
    return Event(patente, SOLICITAR_SALIDA);
}

/**
 * @brief Finaliza el sensor de salida al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void sensor_salida::exit()
{
    // Código ejecutado al final de la simulación.
}
