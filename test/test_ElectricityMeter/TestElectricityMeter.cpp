#include <QtCore>
#include <QtTest>

#include "ElectricityMeter.h"

class TestElectricityMeter : public QObject
{
    Q_OBJECT

    private:
    public:

    private slots:
        void test_add_500();
        void test_add_1000();

        void test_valueTimeToSend01();
        void test_valueTimeToSend02();
};

/**
 * Add values, 500 puls/impl
 */
void TestElectricityMeter::test_add_500()
{
    ElectricityMeter m(ELECTRICITYMETER_IMPL_PER_KWH_500);

    for( int i=0 ; i<499 ; i++ )
    {
        m.pulse();
    }

    // 0.998 kWh
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)0);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)998);
    //qdebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 1.000 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)0);
    //qdebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 1.002 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)2);
    //qdebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    for( int i=0 ; i<498 ; i++ )
    {
        m.pulse();
    }

    // 1.998 kWh
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)998);
    //qdebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 2.000 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)2);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)0);
    //qdebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 2.002 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)2);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)2);
    //qdebug() << m.pulseCount_kWh << m.pulseCount_Wh;
}

/**
 * Add values, 1000 puls/impl
 */
void TestElectricityMeter::test_add_1000()
{
    ElectricityMeter m(ELECTRICITYMETER_IMPL_PER_KWH_1000);

#define SIZE_TEST1000 30
    char str[SIZE_TEST1000];

    for( int i=0 ; i<999 ; i++ )
    {
        m.pulse();
    }

    // 0.999 kWh
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)0);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)999);
    QCOMPARE(m.getValue(str, SIZE_TEST1000), true);
    QCOMPARE("energy=0.999kWh", str);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 1.000 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)0);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;
    QCOMPARE(m.getValue(str, SIZE_TEST1000), true);
    QCOMPARE("energy=1.000kWh", str);

    // 1.001 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)1);
    QCOMPARE(m.getValue(str, SIZE_TEST1000), true);
    QCOMPARE("energy=1.001kWh", str);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    for( int i=0 ; i<998 ; i++ )
    {
        m.pulse();
    }

    // 1.999 kWh
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)999);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 2.000 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)2);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)0);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 2.001 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)2);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)1);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;
}

/**
 * This tests focuses on the getValue and getLastValue
 */
void TestElectricityMeter::test_valueTimeToSend01()
{
#define SIZE_TIME2SEND 30
    char str[SIZE_TIME2SEND];
    ElectricityMeter m(ELECTRICITYMETER_IMPL_PER_KWH_1000);

    //Since we need to send a value the next time
    //we ask to get the 3 points.
    // 1. Before change, old value, old time
    // 2. Change, new value, current time
    // 3. After change, new value, current time (since we do this later)

    //Init mode, no new data.
    QCOMPARE(false, m.isItNextTime());

    QCOMPARE(false, m.newValue());
    QCOMPARE(false, m.newValue());

    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.000kWh", str);
    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.000kWh", str);

    //First puls
    // last value is still 0
    // newValue should tell there is new data.
    m.pulse();
    QCOMPARE(false, m.isItNextTime());

    QCOMPARE(true,  m.newValue());
    QCOMPARE(true,  m.newValue());

    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.000kWh", str);
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.001kWh", str);

    QCOMPARE(true,  m.newValue());
    QCOMPARE(true,  m.newValue());

    //If we managed to send the value to the server
    //save current value as last value.
    m.saveValue();
    QCOMPARE(true,  m.isItNextTime());
    QCOMPARE(false, m.isItNextTime());

    QCOMPARE(false, m.newValue());
    QCOMPARE(false, m.newValue());

    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.001kWh", str);
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.001kWh", str);

    //Next pulse, should be new data
    m.pulse();
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    QCOMPARE(true,  m.newValue());

    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.001kWh", str);
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.002kWh", str);


    m.saveValue();
    QCOMPARE(true,  m.isItNextTime());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(false, m.newValue());
    QCOMPARE(false, m.newValue());

    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.002kWh", str);
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.002kWh", str);
    m.saveValue();

    for( int i=0 ; i<997 ; i++ )
    {
        m.pulse();
    }

    //Loop
    QCOMPARE(true, m.oldValue());
    QCOMPARE(true, m.newValue());
    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.002kWh", str);
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.999kWh", str);
    m.saveValue();

    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(true,  m.isItNextTime());
    QCOMPARE(false, m.newValue());

    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(false, m.newValue());

    m.pulse();

    //Loop
    QCOMPARE(true, m.oldValue());
    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=0.999kWh", str);
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=1.000kWh", str);
    m.saveValue();

    m.pulse();

    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(m.getLastValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=1.000kWh", str);
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    QCOMPARE(m.getValue(str, SIZE_TIME2SEND), true);
    QCOMPARE("energy=1.001kWh", str);
    m.saveValue();
}

/**
 * This test focuses on the state mach.
 */
void TestElectricityMeter::test_valueTimeToSend02()
{
    ElectricityMeter m(ELECTRICITYMETER_IMPL_PER_KWH_1000);

    //Init mode, no new data.

    //Part1 - A single impuls
    //Loop 1
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(false, m.newValue());

    m.pulse();

    //Loop 2
    QCOMPARE(true,  m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());

    //Since that was new value,
    //we mark that as sent and save the result.
    m.saveValue();

    //Loop 3
    QCOMPARE(false, m.oldValue());
    QCOMPARE(true,  m.isItNextTime());
    QCOMPARE(false, m.newValue());

	//Now nothing since there is nothing...
    //Loop 4
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(false, m.newValue());

    //Loop 5
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(false, m.newValue());


    //Part2 - Continous impulses, avoid duplicates
    m.pulse();

    //Loop
    QCOMPARE(true , m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    m.saveValue(); // Since new value

    m.pulse();
    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    m.saveValue(); // Since new value

    m.pulse();
    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    m.saveValue(); // Since new value

    m.pulse();
    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    m.saveValue(); // Since new value

    //Part3 - no more input

    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(true,  m.isItNextTime());
    QCOMPARE(false, m.newValue());

    for( int i=0 ; i<10 ; i++ )
    {
        //Loop
        QCOMPARE(false, m.oldValue());
        QCOMPARE(false, m.isItNextTime());
        QCOMPARE(false, m.newValue());
    }

    //Part4 - another impulse
    m.pulse();

    //Loop
    QCOMPARE(true , m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(true,  m.newValue());
    m.saveValue(); // Since new value

    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(true,  m.isItNextTime());
    QCOMPARE(false, m.newValue());

    //Loop
    QCOMPARE(false, m.oldValue());
    QCOMPARE(false, m.isItNextTime());
    QCOMPARE(false, m.newValue());
}

QTEST_MAIN(TestElectricityMeter)
#include "TestElectricityMeter.moc"
