/**
 **********************************************************************************
 * @file   sht3x.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  SHT3x series sensor handling
 *         Functionalities of the this file:
 *          + Read Temperature in Raw data, Celsius and Fahrenheit
 *          + Read Humidity in Raw data and percentage
 *          + Control internal heater
 **********************************************************************************
 *
 * Copyright (c) 2021 Mahda Embedded System (MIT License)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************
 */

/* Includes ---------------------------------------------------------------------*/
#include "SHT3x.h"



/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  I2C device addresses
 */
#define SHT3X_I2C_ADDRESS_A   0x44
#define SHT3X_I2C_ADDRESS_B   0x45


/**
 * @brief  Measurement commands in single shot mode
 */
#define SHT3X_COMMAND_SINGLESHOT_ENABLE_MSB           0x2C
#define SHT3X_COMMAND_SINGLESHOT_ENABLE_HIGH_LSB      0x06
#define SHT3X_COMMAND_SINGLESHOT_ENABLE_MEDIUM_LSB    0x0D
#define SHT3X_COMMAND_SINGLESHOT_ENABLE_LOW_LSB       0x10
#define SHT3X_COMMAND_SINGLESHOT_DISABLE_MSB          0x24
#define SHT3X_COMMAND_SINGLESHOT_DISABLE_HIGH_LSB     0x00
#define SHT3X_COMMAND_SINGLESHOT_DISABLE_MEDIUM_LSB   0x0B
#define SHT3X_COMMAND_SINGLESHOT_DISABLE_LOW_LSB      0x16

/**
 * @brief  Measurement commands for periodic data acquisition mode
 */
#define SHT3X_COMMAND_PERIODIC_05MPS_MSB          0x20
#define SHT3X_COMMAND_PERIODIC_05MPS_HIGH_LSB     0x32
#define SHT3X_COMMAND_PERIODIC_05MPS_MEDIUM_LSB   0x24
#define SHT3X_COMMAND_PERIODIC_05MPS_LOW_LSB      0x2F
#define SHT3X_COMMAND_PERIODIC_1MPS_MSB           0x21
#define SHT3X_COMMAND_PERIODIC_1MPS_HIGH_LSB      0x30
#define SHT3X_COMMAND_PERIODIC_1MPS_MEDIUM_LSB    0x26
#define SHT3X_COMMAND_PERIODIC_1MPS_LOW_LSB       0x2D
#define SHT3X_COMMAND_PERIODIC_2MPS_MSB           0x22
#define SHT3X_COMMAND_PERIODIC_2MPS_HIGH_LSB      0x36
#define SHT3X_COMMAND_PERIODIC_2MPS_MEDIUM_LSB    0x20
#define SHT3X_COMMAND_PERIODIC_2MPS_LOW_LSB       0x2B
#define SHT3X_COMMAND_PERIODIC_4MPS_MSB           0x23
#define SHT3X_COMMAND_PERIODIC_4MPS_HIGH_LSB      0x34
#define SHT3X_COMMAND_PERIODIC_4MPS_MEDIUM_LSB    0x22
#define SHT3X_COMMAND_PERIODIC_4MPS_LOW_LSB       0x29
#define SHT3X_COMMAND_PERIODIC_10MPS_MSB          0x27
#define SHT3X_COMMAND_PERIODIC_10MPS_HIGH_LSB     0x37
#define SHT3X_COMMAND_PERIODIC_10MPS_MEDIUM_LSB   0x21
#define SHT3X_COMMAND_PERIODIC_10MPS_LOW_LSB      0x2A

/**
 * @brief  Command for a periodic data acquisition with the ART feature
 */
#define SHT3X_COMMAND_ART_MSB   0x2B
#define SHT3X_COMMAND_ART_LSB   0x32

/**
 * @brief  Break command
 */
#define SHT3X_COMMAND_STOP_PERIODIC_MSB   0x30
#define SHT3X_COMMAND_STOP_PERIODIC_LSB   0x93

/**
 * @brief  Soft reset command
 */
#define SHT3X_COMMAND_SOFT_RESET_MSB  0x30
#define SHT3X_COMMAND_SOFT_RESET_LSB  0xA2

