CONFIG += qtestlib debug
TEMPLATE = app
TARGET = 
DEFINES += private=public

# Test code
DEPENDPATH += .
INCLUDEPATH += .
SOURCES += TestElectricityMeter.cpp

# Code to test
DEPENDPATH  += ../../FunTechHouse_ElectricityMeter/
INCLUDEPATH += ../../FunTechHouse_ElectricityMeter/
SOURCES += ElectricityMeter.cpp
SOURCES += MQTT_Logic.cpp

