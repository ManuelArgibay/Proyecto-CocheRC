
# Proyecto Coche RC — MSP430FR2355

Vehículo RC autónomo e interactivo desarrollado como proyecto de la asignatura 
de Microcontroladores y Sistemas Embebidos (Universidad de Barcelona).

Desarrollado en colaboración con [Nombre compañero](https://github.com/usuario).


## Descripción

El sistema implementa un coche controlable de forma remota e inteligente, 
programado sobre un microcontrolador TI MSP430FR2355. Integra múltiples 
periféricos y modos de operación que combinan control manual y comportamiento 
autónomo.


## Funcionalidades

- **Control remoto vía WiFi (UART):** comunicación inalámbrica para manejo 
  remoto del vehículo.
- **Control por joystick:** manejo manual directo montado sobre el propio coche.
- **Line tracking:** seguimiento de líneas mediante sensores ópticos.
- **Evasión de luz (LDR + ADC):** dos fotorresistencias en los laterales 
  traseros detectan iluminación direccional; el coche se aleja de la fuente 
  de luz mediante control diferencial.
- **Sensor de ultrasonido:** detección de obstáculos frontales.
- **Display:** visualización de estado e información del sistema.
- **Comunicación I2C:** bus de comunicación entre periféricos.


## Hardware

- **MCU:** TI MSP430FR2355
- **Conectividad:** Módulo WiFi (UART)
- **Sensores:** LDRs, ultrasonido, sensores de línea,
- **Control:** Joystick analógico
- **Visualización:** Display (I2C)
- **PCB:** Diseño propio en KiCAD (2 capas), fabricada externamente


## Estructura del repositorio

├── main.c              # Programa principal y lógica de control
├── ADC.c / ADC.h       # Drivers ADC (lectura LDRs y joystick)
├── I2C.c / I2C.h       # Drivers comunicación I2C
├── joystick.c / .h     # Lógica de control por joystick
├── clocks_timers.c/.h  # Configuración de clocks y timers
├── delay.c / delay.h   # Funciones de temporización


## Herramientas

- **IDE:** Code Composer Studio (CCS)
- **PCB:** KiCAD
- **Verificación:** Osciloscopio, multímetro, generador de funciones
- **Lenguaje:** C