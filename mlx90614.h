#ifndef MLX90614_H
#define MLX90614_H

#include <stdint.h>

extern void MLX90614_DisablePWM(void);
extern void MLX90614_Init(void);
extern int MLX90614_TestConnection(void);
extern uint16_t MLX90614_ReadAmbientTemp(void);
extern uint16_t MLX90614_ReadObjectTemp(void);
extern void MLX90614_SetAddress(uint8_t address);

#endif
