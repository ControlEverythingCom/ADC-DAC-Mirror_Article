bool ready = false;
int outputStatus = 0;

void myHandler(const char *event, const char *data);

void setup() {
    Particle.subscribe("Output", myHandler, MY_DEVICES);
    Wire.begin();
}

void loop() {
}

void myHandler(const char *event, const char *data)
{
    Serial.println("Handler Triggered");
    String newSetting = String(data);
    float f = newSetting.toFloat();
    f = f/100;
    int pos = 40200*f;
    int msb = pos/256;
    int lsb = pos - (msb*256);
    Serial.printf("msb: %i \n",msb);
    Serial.printf("lsb: %i \n",lsb);
    Wire.beginTransmission(12);
    Wire.write(49);
    Wire.write(msb);
    if(lsb >=0){
        Wire.write(lsb);
    }
    Wire.endTransmission();
}