/*
Sobre los botones:
0 = leva inferior izq
1 = leva inferior der
2 = leva superior izq
3 = leva superior der
4 = Boton superior izq
5 = Boton superior der

*/

#include <Arduino.h>
#include "HardwareSerial.h"
#include <Wire.h>
#include <string.h>
#include <SoftwareSerial.h>

#define MCP23008_ADDRESS 0x20
#define IODIR_REGISTER 0x00
#define GPIO_REGISTER 0x09

#define DE_RE_PIN 0
#define RX_PIN 9
#define TX_PIN 10

HardwareSerial nextion(2);

int menu = 0; // 0=principal, 1=info, 2=config
String Rapidez;
String RapidezIdeal;
String Tension;
String Consumo;
int sliderValue = 0; // 0=min -- 100=max
int Vueltas = 0;
int PorcentajeBateria;
bool botones[6] = {false};
uint8_t gpioState;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long lapTime = 0;

bool running = false;

void setup()
{
    Serial.begin(115200);
    Serial1.begin(2400, SERIAL_8N1, RX_PIN, TX_PIN);
    pinMode(DE_RE_PIN, OUTPUT);
    digitalWrite(DE_RE_PIN, LOW);

    Wire.begin(21, 22);

    pinMode(CONTROL_PIN, OUTPUT);
    digitalWrite(CONTROL_PIN, LOW);

    analogReadResolution(12);

    Wire.beginTransmission(MCP23008_ADDRESS);
    Wire.write(IODIR_REGISTER);
    Wire.write(0xFF);
    Wire.endTransmission();

    nextion.begin(9600, SERIAL_8N1, 16, 17);

    sendCommand("page Principal");

    delay(100);
}

void loop()
{
    unsigned long currentTime = millis() - startTime; // a ver
    ReadBotones();
    HandleBotones();
    ReadMax485();
    setScreen();
}
void setScreen()
{
    switch (menu)
    {
    case 0:
        sendCommand("page Principal");
        sendCommand(String("t3 " + RapidezIdeal).c_str());
        sendCommand(String("n0 " + Rapidez).c_str());
        sendCommand(String("t5 " + Tension).c_str());
        sendCommand(String("t6 " + Consumo).c_str());
        setSliderValue(String("j1 " + sliderValue).c_str(), sliderValue);
        break;

    case 1:
        sendCommand("page Info");
        sendCommand(String("n0 " + Rapidez).c_str());
        sendCommand(String("t5 " + Tension).c_str());
        sendCommand(String("t6 " + Consumo).c_str());
        sendCommand(String("t9 " + Vueltas).c_str());
        sendCommand(String("t8 " + lapTime).c_str());
        sendCommand(String("t12 " + PorcentajeBateria).c_str());
        setSliderValue(String("j1 " + sliderValue).c_str(), sliderValue);

        break;

    case 2:
        sendCommand("page Config");
        break;

    default:
        sendCommand("page Principal");
        break;
    }
}
void sendCommand(const char *cmd)
{
    nextion.print(cmd);
    nextion.write(0xFF);
    nextion.write(0xFF);
    nextion.write(0xFF);
}

void setSliderValue(const char *component, int value)
{
    String command = String(component) + ".val=" + String(value);
    sendCommand(command.c_str());
}

void ReadBotones()
{
    Wire.beginTransmission(MCP23008_ADDRESS);
    Wire.write(GPIO_REGISTER);
    Wire.endTransmission();

    Wire.requestFrom(MCP23008_ADDRESS, 1);

    if (Wire.available())
    {
        gpioState = Wire.read();
        Serial.print("GPIO State: ");
        Serial.println(gpioState, BIN);
    }
}
void HandleBotones()
{
    // FALTA DEFINIR BOTONES
    // FALTA DEFINIR BOTONES
    // FALTA DEFINIR BOTONES
    // FALTA DEFINIR BOTONES
    if (gpioState & (1 << 0)) // Cambiar de Menu //Remplazar 'x' por el numerp de boton
    {
        if (menu == 0)
            menu = 1;
        if (menu == 1)
            menu = 0;
    }
    else if (gpioState & (1 << 0)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        startStopwatch();
    }
    else if (gpioState & (1 << 0)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        recordLap();
    }
    else if (gpioState & (1 << 0) && gpioState & (1 << 0)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        resetStopwatch();
    }
    else if (gpioState & (1 << 0) && gpioState & (1 << 0)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        stopStopwatch();
    }
}

void startStopwatch()
{
    Vueltas = 0;
    if (!running)
    {
        startTime = millis() - elapsedTime;
        running = true;
        Serial.println("Stopwatch started.");
    }
}

void stopStopwatch()
{
    if (running)
    {
        elapsedTime = millis() - startTime;
        running = false;
        Serial.println("Stopwatch stopped.");
    }
}

void resetStopwatch()
{
    startTime = 0;
    elapsedTime = 0;
    lapTime = 0;
    running = false;
    Serial.println("Stopwatch reset.");
}

void recordLap()
{
    Vueltas += 1;
    if (running)
    {
        unsigned long currentLapTime = millis() - startTime - lapTime;
        lapTime = millis() - startTime;
        Serial.print("Lap Time: ");
        Serial.println(currentLapTime);
    }
}

void SendTime(unsigned long time)
{
    unsigned long minutes = (time / 60000) % 60;
    unsigned long seconds = (time / 1000) % 60;
    unsigned long milliseconds = time % 1000;
    String timeString = String(minutes) + ":" + String(seconds) + ":" + String(milliseconds, 1);
}

void ReadMax485()
{
    const char delimiter[] = "&";
    if (Serial1.available())
    {

        String message = Serial1.readStringUntil('#');

        Serial.println(message);
        char msg[message.length() + 1];
        message.toCharArray(msg, sizeof(msg));

        Tension = strtok(msg, delimiter);
        Consumo = strtok(NULL, delimiter);
        Rapidez = strtok(NULL, delimiter);

        // VVVVV A borrar Una vez que funciona VVVV
        if (Tension != NULL)
        {
            Serial.print("Tension: ");
            Serial.println(Tension);
        }
        if (Consumo != NULL)
        {
            Serial.print("Consumo: ");
            Serial.println(Consumo);
        }
        if (Rapidez != NULL)
        {
            Serial.print("Speed: ");
            Serial.println(Rapidez);
        }
        // ^^^^^^^^^^^^^^^^^

    }
}
