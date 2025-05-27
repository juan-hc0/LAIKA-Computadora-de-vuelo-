#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_I2C_ADDRESS 0x76  // Dirección común del sensor
#define PRECIONNIVELDELMAR (1025.0) // hPa, ajusta según tu localidad (CDMX en tu caso)

Adafruit_BME280 bme;
unsigned long delayTime;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Espera a que el monitor serial esté listo (importante para algunas placas)
  
  Serial.println(F("Lectura de altitud con BME280"));
  
  Wire.begin(8, 7);
  if (!bme.begin(BME280_I2C_ADDRESS)) {
    Serial.println("Error: No se encontró el sensor BME280.");
    while (1); // Detiene el programa
  }

  Serial.println("BME280 detectado correctamente.");

  // Configuración para priorizar altitud (Indoor Navigation)
  Serial.println("-- Configuración de navegación interior (prioridad: altitud) --");
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X2,   // Temperatura
                  Adafruit_BME280::SAMPLING_X16,  // Presión (precisión alta)
                  Adafruit_BME280::SAMPLING_NONE,   // Humedad
                  Adafruit_BME280::FILTER_X16,    // Suavizado
                  Adafruit_BME280::STANDBY_MS_0_5);

  delayTime = 100; // Una lectura por segundo (puedes reducirlo si quieres más frecuencia)
}

void loop() {
  //Serial.print("Altitud: ");
  Serial.println(bme.readAltitude(PRECIONNIVELDELMAR));
  //Serial.println(" m");

  delay(delayTime);
}

