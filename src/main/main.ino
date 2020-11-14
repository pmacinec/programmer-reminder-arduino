#include <BH1750.h>
#include <BME280I2C.h>
#include <U8glib.h>
#include "settings.h"
#include "icons.h"
#include "song.h"

BH1750 lightMeter;
BME280I2C bme;

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

int basicModeButton = 5;
int pomodoroModeButton = 6;
int stopBuzzerButton = 7;

int buzzer = 8;

int screenWidth = 128;

/**
 * Print centered text.
 * 
 * @param text Text to be printed.
 * @param positionY Height (y) position to print text at.
 */
void printCenteredText(String text, int positionY) {
  u8g.setPrintPos((screenWidth - u8g.getStrPixelWidth(text.c_str())) / 2, positionY);
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
  delay(introScreenDuration * 1000);
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
    u8g.print("Basic (WHITE BUTTON)");
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
    u8g.setFont(u8g_font_profont15);
    printCenteredText("Pomodoro", 35);
  } while (u8g.nextPage());
  delay(pomodoroIntroScreenDuration * 1000);
}

/**
 * Print Pomodoro intro page.
 */
void printBasicWorkModeIntroPage() {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont12);  
    printCenteredText("Wish you productive", 25); 
    printCenteredText("day at work!", 50);
  } while (u8g.nextPage());
  delay(basicWorkModeIntroScreenDuration * 1000);
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
      u8g.setFont(u8g_font_profont12);
      printCenteredText(text, 30);
      u8g.setFont(u8g_font_profont10);
      printCenteredText("Stop with GREEN button", 45);
    } while (u8g.nextPage());

    if (wasButtonPushed(stopBuzzerButton)) return;

    tone(buzzer, NOTE_E4, 0.5);
    delay(100);
    noTone(buzzer);
  }
}

/**
 * Get time string from elapsed seconds.
 * 
 * @param secondsElapsed Elapsed seconds.
 * @param hideHours Whether to hide hours or not (only MM:SS string will be returned).
 * @returns Time string in HH:MM:SS format.
 */
String getTimeString(int secondsElapsed, bool hideHours = false) {
  int hours = secondsElapsed / 3600;
  int minutes = secondsElapsed / 60;
  int seconds = secondsElapsed % 60;

  String timeString = "";

  if (!hideHours) {
    timeString += (hours < 10) ? "0" + String(hours) : String(hours);
    timeString += ":";
  }
  timeString += (minutes < 10) ? "0" + String(minutes) : String(minutes);
  timeString += ":";
  timeString += (seconds < 10) ? "0" + String(seconds) : String(seconds);
  
  return timeString;
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

  int secondsPassed = 0;

  int minutesRemaining = secs / 60;
  int secondsRemaining = secs % 60;
  String timeToPrint;

  while (secondsPassed <= secs) {
    timeToPrint = getTimeString(secs - secondsPassed, true);
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_profont15);
      printCenteredText(timeToPrint, 35);
    } while (u8g.nextPage());
  
    secondsPassed = (millis() - startTime) / 1000;

    if (wasButtonPushed(interruptPin)) return 1;
  }
  return 0;
}

/**
 * Pomodoro timer functionality.
 */
void pomodoroTimer() {
  int timerResponse = 0;

  while (true) {
    if (timerResponse == 1) return;

    for (int pomodoroLoops = 0; pomodoroLoops <= pomodoroWorkLoops - 1; pomodoroLoops++) {
      alarm("Work!");
      timerResponse = timer(pomodoroWorkDuration, pomodoroModeButton);
      if (timerResponse == 1) return;

      if (pomodoroLoops < 3) {
        alarm("Short rest");
        timerResponse = timer(pomodoroShortRestDuration, pomodoroModeButton);
        if (timerResponse == 1) return;
      }
    }

    alarm("Long rest");
    timerResponse = timer(pomodoroLongRestDuration, pomodoroModeButton);
    if (timerResponse == 1) return;
  }
}

/**
 * Print worked time.
 * 
 * @param workedSeconds Worked time in seconds.
 * @param count Whether to count worked time or remain static.
 * @returns Whether screen print ended normally (0) or by interruption (1);
 */
int printWorkedTime(int workedSeconds, bool count = true) {
  unsigned long startTime = millis();

  int secondsPassed = 0;

  String timeToPrint;
  String workedTimeMessage = count ? "Worked time" : "Worked time today";

  while (secondsPassed <= basicWorkModeScreensChange) {
    timeToPrint = getTimeString(workedSeconds + secondsPassed);
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_profont15);
      
      printCenteredText(workedTimeMessage, 20);
      printCenteredText(timeToPrint, 40);
    } while (u8g.nextPage());
 
    if (!count) {
      delay(basicWorkModeScreensChange * 1000);
      return 0;
    }
    else {
      secondsPassed = (millis() - startTime) / 1000;
    }

    if (wasButtonPushed(basicModeButton)) return 1;
  }

  return 0;
}

/**
 * Print environment measured values.
 * 
 * @param illuminance Illuminance in Lux.
 * @param temperature Temperature in degree Celsius.
 * @param humidity Relative humidity percentage.
 * @returns Whether screen print ended normally (0) or by interruption (1);
 */
int printEnvironmentMeasuredValues(float illuminance, float temperature, float humidity) {
  unsigned long startTime = millis();

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.drawBitmapP(0, 0, 2, 16, illuminanceBitmap);
    u8g.setPrintPos(25, 12);
    u8g.print(String(illuminance) + " Lux");

    u8g.drawBitmapP(0, 22, 2, 16, temperatureBitmap);
    u8g.setPrintPos(25, 34);
    u8g.print(String(temperature) + "  C");
    u8g.drawCircle(68, 26, 2);

    u8g.drawBitmapP(0, 43, 2, 16, humidityBitmap);
    u8g.setPrintPos(25, 56);
    u8g.print(String(humidity) + " rH%");
  } while (u8g.nextPage());

  while ((millis() - startTime) / 1000 <= basicWorkModeScreensChange) {
    if (wasButtonPushed(basicModeButton)) return 1;
  }
}

