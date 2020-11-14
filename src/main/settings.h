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
float illuminanceLowerBoundary = 400.0;
float temperatureLowerBoundary = 21.0;
float temperatureUpperBoundary = 26.0;
float humidityLowerBoundary = 35.0;
float humidityUpperBoundary = 60.0;

// Pomodoro settings
int pomodoroWorkDuration = 25 * 60;  // 25 minutes * 60 seconds
int pomodoroShortRestDuration = 5 * 60; // 5 minutes * 60 seconds
int pomodoroLongRestDuration = 25 * 60; // 25 minutes * 60 seconds
int pomodoroWorkLoops = 4;
