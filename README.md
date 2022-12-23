# ESPHome Telaire T67xx sensors component

## Features

Supported sensor modules:
- T6703 (Tested)
- T6713
- T6723

Supported interfaces:
- I<sup>2</sup>C (Wire)
  
Supported sensors:
- CO2

## Examples

### Simple example configuration entry

```yaml
# use default i2c settings for your platform/board
i2c:

# append this component from github
external_components:
  - source: github://yellow-sky/esphome_t67xx
    components: [ t67xx_i2c ]

# define co2 sensor
sensor:
  platform: t67xx_i2c
  co2:
    name: "T6703 CO2 Value"
    id: T6703_co2
  update_interval: 10s



```

## Datasheet links
- https://f.hubspotusercontent40.net/hubfs/9035299/Documents/AAS-916-142A-Telaire-T67xx-CO2-Sensor-022719-web.pdf  
- https://www.mouser.com/catalog/additional/Telaire_T6700_Series_CO2_sensor_Module_Application_Note.pdf  