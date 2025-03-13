import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, i2c
from esphome.const import (
    CONF_CO2,
    CONF_STATUS,
    CONF_VERSION,
    CONF_ID,
    ICON_MOLECULE_CO2,
    ICON_EMPTY,
    DEVICE_CLASS_CARBON_DIOXIDE,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_NONE,
    UNIT_PARTS_PER_MILLION,
    UNIT_EMPTY,
)

DEPENDENCIES = ["i2c"]

t67xx_i2cc_ns = cg.esphome_ns.namespace("t67xx_i2cc")
T67xxI2CComponent = t67xx_i2cc_ns.class_(
    "T67xxI2CComponent", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(T67xxI2CComponent),
            cv.Required(CONF_CO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_MOLECULE_CO2,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CARBON_DIOXIDE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Required(CONF_STATUS): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                icon=ICON_EMPTY,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_NONE,
            ),
            cv.Required(CONF_VERSION): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                icon=ICON_EMPTY,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_NONE,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x15))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_CO2 in config:
        sens = await sensor.new_sensor(config[CONF_CO2])
        cg.add(var.set_co2_sensor(sens))

    if CONF_STATUS in config:
        sens = await sensor.new_sensor(config[CONF_STATUS])
        cg.add(var.set_status(sens))

    if CONF_VERSION in config:
        sens = await sensor.new_sensor(config[CONF_VERSION])
        cg.add(var.set_fw_rev(sens))
