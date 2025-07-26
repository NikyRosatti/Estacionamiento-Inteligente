#include "monitor.h"

/** @brief Representa un valor infinito para el tipo double. */
double monitor::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa el estado del monitor
 *
 * Setea sigma en infinito y las distintas variables estadísticas necesarias.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void monitor::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    va_end(parameters);

    // Inicializacion de constantes
    K_ESTACIONAMENTO = 30.0;
    // Puertos de entrada
    BARRERA_ENTRADA_ADMITIDOS = 0;
    BARRERA_ENTRADA_RECHAZADOS = 1;
    BARRERA_SALIDA_EGRESO_ABSOLUTO = 2;
    // Puertos de salida
    TIEMPO_PROMEDIO_PERMANENCIA = 0;
    TASA_DE_RECHAZO = 1;
    OCUPACION_PROMEDIO = 2;

    // Inicializacion de variables del estado
    total_intentos = 0.0;
    rechazados = 0.0;
    total_admitidos = 0.0;
    total_salidos = 0.0;
    tiempo_ultimo_evento = t;
    ocupacion_actual = 0.0;
    suma_tiempos_permanencia = 0.0;
    acumulado_ocupacion = 0.0;
    promedio_permanencia = 0.0;
    tasa_rechazo = 0.0;
    ocupacion_promedio = 0.0;
    propiedad_safety = true;     // ocupacion_actual <= K_ESTACIONAMENTO
    propiedad_liveness_1 = true; // todo auto entro & salio, al inicio true
    sigma = infinito;

    csv.open("zzCSVMano_Monitor_Liveness.csv", std::ios::out | std::ios::trunc);
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double monitor::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna del monitor
 *
 * Reinicia sigma a infinito ya que no hay eventos internos propios que se generen activamente.
 *
 * @param t Tiempo en el que ocurre el evento interno.
 */
void monitor::dint(double t)
{
    sigma = infinito;
}

/**
 * @brief Maneja eventos externos entrantes al monitor.
 *
 * Cuando un evento externo llega al monitor, se hace el cálculo de estadísticos parciales
 * y se actualiza el estado.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void monitor::dext(Event x, double t)
{
    ultimo_puerto = x.port;
    acumulado_ocupacion += ocupacion_actual * (t - tiempo_ultimo_evento);
    tiempo_ultimo_evento = t;
    int patente = x.getDouble();
    if (x.port == BARRERA_ENTRADA_ADMITIDOS)
    {
        // un auto admitido termino su paso por la barrera
        total_intentos++;
        total_admitidos++;
        ocupacion_actual++;

        lista_par_patentes_estado_sistema.push_back(std::make_pair(x.getDouble(), true));

        if (patente >= tiempos_entrada.size())
        {
            tiempos_entrada.resize(patente + 1, -1); // opcionalmente -1 como marcador
        }
        tiempos_entrada[patente] = t;

        if (patente >= autos_entrantes.size())
        {
            autos_entrantes.resize(patente + 1, -1); // opcionalmente -1 como marcador
        }
        autos_entrantes[patente] = x.getDouble();

        // actualizar propiedad safety
        propiedad_safety = propiedad_safety && (ocupacion_actual <= K_ESTACIONAMENTO);
        sigma = 0.0;
    }
    else if (x.port == BARRERA_ENTRADA_RECHAZADOS)
    {
        // un auto rechazado termino su paso por la barrera
        total_intentos++;
        rechazados++;

        lista_par_patentes_estado_sistema.push_back(std::make_pair(x.getDouble(), false));

        // construyo el estadistico parcial de tasa de rechazo
        tasa_rechazo = rechazados / total_intentos;

        // actualizar propiedad safety
        propiedad_safety = propiedad_safety && (ocupacion_actual <= K_ESTACIONAMENTO);
        sigma = 0.0;
    }
    else if (x.port == BARRERA_SALIDA_EGRESO_ABSOLUTO)
    {
        // llega la patente de un auto que salio del sistema de manera absoluta
        if (patente >= tiempos_salida.size())
        {
            tiempos_salida.resize(patente + 1, -1); // opcionalmente -1 como marcador
        }
        tiempos_salida[patente] = t;

        if (patente >= autos_salientes.size())
        {
            autos_salientes.resize(patente + 1, -1); // opcionalmente -1 como marcador
        }
        autos_salientes[patente] = x.getDouble();

        double permanencia = tiempos_salida[patente] - tiempos_entrada[patente];
        // construyo el estadistico parcial de promedio permanencia
        suma_tiempos_permanencia += permanencia;
        total_salidos++;
        promedio_permanencia = suma_tiempos_permanencia / total_salidos;

        ocupacion_actual--;

        // defensiva de division por cero
        if (t > 0)
        {
            // construyo el estadistico parcial de ocupacion promedio
            ocupacion_promedio = acumulado_ocupacion / t;
        }

        // actualizar propiedad safety
        propiedad_safety = propiedad_safety && (ocupacion_actual <= K_ESTACIONAMENTO);
        actualizarPropiedadLiveness1(t, x.getDouble());
        sigma = 0.0;
    }
}

/**
 * @brief Genera eventos de salida desde el monitor a otros componentes.
 *
 * Como el monitor actúa de manera pasiva, los valores devueltos se corresponden
 * a la actualización de los distintos estadísticos pero no influyen en el funcionamiento
 * del sistema de estacionamiento.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento con valor estadístico y puerto destino.
 */
