#include <Servo.h>
Servo servo;

#define IN A0

float Kp = 2.0;
float Ki = 0.5;
float Kd = 0.5;

const int N = 7;
float sensorValues[N] = {
    400, 324, 269, 215, 186, 180, 176};
float positions[N] = {
    -14, -10, -5, 0, 5, 10, 14};

int neutralAngle = 90;
int direction = -1;

unsigned long previousTime;
float previousError = 0;
float integral = 0;
float filteredRaw = 226;
bool firstMeasurement = true;

void setup()
{
    Serial.begin(9600);
    servo.attach(9);
    servo.write(90);
    delay(1000);
    previousTime = millis();
}

void loop()
{
    unsigned long now = millis();
    if (now - previousTime < 30)
    {
        return;
    }
    float deltaTime = (now - previousTime) / 1000.0;
    previousTime = now;

    float raw = analogRead(IN);
    if (firstMeasurement)
        filteredRaw = raw;
    filteredRaw = 0.5 * raw + 0.5 * filteredRaw;

    float position = getPosition(filteredRaw);
    float error = -position;
    float P = Kp * error;

    Serial.println(raw);
    Serial.println(filteredRaw);
    Serial.println(position);

    integral += error * deltaTime;
    integral = constrain(integral, -30.0, 30.0);
    float I = Ki * integral;
    float D = Kd * (error - previousError) / deltaTime;
    float PID = P + I + D;

    float angle = neutralAngle + direction * PID;
    angle = constrain(angle, 70.0, 110.0);
    servo.write(round(angle));

    previousError = error;
}

float getPosition(float value)
{
    if (value >= sensorValues[0])
    {
        return -14;
    }
    if (value <= sensorValues[N - 1])
    {
        return 14;
    }
    for (int i = 0; i < N - 1; i++)
    {
        if (value <= sensorValues[i] && value >= sensorValues[i + 1])
        {
            float sensorDelta = sensorValues[i + 1] - sensorValues[i];
            float positionsDelta = positions[i] + positions[i + 1];
            float valueRawDelta = value - sensorValues[i];
            return positions[i] + valueRawDelta * positionsDelta / sensorDelta;
        }
    }
    return 0;
}