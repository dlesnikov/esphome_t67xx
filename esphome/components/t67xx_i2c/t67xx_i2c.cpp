#include "t67xx_i2c.h"
#include "esphome.h"
#include "esphome/core/log.h"

namespace esphome {
namespace t67xx_i2c {

static const char *const TAG = "t67xx_i2c";

void T67xxI2CComponent::update() {
  this->status_clear_warning();
  const uint16_t status = this->readStatus();
  const uint16_t ppm = this->readPPM();

  ESP_LOGD(TAG, "T67XX I2C Received CO₂=%uppm Status=%u", ppm, status);
  if (this->co2_sensor_ != nullptr)
    this->co2_sensor_->publish_state(ppm);
}

void T67xxI2CComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Telaire:");
  LOG_SENSOR("  ", "CO2", this->co2_sensor_);
}

uint16_t T67xxI2CComponent::readPPM(void)
{
    uint16_t ppm = this->readRegister(T67XX_REG_PPM);
    return ppm;
};

uint16_t T67xxI2CComponent::readStatus(void)
{
    uint16_t status = this->readRegister(T67XX_REG_STATUS);
    return status;
};

uint16_t T67xxI2CComponent::readRegister(uint16_t addr)
{
  uint8_t sendBuffer[5] = {
     0x04,              // Read register
     byte(addr >> 8),   // Addr MSB
     byte(addr & 0xFF), // Addr LSB
     0x00,              // Registers count MSB
     0x01               // Registers count LSB
  };

  this->write(sendBuffer, 5);

  delay(T67XX_READ_DELAY);

  uint8_t readBuffer[4];
  this->read(readBuffer, 4);

#ifdef T67XX_DEBUG
  Serial.print("T67XX: Returned function = ");
  Serial.println(readBuffer[0]);
  Serial.print("T67XX: Returned byte count = ");
  Serial.println(readBuffer[1]);
  Serial.print("T67XX: Returned Value MSB = ");
  Serial.println(readBuffer[2]);
  Serial.print("T67XX: Returned Value LSB = ");
  Serial.println(readBuffer[3]);
#endif
  return ((readBuffer[2] << 8) | readBuffer[3]);
}

}  // namespace t67xx_i2c
}  // namespace esphome