/**
 * @brief  Heater commands
 */
#define SHT3X_COMMAND_HEATER_ENABLE_MSB     0x30
#define SHT3X_COMMAND_HEATER_ENABLE_LSB     0x6D
#define SHT3X_COMMAND_HEATER_DISABLE_MSB    0x30
#define SHT3X_COMMAND_HEATER_DISABLE_LSB    0x66

/**
 * @brief  Status commands
 */
#define SHT3X_COMMAND_STATUS_READ_MSB     0xF3
#define SHT3X_COMMAND_STATUS_READ_LSB     0x2D
#define SHT3X_COMMAND_STATUS_CLEAR_MSB    0x30
#define SHT3X_COMMAND_STATUS_CLEAR_LSB    0x41



/**
 ==================================================================================
                       ##### Private Functions #####                               
 ==================================================================================
 */

void
SHT3x_ConvSample(SHT3x_Sample_t *Sample)
{
  float TempBuff = 0;

  Sample->HumidityPercent = (Sample->HumRaw / 65535.0) * 100;

  TempBuff = Sample->TempRaw / 65535.0;
  Sample->TempCelsius = (TempBuff * 175) - 45;
  Sample->TempFahrenheit = (TempBuff * 315) - 49;
}



/**
 ==================================================================================
                  ##### Public Measurement Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Readout of Measurement Results at Single Shot Mode
 * @param  Handler: Pointer to handler
 * @param  Sample: Pointer to sample structure
 * @param  Repeatability: Specify repeatability level. (See Datasheet for more 
 *                        information)
 *                        The repeatability level can be set to 3 levels:
 *         - 0: Low
 *         - 1: Medium
 *         - 2: High
 * 
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 *         - SHT3x_INVALID_PARAM: One of parameters is invalid.
 *         - SHT3x_CRC_ERROR: CRC check error.
 */
SHT3x_Result_t
SHT3x_ReadSample_SingleShot(SHT3x_Handler_t *Handler,
                            SHT3x_Sample_t *Sample,
                            uint8_t Repeatability)
{
  uint8_t cmd[2];
  uint8_t Buffer[6];

  if (SHT3X_CLOCK_STRETCHING)
  {
    cmd[0] = SHT3X_COMMAND_SINGLESHOT_ENABLE_MSB;
    switch (Repeatability)
    {
    case 0:
      cmd[1] = SHT3X_COMMAND_SINGLESHOT_ENABLE_LOW_LSB;
      break;
    
    case 1:
      cmd[1] = SHT3X_COMMAND_SINGLESHOT_ENABLE_MEDIUM_LSB;
      break;

    case 2:
      cmd[1] = SHT3X_COMMAND_SINGLESHOT_ENABLE_HIGH_LSB;
      break;

    default:
      return SHT3x_INVALID_PARAM;
      break;
    }
  }
  else
  {
    cmd[0] = SHT3X_COMMAND_SINGLESHOT_DISABLE_MSB;
    switch (Repeatability)
    {
    case 0:
      cmd[1] = SHT3X_COMMAND_SINGLESHOT_DISABLE_LOW_LSB;
      break;
    
    case 1:
      cmd[1] = SHT3X_COMMAND_SINGLESHOT_DISABLE_MEDIUM_LSB;
      break;

    case 2:
      cmd[1] = SHT3X_COMMAND_SINGLESHOT_DISABLE_HIGH_LSB;
      break;

    default:
      return SHT3x_INVALID_PARAM;
      break;
    }
  }

  if (Handler->PlatformSend(Handler->AddressI2C, cmd, 2) != 0)
    return SHT3x_FAIL;

  for (uint8_t Counter = 0; Counter < 20; Counter++)
  {
    if (Handler->PlatformReceive(Handler->AddressI2C, Buffer, 6) == 0)
      break;
    
    Handler->PlatformDelay(1);
  }

  Sample->TempRaw = (Buffer[0] << 8) | Buffer[1];
  Sample->HumRaw = (Buffer[3] << 8) | Buffer[4];

  if (Handler->PlatformCRC(Sample->TempRaw, Buffer[2]) != 0)
    return SHT3x_CRC_ERROR;
  
  if (Handler->PlatformCRC(Sample->HumRaw, Buffer[5]) != 0)
    return SHT3x_CRC_ERROR;

  SHT3x_ConvSample(Sample);

  return SHT3x_OK;
}



