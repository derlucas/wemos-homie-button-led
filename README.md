# wemos-homie-button-led

Homie ESP8266 Firmware for Wemos D1 mini with a Button and a LED


## Button

The button input will send "true" or "false" depending on the current pressed state. Pressing the Button for at least ten seconds will erase the Homie config and reset the Device to config mode.

The MQTT topic of the Button is:

> homie/$deviceid/button/button


## LED

The Device can control a LED (maybe within the Button) for indicating status or outputting some information.

The MQTT topic for the LED is:

> homie/$deviceid/led/dimmer/set

You need to send a number between 0 and 1024 to control the brightness.

There is also a builtin fading function for the LED:

> homie/$deviceid/led/fade/set

Send "fade1" or "fade2" to use presets, or send "fade3,$interval,$intensity".

$interval is in milliseconds for the period of the automatic fading and $intensity is from 0 to 100 to control fading brightness.
