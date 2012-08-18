#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

// Update these with values suitable for your network.
byte mac[]    = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x02 };
byte ip[]     = { 192, 168, 0, 32 };
byte server[] = { 192, 168, 0, 64 };

char project_name[]  = "FunTechHouse_ElectricityMeter";
char topic_meter01[] = "FunTechHouse/Energy/meter01";
char topic_meter02[] = "FunTechHouse/Energy/meter02";

//Number of pulses, used to measure energy.
volatile unsigned int pulseCount1_Wh  = 0;   
volatile unsigned int pulseCount1_kWh = 0;   

volatile unsigned int pulseCount2_Wh  = 0;   
volatile unsigned int pulseCount2_kWh = 0;   

volatile unsigned int updateCount = 0;   

void callback(char* topic, byte* payload,unsigned int length) 
{
    // handle message arrived
}

// The interrupt routine
void onPulse1()
{
    //pulseCounter
    pulseCount1_Wh++;
    if(pulseCount1_Wh == 1000)
    {
        pulseCount1_Wh = 0;
        pulseCount1_kWh++;
    }
}

// The interrupt routine
void onPulse2()
{  
    //pulseCounter
    pulseCount2_Wh++;
    if(pulseCount2_Wh == 1000)
    {
        pulseCount2_Wh = 0;
        pulseCount2_kWh++;
    }
}

PubSubClient client(server, 1883, callback);

void setup()
{
    // KWH interrupt attached to IRQ 0  = pin2
    attachInterrupt(0, onPulse1, FALLING);
    // KWH interrupt attached to IRQ 1  = pin3    
    attachInterrupt(1, onPulse2, FALLING);

    Ethernet.begin(mac, ip);
    if (client.connect(project_name)) 
    {
        client.publish(topic_meter01, "#Hello world");
        client.publish(topic_meter02, "#Hello world");
        //client.subscribe("inTopic");
    }
}

void loop()
{
    //Talk with the server so he dont forget us.
    if(client.loop() == false)
    {
        client.connect(project_name);
    }

    //But only send data every minute or so
    // 6 ->   6*10s =  60s = 1min
    //12 -> 2*6*10s = 120s = 2min
    //18 -> 3*6*10s = 180s = 3min
    //BUT since the delay is not that accurate, 
    // 18 is more like 3.5 to 4 minutes in real life...
    if(updateCount > 18)
    {
        updateCount = 0;
        char str[30];

        snprintf(str, 30, "energy=%u.%03u kWh", pulseCount1_kWh, pulseCount1_Wh);
        if(client.connected())
        {
            client.publish(topic_meter01, str);
        }

        snprintf(str, 30, "energy=%u.%03u kWh", pulseCount2_kWh, pulseCount2_Wh);
        if(client.connected())
        {
            client.publish(topic_meter02, str);
        }
    }

    updateCount++; 
    delay(10000); // 10*1000ms = 10s
}
