/*
Sobre los botones:
0 = leva inferior izq
1 = leva inferior der
2 = leva superior izq
3 = leva superior der
4 = Boton superior izq
5 = Boton superior der

*/

#include "HardwareSerial.h"
#include <Wire.h>
#include <string.h>
#include <SoftwareSerial.h>

#define MCP23008_ADDRESS 0x20
#define IODIR_REGISTER 0x00
#define GPIO_REGISTER 0x09

#define RX_PIN 9
#define TX_PIN -1
#define CONTROL_PIN 10

HardwareSerial nextion(2);

SoftwareSerial softSerial(TX_PIN, RX_PIN);

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
    softSerial.begin(9600);
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
    Readbotones();
    ReadTrasera();
    HandleBotones();
    ReadMax485();
    setScreen();

}
void setScreen(){
    switch (menu)
    {
    case 0:
        sendCommand("page Principal");
        sendCommand("t3", RapidezIdeal);
        sendCommand("n0", Rapidez);
        sendCommand("t5", Tension);
        sendCommand("t6", Consumo);
        setSliderValue("j1", sliderValue);
        break;

    case 1:
        sendCommand("page Info");
        sendCommand("n0", Rapidez);
        sendCommand("t5", Tension);
        sendCommand("t6", Consumo);
        sendCommand("t9", Vueltas);
        sendCommand("t8", lapTime);
        sendCommand("t12", PorcentajeBateria);
        setSliderValue("j1", sliderValue);

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
    if (gpioState & (1 << x)) // Cambiar de Menu //Remplazar 'x' por el numerp de boton
    {
        if (menu == 0)
            menu = 1;
        if (menu == 1)
            menu = 0;
    }
    else if (gpioState & (1 << x)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        startStopwatch();
    }
    else if (gpioState & (1 << x)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        recordLap();
    }
    else if (gpioState & (1 << x) && gpioState & (1 << y)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
    {
        resetStopwatch();
    }
    else if (gpioState & (1 << x) && gpioState & (1 << y)) // Iniciar Timer //Remplazar 'x' por el numerp de boton
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
        printTime(currentLapTime);
    }
}

void SendTime(unsigned long time)
{
    unsigned long minutes = (time / 60000) % 60;
    unsigned long seconds = (time / 1000) % 60;
    unsigned long milliseconds = time % 1000;
    String timeString = String(minutes) + ":" + String(seconds).padStart(2, '0') + ":" + String(milliseconds).padStart(3, '0');
}

void ReadMax485()
{
    const char delimiter[] = "&";
    if (softSerial.available())
    {
        String message = softSerial.readStringUntil('#'); 
        Serial.println(message);
        char msg[message.length() + 1]; 
        message.toCharArray(msg, sizeof(msg));

        Tension = strtok(msg, delimiter); 
        Consumo = strtok(NULL, delimiter);
        Rapidez = strtok(NULL, delimiter); 

        //VVVVV A borrar Una vez que funciona VVVV
        if (Tension != NULL) {
            Serial.print("Tension: ");
            Serial.println(Tension);
        }
        if (Consumo != NULL) {
            Serial.print("Consumo: ");
            Serial.println(Consumo);
        }
        if (Speed != NULL) {
            Serial.print("Speed: ");
            Serial.println(Speed);
        }
        // ^^^^^^^^^^^^^^^^^
    }
}

