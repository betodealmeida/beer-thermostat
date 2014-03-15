/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include <DHT.h>

// sensor pin
#define DHTPIN 6
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

float minTemperature = 7.;
float maxTemperature = 22.;
int threshold = 1;
int targetTemperature;
int currentTemperature;
int currentHumidity;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
    dht.begin();

    // set up the LCD's number of columns and rows: 
    lcd.begin(16, 2);

    // store initial temperature 
    targetTemperature = getTemperature(analogRead(A0));
    currentTemperature = int(dht.readTemperature());
    currentHumidity = int(dht.readHumidity());
    draw();
}

void loop() {
    int temperature, humidity;
    bool redraw = false;

    // check target temperature
    temperature = getTemperature(analogRead(A0));
    if (temperature != targetTemperature) {
        targetTemperature = temperature;
        redraw = true;
    }

    // check current temperature
    temperature = int(dht.readTemperature());
    if (temperature != currentTemperature) {
        currentTemperature = temperature;
        redraw = true;
    }

    // check current humidity;
    humidity = int(dht.readHumidity());
    if (humidity != currentHumidity) {
        currentHumidity = humidity;
        redraw = true;
    }

    if (redraw) draw();

    // turn on/off
}

void draw() {
    lcd.clear();

    // current temperature
    lcd.setCursor(0, 0);
    lcd.print("now:");
    lcd.setCursor(5, 0);
    lcd.print(currentTemperature, DEC);
    lcd.print("C");

    // current humidity
    lcd.print(" / ");
    lcd.print(currentHumidity, DEC);
    lcd.print("%");

    // target temperature
    lcd.setCursor(0, 1);
    lcd.print("set:");
    lcd.setCursor(5, 1);
    lcd.print(targetTemperature, DEC);
    lcd.print("C");

    // append "ale" or "lager"
    if ((targetTemperature >= 7) && (targetTemperature <= 9)) {
        lcd.print("  (lager)");
    } else if ((targetTemperature >= 10) && (targetTemperature <= 13)) {
        lcd.print(" (lager)");
    } else if ((targetTemperature >= 20) && (targetTemperature <= 22)) {
        lcd.print(" (ale)");
    }
}

int getTemperature(float value) {
    float offset = minTemperature;
    float delta = 1 + maxTemperature - minTemperature;
    return int(offset + delta * value / 1024.);
}
