#include "t67xx_i2cc.h"
#include "esphome.h"
#include "esphome/core/log.h"

// #define T67XX_DEBUG 1

namespace esphome {
namespace t67xx_i2cc {

static const char *const TAG = "t67xx_i2cc";
static bool continuous_calibrate = false;

void T67xxI2CComponent::update()
{
  i2c::ErrorCode err;
  static uint8_t abc_logic_enabled = 0;
  status_clear_warning();

  if (abc_logic_enabled == 0)
  {
    abc_logic_enabled++;
    t67xx_fw_ver = readFwVer(err);
    ESP_LOGD(TAG, "T67XX FW ver 0x%04x, err %u", t67xx_fw_ver, err);
    if (fw_rev_ != nullptr)
    {
      fw_rev_->publish_state(t67xx_fw_ver);
    }

    return;
  }
  else if (abc_logic_enabled < 15)
  {
    abc_logic_enabled++;
  }
  else if (abc_logic_enabled == 15)
  {
    abc_logic_enabled++;
    writeRegister(T67XX_REG_ABC, 0xFF00, err);
    ESP_LOGD(TAG, "T67XX enable ABC Logic™, err %u", err);

    /*
    delay(T67XX_READ_DELAY);
    uint16_t u16 = readMoD(err);
    ESP_LOGD(TAG, "T67XX Measure on Demand 0x%04x, err %u", u16, err);
    */

    return;
  }

  if (write_cal_scheduled)
  {
    write_cal_scheduled = false;
    writeRegister(T67XX_REG_CAL, 0xFF00, err);
    ESP_LOGD(TAG, "T67XX start calibration, err %u", err);

    return;
  }

  i2c::ErrorCode err_ppm;
  i2c::ErrorCode err_status;
  const uint16_t ppm = readPPM(err_ppm);
  const uint16_t status = readStatus(err_status);

  ESP_LOGD(TAG, "T67XX I2C Received CO₂=%uppm, err %u, Status=0x%04x, err %u", ppm, err_ppm, status, err_status);
  if (co2_sensor_ != nullptr)
  {
    co2_sensor_->publish_state(ppm);
  }
  if (status_ != nullptr)
  {
    status_->publish_state(status);
  }
}

void T67xxI2CComponent::dump_config(void)
{
  ESP_LOGCONFIG(TAG, "Telaire:");
  LOG_SENSOR("  ", "CO2", co2_sensor_);
}

uint16_t T67xxI2CComponent::readPPM(i2c::ErrorCode& err)
{
  static uint32_t ppm_bad_cnt = 0;
  uint16_t ppm = readIRegister(T67XX_REG_PPM, err);

  if (continuous_calibrate || ppm > 10000)
  {
    ppm_bad_cnt++;
    if (ppm_bad_cnt == 6 || (ppm_bad_cnt % 108) == 0)
    {
#ifdef T67XX_DEBUG
      ESP_LOGD(TAG, "ppm_bad_cnt %u", ppm_bad_cnt);
#endif
      write_cal_scheduled = true;
    }
  }
  else
  {
    ppm_bad_cnt = 0;
  }

  return ppm;
};

uint16_t T67xxI2CComponent::readStatus(i2c::ErrorCode& err)
{
  uint16_t status = readIRegister(T67XX_REG_STATUS, err);

  return status;
};

uint16_t T67xxI2CComponent::readFwVer(i2c::ErrorCode& err)
{
  uint16_t status = readIRegister(T67XX_REG_FW, err);

  return status;
};

uint16_t T67xxI2CComponent::readMoD(i2c::ErrorCode& err)
{
  uint16_t status = readHRegister(T67XX_REG_MOD, err);

  return status;
};

uint16_t T67xxI2CComponent::readIRegister(const uint16_t addr, i2c::ErrorCode& err)
{
  return _readRegister(addr, 4 /* read input registers */, err);
}

uint16_t T67xxI2CComponent::readHRegister(const uint16_t addr, i2c::ErrorCode& err)
{
  return _readRegister(addr, 3 /* read holding registers */, err);
}

uint16_t T67xxI2CComponent::_readRegister(const uint16_t addr, uint8_t fc, i2c::ErrorCode& err)
{
  uint16_t ret = 0;
  uint8_t sendBuffer[5] = {
     fc,                // Function code
     byte(addr >> 8),   // Addr MSB
     byte(addr & 0xFF), // Addr LSB
     0x00,              // Registers count MSB
     0x01               // Registers count LSB
  };

  err = write(sendBuffer, 5);

  if (err == i2c::NO_ERROR)
  {
    delay(T67XX_READ_DELAY);

    uint8_t readBuffer[4] = {};
    err = read(readBuffer, 4);

#ifdef T67XX_DEBUG
    ESP_LOGD(TAG, "T67XX read reg 0x%04x = %02x %02x %02x %02x",
             addr,
             readBuffer[0],
             readBuffer[1],
             readBuffer[2],
             readBuffer[3]);
#endif

    ret = ((uint16_t(readBuffer[2]) << 8) | readBuffer[3]);
  }

  return ret;
}

void T67xxI2CComponent::writeRegister(const uint16_t addr, const uint16_t value, i2c::ErrorCode& err)
{
  uint8_t sendBuffer[5] = {
     0x05,              // Write register
     byte(addr >> 8),   // Addr MSB
     byte(addr & 0xFF), // Addr LSB
     byte(value >> 8),  // Value MSB
     byte(value & 0xFF) // Value LSB
  };

  err = write(sendBuffer, 5);

  if (err == i2c::NO_ERROR)
  {
    delay(T67XX_WRITE_DELAY);

    uint8_t readBuffer[5] = {};
    err = read(readBuffer, 5);

#ifdef T67XX_DEBUG
    ESP_LOGD(TAG, "T67XX write reg 0x%04x val 0x%04x = %02x %02x %02x %02x %02x",
             addr,
             value,
             readBuffer[0],
             readBuffer[1],
             readBuffer[2],
             readBuffer[3],
             readBuffer[4]);
#endif
  }
}

}  // namespace t67xx_i2cc
}  // namespace esphome