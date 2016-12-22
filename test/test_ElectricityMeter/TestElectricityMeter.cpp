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

#define SIZE 30
    char str[SIZE];

    for( int i=0 ; i<999 ; i++ )
    {
        m.pulse();
    }

    // 0.999 kWh
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)0);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)999);
    QCOMPARE(m.getValue(str, SIZE), true);
    QCOMPARE("energy=0.999kWh", str);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;

    // 1.000 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)0);
    //qDebug() << m.pulseCount_kWh << m.pulseCount_Wh;
    QCOMPARE(m.getValue(str, SIZE), true);
    QCOMPARE("energy=1.000kWh", str);

    // 1.001 kWh
    m.pulse();
    QCOMPARE((unsigned int)m.pulseCount_kWh,(unsigned int)1);
    QCOMPARE((unsigned int)m.pulseCount_Wh, (unsigned int)1);
    QCOMPARE(m.getValue(str, SIZE), true);
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

QTEST_MAIN(TestElectricityMeter)
#include "TestElectricityMeter.moc"
