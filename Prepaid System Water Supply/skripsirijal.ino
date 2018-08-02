#include <SPI.h>
#include <SD.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define sensorInterrupt 0  // 0 = digital pin 2
#define sensorPin       2
#define relayPin        A0
#define buzzerPin       A1

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9, 8, 3, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 5, 4}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C lcd(0x27, 16, 2);

File myFile;

float calibrationFactor = 4.5;
String inputCode;
int waterIn;
int waterOut;
int waterpool;
double waterpoolLitre;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

int dekripsiKode(String code){
  return code.substring(11,15).toInt();
}

void setup() {
  Serial.begin(9600);
  sd_init();
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

  waterIn=0;
  waterOut=0;
  waterpool=0;
  lcd.begin();
  lcd.backlight();
  lcd.print("Welcome to");
  lcd.setCursor(0,1);
  lcd.print("WaterMeter");
  delay(500);
  lcd.clear();
  waterpoolWrite(0);
  delay(1000);
}

void loop() {
  waterpool = waterpoolRead().toInt();
  delay(500);
  SD.remove("test.txt");
  delay(500);
  char key = keypad.getKey();
  if(key=='*'){
    digitalWrite(buzzerPin, LOW);
    lcd.clear();
    delay(200);
    lcd.print("code:");
    lcd.setCursor(0,1);
    while(1){
      char x = keypad.getKey();
      if(x){
        if(x!='#'){
          lcd.print(x);
          inputCode+=x;
        }
        if(x=='#'){
          break;
        }
      }
    }
  }
  waterIn = dekripsiKode(inputCode);
  if(waterIn>0){
    waterpool+=waterIn;
    inputCode="";
  }
  waterOut = getSensorData();
  waterpool-=waterOut;
  if(waterpool<=0){
    waterpool=0;
  }
  waterpoolWrite(waterpool);
  delay(500);
  waterpoolLitre = waterpool/1000.00;
  if(waterpool<1000){
    digitalWrite(buzzerPin, !digitalRead(buzzerPin));
  }
  else{
    digitalWrite(buzzerPin, LOW);
  }
  if(waterpool<=0){
    digitalWrite(relayPin, HIGH);
  }
  else{
    digitalWrite(relayPin, LOW);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Saldo Air:");
  lcd.setCursor(0,1);
  lcd.print(waterpoolLitre,3);
  lcd.setCursor(10,1);
  lcd.print("Litres");
  delay(1000);
}
