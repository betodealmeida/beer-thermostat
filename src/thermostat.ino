/*
 Beer thermostat for brewing.
 */

#include <PID_v1.h>
#include <OneWire.h>
#include <LiquidCrystal.h>
#include <DallasTemperature.h>

// sensor pin
#define ONE_WIRE_BUS 6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// LCD init
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int backLight = 13;

// PID relay control
#define RELAY_PIN 7
double setPoint;  // desired temperature
double input;     // current temperature
double output;    // for PID
PID myPID(&input, &output, &setPoint, 2, 5, 1, REVERSE);
int windowSize = 5*60*1000;  // 5 minutes in ms
unsigned long windowStartTime;

// pot
#define POT_PIN A0
double minTemperature = 7.;
double maxTemperature = 22.;

void setup() {
    lcd.begin(16, 2);
    pinMode(backLight, OUTPUT);
    digitalWrite(backLight, HIGH);
    sensors.begin();
    windowStartTime = millis();

    // store initial temperature 
    setPoint = getSetPoint();
    input = getInput();

    // set PID
    myPID.SetOutputLimits(0, windowSize);
    myPID.SetMode(AUTOMATIC);

    // draw LCD
    draw();
}

void loop() {
    double temp;
    bool redraw = false;

    // check target temperature
    temp = getSetPoint();
    if (temp != setPoint) {
        setPoint = temp;
        redraw = true;
    }

    // check current temperature
    temp = getInput();
    if (temp != input) {
        input = temp;
        redraw = true;
    }

    if (redraw) draw();

    // do PID
    myPID.Compute();
    unsigned long now = millis();
    if (now - windowStartTime > windowSize) {
        windowStartTime += windowSize;
    }
    if (output > now - windowStartTime) {
        //digitalWrite(RelayPin,HIGH);
        lcd.setCursor(15, 0);
        lcd.print("*");
    } else {
        //digitalWrite(RelayPin,LOW);
        lcd.setCursor(15, 0);
        lcd.print(" ");
    }
}

void draw() {
    lcd.clear();

    // current temperature
    lcd.setCursor(0, 0);
    lcd.print("now:");
    lcd.setCursor(5, 0);
    lcd.print(int(input), DEC);
    lcd.print("C");

    // target temperature
    lcd.setCursor(0, 1);
    lcd.print("set:");
    lcd.setCursor(5, 1);
    lcd.print(int(setPoint), DEC);
    lcd.print("C");

    // append "ale" or "lager"
    if ((setPoint >= 7) && (setPoint <= 9+1)) {
        lcd.print("  (lager)");
    } else if ((setPoint >= 10) && (setPoint < 13+1)) {
        lcd.print(" (lager)");
    } else if ((setPoint >= 20) && (setPoint < 22+1)) {
        lcd.print(" (ale)");
    }
}

double getSetPoint() {
    double value = analogRead(POT_PIN);
    double offset = minTemperature;
    double delta = 1 + maxTemperature - minTemperature;
    return offset + delta * value / 1024.;
}

double getInput() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

