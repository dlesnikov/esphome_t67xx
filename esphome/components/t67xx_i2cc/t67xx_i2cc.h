#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

// TODO: common with uart?
enum {
  T67XX_REG_FW     = 0x1389,
  T67XX_REG_STATUS = 0x138A,
  T67XX_REG_PPM    = 0x138B,
  T67XX_REG_CAL    = 0x03EC,
  T67XX_REG_ABC    = 0x03EE,
  T67XX_REG_MOD    = 0x100B  // Measure on demand
};

#define T67XX_READ_DELAY 10
#define T67XX_WRITE_DELAY 10

namespace esphome {
namespace t67xx_i2cc {

class T67xxI2CComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void set_co2_sensor(sensor::Sensor *co2_sensor) { co2_sensor_ = co2_sensor; }
  void set_fw_rev(sensor::Sensor *fw_rev) { fw_rev_ = fw_rev; }
  void set_status(sensor::Sensor *status) { status_ = status; }

  void update() override;
  void dump_config() override;

 protected:
  uint16_t t67xx_fw_ver = 0;
  bool write_cal_scheduled = false;
  sensor::Sensor *co2_sensor_{nullptr};
  sensor::Sensor *fw_rev_{nullptr};
  sensor::Sensor *status_{nullptr};

  uint16_t readPPM(i2c::ErrorCode& err);
  uint16_t readStatus(i2c::ErrorCode& err);
  uint16_t readFwVer(i2c::ErrorCode& err);
  uint16_t readMoD(i2c::ErrorCode& err);
  uint16_t readIRegister(const uint16_t addr, i2c::ErrorCode& err); // Read Input register
  uint16_t readHRegister(const uint16_t addr, i2c::ErrorCode& err); // Read Holding register
  uint16_t _readRegister(const uint16_t addr, uint8_t fc, i2c::ErrorCode& err);
  void writeRegister(const uint16_t addr, const uint16_t value, i2c::ErrorCode& err);
};

}  // namespace t67xx_i2c
}  // namespace esphome