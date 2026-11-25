// Pin Definitions
const int PIN_UNDERGROUND = A0; // Channel A: Resistor ladder (Buttons 1-4)
const int PIN_INWALL      = A1; // Channel B: Resistor ladder (Buttons 1-3)
const int PIN_SOIL        = A3; // Soil Moisture Sensor
const int PIN_POT         = A2; // Potentiometer (Overcurrent Simulation)

const int PIN_BUZZER   = 8;  // Buzzer
const int PIN_LED_OK   = 12; // Green LED
const int PIN_LED_WARN = 13; // Red LED

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_WARN, OUTPUT);

  Serial.println("====================================");
  Serial.println(" Cable Fault Detection System Ready ");
  Serial.println("====================================");
  delay(1000);
}

void loop() {
  // Read Analog Values
  int valUG   = analogRead(PIN_UNDERGROUND);
  int valWall = analogRead(PIN_INWALL);
  int valSoil = analogRead(PIN_SOIL);
  int valPot  = analogRead(PIN_POT);

  String line1 = "System Normal";
  String line2 = "Status: OK";
  bool isFault = false;

  // --- 1. CHECK OVERCURRENT (POTENTIOMETER) ---
  if (valPot > 650) { 
    line1 = "FAULT: OVERCURRENT";
    line2 = "Power Surge!";
    isFault = true;
  }

  // --- 2. CHECK UNDERGROUND CABLE FAULT (A0) ---
  else if (valUG > 300) {
    isFault = true;
    line1 = "UG FAULT DETECTED";
    if (valUG > 800)      line2 = "Dist: 1 KM";
    else if (valUG > 550) line2 = "Dist: 2 KM";
    else if (valUG > 350) line2 = "Dist: 3 KM";
    else                  line2 = "Dist: 4 KM";
  }

  // --- 3. CHECK IN-WALL CONDUIT FAULT (A1) ---
  else if (valWall < 100) {
    isFault = true;
    line1 = "WALL FAULT DETECTED";
    if (valWall > 750)      line2 = "Loc: Room 1";
    else if (valWall > 450) line2 = "Loc: Room 2";
    else                    line2 = "Loc: Room 3";
  }

  // --- 4. CHECK MOISTURE/LEAKAGE (A2) ---
  else if (valSoil < 500) { // Soil sensor reads LOW when wet
    isFault = true;
    line1 = "MOISTURE ALERT";
    line2 = "Water Ingress!";
  }

  // --- OUTPUT TO SERIAL MONITOR ---
  Serial.print("UG: "); Serial.print(valUG);
  Serial.print(" | Wall: "); Serial.print(valWall);
  Serial.print(" | Soil: "); Serial.print(valSoil);
  Serial.print(" | Pot: "); Serial.print(valPot);
  Serial.print(" --> [ "); Serial.print(line1);
  Serial.print(" | "); Serial.print(line2);
  Serial.println(" ]");

  // --- TRIGGER LEDS AND BUZZER ---
  if (isFault) {
    digitalWrite(PIN_LED_OK, LOW);
    digitalWrite(PIN_LED_WARN, HIGH);
    tone(PIN_BUZZER, 1000); // Beep alarm
  } else {
    digitalWrite(PIN_LED_OK, HIGH);
    digitalWrite(PIN_LED_WARN, LOW);
    noTone(PIN_BUZZER);
  }

  delay(300); // Refresh rate
}
