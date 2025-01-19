# ESP32 Air Quality Monitoring System
This project utilizes an ESP32 microcontroller to monitor air quality and environmental parameters such as temperature and humidity. The data collected from sensors is sent to the Arduino IoT Cloud for real-time monitoring and logging.

## Features
  - Measures **temperature** and **humidity** using the **DHT22 sensor**.
  - Monitors **air quality** using an **MQ135 gas sensor**.
  - Implements **FreeRTOS** tasks for efficient multitasking.
  - Sends real-time data to the **Arduino IoT Cloud** for remote monitoring.
  - Ensures data consistency and safety using a **mutex (semaphore)** to prevent conflicts in shared resources.

## Components Used
  - **ESP32 Microcontroller:** The central processing unit for the system.
  - **DHT22 Sensor:** For measuring temperature and humidity.
  - **MQ135 Gas Sensor:** For measuring air quality (detecting gases like CO2, ammonia, benzene, etc.).


## How It Works
### 1. Sensor Readings
  - The DHT22 sensor collects temperature and humidity data.
  - The MQ135 sensor detects air quality and provides a gas concentration value.

### 2. FreeRTOS Tasks
  - SensorTask: Reads temperature and humidity values.
  - AirQualityTask: Reads gas concentration values from the MQ135 sensor.
  - CloudTask: Updates the Arduino IoT Cloud with the latest sensor readings.

### 3. Cloud Integration
  - Data is uploaded to the Arduino IoT Cloud for real-time visualization and remote access.

## Circuit Diagram
Connect the components as follows:
  - **DHT22 Sensor**
      - VCC → 3.3V
      - GND → GND
      - Data → GPIO 4

  - **MQ135 Gas Sensor**
      - VCC → 5V
      - GND → GND
      - Analog Out → GPIO 34

## Installation and Setup
### 1. Hardware Setup
  - Connect the sensors to the ESP32 as per the circuit diagram.

### 2. Software Requirements
  - Install the Arduino IDE.
  - Install the following libraries via the Arduino Library Manager:
      - DHT sensor library
      - ArduinoIoTCloud

### 3. Code Deployment
  - Clone this repository.
  - Open the project in the Arduino IDE.
  - Configure your Wi-Fi credentials and Arduino IoT Cloud settings in the **thingProperties.h** file.
  - Upload the code to the ESP32 board.

### 4. Monitor Data
  - Open the Arduino IoT Cloud dashboard to monitor sensor readings in real-time.


## FreeRTOS Task Prioritization
  - **SensorTask:** Priority 2, runs on Core 1, updates every 2 seconds.
  - **AirQualityTask:** Priority 2, runs on Core 1, updates every 3 seconds.
  - **CloudTask:** Priority 1, runs on Core 0, updates every 1 second.


## Example Output
  - Serial Monitor Output
    ```
    Humidity: 45.67, Temperature: 25.89
    Gas Value: 345
    Message Sent to Cloud: Temp: 25.89, Hum: 45.67, Air Quality: 345
    ```


## Future Enhancements
  - Add support for more gas sensors to monitor specific pollutants.
  - Implement data logging for long-term environmental monitoring.
  - Include alert mechanisms for thresholds.


## License
This project is licensed under the MIT License.
