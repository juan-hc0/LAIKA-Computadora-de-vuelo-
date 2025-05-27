#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050.h>
#include <SPI.h>
#include <RF24.h>

// CE_PIN 10
// CSN_PIN 9
// SCK_PIN 4
// MISO_PIN 5
// MOSI_PIN 6

// Configuración del BME280
Adafruit_BME280 bme;
#define PRECIONNIVELDELMAR (1025)

// Configuración del MPU6050
Adafruit_MPU6050 mpu;  // Declaramos el objeto MPU6050

// Configuración del NRF24
RF24 radio(10, 9); // Pines CE y CSN
const byte address[6] = "00001"; // Dirección para comunicación

// Estructura para datos a enviar por RF
struct DataPackage {
  float tem;  // Temperatura
  float hum;  // Humedad
  float pre;  // Presión
  float alt;  // Altitud
  float roll, pitch, yaw; // Datos del MPU6050
  float accelZ; // Aceleraciones en m/s^2
};
DataPackage data;

// Variable para la referencia de altitud
float referenciaN = 0.0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  Serial.println("Inicializando...");

  // Configuración del BME280
  if (!bme.begin(0x76)) {
    Serial.println("No se encontró un BME280, verifica la conexión.");
    while (1);
  }
  Serial.println("BME280 detectado.");

  // Configuración del MPU6050
  if (!mpu.begin()) {  // Asegúrate de que el objeto 'mpu' esté inicializado correctamente
    Serial.println("No se pudo encontrar un MPU6050, verifica las conexiones.");
    while (1);
  }
  Serial.println("MPU6050 detectado.");

  // Configuración del NRF24
  Serial.println("Inicializando NRF24...");
  if (!radio.begin()) {
    Serial.println("Error al inicializar el NRF24.");
    while (1);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
  radio.setChannel(70);

  Serial.println("NRF24 inicializado.");
}

void loop() {
  // Leer datos del BME280
  data.tem = bme.readTemperature();
  data.hum = bme.readHumidity();
  data.pre = bme.readPressure() / 100.0F;
  data.alt = bme.readAltitude(PRECIONNIVELDELMAR) - referenciaN;

  // Leer datos del MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Cálculo del ángulo Roll (Eje X) en grados
  data.roll = (atan2(a.acceleration.y, a.acceleration.z) * 180 / PI);
  data.pitch = (atan2(a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI);
  data.yaw = 0.0; // El MPU6050 no proporciona yaw directamente, lo dejamos en 0 por ahora.
  data.accelZ = a.acceleration.z;

  // Mostrar datos en el monitor serial
  Serial.print("Temperatura: "); Serial.println(data.tem);
  Serial.print("Humedad: "); Serial.println(data.hum);
  Serial.print("Presión: "); Serial.println(data.pre);
  Serial.print("Altitud: "); Serial.println(data.alt);
  Serial.print("Roll: "); Serial.print(data.roll);
  Serial.print(", Pitch: "); Serial.print(data.pitch);
  Serial.print(", Yaw: "); Serial.println(data.yaw);

  // Enviar datos por RF24
  if (radio.write(&data, sizeof(DataPackage))) {
    Serial.println("Datos enviados correctamente.");
  } else {
    Serial.println("Error al enviar datos.");
  }

  delay(1000);
}

