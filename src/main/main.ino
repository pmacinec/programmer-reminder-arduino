#include "settings.h"
#include "pitches.h"
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

// TODO move into pins declaration
int basicModeButton = 2;
int pomodoroModeButton = 4;
int stopBuzzerButton = 3;
int buzzer = 8;


/**
 * Print device intro page.
 * 
 * @param waitSecs Number of seconds to wait with intro page shown.
 */
void printIntroPage(float waitSecs = 2.0) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.setPrintPos(30, 20);
    u8g.print("Programmer");
    u8g.setPrintPos(40, 40);
    u8g.print("reminder");
  } while(u8g.nextPage());
  delay(waitSecs * 1000);
}

/**
 * Print greetings page.
 * 
 * @param waitSecs Number of seconds to wait with greetings page shown.
 */
void printGreetingsPage(float waitSecs = 3.0) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.setPrintPos(0, 10);
    u8g.print("Hello " + programmerName + "!");
    u8g.setFont(u8g_font_profont12);
    u8g.setPrintPos(0, 40);
    u8g.print("Wish you productive");
    u8g.setPrintPos(0, 55);
    u8g.print("day at work!");
  } while (u8g.nextPage());
  delay(waitSecs * 1000);
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
 * 
 * @params waitSecs Number of seconds to wait with Pomodoro intro page shown.
 */
void printPomodoroIntroPage(float waitSecs = 2.0) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.setPrintPos(35, 35);
    u8g.print("Pomodoro");
  } while (u8g.nextPage());
  delay(waitSecs * 1000);
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
 * Print simple text.
 * 
 * @param text Simple string text to be printed.
 * @param waitSecs Number of seconds to wait on page with shown text.
 */
void printSimpleText(String text, float waitSecs = 1.0) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_profont15);
    u8g.setPrintPos(35, 35);
    u8g.print(text);
  } while (u8g.nextPage());
  delay(waitSecs * 1000);
}


/**
 * Timer with printing counting on display.
 * 
 * @param secs Number of seconds to count down.
 * @param inerruptPin Pin that can interrupt timer.
 * @returns Whether timer ended normally (0) or by interruption (1);
 */
int timer(float secs, int interruptPin) {
  // zastavenie s podmienkou a return 0
  unsigned long startTime = millis();
  int secondsPassed = 0;

  while (secondsPassed < secs * 1000) {
    secondsPassed = (millis() - startTime) * 1000;
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_profont15);
      u8g.setPrintPos(35, 35);
      u8g.print(secondsPassed);
    } while (u8g.nextPage());

    if (wasButtonPushed(interruptPin)) {
      return 1;
    }
  }
  return 0;
}

// TODO more settings about this
// TODO params, but how to do that wit settings and multiple code files?
// TODO beeping between cycles
/**
 * Pomodoro timer functionality.
 */
void pomodoroTimer() {
  int timerResponse = 0;

  while (true) {
    if (timerResponse == 1) {
      return;
    }

    for (int pomodoroLoops = 0; pomodoroLoops <= 3; pomodoroLoops++) {
       printSimpleText("Work!");
       timerResponse = timer(pomodoroWorkDuration, pomodoroModeButton);

       if (pomodoroLoops < 3) {
         printSimpleText("Short rest");
         timerResponse = timer(5 * 60, pomodoroModeButton);
       }
    }

    printSimpleText("Long rest");
    timerResponse = timer(25 * 60, pomodoroModeButton);
  }
}

void setup() {
  // Initialize input/ouput modules pins
  pinMode(basicModeButton, INPUT);
  pinMode(pomodoroModeButton, INPUT);
 
  printIntroPage(intro_page_wait_secs);
  printGreetingsPage(greetings_page_wait_secs);
}

void loop() {
  printMenuPage();
 
  if (wasButtonPushed(basicModeButton)) {
    // TODO
  }

  if (wasButtonPushed(pomodoroModeButton)) {
    printPomodoroIntroPage(pomodoro_intro_page_wait_secs);
    pomodoroTimer();
  }
}
