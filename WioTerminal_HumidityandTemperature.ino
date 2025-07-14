#include <Seeed_Arduino_FreeRTOS.h>
#include <TFT_eSPI.h>
#include <DHT.h>

// --- Definiciones de sensores ---
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MOISTURE_PIN A1

// --- Manejadores de tareas ---
TaskHandle_t Handle_aTask;
TaskHandle_t Handle_bTask;
TaskHandle_t Handle_monitorTask;

// --- TFT y Sprites ---
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite img2 = TFT_eSprite(&tft);

// --- Tarea A: DHT11 ---
static void LCD_TASK_1 (void* pvParameters) {
    Serial.println("Thread A: Started");
    img.createSprite(100, 100);
    img.setTextSize(2);
    img.setTextColor(TFT_WHITE);

    for (;;) {
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();

        img.fillSprite(tft.color565(229,58,64)); // rojo
        if (isnan(temp) || isnan(hum)) {
            img.drawString("Err", 10, 25);
        } else {
            img.drawString("T:" + String(temp, 1) + "C", 10, 10);
            img.drawString("H:" + String(hum, 1) + "%", 10, 40);
        }
        img.pushSprite(30, 70);
        delay(2000);
    }
}

// --- Tarea B: Sensor de humedad VH400 ---
static void LCD_TASK_2 (void* pvParameters) {
    Serial.println("Thread B: Started");
    img2.createSprite(100, 100);
    img2.setTextSize(2);
    img2.setTextColor(TFT_WHITE);

    for (;;) {
        int moistureRaw = analogRead(MOISTURE_PIN);
        int percentage = map(moistureRaw, 1023, 300, 0, 100); // calibración estimada

        img2.fillSprite(tft.color565(48,179,222)); // azul
        img2.drawString("Soil:", 10, 10);
        img2.drawString(String(percentage) + "%", 10, 40);
        img2.pushSprite(190, 70);
        delay(2000);
    }
}

// --- Tarea Monitor ---
void taskMonitor(void* pvParameters) {
    int measurement;

    Serial.println("Task Monitor: Started");

    for (int x = 0; x < 10; ++x) {
        Serial.println("");
        Serial.println("******************************");
        Serial.println("[Stacks Free Bytes Remaining]");

        measurement = uxTaskGetStackHighWaterMark(Handle_aTask);
        Serial.print("Thread A: ");
        Serial.println(measurement);

        measurement = uxTaskGetStackHighWaterMark(Handle_bTask);
        Serial.print("Thread B: ");
        Serial.println(measurement);

        measurement = uxTaskGetStackHighWaterMark(Handle_monitorTask);
        Serial.print("Monitor Stack: ");
        Serial.println(measurement);

        Serial.println("******************************");

        delay(10000); // cada 10 segundos
    }

    Serial.println("Task Monitor: Deleting");
    vTaskDelete(NULL);
}

// --- Setup inicial ---
void setup() {
    Serial.begin(115200);
    vNopDelayMS(1000);
    while (!Serial);

    dht.begin();

    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(tft.color565(9,7,7)); // negro
    tft.setTextColor(tft.color565(239,220,5)); // amarillo
    tft.setTextSize(2);
    tft.drawString("Temp & Hum", 30, 50);
    tft.drawString("Moisture", 190, 50);

    Serial.println("");
    Serial.println("******************************");
    Serial.println("        Program start         ");
    Serial.println("******************************");

    // Crear tareas con prioridad y stack
    xTaskCreate(LCD_TASK_1, "Task A", 256, NULL, tskIDLE_PRIORITY + 3, &Handle_aTask);
    xTaskCreate(LCD_TASK_2, "Task B", 256, NULL, tskIDLE_PRIORITY + 2, &Handle_bTask);
    xTaskCreate(taskMonitor, "Task Monitor", 128, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

    vTaskStartScheduler();
}

void loop() {
    // No usar loop en FreeRTOS
}
