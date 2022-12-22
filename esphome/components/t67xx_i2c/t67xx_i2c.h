#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

// TODO: common with uart?
enum {
  T67XX_REG_STATUS = 0x138A,
  T67XX_REG_PPM = 0x138B
};

#define T67XX_READ_DELAY 10
#define T67XX_MEASURE_DELAY 2250

namespace esphome {
namespace t67xx_i2c {

class T67xxI2CComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void set_co2_sensor(sensor::Sensor *co2_sensor) { co2_sensor_ = co2_sensor; }

  void update() override;
  void dump_config() override;

 protected:
  sensor::Sensor *co2_sensor_{nullptr};
  uint16_t readPPM(void);
  uint16_t readStatus(void);
  uint16_t readRegister(uint16_t addr);
};

}  // namespace t67xx_i2c
}  // namespace esphome