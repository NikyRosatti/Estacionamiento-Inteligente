#include "controlador.h"

/** @brief Representa un valor infinito para el tipo double. */
double controlador::infinito = std::numeric_limits<double>::infinity();

/**
 * @brief Inicializa el controlador con tiempos y estados por defecto.
 *
 * Se establecen los valores iniciales de sigma, flags de bloqueo,
 * contador de autos y se abre un archivo CSV para registro paso a paso.
 *
 * @param t Tiempo de inicio de la simulación.
 * @param ... Parámetros adicionales provenientes del editor de simulación.
 */
void controlador::init(double t, ...)
{
    // Inicialización de variables según los parámetros desde el editor
    va_list parameters;
    va_start(parameters, t);
    // Capacidad maxima de autos en el estacionamiento.
    capacidad_max_estacionamiento = va_arg(parameters, double);
    tiempo_max_rta_controlador = va_arg(parameters, double);
    va_end(parameters);

    // Tiempos iniciales indefinidos (infinitos)
    sigma = infinito;

    // Contador de autos presentes
    cant_autos = 0;

    // Estados de espera y control de barreras
    esperando_entrar = false;
    pasar_a_barrera_entrada = false;

    // Puertos de entrada
    VEHICULO_HA_INGRESADO = 0;
    DETECTAR_VEHICULO = 1;
    SOLICITAR_SALIDA = 2;
    VEHICULO_HA_SALIDO = 3;
    RECHAZADOS = 4;
    // Puertos de salida
    PERMITIR_ENTRADA_BARRERA_DIRECTO = 0;
    PERMITIR_ENTRADA_ESTACIONAMIENTO_Y_VOLVER_SENSAR_ENTRADA = 1;
    DENEGAR_ENTRADA = 2;
    VOLVER_A_SENSAR_SALIDA = 3;
    PERMITIR_SALIDA = 4;
    PERMITIR_ENTRADA_BARRERA_DEMORA_Y_VOLVER_SENSAR_SALIDA = 5;
    VOLVER_A_SENSAR_ENTRADA = 6;

    // Apertura de archivo CSV para log de pasos
    csv.open("CSV_Controlador_PasoPaso.csv", std::ios::out | std::ios::trunc);
    csv << "Tiempo inicial: " << sigma << "\n";
    csv << "Cant Autos Est: " << cant_autos << "\n";
}

/**
 * @brief Calcula el tiempo al siguiente evento interno.
 *
 * @param t Tiempo actual de la simulación.
 * @return Valor de sigma indicando cuándo ocurrirá el próximo evento interno.
 */
double controlador::ta(double t)
{
    return sigma;
}

/**
 * @brief Maneja la transición interna del controlador.
 *
 * Se reinicia sigma a infinito y se libera la señal para la barrera de entrada.
 * @param t Tiempo en el que ocurre el evento interno.
 */
void controlador::dint(double t)
{
    csv << "Ejecute un evento interno " << "\n";
    sigma = infinito;
    pasar_a_barrera_entrada = false;
}

/**
 * @brief Maneja eventos externos entrantes al controlador.
 *
 * Registra información en CSV y actualiza estado según el puerto del evento:
 * - Puerto 1: entrada de auto.
 * - Puerto 0: finalización de paso por barrera de entrada.
 * - Puerto 2: solicitud de salida de auto.
 * - Puerto 3: finalización de paso por barrera de salida.
 *
 * @param x Evento externo recibido.
 * @param t Tiempo actual de la simulación.
 */
