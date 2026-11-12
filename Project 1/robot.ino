#define PWMA 5
#define AIN2 A0
#define AIN1 A1
#define PWMB 6
#define BIN1 A2
#define BIN2 A3
#define ENCODER_A 2
#define ENCODER_B 3
volatile long encoderA = 0;
volatile long encoderB = 0;
const float PULSES_PER_CM = 21.0 / 20.42;
const float WHEEL_BASE_CM = 9.0;
void setup() {
 Serial.begin(115200);
 pinMode(PWMA, OUTPUT);
 pinMode(AIN1, OUTPUT);
 pinMode(AIN2, OUTPUT);
 pinMode(PWMB, OUTPUT);
 pinMode(BIN1, OUTPUT);
 pinMode(BIN2, OUTPUT);
 pinMode(ENCODER_A, INPUT);
 pinMode(ENCODER_B, INPUT);
 attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderA_ISR, RISING);
 attachInterrupt(digitalPinToInterrupt(ENCODER_B), encoderB_ISR, RISING);
 stopRobot();
 Serial.println("Robot is ready");
}
void loop() {
 if (Serial.available() > 0) {
   String input = Serial.readStringUntil('\n');
   input.trim();
   if (input.length() > 0) {
     processCommand(input);
   }
 }
}
void processCommand(String input) {
 char command = input.charAt(0);
 if (command == 's') {
   stopRobot();
   Serial.println("STOP");
   return;
 }
 if (command == 'e') {
   Serial.print("Encoder A: ");
   Serial.print(encoderA);
   Serial.print(" | Encoder B: ");
   Serial.println(encoderB);
   return;
 }
 if (command == 'c') {
   encoderA = 0;
   encoderB = 0;
   Serial.println("Encoders reset");
   return;
 }
 if (command == 'b') {
   getDistance();
   return;
 }
 int value = input.substring(1).toInt();
 if (command == 'v') {
   setRobotSpeed(value);
 }
 else if (command == 'm') {
   moveDistance(value);
 }
 else if (command == 'r') {
   rotateRobot(value);
 }
 else {
   Serial.println("Unknown command");
 }
}
void moveDistance(int distanceCm) {
 if (distanceCm == 0) {
   stopRobot();
   return;
 }
 long targetPulses = round(abs(distanceCm) * PULSES_PER_CM);
 encoderA = 0;
 encoderB = 0;
 int pwm = 110;
 if (distanceCm < 0) {
   digitalWrite(AIN1, HIGH);
   digitalWrite(AIN2, LOW);
   digitalWrite(BIN1, HIGH);
   digitalWrite(BIN2, LOW);
 }
 else {
   digitalWrite(AIN1, LOW);
   digitalWrite(AIN2, HIGH);
   digitalWrite(BIN1, LOW);
   digitalWrite(BIN2, HIGH);
 }
 analogWrite(PWMA, pwm);
 analogWrite(PWMB, pwm);
 while (encoderA < targetPulses || encoderB < targetPulses) {
   if (encoderA >= targetPulses) {
     analogWrite(PWMA, 0);
   }
   if (encoderB >= targetPulses) {
     analogWrite(PWMB, 0);
   }
 }
 stopRobot();
 Serial.print("Moved: ");
 Serial.print(distanceCm);
 Serial.println(" cm");
 Serial.print("Encoder A: ");
 Serial.print(encoderA);
 Serial.print(" | Encoder B: ");
 Serial.println(encoderB);
}
void rotateRobot(int degrees) {
 if (degrees == 0) {
   stopRobot();
   return;
 }
 float distanceCm = PI * WHEEL_BASE_CM * abs(degrees) / 360.0;
 long targetPulses = round(distanceCm * PULSES_PER_CM);
 encoderA = 0;
 encoderB = 0;
 int pwm = 110;
 if (degrees > 0) {
   digitalWrite(AIN1, LOW);
   digitalWrite(AIN2, HIGH);
   digitalWrite(BIN1, HIGH);
   digitalWrite(BIN2, LOW);
 }
 else {
   digitalWrite(AIN1, HIGH);
   digitalWrite(AIN2, LOW);
   digitalWrite(BIN1, LOW);
   digitalWrite(BIN2, HIGH);
 }
 analogWrite(PWMA, pwm);
 analogWrite(PWMB, pwm);
 while (encoderA < targetPulses || encoderB < targetPulses) {
   if (encoderA >= targetPulses) {
     analogWrite(PWMA, 0);
   }
   if (encoderB >= targetPulses) {
     analogWrite(PWMB, 0);
   }
 }
 stopRobot();
 Serial.print("Rotated: ");
 Serial.print(degrees);
 Serial.println(" degrees");
}
void getDistance() {
 float distanceA = encoderA / PULSES_PER_CM;
 float distanceB = encoderB / PULSES_PER_CM;
 float distance = (distanceA + distanceB) / 2.0;
 Serial.print("Distance: ");
 Serial.print(distance);
 Serial.println(" cm");
}
void setRobotSpeed(int speedValue) {
 speedValue = constrain(speedValue, -100, 100);
 int pwm = map(abs(speedValue), 0, 100, 0, 255);
 if (speedValue < 0) {
   digitalWrite(AIN1, HIGH);
   digitalWrite(AIN2, LOW);
   digitalWrite(BIN1, HIGH);
   digitalWrite(BIN2, LOW);
   analogWrite(PWMA, pwm);
   analogWrite(PWMB, pwm);
   Serial.print("Backward: ");
   Serial.println(speedValue);
 }
 else if (speedValue > 0) {
   digitalWrite(AIN1, LOW);
   digitalWrite(AIN2, HIGH);
   digitalWrite(BIN1, LOW);
   digitalWrite(BIN2, HIGH);
   analogWrite(PWMA, pwm);
   analogWrite(PWMB, pwm);
   Serial.print("Forward: ");
   Serial.println(speedValue);
 }
 else {
   stopRobot();
 }
}
void stopRobot() {
 analogWrite(PWMA, 0);
 analogWrite(PWMB, 0);
 digitalWrite(AIN1, LOW);
 digitalWrite(AIN2, LOW);
 digitalWrite(BIN1, LOW);
 digitalWrite(BIN2, LOW);
}
void encoderA_ISR() {
 encoderA++;
}
void encoderB_ISR() {
 encoderB++;
}
