# DinoOLED

Juego inspirado en el dinosaurio offline de Google Chrome, desarrollado en Arduino para una pantalla OLED 0.96" I2C SSD1306 y un botón/switch para controlar el salto.

## Descripción

Este proyecto recrea una versión simple del clásico juego del dinosaurio que aparece cuando no hay conexión a internet en Chrome. El jugador controla un dinosaurio que debe saltar para esquivar obstáculos que se mueven de derecha a izquierda en la pantalla.

El proyecto está pensado para funcionar con:
- Arduino Uno o Nano
- Pantalla OLED 0.96" I2C 128x64
- Un botón o switch para saltar

## Características

- Pantalla OLED 128x64
- Control con un solo botón
- Sistema de salto con gravedad
- Obstáculos en movimiento
- Detección de colisiones
- Pantalla de Game Over
- Reinicio del juego con el mismo botón
- Incremento progresivo de dificultad

## Hardware requerido

- 1 Arduino Uno o Arduino Nano
- 1 pantalla OLED 0.96" I2C SSD1306
- 1 botón o switch
- Cables Dupont
- Protoboard opcional

## Conexiones

### Pantalla OLED I2C
| OLED | Arduino Uno/Nano |
|------|------------------|
| VCC  | 5V               |
| GND  | GND              |
| SCL  | A5               |
| SDA  | A4               |

### Botón / Switch
| Botón | Arduino |
|-------|---------|
| Pin 1 | D2      |
| Pin 2 | GND     |

Nota: el código usa `INPUT_PULLUP`, por lo que no se necesita resistencia externa.

## Librerías necesarias

Instala estas librerías desde el Library Manager del Arduino IDE:

- Adafruit GFX
- Adafruit SSD1306

## Instalación

1. Clona este repositorio:
   ```bash
   git clone https://github.com/tuusuario/DinoOLED.git