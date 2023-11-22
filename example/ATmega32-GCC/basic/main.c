/**
 **********************************************************************************
 * @file   main.c
 * @author Hossein.M (https://github.com/Hossein-M98)
 * @brief  example code for SHT3x Driver (for ATmega32)
 **********************************************************************************
 *
 * Copyright (c) 2023 Mahda Embedded System (MIT License)
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

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "Retarget.h"
#include "SHT3x.h"
#include "SHT3x_platform.h"


int main(void)
{
  SHT3x_Handler_t Handler = {0};
  SHT3x_Sample_t  Sample = {0};

  Retarget_Init(F_CPU, 9600);
  printf("SHT3x Driver Example\r\n\r\n")

  SHT3x_Platform_Init(&Handler);
  SHT3x_Init(&Handler, 0);
  SHT3x_SetModeSingleShot(&Handler, SHT3x_REPEATABILITY_HIGH);

  while (1)
  {
    SHT3x_ReadSample(&Handler, &Sample);
    printf("Temperature: %f°C\r\n"
           "Humidity: %f%%\r\n\r\n",
           Sample.TempCelsius,
           Sample.HumidityPercent);

    _delay_ms(1000);
  }

  SHT3x_DeInit(&Handler);
  return 0;
}
