// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 João Pedro


#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pin_t pin;
  int pulseValue;
} chip_data_t;

void chip_timer_callback(void *data) {
  chip_data_t *chip_data = (chip_data_t*)data;

  // Lê o valor de BPM do slider (40-200)
  int pulseValue = attr_read(chip_data->pulseValue);
  
  // Converte BPM para voltage (0-3.3V)
  // BPM 40-200 -> Voltage 0-3.3V
  float volts = (pulseValue - 40) * (3.3 / (200 - 40));
  volts = (volts < 0) ? 0 : (volts > 3.3) ? 3.3 : volts;
  
  printf("BPM: %d | Voltage: %.2fV\n", pulseValue, volts);
  
  // Envia o voltage no pino
  pin_dac_write(chip_data->pin, volts);
}

void chip_init() {
  printf("Pulse Sensor Custom Chip Inicializado!\n");
  chip_data_t *chip_data = (chip_data_t*)malloc(sizeof(chip_data_t));

  // Inicializa o atributo pulseValue
  chip_data->pulseValue = attr_init("pulseValue", 40); 
  
  // Inicializa o pino de saída analógica
  chip_data->pin = pin_init("OUT0", ANALOG);

  const timer_config_t config = {
    .callback = chip_timer_callback,
    .user_data = chip_data,
  };

  timer_t timer_id = timer_init(&config);
  timer_start(timer_id, 100, true);  // Atualiza a cada 100ms
}