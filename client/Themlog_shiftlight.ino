// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <LiquidCrystal.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define GREEN pixels.Color(0, 200, 0)
#define YELLOW pixels.Color(255, 200, 0)
#define ORANGE pixels.Color(255, 100, 0)
#define RED pixels.Color(255, 0, 0)

#define BUTTON_PIN 8

const int DISP_OPT_LIMIT_TEMP = 0;
const int DISP_OPT_THUNDERBOLT = 1;
const int DISP_OPT_NUM = 2;
int DISP_OPT_CURRENT = 0;

const String CPU_LIMIT_COMMAND = "L";
const String CPU_TEMP_COMMAND = "T";


boolean waitingForStatus = true;
String incomingCommand;
int currentLimitValue = -1;
int currentTempValue = -1;
boolean buttonState = false;

void setup() {
  Serial.setTimeout(500);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
  pixels.begin();
  lcd.begin(16, 2);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("status update...");
}

void loop() {
  toggleButton();
  updateBrightness();

  if (waitingForStatus) {
    blink();
    showcase();
  }
  checkCommandInput();
}

void toggleButton() {
  boolean isPressed = digitalRead(BUTTON_PIN);
  if (isPressed != buttonState) {
    if (isPressed) {
      switchDisplayOptions();
      updateDisplay();
    }
    buttonState = isPressed;
  }
}

void checkCommandInput() {
  if (Serial.available()) {
    // Read incoming command
    incomingCommand = Serial.readStringUntil(',');
    Serial.println(incomingCommand); // Echo for debugging
    //printLcd(incomingCommand); // Display for debugging
    waitingForStatus = false;

    // Parse CPU Limit
    int incomingLimitValue = parseCPULimitCommand(incomingCommand);
    if (incomingLimitValue >= 0 & incomingLimitValue != currentLimitValue) {
      currentLimitValue = incomingLimitValue;
      setLevel(currentLimitValue);
      updateDisplay();
    }


    // Parse CPU Temperature
    int incomingTempValue = parseTempCommand(incomingCommand);
    if (incomingTempValue >= 0 & incomingTempValue != currentTempValue) {
      currentTempValue = incomingTempValue;
      updateDisplay();
    }
  }
}

int parseCPULimitCommand(String command) {
  if (command.startsWith(CPU_LIMIT_COMMAND)) {
    return command.substring(1).toInt();
  } else {
    return -1;
  }
}

int parseTempCommand(String command) {
  if (command.startsWith(CPU_TEMP_COMMAND)) {
    return command.substring(1).toInt();
  } else {
    return -1;
  }
}

void updateDisplay() {
  lcd.clear();

  if (DISP_OPT_CURRENT == DISP_OPT_LIMIT_TEMP) {
    if (currentLimitValue > 0) {
      lcd.print("CPU limit: ");
      lcd.print(currentLimitValue);
      lcd.print("%");
    }
    if (currentTempValue > 0) {
      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(currentTempValue);
      lcd.print((char)223);
      lcd.print("C");
    }
  }
  if (DISP_OPT_CURRENT == DISP_OPT_THUNDERBOLT) {
    lcd.print("Thund L  Thund R");
    //         ________________
    lcd.setCursor(0, 1);
    lcd.print("50");
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(9, 1);
    lcd.print("48");
    lcd.print((char)223);
    lcd.print("C");
  }
}

void switchDisplayOptions() {
  DISP_OPT_CURRENT++;
  if(DISP_OPT_CURRENT == DISP_OPT_NUM){
    DISP_OPT_CURRENT = 0;
  }
}

void printLcd(String value) {
  lcd.clear();
  lcd.print(value);
}

void showcase() {
  int showcaseLevels[] = {
    100, 96, 91, 85, 75, 65, 55, 49
  };
  for (int i = 0; i < 8; i++) {
    setLevel(showcaseLevels[i]);
    delay(50);
    updateBrightness();
  }
}

void setLevel(int cpuLimit) {
  pixels.clear();

  // 100
  pixels.setPixelColor(0, GREEN);
  // 99-95
  if (cpuLimit < 100) {
    pixels.setPixelColor(1, YELLOW);
  }
  // 95-90
  if (cpuLimit < 95) {
    pixels.setPixelColor(2, YELLOW);
  }
  // 90-80
  if (cpuLimit < 90) {
    pixels.setPixelColor(3, ORANGE);
  }
  // 80-70
  if (cpuLimit < 80) {
    pixels.setPixelColor(4, ORANGE);
  }
  // 70-60
  if (cpuLimit < 70) {
    pixels.setPixelColor(5, ORANGE);
  }
  // 60-50
  if (cpuLimit < 60) {
    pixels.setPixelColor(6, RED);
  }
  // 50-0
  if (cpuLimit < 50) {
    pixels.setPixelColor(7, RED);
  }

  pixels.show();
}

int currentBrightness = 0;
void updateBrightness() {
  int newBrightness = constrain(analogRead(A0) / 4, 1, 255);
  if (currentBrightness != newBrightness) {
    pixels.setBrightness(newBrightness);
    pixels.show();
    currentBrightness = newBrightness;
  }
}

void blink() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(50);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(50);
}