/**
 ==================================================================================
               ##### Public Control and Status Functions #####                     
 ==================================================================================
 */

/**
 * @brief  Initializer function
 * @param  Handler: Pointer to handler
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_Init(SHT3x_Handler_t *Handler)
{
  SHT3x_SetAddressI2C(Handler , 0);
  if (Handler->PlatformInit() != 0)
    return SHT3x_FAIL;
  return SHT3x_OK;
}

/**
 * @brief  Deinitialize function
 * @param  Handler: Pointer to handler
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_DeInit(SHT3x_Handler_t *Handler)
{
  if (Handler->PlatformDeInit() != 0)
    return SHT3x_FAIL;
  return SHT3x_OK;
}


/**
 * @brief  Initializer function
 * @param  Handler: Pointer to handler
 * @param  Address: The address depends on ADDR pin state. You should use one of
 *                  this options:
 *         - 0: This address used when ADDR is connected VSS
 *         - 1: This address used when ADDR is connected VDD
 * 
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 */
SHT3x_Result_t
SHT3x_SetAddressI2C(SHT3x_Handler_t *Handler, uint8_t Address)
{
  if (Address == 0)
    Handler->AddressI2C = SHT3X_I2C_ADDRESS_A;
  else
    Handler->AddressI2C = SHT3X_I2C_ADDRESS_B;

  return SHT3x_OK;
}


/**
 * @brief  Read out the status register 
 * @param  Handler: Pointer to handler
 * @param  Status: pointer to status register 
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 *         - SHT3x_CRC_ERROR: CRC check error.
 */
SHT3x_Result_t
SHT3x_ReadStatus(SHT3x_Handler_t *Handler, uint16_t *Status)
{
  uint8_t cmd[2];
  uint8_t Buffer[3];

  cmd[0] = SHT3X_COMMAND_STATUS_READ_MSB;
  cmd[1] = SHT3X_COMMAND_STATUS_READ_LSB;
  if (Handler->PlatformSend(Handler->AddressI2C, cmd, 2) != 0)
    return SHT3x_FAIL;

  if (Handler->PlatformReceive(Handler->AddressI2C, Buffer, 3) != 0)
    return SHT3x_FAIL;

  *Status = (Buffer[0]<<8) | Buffer[1];
  if (Handler->PlatformCRC(*Status, Buffer[2]) != 0)
    return SHT3x_CRC_ERROR;

  return SHT3x_OK;
}


/**
 * @brief  Clear status register
 * @param  Handler: Pointer to handler
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_ClearStatus(SHT3x_Handler_t *Handler)
{
  uint8_t cmd[2];

  cmd[0] = SHT3X_COMMAND_STATUS_CLEAR_MSB;
  cmd[1] = SHT3X_COMMAND_STATUS_CLEAR_LSB;
  if (Handler->PlatformSend(Handler->AddressI2C, cmd, 2) != 0)
    return SHT3x_FAIL;
  
  return SHT3x_OK;
}


/**
 * @brief  Turn ON and OFF internal heater
 * @param  Handler: Pointer to handler
 * @param  Heater: 
 *         - 0: Disable heater
 *         - 1: Enable heater
 * 
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_SetHeater(SHT3x_Handler_t *Handler, uint8_t Heater)
{
  uint8_t cmd[2];

  if(Heater)
  {
    cmd[0] = SHT3X_COMMAND_HEATER_ENABLE_MSB;
    cmd[1] = SHT3X_COMMAND_HEATER_ENABLE_LSB;
  }
  else
  {
    cmd[0] = SHT3X_COMMAND_HEATER_DISABLE_MSB;
    cmd[1] = SHT3X_COMMAND_HEATER_DISABLE_LSB;
  }

  if (Handler->PlatformSend(Handler->AddressI2C, cmd, 2) != 0)
    return SHT3x_FAIL;
  
  return SHT3x_OK;
}
