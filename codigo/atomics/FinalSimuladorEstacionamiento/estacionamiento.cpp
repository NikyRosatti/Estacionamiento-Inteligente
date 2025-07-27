#include "estacionamiento.h"

/** @brief Representa un valor infinito para el tipo double. */
double estacionamiento::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa el estado del estacionamiento
 *
 * Se establecen los valores iniciales de sigma infinito y cola de prioridad vacía.
 * Inicializa la semilla, motor y distribucion relacionados a la distribución
 * uniforme según los parámetros dados desde el editor.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void estacionamiento::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    double seed = va_arg(parameters, double); // Semilla
    double a = va_arg(parameters, double);    // Rango inicial de la uniforme
    double b = va_arg(parameters, double);    // Rango final de la uniforme
    va_end(parameters);

    engine_.seed(static_cast<unsigned int>(seed));
    dist_ = std::uniform_real_distribution<double>(a, b);

    csv.open("CSV_Estacionamiento_PasoPaso.csv", std::ios::out | std::ios::trunc);
    csv << "semilla= " << seed << "\n";
    csv << "a= " << a << "\n";
    csv << "b= " << b << "\n";

    // Puerto de entrada
	PERMITIR_ENTRADA = 0;
	// Puerto de salida
	VEHICULO_QUIERE_SALIR = 0;

    sigma = infinito;
    cola_prioridad = decltype(cola_prioridad)();
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double estacionamiento::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna del estacionamiento.
 *
 * Se setea el sigma según si la cola está vacía o no.
 * @param t Tiempo en el que ocurre el evento interno.
 */
void estacionamiento::dint(double t)
{
    if (!cola_prioridad.empty())
    {
        csv << "\n";
        csv << "prox salir! " << cola_prioridad.top().first << "\n";
        csv << "tarda: " << cola_prioridad.top().second << "\n";
        csv << "t actual: " << t << "\n";
        csv << "DINT cantidad de autos en el estacionamiento: " << cola_prioridad.size() << "\n";
        csv << "\n";
        // Actualizar tiempo de permanencia, ya que cuando un auto se va, queda desactualizado el resto de autos dentro del estacionamiento
        actualizarTiempos(t);
        sigma = cola_prioridad.top().second;
    }
    else
    {
        csv << "no hay nadie p salir" << "\n";
        sigma = infinito;
    }
}

/**
 * @brief Maneja eventos externos entrantes al estacionamiento.
 *
 * Al llegar un evento externo, actualizamos los tiempos de todos los autos encolados.
 * Luego encolamos la patente del auto junto con un tiempo uniforme.
 * Por último seteamos sigma en el que está más próximo a salir.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void estacionamiento::dext(Event x, double t)
{
    // Entrada por el único puerto: x.port == PERMITIR_ENTRADA == 0
    csv << "\n";
    csv << "llega uno! " << "\n"
        << "tiempo actual: " << t << "\n"
        << "patente actual: " << x.getDouble() << "\n";
    // agrego el elemento a la cola de prioridad
    actualizarTiempos(t);
    double tiempo_salida = dist_(engine_);
    csv << "tiempo permanencia dado por el estacionamiento= " << tiempo_salida << "\n";
    cola_prioridad.push(std::make_pair(x.getDouble(), tiempo_salida));
    csv << "DEXT cantidad de autos en el estacionamiento: " << cola_prioridad.size() << "\n";
    csv << "\n";
    // le doy el tiempo mas proximo a salir del estacionamiento(minimo)
    sigma = cola_prioridad.top().second;
}

/**
 * @brief Genera eventos de salida desde el estacionamiento a otros componentes.
 *
 * Remueve y devuelve la patente que le toca salir del estacionamiento.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event estacionamiento::lambda(double t)
{
    double patente = cola_prioridad.top().first;
    csv << "\n";
    csv << "tiempo lambda ya: " << t
        << " patente que sale " << patente << "\n";
    csv << "\n";
    cola_prioridad.pop();
    double *p = new double(patente);
    return Event(p, VEHICULO_QUIERE_SALIR);
}

/**
 * @brief Finaliza el estacionamiento al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void estacionamiento::exit()
{
    csv.close();
    csv_exit.open("Exit_Estacionamiento_Propiedades.csv", std::ios::out | std::ios::trunc);
    csv_exit << "quedaron: " << cola_prioridad.size() << " autos en el estacionamiento despues de que terminara por tiempo la simulacion" << "\n";
    csv_exit.close();
}

/**
 * @brief Función auxiliar para actualizar los tiempos de la cola de prioridad
 *
 * Dado el tiempo actual de la simulación, recorre la cola de prioridad y actualiza
 * los tiempos: de tiempos absolutos los pasa a tiempos remanentes.
 *
 * @param t Tiempo actual de la simulación
 */
void estacionamiento::actualizarTiempos(double t)
{
    double tiempo;
    double patente;

    std::priority_queue<
        std::pair<double, double>,
        std::vector<std::pair<double, double>>,
        CompareSecondMin>
        colaAux;

    if (!cola_prioridad.empty())
    {
        csv << "\n";
        csv << "Cantidad de autos en el estacionamiento: " << cola_prioridad.size() << "\n" ;
        csv << "Actualizacion de tiempos en el estacionamiento "
            << "Tiempo actual: " << t << "\n";
    }

    while (!cola_prioridad.empty())
    {
        patente = cola_prioridad.top().first;
        csv << "Patente: " << patente << "\n";
        csv << "Tiempo de permanencia desactualizado: " << cola_prioridad.top().second << "\n";
        tiempo = cola_prioridad.top().second - e;
        csv << "Tiempo de permanencia actualizado: " << tiempo << "\n";
        cola_prioridad.pop();
        if (tiempo < 0.0)
            tiempo = 0.0;
        colaAux.push(std::make_pair(patente, tiempo));
    }
    csv << "\n";
    cola_prioridad = std::move(colaAux);
}
