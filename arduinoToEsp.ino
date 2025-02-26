#include <DHT11.h>

#define ledPin 13
#define EP_D9 9  // Vibration sensor on D9
#define EP_D10 10 // Vibration sensor on D10
#define GAS_SENSOR A0 // Gas sensor on analog pin A0

DHT11 dht11(2);

void setup() {
    // Pin modes
    pinMode(ledPin, OUTPUT);
    pinMode(EP_D9, INPUT);
    pinMode(EP_D10, INPUT);

    // Serial communication
    Serial.begin(9600);
}

void loop() {
    // Read vibration sensors
    long measurementD9 = TP_init(EP_D9);
    long measurementD10 = TP_init(EP_D10);

    // Read DHT sensor
    int temperature = 0;
    int humidity = 0;
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Read gas sensor
    int gasValue = analogRead(GAS_SENSOR);

    // Send all sensor data to NodeMCU via Serial
    Serial.print(measurementD9);
    Serial.print(",");
    Serial.print(measurementD10);
    Serial.print(",");
    if (result == 0) {
        Serial.print(temperature);
        Serial.print(",");
        Serial.print(humidity);
    } else {
        Serial.print("Error,Error"); // Placeholder for DHT11 error
    }
    Serial.print(",");
    Serial.println(gasValue);

    // Control LED based on vibration thresholds
    if (measurementD9 > 500 || measurementD10 > 500) {
        digitalWrite(ledPin, HIGH); // Turn LED on
    } else {
        digitalWrite(ledPin, LOW); // Turn LED off
    }

    delay(2000); // Delay to prevent spamming serial monitor
}

long TP_init(int pin) {
    delay(10);
    long measurement = pulseIn(pin, HIGH); // Measure pulse duration on pin
    return measurement;
}
