#include <SPI.h>
#include <SD.h>

// Definir los pines SPI para ESP32-C3
#define SCK_PIN 4
#define MOSI_PIN 6
#define MISO_PIN 5
#define CS_PIN 10 

void setup() {
  // Iniciar la comunicación serie
  Serial.begin(115200);
  while (!Serial) {
    ; // Espera a que se conecte el puerto serie. Necesario solo para el puerto USB nativo.
  }

  // Configurar pines SPI
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  // Inicializa la tarjeta SD
  if (!SD.begin(CS_PIN)) {
    Serial.println("Error de inicialización. Cosas que comprobar:");
  
    while (1); // Detener ejecución si hay un error
  }
  Serial.println("Inicialización realizada.");

  // Comprobar si se puede abrir un archivo
  File testFile = SD.open("/prueba.txt", FILE_WRITE);
  if (testFile) {
    Serial.println("test.txt abierto correctamente para escribir.");
    testFile.println("Likosos");
    testFile.close();
    Serial.println("Se escribió correctamente en test.txt.");
  } else {
    Serial.println("Error al abrir test.txt para escribir.");
  }
}

void loop() {
  //
}
