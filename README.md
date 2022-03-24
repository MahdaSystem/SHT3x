# SHT3x Library
SHT3x series sensor driver.
- Read Temperature in Raw data, Celsius and Fahrenheit
- Read Humidity in Raw data and percentage
- Control internal heater

## Hardware Support
It is easy to port this library to any platform. But now it is ready for use in:
- STM32 (HAL)
- ESP32 (esp-idf)
- AVR (ATmega32)

## How To Use
1. Add `SHT3x.h` and `SHT3x.c` files to your project.  It is optional to use `SHT3x_platform.h` and `SHT3x_platform.c` files (open and config `SHT3x_platform.h` file).
2. Initialize platform-dependent part of handler.
4. Call `SHT3x_Init()`.
5. Call `SHT3x_SetAddressI2C()`.
6. Call other functions and enjoy.

## Example
<details>
<summary>Using SHT3x_platform files</summary>

```c
#include <stdio.h>
#include "SHT3x.h"
#include "SHT3x_platform.h"

int main(void)
{
  SHT3x_Handler_t Handler;
  SHT3x_Sample_t  Sample;

  SHT3x_Platform_Init(&Handler);
  SHT3x_Init(&Handler);
  SHT3x_SetAddressI2C(&Handler, 0);

  while (1)
  {
    SHT3x_ReadSample_SingleShot(&Handler, &Sample, 2);
    printf("Temperature: %f°C\r\n"
           "Humidity: %f%%\r\n\r\n",
           Sample.TempCelsius,
           Sample.HumidityPercent);
  }

  SHT3x_DeInit(&Handler);
  return 0;
}
```
</details>


<details>
<summary>Without using SHT3x_platform files (esp-idf)</summary>

```c
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "SHT3x.h"

#define SHT3X_I2C_NUM   I2C_NUM_1
#define SHT3X_I2C_RATE  100000
#define SHT3X_SCL_GPIO  GPIO_NUM_13
#define SHT3X_SDA_GPIO  GPIO_NUM_14

int8_t
SHT3x_Platform_Init(void)
{
  i2c_config_t conf = {0};
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = SHT3X_SDA_GPIO;
  conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
  conf.scl_io_num = SHT3X_SCL_GPIO;
  conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
  conf.master.clk_speed = SHT3X_I2C_RATE;
  if (i2c_param_config(SHT3X_I2C_NUM, &conf) != ESP_OK)
    return -1;
  if (i2c_driver_install(SHT3X_I2C_NUM, conf.mode, 0, 0, 0) != ESP_OK)
    return -1;
  return 0;
}

int8_t
SHT3x_Platform_DeInit(void)
{
  i2c_driver_delete(SHT3X_I2C_NUM);
  gpio_reset_pin(SHT3X_SDA_GPIO);
  gpio_reset_pin(SHT3X_SCL_GPIO);
  return 0;
}

int8_t
SHT3x_Platform_Send(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  i2c_cmd_handle_t SHT3x_i2c_cmd_handle = 0;
  Address <<= 1;
  Address &= 0xFE;

  SHT3x_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(SHT3x_i2c_cmd_handle);
  i2c_master_write(SHT3x_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_write(SHT3x_i2c_cmd_handle, Data, DataLen, 1);
  i2c_master_stop(SHT3x_i2c_cmd_handle);
  if (i2c_master_cmd_begin(SHT3X_I2C_NUM, SHT3x_i2c_cmd_handle, 1000 / portTICK_RATE_MS) != ESP_OK)
  {
    i2c_cmd_link_delete(SHT3x_i2c_cmd_handle);
    return -1;
  }
  i2c_cmd_link_delete(SHT3x_i2c_cmd_handle);
  return 0;
}

int8_t
SHT3x_Platform_Receive(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  i2c_cmd_handle_t SHT3x_i2c_cmd_handle = 0;
  Address <<= 1;
  Address |= 0x01;

  SHT3x_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(SHT3x_i2c_cmd_handle);
  i2c_master_write(SHT3x_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_read(SHT3x_i2c_cmd_handle, Data, DataLen, I2C_MASTER_LAST_NACK);
  i2c_master_stop(SHT3x_i2c_cmd_handle);
  if (i2c_master_cmd_begin(SHT3X_I2C_NUM, SHT3x_i2c_cmd_handle, 1000 / portTICK_RATE_MS) != ESP_OK)
  {
    i2c_cmd_link_delete(SHT3x_i2c_cmd_handle);
    return -1;
  }
  i2c_cmd_link_delete(SHT3x_i2c_cmd_handle);
  return 0;
}

int8_t
SHT3x_Platform_CRC(uint16_t Data, uint8_t DataCRC)
{
  return 0;
}

int8_t
SHT3x_Platform_Delay(uint8_t Delay)
{
  vTaskDelay(Delay / portTICK_PERIOD_MS);
  return 0;
}

int main(void)
{
  SHT3x_Handler_t Handler;
  SHT3x_Sample_t  Sample;

  Handler.PlatformInit    = SHT3x_Platform_Init;
  Handler.PlatformDeInit  = SHT3x_Platform_DeInit;
  Handler.PlatformSend    = SHT3x_Platform_Send;
  Handler.PlatformReceive = SHT3x_Platform_Receive;
  Handler.PlatformCRC     = SHT3x_Platform_CRC;
  Handler.PlatformDelay   = SHT3x_Platform_Delay;

  SHT3x_Init(&Handler);
  SHT3x_SetAddressI2C(&Handler, 0);

  while (1)
  {
    SHT3x_ReadSample_SingleShot(&Handler, &Sample, 2);
    printf("Temperature: %f°C\r\n"
           "Humidity: %f%%\r\n\r\n",
           Sample.TempCelsius,
           Sample.HumidityPercent);
  }

  SHT3x_DeInit(&Handler);
  return 0;
}
```
</details>
