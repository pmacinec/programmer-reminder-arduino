/**
 * Settings for Programmer reminder device.
 *
 * Change all values below according to your preferences.
 */

// Screens duration (in seconds)
int introScreenDuration = 5;
int pomodoroIntroScreenDuration = 5;
int basicWorkModeIntroScreenDuration = 5;
int basicWorkModeScreensChange = 10;


// Basic work mode
int illuminanceLowerBoundary = 400;
int temperatureLowerBoundary = 21;
int temperatureUpperBoundary = 26;
int humidityLowerBoundary = 35;
int humidityUpperBoundary = 60;

int eyesRestTime = 20 * 60;  // 20 minutes * 60 seconds (20 minutes)
int eyesRestDelay = 2 * 60;  // 2 minutes * 60 seconds (2 minutes)

int waterDrinkingRestTime = 60 * 60;  // 60 minutes * 60 seconds (1 hour)
int waterDrinkingRestDelay = 3 * 60;  // 3 minutes * 60 seconds (3 minutes)

int exerciseRestTime = 60 * 60 * 2; // 60 minutes * 60 seconds * 2 hours (2 hours)
int exerciseRestDelay = 10 * 60;  // 10 minutes * 60 seconds (10 minutes)


// Pomodoro settings
int pomodoroWorkDuration = 25 * 60;  // 25 minutes * 60 seconds (25 minutes)
int pomodoroShortRestDuration = 5 * 60; // 5 minutes * 60 seconds (5 minutes)
int pomodoroLongRestDuration = 25 * 60; // 25 minutes * 60 seconds (25 minutes)
int pomodoroWorkLoops = 4;
