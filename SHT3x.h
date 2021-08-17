/**
 **********************************************************************************
 * @file   sht3x.h
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

/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef	_SHT3X_H_
#define _SHT3X_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Exported Data Types ----------------------------------------------------------*/
/**
 * @brief  Handler data type
 * @note   User must initialize this this functions before using library:
 *         - PlatformInit
 *         - PlatformDeInit
 *         - PlatformSend
 *         - PlatformReceive
 *         - PlatformCRC
 *         - PlatformDelay
 * @note   If success the functions must return 0 
 */
typedef struct SHT3x_Handler_s
{
  uint8_t AddressI2C;

  // Initializes platform dependent part
  int8_t (*PlatformInit)(void);
  // De-initializes platform dependent part
  int8_t (*PlatformDeInit)(void);
  // Send Data to the slave with the address of Address. (0 <= Address <= 127)
  int8_t (*PlatformSend)(uint8_t Address, uint8_t *Data, uint8_t Len);
  // Receive Data from the slave with the address of Address. (0 <= Address <= 127)
  int8_t (*PlatformReceive)(uint8_t Address, uint8_t *Data, uint8_t Len);
  // Check CRC of Data (If you do not want to check CRC, this function must
  // allways return 0)
  int8_t (*PlatformCRC)(uint16_t Data, uint8_t DataCRC);
  // Delay in ms
  int8_t (*PlatformDelay)(uint8_t Delay);
} SHT3x_Handler_t;

/**
 * @brief  Library functions result data type
 */
typedef enum SHT3x_Result_e
{
  SHT3x_OK = 0,
  SHT3x_FAIL = 1,
  SHT3x_INVALID_PARAM = 2,
  SHT3x_CRC_ERROR = 3
} SHT3x_Result_t;

/**
 * @brief  Sample data type
 */
typedef struct SHT3x_Sample_s
{
  uint16_t  TempRaw;
  uint16_t  HumRaw;
  float     TempCelsius;
  float     TempFahrenheit;
  float     HumidityPercent;
} SHT3x_Sample_t;


/* Functionality Options --------------------------------------------------------*/
/**
 * @brief  When a command without clock stretching has been issued, the sensor
 *         responds to a read header with a not acknowledge (NACK), if no data
 *         is present.
 *         When a command with clock stretching has been issued, the sensor
 *         responds to a read header with an ACK and subsequently pulls down the
 *         SCL line. The SCL line is pulled down until the measurement is
 *         complete. As soon as the measurement is complete, the sensor releases
 *         the SCL line and sends the measurement results.
 *         You can set this constant in 2 levels:
 *         - 0: Disable clock stretching
 *         - 1: Enable clock stretching
 */
#define SHT3X_CLOCK_STRETCHING    0



/**
 ==================================================================================
                       ##### Measurement Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Readout of Measurement Results at Single Shot Mode
 * @param  Handler: Pointer to handler
 * @param  Sample: Pointer to sample structure
 * @param  Repeatability: Specify repeatability level (See Datasheet for more 
 *                        information). The repeatability level can be set to 3
 *                        levels:
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
                            uint8_t Repeatability);



/**
 ==================================================================================
                    ##### Control and Status Functions #####                       
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
SHT3x_Init(SHT3x_Handler_t *Handler);


/**
 * @brief  Deinitialize function
 * @param  Handler: Pointer to handler
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 */
SHT3x_Result_t
SHT3x_DeInit(SHT3x_Handler_t *Handler);


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
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_SetAddressI2C(SHT3x_Handler_t *Handler, uint8_t Address);


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
SHT3x_ReadStatus(SHT3x_Handler_t *Handler, uint16_t *Status);


/**
 * @brief  Clear status register
 * @param  Handler: Pointer to handler
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_ClearStatus(SHT3x_Handler_t *Handler);


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
SHT3x_SetHeater(SHT3x_Handler_t *Handler, uint8_t Heater);



#ifdef __cplusplus
}
#endif

#endif //! _SHT3X_H_
