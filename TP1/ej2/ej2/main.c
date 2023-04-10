/*
* ej2.c
*
* Created: 27/03/2023 18:35:25
* Author : Usuario
*/

#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>

//Counter
uint8_t counter = 0;
// Mapa de bits para el display de 7 segmentos

const uint8_t segment_map[] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111  // 9
};

void update_display() {
	// Obtener el valor del contador
	uint8_t value = counter % 10;
	// Convertir el valor a los segmentos del display
	uint8_t segments = segment_map[value];
	// Escribir los segmentos en los pines correspondientes del MCU
	PORTB = segments;
}

int main(void){
	DDRB = 0x7F;
	PORTB = segment_map[0];
	while (1) {
		if ((PINB & (1 << PINB7))){
			_delay_ms(25);
			while((PINB & (1 << PINB7)));
			_delay_ms(25);
			counter++;
			update_display();
		}
	}
}