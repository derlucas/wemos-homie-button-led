#include <Homie.h>
#include <ArduinoOTA.h>

#define BTN_A 0
#define LED   5

Bounce debouncerA = Bounce();
int lastA = -1;
int ledValue, ledFunc = 0;
long ledTime=0;
int ledPeriod = 2000;
int ledDisplace = 500;
int ledIntens = 100;

HomieNode doorNode("button", "Button");
HomieNode ledNode("led", "led");

void loopHandler() {
  int aValue = debouncerA.read();
  
  if (aValue != lastA) {
     Homie.getLogger() << "A is now " << (aValue ? "open" : "close") << endl;
     doorNode.setProperty("button").send(aValue ? "false" : "true");
     lastA = aValue;
  }
  
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool ledHandlerDimmer(const HomieRange& range, const String& value) {

  ledFunc = 0;  // disable functions
  int val = value.toInt();
  if(val <= 0) val = 0;
  if(val >= 1024) val = 1024;   // invert value because LED is connected to 3,3V

  val = 1024 - val;
  analogWrite(LED, val);
  ledNode.setProperty("dimmer").send(value);

}
bool ledHandlerFade(const HomieRange& range, const String& value) {

  if(value == "fade1") {
    ledPeriod = 2000;
    ledIntens = 100;
    ledFunc = 1;
    ledNode.setProperty("fade").send("fade1");
  } else if(value == "fade2") {
    ledPeriod = 8000;
    ledIntens = 50;
    ledFunc = 1;
    ledNode.setProperty("fade").send("fade2");
  } else if(value.indexOf("fade3,") == 0) {
    String period_str = getValue(value, ',', 1);
    String intens_str = getValue(value, ',', 2);
    int period = period_str.toInt();
    int intens = intens_str.toInt();
    if(intens <= 1) intens = 1;
    if(intens > 100) intens = 100;
    if(period <= 200) period = 200;
    if(period > 10000) period = 10000;
    ledPeriod = period;
    ledIntens = intens;
    ledFunc = 1;
    ledNode.setProperty("fade").send("fade3");
  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  analogWrite(LED, 1024);
  
  debouncerA.attach(BTN_A);
  debouncerA.interval(50);
  
  Homie_setFirmware("button", "1.0.1");
  Homie.setResetTrigger(BTN_A, LOW, 10000);  // BTN0 = Flash = BTN_A set to 10sec
  Homie.setLoopFunction(loopHandler);

  doorNode.advertise("button");
  ledNode.advertise("dimmer").settable(ledHandlerDimmer);
  ledNode.advertise("fade").settable(ledHandlerFade);
 
  Homie.setup();
  ArduinoOTA.setHostname(Homie.getConfiguration().deviceId);
  ArduinoOTA.begin();
}

void loop() {
  Homie.loop();
  debouncerA.update();

  if(ledFunc == 1) {
    ledTime = millis();
    ledValue = 512 + 512 * cos(2*PI/ledPeriod*ledTime);
    analogWrite(LED, 1024 - (ledValue * ledIntens/100.0));
  }
  
  ArduinoOTA.handle();
}
