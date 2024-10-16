#include <SoftwareSerial.h>
#include <EEPROM.h>

#define CURRENT_SENSOR_PIN 2
#define WCS1800_SENSITIVITY 0.066

#define VOLTAGE_SENSOR_PIN 6
#define ADC_RESOLUTION 4095
#define VREF 3.3

#define R1 16000000 // 16 M
#define R2 1000000  // 1 M

#define TX_PIN 8
#define RX_PIN -1
#define CONTROL_PIN 10

#define CURRENT_SENSOR_PIN x // Falta definir
#define EEPROM_ADDRESS 0
#define SAMPLING_INTERVAL 100
#define TIME_INTERVAL_HOURS 3600

float totalAh = 0.0;

unsigned long previousMillis = 0;

SoftwareSerial softSerial(TX_PIN, RX_PIN);

void setup()
{
    Serial.begin(115200);
    softSerial.begin(9600);
    EEPROM.begin(512);
    EEPROM.get(EEPROM_ADDRESS, totalAh);
    Serial.println(totalAh);
    pinMode(CONTROL_PIN, OUTPUT);
    digitalWrite(CONTROL_PIN, HIGH);
}

void loop()
{
    String message = string(totalAh) + " A/h&" + String(readVoltage) +" v&" + String(ReadSpeed) + " Km/h#";
    softSerial.print(message);
    Serial.println(message);
    delay(100);
}

float ReadCurrent()
{
    int adcValue = analogRead(CURRENT_SENSOR_PIN);
    float voltage = (adcValue * VREF) / ADC_RESOLUTION;
    float current = (voltage - (VREF / 2)) / WCS1800_SENSITIVITY;
    CalculateAh(current);
    return current;
}
float readVoltage()
{
    int adcValue = analogRead(VOLTAGE_PIN);
    float vOut = (adcValue * VREF) / ADC_RESOLUTION;
    float vIn = vOut * (1 + (float(R1) / R2));
    return vIn;
}
void CalculateAh(float current)
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLING_INTERVAL)
    {
        previousMillis = currentMillis;
        totalAh += (current * (SAMPLING_INTERVAL / 3600000.0));
        EEPROM.put(EEPROM_ADDRESS, totalAh);
        EEPROM.commit();
        return totalAh;
    }
}
float ReadSpeed()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastSampleTime >= SAMPLE_TIME)
    {
        lastSampleTime = currentMillis;
        speed = (CIRCUMFERENCE / (SAMPLE_TIME / 1000.0)) * pulseCount;
        pulseCount = 0;
        float SpeedInKm = speed * 3.6;
        return SpeedInKm;
    }
}