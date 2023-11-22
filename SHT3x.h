/**
 **********************************************************************************
 * @file   SHT3x.h
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
 * @brief  Library functions result data type
 */
typedef enum SHT3x_Result_e
{
  SHT3x_OK            = 0,
  SHT3x_FAIL          = 1,
  SHT3x_INVALID_PARAM = 2,
  SHT3x_CRC_ERROR     = 3,
  SHT3x_NO_DATA       = 4,
} SHT3x_Result_t;

/**
 * @brief  Measuring mode
 */
typedef enum SHT3x_Mode_e
{
  SHT3x_MODE_SINGLESHOT = 0,
  SHT3x_MODE_PERIODIC   = 1,
  SHT3x_MODE_ART        = 2,
} SHT3x_Mode_t;

/**
 * @brief  Measuring Speed in Periodic mode
 */
typedef enum SHT3x_Speed_e
{
  SHT3x_SPEED_05MPS = 0,
  SHT3x_SPEED_1MPS  = 1,
  SHT3x_SPEED_2MPS  = 2,
  SHT3x_SPEED_4MPS  = 3,
  SHT3x_SPEED_10MPS = 4,
} SHT3x_Speed_t;


/**
 * @brief  Measuring Repeatability
 */
typedef enum SHT3x_Repeatability_e
{
  SHT3x_REPEATABILITY_LOW     = 0,
  SHT3x_REPEATABILITY_MEDIUM  = 1,
  SHT3x_REPEATABILITY_HIGH    = 2,
} SHT3x_Repeatability_t;


/**
 * @brief  Function type for Initialize/Deinitialize the platform dependent layer.
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*SHT3x_PlatformInitDeinit_t)(void);

/**
 * @brief  Function type for Send/Receive data to/from the slave.
 * @param  Address: Address of slave (0 <= Address <= 127)
 * @param  Data: Pointer to data
 * @param  Len: data len in Bytes
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: Failed to send/receive.
 *         - -2: Bus is busy.
 *         - -3: Slave doesn't ACK the transfer.
 */
typedef int8_t (*SHT3x_PlatformSendReceive_t)(uint8_t Address,
                                              uint8_t *Data, uint8_t Len);

/**
 * @brief  Function type for check CRC of the data received.
 * @param  Data: 16 bit data received
 * @param  DataCRC: CRC of data received
 * @retval 
 *         -  0: The data is valid.
 *         - -1: The data is not valid.
 */
typedef int8_t (*SHT3x_PlatformCRC_t)(uint16_t Data, uint8_t DataCRC);

/**
 * @brief  Function type for delay in ms.
 * @param  Delay: Delay duration in ms
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed.
 */
typedef int8_t (*SHT3x_PlatformDelay_t)(uint8_t Delay);

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
  SHT3x_Mode_t Mode;
  SHT3x_Repeatability_t Repeatability;
  SHT3x_Speed_t Speed;

  // Initializes platform dependent layer
  SHT3x_PlatformInitDeinit_t PlatformInit;
  // De-initializes platform dependent layer
  SHT3x_PlatformInitDeinit_t PlatformDeInit;
  // Send Data to the SHT3x.
  SHT3x_PlatformSendReceive_t PlatformSend;
  // Receive Data from the SHT3x.
  SHT3x_PlatformSendReceive_t PlatformReceive;
  // Delay in ms
  SHT3x_PlatformDelay_t PlatformDelay;
  // Check CRC of Data (If you do not want to check CRC, this function must
  // allways return 0)
  SHT3x_PlatformCRC_t PlatformCRC;
} SHT3x_Handler_t;

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



/**
 ==================================================================================
                       ##### Measurement Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Set Measurement mode Single Shot
 * @param  Handler: Pointer to handler
 * @param  Repeatability: Specify repeatability level (See Datasheet for more 
 *                        information)
 * 
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 *         - SHT3x_INVALID_PARAM: One of parameters is invalid.
 */
SHT3x_Result_t
SHT3x_SetModeSingleShot(SHT3x_Handler_t *Handler,
                        SHT3x_Repeatability_t Repeatability);


/**
 * @brief  Set Measurement mode Periodic
 * @param  Handler: Pointer to handler
 * @param  Speed: Specify acquisition frequency
 * @param  Repeatability: Specify repeatability level (See Datasheet for more 
 *                        information)
 * 
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 *         - SHT3x_INVALID_PARAM: One of parameters is invalid.
 */
SHT3x_Result_t
SHT3x_SetModePeriodic(SHT3x_Handler_t *Handler,
                      SHT3x_Speed_t Speed,
                      SHT3x_Repeatability_t Repeatability);


/**
 * @brief  Set Measurement mode ART (Accelerated Response Time)
 * @param  Handler: Pointer to handler
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 */
SHT3x_Result_t
SHT3x_SetModeART(SHT3x_Handler_t *Handler);


/**
 * @brief  Read a sample
 * @note   In Single Shot mode, the function starts measuring and waits up to
 *         20ms to finish.
 *
 * @param  Handler: Pointer to handler
 * @param  Sample: Pointer to sample buffer
 * @retval SHT3x_Result_t
 *         - SHT3x_OK: Operation was successful.
 *         - SHT3x_FAIL: Failed to send or receive data.
 *         - SHT3x_CRC_ERROR: CRC check error.
 *         - SHT3x_NO_DATA: No measurement data is present.
 */
SHT3x_Result_t
SHT3x_ReadSample(SHT3x_Handler_t *Handler, SHT3x_Sample_t *Sample);



/**
 ==================================================================================
                    ##### Control and Status Functions #####                       
 ==================================================================================
 */

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
 *         - SHT3x_INVALID_PARAM: One of parameters is invalid.
 */
SHT3x_Result_t
SHT3x_Init(SHT3x_Handler_t *Handler, uint8_t Address);


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
