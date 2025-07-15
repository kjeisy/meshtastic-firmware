/*
 * Analog Moisture Sensor Support (Grove or compatible)
 *
 * Usage:
 *   - Connect a Grove Moisture Sensor (or compatible analog sensor) to an available analog pin.
 *   - Enable the sensor in configuration.h or your board's variant.h:
 *       #define ANALOG_MOISTURE_SENSOR_ENABLED 1
 *   - Set the analog pin:
 *       #define ANALOG_MOISTURE_PIN <pin> // e.g., 1 for GPIO1 on ESP32, or A0/A1/A2 on nRF52
 *   - Optional: Calibrate for your soil type:
 *       #define ANALOG_MOISTURE_DRY_VALUE 300
 *       #define ANALOG_MOISTURE_HUMID_VALUE 700
 *       #define ANALOG_MOISTURE_WET_VALUE 950
 *   - Optional: Adjust sampling:
 *       #define ANALOG_MOISTURE_SAMPLES 10
 *
 * Calibration:
 *   - Dry soil: 0-300 ADC value = 0-30% moisture
 *   - Humid soil: 300-700 ADC value = 30-70% moisture
 *   - Wet soil: 700-950 ADC value = 70-100% moisture
 *   - Adjust values as needed for your sensor/soil.
 */
#include "configuration.h"

#if !MESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/meshtastic/telemetry.pb.h"
#include "TelemetrySensor.h"

class AnalogMoistureSensor : public TelemetrySensor
{
  private:
    uint8_t moisturePin;
    uint16_t dryValue;
    uint16_t humidValue;
    uint16_t wetValue;
    uint8_t samples;
    bool initialized;

  protected:
    virtual void setup() override;

  public:
    AnalogMoistureSensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(meshtastic_Telemetry *measurement) override;

    // Reading methods
    uint16_t readRawMoisture();
    uint8_t rawMoistureToPercentage(uint16_t rawMoisture);
};

#endif