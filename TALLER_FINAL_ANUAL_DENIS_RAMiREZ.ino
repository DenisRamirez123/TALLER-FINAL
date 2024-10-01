/* 
       Centro Educativo Tecnico laboral Kinal
         Taller de Electronica 
         Denis Estuardo de Jesús Ramírez Padilla
                        2020288
          EB5AM
         Alejandro Navas

                  PRACTICA FINAL DE TALLER

*/

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>  // Biblioteca del acelerómetro ADXL345
#include <DFRobotDFPlayerMini.h> // Biblioteca del DFminiplayer
#include <SoftwareSerial.h>      
#include <Adafruit_NeoPixel.h>   // Biblioteca de la tira Neopixel

// Pines
const int boton1 = 2; // Botón 1 conectado al pin 2
const int boton2 = 3; // Botón 2 conectado al pin 3
const int pinNeopixel = 7; // Pin para controlar el anillo de Neopixel

// Configuración del DFPlayer
SoftwareSerial mySerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Configuración del Neopixel
const int numPixels = 59; // Número de LEDs en el anillo de Neopixel 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, pinNeopixel, NEO_GRB + NEO_KHZ800);

// Configuración del acelerómetro ADXL345
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Variables para manejar el estado de los botones
bool sonido1Reproducido = false;
bool sonido2Reproducido = false;
bool sonidoMovimientoReproducido = false;

void setup() {
  // Inicialización de los pines de los botones
  pinMode(boton1, INPUT_PULLUP); // Usa resistencia pull-up interna
  pinMode(boton2, INPUT_PULLUP); 

  // Inicialización del puerto serie para comunicación con DFPlayer
  mySerial.begin(9600);
  Serial.begin(115200);

  // Inicialización del DFPlayer
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("Error al inicializar el DFPlayer");
    while (true); // Detener ejecución si hay error
  }
  Serial.println("DFPlayer Mini inicializado correctamente.");
  myDFPlayer.volume(30);  // volumen de la bocina a 30% (max.)

  // Inicialización del Neopixel
  strip.begin();
  strip.show(); // Inicializa los LEDs como apagados

  // Inicialización del acelerómetro ADXL345
  if (!accel.begin()) {
    Serial.println("Error al inicializar el acelerómetro ADXL345.");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_4_G); // Configura la sensibilidad del acelerómetro
}

void loop() {
  // Revisa si el botón 1 está presionado
  if (digitalRead(boton1) == LOW && !sonido1Reproducido) { 
    Serial.println("Botón 1 presionado, reproduciendo pista 001 y encendiendo Neopixels con color aleatorio");
    
    encenderNeopixelsAleatorio();   // Enciende los Neopixels con color aleatorio
    myDFPlayer.play(1);    // Reproduce la pista 001
    sonido1Reproducido = true; // Marca que el sonido 001 ya fue reproducido
  }

  // Revisa si el botón 2 está presionado
  if (digitalRead(boton2) == LOW && !sonido2Reproducido) {
    Serial.println("Botón 2 presionado, reproduciendo pista 002 y apagando Neopixels");
    
    myDFPlayer.play(2);    // Reproduce la pista 002
    apagarNeopixels();     // Apaga los Neopixels en reversa
    sonido2Reproducido = true; // Marca que el sonido 002 ya fue reproducido
  }

  // Detecta movimiento con el acelerómetro
  sensors_event_t event;
  accel.getEvent(&event);
  
  // Verifica si hay movimiento en alguno de los ejes
  if ((abs(event.acceleration.x) > 1.5) || (abs(event.acceleration.y) > 1.5) || (abs(event.acceleration.z) > 1.5)) {
    if (!sonidoMovimientoReproducido) {
      Serial.println("Movimiento detectado, reproduciendo pista 004");
      myDFPlayer.play(4); // Reproduce la pista 004 cuando hay movimiento
      sonidoMovimientoReproducido = true; // Evita la reproducción múltiple
    }
  } else {
    sonidoMovimientoReproducido = false; // Permite que el sonido se vuelva a reproducir con nuevo movimiento
  }

  // Reiniciar el estado de los botones para futuras pulsaciones
  if (digitalRead(boton1) == HIGH) {
    sonido1Reproducido = false; // Permite que el sonido 001 se pueda reproducir de nuevo
  }
  if (digitalRead(boton2) == HIGH) {
    sonido2Reproducido = false; // Permite que el sonido 002 se pueda reproducir de nuevo
  }
}

// Función para encender los Neopixels con color aleatorio
void encenderNeopixelsAleatorio() {
  // Genera un color aleatorio
  uint8_t rojo = random(0, 256);  // Valor aleatorio entre 0 y 255
  uint8_t verde = random(0, 256); // Valor aleatorio entre 0 y 255
  uint8_t azul = random(0, 256);  // Valor aleatorio entre 0 y 255

  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(rojo, verde, azul)); // Color aleatorio
    strip.show();  // Actualiza el anillo de Neopixels
    delay(10);    // Retardo para encender cada LED de forma secuencial
  }
}

// Función para apagar los Neopixels en reversa
void apagarNeopixels() {
  for (int i = numPixels - 1; i >= 0; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Apagar LED
    strip.show();  // Actualiza el anillo de Neopixels
    delay(10);    // Retardo para apagar cada LED de forma secuencial
  }
}
