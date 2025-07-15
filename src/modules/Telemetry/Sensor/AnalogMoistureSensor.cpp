/*
 * Analog Moisture Sensor Implementation
 */
#include "configuration.h"

#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/meshtastic/telemetry.pb.h"
#include "AnalogMoistureSensor.h"
#include "power.h"

// All configuration is handled via ANALOG_MOISTURE_... macros in configuration.h or variant.h

AnalogMoistureSensor::AnalogMoistureSensor()
    : TelemetrySensor(meshtastic_TelemetrySensorType_SENSOR_UNSET, "AnalogMoisture"), moisturePin(ANALOG_MOISTURE_PIN),
      dryValue(ANALOG_MOISTURE_DRY_VALUE), humidValue(ANALOG_MOISTURE_HUMID_VALUE), wetValue(ANALOG_MOISTURE_WET_VALUE),
      samples(ANALOG_MOISTURE_SAMPLES), initialized(false)
{
}

int32_t AnalogMoistureSensor::runOnce()
{
    if (!initialized) {
        LOG_INFO("Init sensor: %s", sensorName);

        // Configure the analog pin
        pinMode(moisturePin, INPUT);

        // Test if we can read from the pin
        uint16_t testReading = analogRead(moisturePin);
        if (testReading != 0 || testReading != 4095) { // Valid ADC reading
            status = true;
            initialized = true;
            LOG_INFO("Analog moisture sensor initialized on pin %d", moisturePin);
        } else {
            status = false;
            LOG_ERROR("Failed to initialize analog moisture sensor on pin %d", moisturePin);
        }
    }

    return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
}

void AnalogMoistureSensor::setup() {}

bool AnalogMoistureSensor::getMetrics(meshtastic_Telemetry *measurement)
{
    if (!status || !initialized) {
        return false;
    }

    // Read moisture percentage
    uint16_t rawMoisture = readRawMoisture();
    uint8_t moisturePercent = rawMoistureToPercentage(rawMoisture);

    // Set the moisture data in the telemetry structure
    // Note: We'll use the soil_moisture field which is already defined in the protobuf
    measurement->variant.environment_metrics.has_soil_moisture = true;
    measurement->variant.environment_metrics.soil_moisture = moisturePercent;

    LOG_DEBUG("Moisture: %d%% (%d)", moisturePercent, rawMoisture);

    return true;
}

uint16_t AnalogMoistureSensor::readRawMoisture()
{
    uint32_t total = 0;

    // Take multiple samples and average them
    for (uint8_t i = 0; i < samples; i++) {
        total += analogRead(moisturePin);
        delay(10); // Small delay between readings
    }

    return total / samples;
}

uint8_t AnalogMoistureSensor::rawMoistureToPercentage(uint16_t rawMoisture)
{
    // Convert raw ADC value to moisture percentage based on Grove calibration
    uint8_t percentage;

    if (rawMoisture <= dryValue) {
        // Dry soil: 0-300 ADC value = 0-30%
        percentage = map(rawMoisture, 0, dryValue, 0, 30);
    } else if (rawMoisture <= humidValue) {
        // Humid soil: 300-700 ADC value = 30-70%
        percentage = map(rawMoisture, dryValue, humidValue, 30, 70);
    } else if (rawMoisture <= wetValue) {
        // Wet soil: 700-950 ADC value = 70-100%
        percentage = map(rawMoisture, humidValue, wetValue, 70, 100);
    } else {
        // Above wet threshold = 100%
        percentage = 100;
    }

    // Ensure percentage is within valid range
    if (percentage > 100)
        percentage = 100;

    return percentage;
}

#endif