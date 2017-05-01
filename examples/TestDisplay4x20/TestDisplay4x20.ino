/**
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * Wiring (radio wiring on www.mysensors.org)
 * ------------------------------------
 * Arduino       I2C Display
 * ------------------------------------
 * GN            GND
 * +5            VCC
 * A4            SDA
 * A5            SCL
 *
 * http://www.mysensors.org/build/display
 *
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
#define MY_RF24_CHANNEL	78

#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

unsigned long lastUpdate=0;

// Initialize display. Google the correct settings for your display.
// The follwoing setting should work for the recommended display in the MySensors "shop".
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  // initialize the lcd for 20 chars 4 lines and turn on backlight
  lcd.begin(20,4);

  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++) {
    lcd.backlight();
    wait(250);
    lcd.noBacklight();
    wait(250);
  }
  lcd.backlight(); // finish with backlight on
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Display4x20", "1.0");
}

void loop()
{
  unsigned long now = millis();

  if (now-lastUpdate > 20000) {
    updateDisplay();
    lastUpdate = now;
  }
}

void updateDisplay(){
  lcd.home();
  //-------- Write characters on the display ----------------
// NOTE: Cursor Position: CHAR, LINE) start at 0
  lcd.setCursor(3,0); //Start at character 4 on line 0
  lcd.print("Hello, world!");
  wait(1000);
  lcd.setCursor(2,1);
  lcd.print("From MySensors");
  wait(1000);
  lcd.setCursor(0,2);
  lcd.print("20 by 4 Line Display");
}
