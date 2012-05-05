#include <SPI.h>
#include <Ethernet.h>
//#include <PubSubClient.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.
byte mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x02 };
byte ip[]     = { 192, 168, 0, 32 };
byte server[] = { 192, 168, 0, 64 };

//Number of pulses, used to measure energy.
volatile long pulseCount1 = 0;   
volatile long pulseCount2 = 0;   

void callback(char* topic, byte* payload,unsigned int length) 
{
    // handle message arrived
}

// The interrupt routine
void onPulse1()
{
    //pulseCounter
    pulseCount1++;
}

// The interrupt routine
void onPulse2()
{
    //pulseCounter
    pulseCount2++;
}

PubSubClient client(server, 1883, callback);

void setup()
{
    // KWH interrupt attached to IRQ 0  = pin2
    attachInterrupt(0, onPulse1, FALLING);
    // KWH interrupt attached to IRQ 1  = pin3    
    attachInterrupt(1, onPulse2, FALLING);

    Ethernet.begin(mac, ip);
    if (client.connect("FunTechHouse_ElectricityMeter")) 
    {
        client.publish("FunTechHouse/Energy/meter01","#Hello world");
        client.publish("FunTechHouse/Energy/meter02","#Hello world");
        //client.subscribe("inTopic");
    }
}

//uint8_t miss_cnt = 0;

void loop()
{
    if(client.loop() == false)
    {
        client.connect("arduinoClient");
        //miss_cnt++;
    }

    char str[30];
    
    snprintf(str, 30, "energy=%d", pulseCount1);
    if(client.connected())
    {
        client.publish("FunTechHouse/Energy/meter01",str);
    }
    
    snprintf(str, 30, "energy=%d", pulseCount2);
    if(client.connected())
    {
        client.publish("FunTechHouse/Energy/meter02",str);
    }

    delay(10000); 
}


