#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <task.h>
#include <semphr.h>

#define LED_VERDE 11
#define LED_AMARILLO 12
#define LED_ROJO 13
#define BOTON 2

SemaphoreHandle_t mutex;

volatile bool botonPresionado = false;

int estadoSemaforo=0;

void setup() {
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BOTON, INPUT_PULLUP);

  // Inicializar el mutex
  mutex = xSemaphoreCreateMutex();

  // Iniciar el sistema FreeRTOS

  attachInterrupt(digitalPinToInterrupt(BOTON), botonInterrupcion, FALLING);
  xTaskCreate(tareaSemaforo, "TareaSemaforo", 100, NULL, 1, NULL);
  xTaskCreate(tareaBoton, "TareaBoton", 100, NULL, 2, NULL);
  vTaskStartScheduler();
}

void loop() {
  // El loop principal no hace nada en este ejemplo
}

void prenderLed(int led) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(led, HIGH);
}
void apagarLed(int led) {
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARILLO, HIGH);
  digitalWrite(LED_ROJO, HIGH);
  digitalWrite(led, LOW);
}
void botonInterrupcion() {
  // Cambiar la variable de botón presionado en la interrupción
  botonPresionado = true;
}

void tareaSemaforo(void *parameter) {

  while (true) {
    // Tomar el mutex antes de cambiar los LEDs
    if (xSemaphoreTake(mutex, pdFALSE) == pdTRUE) {
      // Cambiar a verde
      if(estadoSemaforo==0){
        prenderLed(LED_AMARILLO);
        estadoSemaforo=1;
      }else if(estadoSemaforo==1){
        prenderLed(LED_ROJO);
        estadoSemaforo=2;
      }
      else {
        prenderLed(LED_VERDE);
        estadoSemaforo=0;
      }
      xSemaphoreGive(mutex);
      // Esperar un tiempo antes de reiniciar el ciclo
      delay(2000);
    }
  }
}
void tareaBoton(void *parameter) {
  while (true) {
    // Si el botón está presionado
    if (digitalRead(BOTON)) {
      // Tomar el mutex antes de cambiar los LEDs
      if (xSemaphoreTake(mutex, pdFALSE) == pdTRUE) {
        if(estadoSemaforo==0){
          apagarLed(LED_VERDE);
        }
        else if(estadoSemaforo==1){
          apagarLed(LED_AMARILLO);
        }
        else{
          apagarLed(LED_ROJO);
        }
        // Liberar el mutex después de cambiar los LEDs
        delay(2000);
        xSemaphoreGive(mutex);
      
      }
    }
    // Esperar un tiempo antes de volver a revisar el botón
    delay(50);
  }
}
