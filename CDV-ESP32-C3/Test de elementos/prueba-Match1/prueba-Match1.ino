const int pinSalida = 1;  // Cambia al pin que necesites
unsigned long lastTime = 0;  // Guarda el último tiempo de activación
const unsigned long interval = 3000;  

void setup() {
  Serial.begin(115200);  
  pinMode(pinSalida, OUTPUT);
  digitalWrite(pinSalida, LOW);  // inicia Low
}

void loop() {

  if (millis() - lastTime >= interval) {
    Serial.println("Activación automática");
    activarPin();
  }
}

void activarPin() {
  digitalWrite(pinSalida, HIGH);
  
  Serial.println("Pin en HIGH (3.3V)");
  delay(1000); 
  digitalWrite(pinSalida, LOW);
  Serial.println("Pin en LOW (0V)");
  lastTime = millis(); 
}