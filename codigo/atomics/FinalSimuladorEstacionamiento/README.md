# Simulación de Estacionamiento

<div class="intro-block">

![UNRC](./unrc_logo.png) ![Simulator](./simulator_logo.png)

---

**Universidad Nacional de Río Cuarto**  
Facultad de Ciencias Exactas, Físico-Químicas y Naturales  
Proyecto de simulación basado en el formalismo DEVS.

---

</div>

## 🎯 Objetivo

Simular un sistema de estacionamiento inteligente con control de acceso, sensores, barreras automáticas y registro de estadísticas.

---

## 🧱 Componentes principales

| Módulo             | Descripción |
|--------------------|-------------|
| [controlador](@ref controlador)        | Coordina el ingreso y egreso de vehículos. |
| [sensor_entrada](@ref sensor_entrada)  | Detecta vehículos que quieren ingresar. |
| [sensor_salida](@ref sensor_salida)    | Detecta vehículos que quieren salir. |
| [barrera_entrada](@ref barrera_entrada) | Abre y cierra según autorización. |
| [barrera_salida](@ref barrera_salida)   | Controla el egreso. |
| [estacionamiento](@ref estacionamiento) | Almacena vehículos hasta que salen. |
| [monitor](@ref monitor)                | Recolecta métricas: ocupación, promedio de permanencia, etc. |
| [gen_aleatorio](@ref gen_aleatorio)    | Modela la llegada estocástica de vehículos. |
| [guardar_en_disco](@ref guardar_en_disco) | Registra la información relevante en archivos CSV. |

---

## 📊 Métricas observables

- Ocupación promedio
- Porcentaje de rechazos
- Tiempo promedio de permanencia

---

## 💡 Propiedades verificadas

- **Liveness**: todo vehículo que ingresa eventualmente puede salir.
- **Liveness**: tiempo que tarda en salir respecto del tiempo de permanencia en estacionamiento.
- **Safety**: no se permite ingreso si no hay lugar disponible.

---

© Nicolle Rosatti - Joaquin Tissera – "Simulación"
