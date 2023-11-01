#include <Arduino.h>
#include <Servo.h>
#define LIGHT_SENSE_PIN A0 
#define LED_OUTPUT_PIN 8
#define INTERRUPT_PIN 2
volatile byte state = LOW;
int delay_const = 1000;
int scan_delay = 0;
int sensitivity = 0;
int curr_val;
int max_angle;
int curr_angle = 0;
int max_val = 0;
int counter = 0;
Servo myservo; 

void blink() {
  state = !digitalRead(INTERRUPT_PIN);
}
int get_val(){
    int val;
    val = analogRead(LIGHT_SENSE_PIN);
    val = 1023-val;
    return val;
}
void scan(){
        delay(delay_const); //delay before scan begins
        max_val = 0; //reset the max value
        Serial.println("Scanning the area");
        digitalWrite(LED_OUTPUT_PIN, true); //turn on LED 
        delay(delay_const);
        for (curr_angle = 0; curr_angle <= 180; curr_angle += 1) { // goes from 0 degrees to 180 degrees, in steps of 1 degree
            myservo.write(curr_angle); // tell servo to go to position in variable 'pos'
            Serial.println(curr_angle);
            delay(15); // waits 15ms for the servo to reach the position
            curr_val = get_val();
            Serial.print("light amount: ");
            Serial.println(curr_val);
            if(curr_val>max_val){
                Serial.println("updating max val and angle: ");
                max_val = curr_val;
                max_angle = curr_angle;
                Serial.println(max_val);
                Serial.println(max_angle);
            }
        }
    digitalWrite(LED_OUTPUT_PIN, false); //turn off LED when scan is complete 
    Serial.println("Scan complete");
    Serial.print("Moving to: ");
    Serial.print(max_angle);
    Serial.println(" degrees");
    myservo.write(max_angle);
    delay(delay_const);
}
void get_inputs(){
    Serial.print("Enter the delay in seconds: ");
    delay(delay_const);
    while(Serial.available() == 0){
        scan_delay = delay_const*Serial.parseInt(); 
    }
    Serial.println(scan_delay);

    Serial.print("Enter the sensitivity (100-200): "); 
    delay(delay_const);
    while(Serial.available() == 0 || sensitivity == 0){
        sensitivity = Serial.parseInt();
    }
    Serial.println(sensitivity);
    delay(delay_const);
}
void setup() {
Serial.begin(9600);
attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), blink, CHANGE);
pinMode(LED_OUTPUT_PIN, OUTPUT);
pinMode(INTERRUPT_PIN, INPUT_PULLUP);
myservo.attach(9); // attaches the servo on pin 9 to the servo object
}
void loop() {
    //get user inputs
    get_inputs();
    Serial.println("Press button anytime to start a scan: ");

    //checks the flag, sensitivity and delay, each event can trigger a scan
    while(true){
        while(state == LOW){
            counter ++;
            curr_val = get_val();
            delay(1); // 1ms delay for every counter increment 
            if(curr_val < (max_val-sensitivity) || curr_val > (max_val+sensitivity)){
                Serial.println("Light Source moved, starting new scan: ");
                break;
            }
            if(counter == scan_delay){
                Serial.println("Delay time reached, starting new scan");
                break;
            }
            continue;
        }
        scan();
    }
}