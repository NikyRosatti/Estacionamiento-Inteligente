#include "sensor_entrada.h"

/** @brief Representa un valor infinito para el tipo double. */
double sensor_entrada::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa el estado del sensor de entrada
 *
 * Setea sigma en infinito y la condición de bloqueado y de procesando.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros: bloqueado inicial (0/1), latencia, tiempo máximo de entrada.
 */
void sensor_entrada::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    double valor = va_arg(parameters, double); // Inicialización del bool bloqueado
    latencia = va_arg(parameters, double); // latencia para detectar el vehiculo.
    TIEMPO_MAX_ENTRADA_VEHICULOS = va_arg(parameters, double); // tiempo max para que entren autos al estacionamiento.
    va_end(parameters);

    pat = 0;

    bloqueado = (valor != 0.0);
    procesando = false;
    fin_simulacion = false;

    // Puertos de entrada
	PATENTE_VEHICULO = 0;
	VOLVER_A_SENSAR = 1;
	// Puerto de salida
	DETECTAR_VEHICULO = 0;

    csv.open("CSVMano_sensorEntradaCola.csv", std::ios::out | std::ios::trunc);

    sigma = infinito;
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double sensor_entrada::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna del sensor de entrada
 *
 * Una transición interna indica que un auto fue enviado al controlador y se debe 
 * esperar su respuesta/señal para volver a sensar.
 * Esto se hace así por que no existe un tiempo fijo en el cuál saber cuándo volver a sensar,
 * sino que depende de los tiempos del controlador.
 *
 * Entonces, setea el sigma en infinito y el flag de procesando en falso.
 *
 * @param t Tiempo en el que ocurre el evento interno.
 */
void sensor_entrada::dint(double t)
{
    if (t >= TIEMPO_MAX_ENTRADA_VEHICULOS)
    {
        csv << "Sensor bloqueado: se alcanzó el tiempo máximo de simulación.\n";
        bloqueado = true;
        fin_simulacion = true;
        sigma = infinito;
    }
    else
    {
        sigma = infinito;
        procesando = false;
    }
}

/**
 * @brief Maneja eventos externos entrantes al sensor de entrada.
 *
 * Se puede recibir el arribo de una patente, por parte del generador, por el puerto cero,
 * una señal por parte del controlador de volver a sensar otro auto, por el puerto uno,
 * o una señal por parte del controlador para avisar que terminó la simulación y bloquear
 * el sensor hasta que termine la simulación, dejando autos encolados en el sensor.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void sensor_entrada::dext(Event x, double t)
{
    // Llega del generador aleatorio que quiere entrar
    if (x.port == PATENTE_VEHICULO)
    {
        csv << "recibido puerto 0, fin_simulacion= " << (fin_simulacion ? "True" : "False") << "\n";
        cola.push(x.getDouble()); // Ni bien llega lo encolamos
        csv << "Se encola: " << x.getDouble() << "\n";
        // La simulación aún continúa
        if (!fin_simulacion)
        {
            if (!bloqueado)
            {
                procesando = true;
                pat = cola.front();
                csv << "Sale de cola desde P0 : " << pat << "\n";
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
                    // Sensor bloqueado. No se puede detectar nuevo vehículo.
                    sigma = infinito;
                }
            }
        }
        // El flag de fin de simulación está prendido
        else
        {
            csv << "Vehículo encolado después del fin de simulación, queda atrapado en la cola del sensor.\n";
            sigma = infinito;
        }
    }
    // Llega una señal del controlador para desbloquearse (volver a sensar)
    else if (x.port == VOLVER_A_SENSAR)
    {
        csv << "recibido puerto 1, fin_simulacion= " << (fin_simulacion ? "True" : "False") << "\n";
        if (!fin_simulacion)
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
                csv << "Sale de cola desde el puerto 1: " << pat << "\n"
                    << "A la hora: " << t << "\n";
                procesando = true;
                bloqueado = true;
                sigma = latencia;
            }
        }
        else
        {
            sigma = infinito;
        }
    }
}

/**
 * @brief Genera eventos de salida desde el sensor de entrada a otros componentes.
 *
 * Devuelve la patente corriente que le toca pasar.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event sensor_entrada::lambda(double t)
{
    double *patente = new double(pat);
    return Event(patente, DETECTAR_VEHICULO);
}

/**
 * @brief Finaliza el sensor de entrada al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void sensor_entrada::exit()
{
    csv.close();
    csv_exit.open("zzCSVMano_sensorEntradaFin.csv", std::ios::out | std::ios::trunc);
    csv_exit << "quedaron " << cola.size() << " autos esperando en el sensor despues del tiempo final simulacion \n";
    csv_exit.close();
}
