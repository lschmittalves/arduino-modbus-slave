#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include <LiquidCrystal.h>

// modbus control constant variables
const int ModbusNode = 1;
const int StatusErrorPin = LED_BUILTIN;
const int StatusLEDPin = 8;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// util variables declarations
char lcdMessage[16];
int actValueReaded;


void configureLeds()
{
  // configure the Status Leds
  pinMode(StatusErrorPin, OUTPUT);
  digitalWrite(StatusErrorPin, LOW);

  pinMode(StatusLEDPin, OUTPUT);
  digitalWrite(StatusLEDPin, HIGH);
}

void initLCD()
{
  lcd.begin(16, 2);
  lcd.print("Initing...");
}

void initModbusServer()
{
  // wait 1 secconds, because the user need to see the status ok led for a moment
  delay(1000);

  lcd.print("Modbus Init...");

  if (ModbusRTUServer.begin(ModbusNode, 9600))
  {
    lcd.print("Modbus Connected");
    delay(1000);
    return;
  }

  lcd.print("Init Failed!!!");

  // report visual error
  digitalWrite(StatusErrorPin, HIGH);
  digitalWrite(StatusLEDPin, LOW);

  // wait 3 secconds and try again
  delay(3000);

  lcd.print("Retrying...");

  // reset visual error
  digitalWrite(StatusErrorPin, LOW);
  digitalWrite(StatusLEDPin, HIGH);

  initModbusServer();
}

void configModbusRegisterArea()
{
  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 1);
}

void readModbusCoilAndPrint(int registerAddress)
{
  // read the current value of the coil
  actValueReaded = ModbusRTUServer.coilRead(registerAddress);

  if (actValueReaded)
  {
    sprintf(lcdMessage, "0x%X is TRUE", registerAddress);
  }
  else
  {
    sprintf(lcdMessage, "0x%X is False", registerAddress);
  }

  lcd.setCursor(0, 1);
  lcd.print(lcdMessage);
}

/*
######################
ARDUINO MAIN FUNCTIONS
######################
*/
void setup()
{

  configureLeds();
  initLCD();
  Serial.begin(9600);

  // start the Modbus RTU server, with (slave) id 1
  initModbusServer();
  configModbusRegisterArea();
}

void loop()
{
  // poll for Modbus RTU requests
  ModbusRTUServer.poll();

  readModbusCoilAndPrint(0x00);
}
