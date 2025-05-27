#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>
#include <ESP32Servo.h>

// Pines SPI para ESP32-C3
#define SCK_PIN 4
#define MOSI_PIN 6
#define MISO_PIN 5
#define CS_PIN 10 

//Cofiguracion paraServo
Servo myServo;
int pinServo = 0;

unsigned long tiempoInicio = 0;
unsigned long tiempoativacion = 0;
const unsigned long intervalo = 100;
unsigned long delayTime;
//const int pinSalida = 1;

int REF = 0;
bool guardado = false;
bool subiendo = false;
bool bajando = false;
float AltMax = 0;

Adafruit_MPU6050 mpu;
#define BME280_I2C_ADDRESS 0x76
Adafruit_BME280 bme;
#define PRECIONNIVELDELMAR (1025) //Presion CDMX

File dataFile;

void setup() {
  Serial.begin(115200);
  //pinMode(pinSalida, OUTPUT);
  //digitalWrite(pinSalida, LOW);

  myServo.attach(pinServo);

  while (!Serial);

  tiempoInicio = millis();
  Wire.begin(8, 7);

  if (!bme.begin(BME280_I2C_ADDRESS)) {
    Serial.println("Error: No se encontró el sensor BME280.");
    while (1);
  }
  Serial.println("BME280 detectado correctamente.");

   // Configuración (Indoor Navigation)
  Serial.println("-- Configuración de navegación interior (prioridad: altitud) --");
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X2,   // Temperatura
                  Adafruit_BME280::SAMPLING_X16,  // Presión (precisión alta)
                  Adafruit_BME280::SAMPLING_NONE,   // Humedad anulada
                  Adafruit_BME280::FILTER_X16,    // filtro Suavizado
                  Adafruit_BME280::STANDBY_MS_0_5);

  if (!mpu.begin()) {
    Serial.println("MPU no detectado");
    while (1);
  }
  Serial.println("MPU detectado");

  // Inicializar SD
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  if (!SD.begin(CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta SD.");
    while (1);
  }
  Serial.println("Tarjeta SD inicializada.");

  // Crear archivo
  dataFile = SD.open("/datosVuelo.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Tiempo(ms),Altura(m),AltMax(m),Pitch(grados),AccelX(m/s2),Activación(m)");
    dataFile.close();
  }
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float pitch = atan2(a.acceleration.x, a.acceleration.z) * 180 / PI;
  float accelX = a.acceleration.x;

  float alt = bme.readAltitude(PRECIONNIVELDELMAR); //donde se obtine la altura 
  float NuevaAltura = alt - REF; 
  float Altactivacion = 0;

  if (!guardado && millis() - tiempoInicio >= intervalo) {
    REF = alt;
    AltMax = 0;
    guardado = true;
    myServo.write(0);
    //digitalWrite(pinSalida,LOW);
  }

  if (NuevaAltura >= -100 && NuevaAltura <= 2) {
    subiendo = false;
    bajando = false;
    AltMax = 4;
  } else {
    if (NuevaAltura > AltMax) {
      subiendo = true;
      bajando = false;
      AltMax = NuevaAltura;
    } else if (subiendo && NuevaAltura < AltMax) {
      bajando = true;
      subiendo = false;
    }
    //Actvacion de mecha
    if (bajando && NuevaAltura < (AltMax - 1)) {
      myServo.write(150);
      //digitalWrite(pinSalida, HIGH);
      //delay(3000);
      //digitalWrite(pinSalida, LOW);
      bajando = false; // 
      Altactivacion = NuevaAltura; //momento en que se activa la mecha
    }
  }

   // Mostrar en serial
  //Serial.print("AltMax: "); 
  //Serial.println(AltMax);
  //Serial.print("Altura restablecida: "); 
  Serial.print(NuevaAltura);
  Serial.print(",");
  Serial.println(Altactivacion);
  //Serial.print("Pitch: "); Serial.println(pitch);
  //Serial.print("AccelX: "); Serial.println(accelX);

  // Guardar en SD
  dataFile = SD.open("/datosVuelo.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(NuevaAltura);
    dataFile.print(",");
    dataFile.print(AltMax);
    dataFile.print(",");
    dataFile.print(pitch);
    dataFile.print(",");
    dataFile.print(accelX);
    dataFile.print(",");
    dataFile.println(Altactivacion);
    dataFile.close();
    
  }

  delay(intervalo);
}