/**
 * Check environment measured values and play alarm if needed.
 * 
 * @param illuminance Illuminance in Lux.
 * @param temperature Temperature in degree Celsius.
 * @param humidity Relative humidity percentage.
 */
void checkEnvironmentMeasuredValues(float illuminance, float temperature, float humidity) {
  if (illuminance < illuminanceLowerBoundary) {
    alarm("Illuminance too low!");
  }

  if (temperature <= temperatureLowerBoundary) {
    alarm("Temperature too low!");
  }

  if (temperature >= temperatureUpperBoundary) {
    alarm("Temperature too high!");
  }

  if (humidity <= humidityLowerBoundary) {
    alarm("Humidity too low!");
  }

  if (humidity >= humidityUpperBoundary) {
    alarm("Humidity too high!");
  }
}

/**
 * Print ideal environment values.
 * 
 * @returns Whether screen print ended normally (0) or by interruption (1);
 */
int printEnvironmentIdealValues() {
  unsigned long startTime = millis();

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.drawBitmapP(0, 0, 2, 16, illuminanceBitmap);
    u8g.setPrintPos(23, 12);
    u8g.print(">" + String(illuminanceLowerBoundary) + " Lux");

    u8g.drawBitmapP(0, 22, 2, 16, temperatureBitmap);
    u8g.setPrintPos(23, 34);
    u8g.print(String(temperatureLowerBoundary) + "-" + String(temperatureUpperBoundary) + "  C");
    u8g.drawCircle(110, 26, 2);

    u8g.drawBitmapP(0, 43, 2, 16, humidityBitmap);
    u8g.setPrintPos(23, 56);
    u8g.print(String(humidityLowerBoundary) + "-" + String(humidityUpperBoundary) + " rH%");
  } while (u8g.nextPage());

  while ((millis() - startTime) / 1000 <= basicWorkModeScreensChange) {
    if (wasButtonPushed(basicModeButton)) return 1;
  }
}

/**
 * Play Ode to Joy song.
 * 
 * Code taken (and modified) from https://github.com/robsoncouto/arduino-songs.
 */
void playSong() {
  // Sizeof gives the number of bytes, each int value is composed of two bytes (16 bits),
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // This calculates the duration of a whole note in ms (60 is tempo)
  int wholenote = (60000 * 2) / 60;

  int divider = 0, noteDuration = 0;

  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    if (wasButtonPushed(basicModeButton)) return;

    // Calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // Regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // Dotted notes are represented with negative durations
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // Increases the duration in half for dotted notes
    }

    // We only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // Stop the waveform generation before the next note.
    noTone(buzzer);
  }
}

/**
 * Perform post-actions of basic working mode is finished.
 * 
 * @param workedSeconds Worked time in seconds.
 */
void finalizeBasicWorkingMode(int workedSeconds) {
  printWorkedTime(workedSeconds, false);
  playSong();
}

/**
 * Start basic work mode.
 */
void basicWorkMode() {
  int workStart = millis();

  int returnState = 0;

  float restsTimer = 0;
  int eyesRestTimeTimer = millis();
  int waterDrinkingRestTimeTimer = eyesRestTimeTimer;
  int exerciseRestTimeTimer = eyesRestTimeTimer;

  float illuminance;
  float temperature;
  float humidity;

  while (true) {
    illuminance = lightMeter.readLightLevel();
    temperature = bme.temp();
    humidity = bme.hum();
    returnState = printEnvironmentMeasuredValues(illuminance, temperature, humidity);
    if (returnState == 1) break;

    checkEnvironmentMeasuredValues(illuminance, temperature, humidity);

    returnState = printEnvironmentIdealValues();
    if (returnState == 1) break;

    returnState = printWorkedTime((millis() - workStart) / 1000);
    if (returnState == 1) break;

    if ((millis() - eyesRestTimeTimer) / 1000 + basicWorkModeScreensChange * 3 / 2 >= eyesRestTime) {
      alarm("Eyes rest!");
      delay(eyesRestDelay * 1000);
      eyesRestTimeTimer = millis();
    }

    if ((millis() - waterDrinkingRestTimeTimer) / 1000 + basicWorkModeScreensChange * 3 / 2 >= waterDrinkingRestTime) {
      alarm("Eyes rest!");
      delay(waterDrinkingRestDelay * 1000);
      waterDrinkingRestTimeTimer = millis();
    }

    if ((millis() - exerciseRestTimeTimer) / 1000 + basicWorkModeScreensChange * 3 / 2 >= exerciseRestTime) {
      alarm("Eyes rest!");
      delay(exerciseRestDelay * 1000);
      exerciseRestTimeTimer = millis();
    }
  }

  finalizeBasicWorkingMode(millis() - workStart);
}

void setup() {
  // Initialize input/ouput modules pins
  pinMode(basicModeButton, INPUT);
  pinMode(pomodoroModeButton, INPUT);
  pinMode(stopBuzzerButton, INPUT);

  // Initialize the BME 280 sensor
  bme.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);

  printIntroPage();
}

void loop() {
  printMenuPage();
 
  if (wasButtonPushed(basicModeButton)) {
    printBasicWorkModeIntroPage();
    basicWorkMode();
  }

  if (wasButtonPushed(pomodoroModeButton)) {
    printPomodoroIntroPage();
    pomodoroTimer();
  }
}
