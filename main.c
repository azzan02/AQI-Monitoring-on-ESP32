#include "thingProperties.h"
#include "DHT.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define DHTpin 4
#define DHTTYPE DHT22
#define AIR_QUALITY_PIN 34

DHT dht(DHTpin, DHTTYPE);

// Task handles
TaskHandle_t SensorTaskHandle = NULL;
TaskHandle_t AirQualityTaskHandle = NULL;
TaskHandle_t CloudTaskHandle = NULL;

// Mutex for protecting shared resources
SemaphoreHandle_t dataAccessMutex;

void setup() {
  Serial.begin(9600);
  delay(1500);
  
  // Initialize mutex
  dataAccessMutex = xSemaphoreCreateMutex();
  if (dataAccessMutex == NULL) {
    Serial.println("Error creating mutex");
    return;
  }
  
  dht.begin();
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();

  // Create tasks with appropriate stack sizes and priorities
  xTaskCreatePinnedToCore(
    sensorTask,
    "SensorTask",
    4096,
    NULL,
    2,  // Higher priority for sensor reading
    &SensorTaskHandle,
    1    // Run on core 1
  );

  xTaskCreatePinnedToCore(
    airQualityTask,
    "AirQualityTask",
    2048,
    NULL,
    2,  // Higher priority for sensor reading
    &AirQualityTaskHandle,
    1    // Run on core 1
  );

  xTaskCreatePinnedToCore(
    cloudTask,
    "CloudTask",
    8192,  // Increased stack size
    NULL,
    1,    // Lower priority for cloud communication
    &CloudTaskHandle,
    0     // Run on core 0
  );
}

void loop() {
  // Empty loop as we're using FreeRTOS tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void sensorTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(2000);

  while (true) {
    float hm = dht.readHumidity();
    float temp = dht.readTemperature();

    if (!isnan(hm) && !isnan(temp)) {
      if (xSemaphoreTake(dataAccessMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        humidity = hm;
        temperature = temp;
        xSemaphoreGive(dataAccessMutex);
        Serial.printf("Humidity: %.2f, Temperature: %.2f\n", (float)humidity, (float)temperature);
      }
    } else {
      Serial.println("Failed to read from DHT sensor!");
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void airQualityTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(3000);

  while (true) {
    int airqualityvalue = analogRead(AIR_QUALITY_PIN);
    
    if (xSemaphoreTake(dataAccessMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      airquality = airqualityvalue;
      xSemaphoreGive(dataAccessMutex);
      Serial.printf("Gas Value: %d\n", (int)airquality);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void cloudTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000);
  
  static char msgBuffer[256]; // Increased buffer size
  float temp_copy, hum_copy;
  int air_copy;

  while (true) {
    // Get copies of the sensor data
    if (xSemaphoreTake(dataAccessMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      temp_copy = temperature;
      hum_copy = humidity;
      air_copy = airquality;
      xSemaphoreGive(dataAccessMutex);
    }

    // Update Arduino IoT Cloud
    ArduinoCloud.update();

    // Format and send message
    snprintf(msgBuffer, sizeof(msgBuffer), 
             "Temp: %.2f, Hum: %.2f, Air Quality: %d", 
             temp_copy, hum_copy, air_copy);
    message = msgBuffer;
    
    Serial.println("Message Sent to Cloud: " + message);

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

// Callback functions
void onMessageChange() {}
void onAirqualityChange() {}
void onHumidityChange() {}
void onTemperatureChange() {}
