# Smart Home Control App
This purpose of this project was to create an Android application that would act as the control interface to a smart home control system. The system is made up of three subsystems: a Lighting system, an HVAC system, and a sprinkler control system. Each of these subsystems is run by an Arduino that the user can control through the app.

## Smart Home Control App
This app was created using MIT App inventor. The app allows the user to control each subsystem through a dedicated menu.
SmartHomeControl.aia contains the source code for the app.
### Lighting System
This system controls lighting and can activate the lights on a schedule or automatically when motion is detected. The menu allows the user to manually turn the system on or off. The user can also set times the system is always on or off. The user can also adjust how long motion is not detected before the lights shut off automatically.
### HVAC System
This system reports the current temperature and controls the Heating and A/C systems based on the user settings. The menu reports the current temperature and allows the user to control the heating and A/C manually or set temperature limits that activate the systems automatically.
### Sprinkler System
This system reads the moisture level in soil and actiavtes the sprinklers when there is not enough moisture. The menu allows the user to see the current moisture level and set a schuedule for the spinklers to run.

## Arduino Scripts
The Arduino Scripts folder contains the source code for each of the Arduinos. Each Arduino acts as a web server that app connects to read its status or change its settings.
### motion_ard.ino
This is the source code for the Arduino that controls the lighting system. The Arduino periodically reads from the motion sensor to see if any motion is detected. When motion is detected, the light is turned on and a timer starts. If no motion is detected when the timer finishes, the light is turned off. The Arduino will also turn the light on or off if it receives a command from the app.
### temp_ard.ino
This is the source code for the Arduino that controls the HVAC system. The Arduino periodically reads from the temperture sensor. This temperature is reported back to the app. There is a high temperature limit and low temperature limit set by the app. If the temperature falls below the low limit, the A/C is turned on. If the temperature rises above the high limit, the heating is turned on. If the temperature is between both limits, both systems are off. The Arduino will also turn either system on or off if it receives a command from the app.
### mois_ard.ino
This is the source code for the Arduino that controls the Sprinkler system. The Arduino periodically reads from the moisture sensor. The moisture level is reported back to the app. The system has an upper threshold and a lower threshold. When the moisture level falls below the lower threshold, the sprinklers are turned on. When the moisture level rises above the higher threshold, the sprinklers are turend off. The Arduino will also turn the sprinklers on or off if it receives a command from the app.
