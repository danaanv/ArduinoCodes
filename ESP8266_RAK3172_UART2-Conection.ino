#include <SoftwareSerial.h>

// Pines UART para RAK3172
#define RAK_RX 13  // D7 - Conectar al TX del RAK3172 (UART2: PA2)
#define RAK_TX 2   // D4 - Conectar al RX del RAK3172 (UART2: PA3)

// Crear SoftwareSerial
SoftwareSerial rakSerial(RAK_RX, RAK_TX);

void setup() {
  // Serial para PC (Monitor Serial)
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n========================================");
  Serial.println("   Test Comunicación ESP8266 - RAK3172");
  Serial.println("========================================");
  Serial.println("Pines: D7(RX) - D4(TX)");
  Serial.println("Baudrate: 115200");
  Serial.println("========================================\n");
  
  // Serial para RAK3172
  rakSerial.begin(115200);
  delay(1000);
  
  // Enviar comando AT de prueba
  Serial.println("Enviando comando: AT\n");
  rakSerial.print("AT\r\n");  // Usar print() y agregar \r\n manualmente
  
  // Esperar respuesta
  delay(500);
  
  Serial.println("Respuesta del RAK3172:");
  Serial.println("---");
  
  unsigned long timeout = millis();
  while (millis() - timeout < 2000) {
    if (rakSerial.available()) {
      char c = rakSerial.read();
      Serial.print(c);
    }
  }
  
  Serial.println("\n---");
  Serial.println("\nPuedes escribir comandos AT en el monitor serial");
  Serial.println("Ejemplos:");
  Serial.println("  AT");
  Serial.println("  AT+VER=?");
  Serial.println("  AT+DEVEUI=?");
  Serial.println("  AT+BAND=?");
  Serial.println("  AT+NWM=?");
  Serial.println("========================================\n");
}

void loop() {
  // Leer desde PC y enviar al RAK3172
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    
    if (comando.length() > 0) {
      Serial.print("\n>> Enviando: ");
      Serial.println(comando);
      Serial.println("---");
      
      // Enviar con terminación correcta
      rakSerial.print(comando);
      rakSerial.print("\r\n");
      
      // Esperar y leer respuesta carácter por carácter
      delay(200);
      unsigned long timeout = millis();
      bool huboRespuesta = false;
      String respuesta = "";
      
      while (millis() - timeout < 2000) {
        if (rakSerial.available()) {
          char c = rakSerial.read();
          Serial.print(c);
          respuesta += c;
          huboRespuesta = true;
          timeout = millis(); // Resetear timeout si hay datos
        }
      }
      
      if (!huboRespuesta) {
        Serial.println("(Sin respuesta)");
      }
      
      Serial.println("---\n");
    }
  }
  
  // Leer desde RAK3172 y mostrar en PC (respuestas asíncronas)
  while (rakSerial.available()) {
    char c = rakSerial.read();
    Serial.print(c);
  }
}