Event monitor::lambda(double t)
{
    // Si el ultimo puerto fue relacionado a la entrada al estacionamiento, saca el promedio de permanencia
    if (ultimo_puerto == BARRERA_ENTRADA_ADMITIDOS)
    {
        double *pp = new double(promedio_permanencia);
        return Event(pp, TIEMPO_PROMEDIO_PERMANENCIA);
    }
    // Si el ultimo puerto fue relacionado a un auto rechazado, saca la tasa de rechazo
    else if (ultimo_puerto == BARRERA_ENTRADA_RECHAZADOS)
    {
        double *tr = new double(tasa_rechazo);
        return Event(tr, TASA_DE_RECHAZO);
    }
    // Si el ultimo puerto fue relacionado a la salida del estacionamiento, saca la ocupacion promedio
    else if (ultimo_puerto == BARRERA_SALIDA_EGRESO_ABSOLUTO)
    {
        double *op = new double(ocupacion_promedio);
        return Event(op, OCUPACION_PROMEDIO);
    }
}

/**
 * @brief Finaliza el generador aleatorio al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void monitor::exit()
{
    csv.close();
    csv.open("CSVMano_Monitor_Propiedades.csv", std::ios::out | std::ios::trunc);
    csv << "propiedad safety cumplida: " << propiedad_safety << "\n";
    csv << "propiedad liveness_1 cumplida: " << propiedad_liveness_1 << "\n";
    csv.close();
}

/**
 * @brief Actualiza la propiedad de Liveness nro 1.
 *
 * @param t Tiempo actual de la simulación.
 * @param patente Patente para actualizar la propiedad.
 */
void monitor::actualizarPropiedadLiveness1(double t, double patente)
{
    // csv.open("zzCSVMano_Monitor_Liveness.csv", std::ios::out | std::ios::trunc);
    csv << "INICIO propiedad liveness_1 cumplida: " << propiedad_liveness_1 << "\n";
    csv << "patente: " << patente << "\n";
    bool entro = false;
    csv << "entro: " << entro << "\n";
    for (int i = 0; i < autos_entrantes.size(); i++)
    {
        if (patente == autos_entrantes[i])
        {
            csv << "lo encontre, posicion: " << i << "\n";
            csv << "autos_entrantes[" << i << "]: " << autos_entrantes[i] << "\n";
            entro = true;
            break;
        }
    }
    csv << "entro: " << entro << "\n";

    bool salio = false;
    csv << "salio: " << salio << "\n";
    for (int i = 0; i < autos_salientes.size(); i++)
    {
        if (patente == autos_salientes[i])
        {
            csv << "lo encontre, posicion: " << i << "\n";
            csv << "autos_salientes[" << i << "]: " << autos_salientes[i] << "\n";
            salio = true;
            break;
        }
    }
    csv << "salio: " << salio << "\n";

    propiedad_liveness_1 = propiedad_liveness_1 && (entro && salio);
    csv << "FIN propiedad liveness_1 cumplida: " << propiedad_liveness_1 << "\n";
    csv << "\n";
}
