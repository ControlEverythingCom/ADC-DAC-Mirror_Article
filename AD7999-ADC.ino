bool ready = false;
unsigned long tOut = 500;

double resistance(int reading);
double voltage(int reading);

int setMaxInput(String max);
int setMinInput(String min);

int outputPercentage = 0;
int inMax = 4095;
int inMin = 0;
int previousReading;
String previousMessage;

void setup() {
    Particle.variable("Output", outputPercentage);
    
    Wire.begin();
    Wire.beginTransmission(0x29);
    byte status = Wire.endTransmission();
    if(status != 0){
        Serial.println("Failed to communicate with sensor");
    }else{
        Serial.println("Communication to sensor initialized");
        ready = true;
    }
}

void loop() {
    while(ready){
        //Read channel 1
        Wire.beginTransmission(0x29);
        Wire.write(16);
        byte status = Wire.endTransmission();
        if(status != 0){
            Serial.println("Failed to communicate with sensor");
        }else{
            ready = true;
        }
        unsigned long startTime = millis();
        Wire.requestFrom(0x29, 2);
        while(Wire.available() != 2 && millis() < startTime+tOut);
        if(Wire.available() !=2){
            Serial.println("Timeout");
            ready = false;
            break;
        }else{
            int msb = Wire.read();
            msb = msb&0x0f;
            int lsb = Wire.read();
            int reading = (msb*256)+lsb;
            
            if(reading == previousReading){
                // Serial.println("Should continue");
                delay(1000);
                continue;
            }
            
            //Input Maxed
            if(reading >= inMax){
                Serial.println("input maxed");
                if(previousMessage.equalsIgnoreCase("100.00")){
                    Serial.println("Already published");
                    delay(1000);
                    continue;
                }
                Serial.println("Publishing max input");
                previousMessage = "100.00";
                Particle.publish("Output", "100.00", 60, PRIVATE);
                previousReading = reading;
                delay(1000);
                continue;
            }
            
            //Input lower limit reached
            if(reading <= inMin){
                Serial.println("Lower limit reached");
                if(previousMessage.equalsIgnoreCase("0.00")){
                    continue;
                }
                previousMessage = "0.00";
                Particle.publish("Output", "0.00", 60, PRIVATE);
                previousReading = reading;
                delay(1000);
                continue;
            }
            float r = ((float)reading/(float)inMax)*100;
            
            String message = String(r);
            if(previousMessage.equalsIgnoreCase(message)){
                delay(1000);
                continue;
            }
            previousMessage = message;
            
            Particle.publish("Output", message, 60, PRIVATE);
            previousReading = reading;
            delay(1000);
        }
        delay(100);
    }
    Wire.beginTransmission(0x29);
    byte status = Wire.endTransmission();
    if(status != 0){
        Serial.println("Failed to communicate with sensor");
    }else{
        Serial.println("Communication to sensor initialized");
        ready = true;
    }
    delay(1000);
    
}