void controlador::dext(Event x, double t)
{
    csv << "Puerto nro: " << x.port << "\n";
    puerto = x.port;
    pat = x.getDouble();
    csv << "tiempo actual: " << t << "\n";

    csv << "\n"
        << "Tiempo: " << t << "\n"
        << ", cant_autos: " << cant_autos << "\n"
        << "\n";

    // Auto completó su paso por la barrera de entrada.
    if (x.port == VEHICULO_HA_INGRESADO)
    {
        csv << "Listo para recibir nuevo vehículo. Última patente: " << pat << "\n";
        sigma = 0.0;
    }
    // Auto quiere pasar la barrera. Llega un auto para entrar.
    else if (x.port == DETECTAR_VEHICULO)
    {
        if (cant_autos < capacidad_max_estacionamiento)
        {
            csv << "Lugar disponible. Autos actuales: " << cant_autos << ". Patente entrante: " << pat << "\n";
            cant_autos++;
            sigma = 0.0; // evento inmediato
        }
        else if (cant_autos == capacidad_max_estacionamiento)
        {
            // Estacionamiento lleno: espera hasta 3 segundos
            csv << "Estacionamiento lleno. Auto en espera durante 3 segundos. Patente: " << pat << "\n";
            sigma = tiempo_max_rta_controlador; // 3 segundos
            pat_entrada = x.getDouble();
            esperando_entrar = true;
        }
    }
    // Auto quiere salir
    else if (x.port == SOLICITAR_SALIDA)
    {
        csv << "Un auto quiere salir: " << "con " << pat << "\n";
        sigma = 0.0; // evento inmediato
    }
    // Auto completó su paso por la barrera de salida
    else if (x.port == VEHICULO_HA_SALIDO)
    {
        csv << "Termina de salir: " << "con " << pat << "\n";
        cant_autos--;
        csv << "Hay estac(cant autos): " << cant_autos << "\n";
        csv << "Valor de esperando_entrar: " << esperando_entrar << "\n";
        if (esperando_entrar)
        {
            // Permite que el auto en espera ingrese
            csv << "Hay alguien esperando a entrar: " << "con " << pat_entrada << "\n";
            pat = pat_entrada;
            cant_autos++;
            csv << "Hay estac(cant autos): " << cant_autos << "\n";
            pasar_a_barrera_entrada = true;
            esperando_entrar = false; // ya va a entrar
        }
        sigma = 0.0;
    }
    // Auto se retiró de la barrera de entrada luego de ser rechazado
    else if (x.port == RECHAZADOS)
    {
        sigma = 0.0;
        esperando_entrar = false;
    }
}

/**
 * @brief Genera eventos de salida desde el controlador a otros componentes.
 *
 * Dependiendo del último puerto y estados internos, devuelve un evento
 * apropiado señalando a barreras o sensores.
 *
 * @param t Tiempo actual de la simulación.
 * @return Evento generado con patente y puerto destino.
 */
Event controlador::lambda(double t)
{
    double *patente = new double(pat);

    if (puerto == VEHICULO_HA_INGRESADO)
    {
        csv << "Info pal sensor(vuelta) y al estacionamiento: " << "\n";
        return Event(patente, PERMITIR_ENTRADA_ESTACIONAMIENTO_Y_VOLVER_SENSAR_ENTRADA); // le digo al sensor que ya puede mandar otro auto
        // además le informo al estacionamiento que llegó uno
    }
    // va a pasar un auto
    else if (puerto == DETECTAR_VEHICULO && (!esperando_entrar))
    {
        csv << "\n";
        csv << "Info p Barrera(ida): " << "\n";
        csv << "\n";
        return Event(patente, PERMITIR_ENTRADA_BARRERA_DIRECTO); // lo mando a la barrera
    }
    // si se llega a ejecutar esto es porque finalizaron los 3 seg de respuesta del controlador
    // y el estacionamiento sigue lleno (no salio nadie en esos 3 segs)
    // por lo tanto no pudo ingresar y se debe rechazar
    else if (puerto == DETECTAR_VEHICULO && esperando_entrar)
    {
        esperando_entrar = false; // porque ya no puede entrar mas
        csv << "Rechazado no pudo entrar: " << pat_entrada << "\n";
        double *patente = new double(pat_entrada);
        return Event(patente, DENEGAR_ENTRADA); // lo mando a la barrera pero como rechazado
    }
    else if (puerto == SOLICITAR_SALIDA)
    {
        csv << "Info p BarreraSalida(ida): " << "\n";
        return Event(patente, PERMITIR_SALIDA);
    }
    else if (puerto == VEHICULO_HA_SALIDO && !pasar_a_barrera_entrada)
    {
        csv << "Info p sensorS: " << "\n";
        return Event(patente, VOLVER_A_SENSAR_SALIDA);
    }
    else if (puerto == VEHICULO_HA_SALIDO && pasar_a_barrera_entrada)
    {
        esperando_entrar = false;
        csv << "Valor de esperando_entrar: " << esperando_entrar << "\n";
        csv << "Info p Barrera(ida) ya que se desocupo el estacionamiento: " << "\n";
        return Event(patente, PERMITIR_ENTRADA_BARRERA_DEMORA_Y_VOLVER_SENSAR_SALIDA);
        // entrando a barrera y notificando al sensor de salida
    }
    else if (puerto == RECHAZADOS)
    {
        // si se rechaza el auto, no ingresa al estacionamiento pero el sensor admite otro.
        return Event(patente, VOLVER_A_SENSAR_ENTRADA);
    }
}

/**
 * @brief Finaliza el controlador al terminar la simulación.
 *
 * Se puede usar para cerrar archivos o liberar recursos.
 */
void controlador::exit()
{
    csv.close();
    csv_exit.open("Exit_Controlador_Propiedades.csv", std::ios::out | std::ios::trunc);
    csv_exit << "quedaron: " << cant_autos << " autos en el estacionamiento despues de que terminara por tiempo la simulacion" << "\n";
    csv_exit.close();
}
