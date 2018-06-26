#define TOOTH_COUNT 116
#define TOOTH_COUNT_OUT 24
#define TOOTH_MISSING_OUT 1

int toothCounter = 0;
int rpmCounter = 0;
int triggeringTeeth[TOOTH_COUNT_OUT];
int triggeringOffset[TOOTH_COUNT_OUT];
int currentTriggeringIndex = 0;
bool trigger;
int toothLength = 133;

bool firstRefToothDetected = false;
bool errorDetected = false;
long lastTrigger = 0;
long lastTeethTimestamp = 0;
long previousTimeBetweenTeeth = 0;
int toothLengthCoef;

void smallToothDetected() {
  toothCounter++;
  if (toothCounter == triggeringTeeth[currentTriggeringIndex]) {
    trigger = true;
  } else {
    long aux = micros();
    previousTimeBetweenTeeth = aux - lastTeethTimestamp;
    lastTeethTimestamp = aux;
  }
}

void referenceToothDetected() {
  if (TOOTH_COUNT != toothCounter) {
    errorDetected = true;
  }
  rpmCounter++;
  toothCounter = 0;
  currentTriggeringIndex = TOOTH_MISSING_OUT;
  PORTB = B00000000;
  toothLength = previousTimeBetweenTeeth * toothLengthCoef;
}

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(2), smallToothDetected, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), referenceToothDetected, RISING);
  toothLengthCoef = TOOTH_COUNT / (TOOTH_COUNT_OUT * 2);
  DDRB = B11000000;
  DDRA = B11111111;
  PORTB = B10000000;
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(23, OUTPUT);
//  Serial.begin(9600);

  for (int i = 0; i < TOOTH_COUNT_OUT; i++) {
    triggeringTeeth[i] = (TOOTH_COUNT * i) / TOOTH_COUNT_OUT;
    triggeringOffset[i] = 100 * ((((float) TOOTH_COUNT * (float) i) / (float) TOOTH_COUNT_OUT) - (float) triggeringTeeth[i]);

//    Serial.print(i);
//    Serial.print("-");
//    Serial.print(triggeringTeeth[i]);
//    Serial.print("-");
//    Serial.println(triggeringOffset[i]);
  }

  lastTrigger = micros();
}

void loop() {
  if (trigger) {
    trigger = false;
    long currentTime = micros();
    long offsetFromLastTrigger = (previousTimeBetweenTeeth * triggeringOffset[currentTriggeringIndex]) / 100;
    delayMicroseconds(offsetFromLastTrigger);
    PORTB = B00000000;
    currentTriggeringIndex++;
    delayMicroseconds(toothLength);
    PORTB = B11111111;
    //  } else {
    //    toothLength = previousTimeBetweenTeeth * 2;
  }
}
