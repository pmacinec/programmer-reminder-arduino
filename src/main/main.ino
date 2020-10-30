#include <BH1750.h>
#include <BME280I2C.h>
#include <U8glib.h>
#include "settings.h"
#include "icons.h"

BH1750 lightMeter;
BME280I2C bme;

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

int basicModeButton = 5;
int pomodoroModeButton = 4;
int stopBuzzerButton = 3;
int buzzer = 8;

int screenWidth = 128;

/**
 * Print centered text.
 * 
 * @param text Text to be printed.
 * @param positionY Height (y) position to print text at.
 */
void printCenteredText(String text, int positionY) {
  u8g.setPrintPos((128 - u8g.getStrPixelWidth(text.c_str())) / 2, positionY);
  u8g.print(text);
}

/**
 * Print device intro page.
 */
void printIntroPage() {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    printCenteredText("Programmer", 20);
    printCenteredText("reminder", 40);
  } while(u8g.nextPage());
  delay(4 * 1000);
}

/**
 * Print menu page.
 */
void printMenuPage(void) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont12);
    u8g.setPrintPos(0, 10);
    u8g.print("Please, choose");
    u8g.setPrintPos(0, 25);
    u8g.print("working mode:");
    u8g.setPrintPos(0, 40);
    u8g.setFont(u8g_font_profont10);
    u8g.print("Basic (YELLOW BUTTON)");
    u8g.setPrintPos(0, 50);
    u8g.print("Pomodoro (BLUE BUTTON)");
  } while (u8g.nextPage());
}

/**
 * Print Pomodoro intro page.
 */
void printPomodoroIntroPage() {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont22);
    printCenteredText("Pomodoro", 35);
  } while (u8g.nextPage());
  delay(3 * 1000);
}

/**
 * Check whether was button pushed.
 * 
 * @param buttonPin Pin at which button is binded.
 * @returns Bool value whether was button pushed or not.
 */
bool wasButtonPushed(int buttonPin) {
  int buttonState = digitalRead(buttonPin);
  return buttonState == HIGH;
}

/**
 * Alarm using piezo-buzzer.
 * 
 * @param text Text to print during alarm.
 */
void alarm(String text) {
  while (true) {
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_profont15);
      printCenteredText(text, 35);
      u8g.setFont(u8g_font_profont10);
      printCenteredText("Stop with GREEN button", 45);
    } while (u8g.nextPage());

    if (wasButtonPushed(stopBuzzerButton)) {
      return;
    }
    tone(buzzer, NOTE_E4, 0.5);
    delay(150);
    noTone(buzzer);
  }
}

/**
 * Timer with printing counting on display.
 * 
 * @param secs Number of seconds to count down.
 * @param inerruptPin Pin that can interrupt timer.
 * @returns Whether timer ended normally (0) or by interruption (1);
 */
int timer(int secs, int interruptPin) {
  unsigned long startTime = millis();
  unsigned long currentTime;

  int secondsPassed = 0;

  int minutesRemaining = secs / 60;
  int secondsRemaining = secs % 60;
  String timeToPrint;

  while (secondsPassed <= secs) {
    timeToPrint = (minutesRemaining < 10) ? "0" + String(minutesRemaining) : String(minutesRemaining);
    timeToPrint += ":";
    timeToPrint += (secondsRemaining < 10) ? "0" + String(secondsRemaining) : String(secondsRemaining);
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_profont22);
      printCenteredText(timeToPrint, 40);
    } while (u8g.nextPage());
  
    currentTime = millis();
    secondsPassed = (currentTime - startTime) / 1000;

    minutesRemaining = (secs - secondsPassed) / 60;
    secondsRemaining = (secs - secondsPassed) % 60;

    if (wasButtonPushed(interruptPin)) {
      return 1;
    }
    delay(150);
  }
  return 0;
}

/**
 * Pomodoro timer functionality.
 */
void pomodoroTimer() {
  int timerResponse = 0;

  while (true) {
    if (timerResponse == 1) {
      return;
    }

    for (int pomodoroLoops = 0; pomodoroLoops <= pomodoroWorkLoops - 1; pomodoroLoops++) {
      alarm("Work!");
      timerResponse = timer(pomodoroWorkDuration, pomodoroModeButton);
      if (timerResponse == 1) { return; }

      if (pomodoroLoops < 3) {
        alarm("Short rest");
        timerResponse = timer(pomodoroShortRestDuration, pomodoroModeButton);
        if (timerResponse == 1) { return; }
      }
    }

    alarm("Long rest");
    timerResponse = timer(pomodoroLongRestDuration, pomodoroModeButton);
    if (timerResponse == 1) { return; }
  }
}


/**
 * Print environment measured values.
 * 
 * @param illuminance Illuminance in Lux.
 * @param temperature Temperature in degree Celsius.
 * @param humidity Relative humidity percentage.
 */
void printEnvironmentMeasuredValues(float illuminance, float temperature, float humidity) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.drawBitmapP(0, 0, 3, 21, illuminanceBitmap);
    u8g.setPrintPos(25, 15);
    u8g.print(String(illuminance) + " Lux");

    u8g.drawBitmapP(0, 22, 3, 21, temperatureBitmap);
    u8g.setPrintPos(25, 36);
    u8g.print(String(temperature) + " C");
    
    u8g.drawBitmapP(0, 43, 3, 21, humidityBitmap);
    u8g.setPrintPos(25, 57);
    u8g.print(String(humidity) + " %");
  } while (u8g.nextPage());
  delay(5 * 1000);
}

/**
 * Print worked time.
 * 
 * @param workedSeconds Worked time in seconds.
 */
void printWorkedTime(int workedSeconds) {
  int hours = workedSeconds / 3600;
  int minutes = workedSeconds / 60;
  int seconds = workedSeconds % 60;

  String timeToPrint;

  timeToPrint = (hours < 10) ? "0" + String(hours) : String(hours);
  timeToPrint += ":";
  timeToPrint += (minutes < 10) ? "0" + String(minutes) : String(minutes);
  timeToPrint += ":";
  timeToPrint += (seconds < 10) ? "0" + String(seconds) : String(seconds);

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont22);
    printCenteredText(timeToPrint, 40);
  } while (u8g.nextPage());
  delay(5 * 1000);
}

/**
 * Start basic work mode.
 */
void basicWorkMode() {
  int workStart = millis();

  float illuminance;
  float temperature;
  float humidity;

  while (true) {
    if (wasButtonPushed(basicModeButton)) {
      printWorkedTime((millis() - workStart) / 1000);
      return;
    }

    illuminance = lightMeter.readLightLevel(true);
    temperature = bme.temp();
    humidity = bme.hum();
    printEnvironmentMeasuredValues(illuminance, temperature, humidity);

    printWorkedTime((millis() - workStart) / 1000);
  }
}

void setup() {
  // Initialize input/ouput modules pins
  pinMode(basicModeButton, INPUT);
  pinMode(pomodoroModeButton, INPUT);

  // Initialize the BME 280 sensor
  bme.begin();
  lightMeter.begin();

  printIntroPage();
}

void loop() {
  printMenuPage();
 
  if (wasButtonPushed(stopBuzzerButton)) {
    basicWorkMode();
  }

  if (wasButtonPushed(pomodoroModeButton)) {
    printPomodoroIntroPage();
    pomodoroTimer();
  }
}
