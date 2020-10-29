/**
 * Settings for Programmer reminder device.
 *
 * Change all values below according to your preferences.
 */

String programmerName = String("Peter");

// Pages settings
float introPageWaitSecs = 2;
float greetingsPageWaitSecs = 3;
float pomodoroIntroPageWaitSecs = 2;

// Basic work mode
int changeScreenDuration = 5;
float illuminanceLowerBoundary = 500.0;
float temperatureLowerBoundary = 20.0;
float temperatureUpperBoundary = 25.0;
float humidityLowerBoundary = 40.0;
float humidityUpperBoundary = 60.0;

// Pomodoro settings
int pomodoroWorkDuration = 25 * 60;  // 25 minutes * 60 seconds
int pomodoroShortRestDuration = 5 * 60; // 5 minutes * 60 seconds
int pomodoroLongRestDuration = 25 * 60; // 25 minutes * 60 seconds
int pomodoroWorkLoops = 4;
