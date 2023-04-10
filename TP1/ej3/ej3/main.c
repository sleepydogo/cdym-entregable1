#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>


//Counter [1,2,3,4]

uint8_t counter[4] = {0,0,0,0};
	
	// Mascaras para los bits de control
const uint8_t display[] = {
	0b000001,
	0b0000010,
	0b0000100,
	0b0001000,
};

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

// Unidad temporal
int uni = 0;

int aux = 0;

int main(void)
{
	DDRB = 0b11111111;
	DDRC = 0b00001111;
	PORTB = segment_map[uni];
	while (1){
		PORTB = segment_map[uni%10];
		PINC = 0b00001000;
		//_delay_ms(1);
		aux=uni/10;
		PORTB = segment_map[aux%10];
		PORTC = 0b00000100;
		//_delay_ms(1);
		aux=uni/100;
		PORTB = segment_map[aux%10];
		PORTC = 0b00000010;
		//_delay_ms(1);
		aux=uni/1000;
		PORTB = segment_map[aux%10];
		PORTC = 0b00000001;
		_delay_ms(1);
		uni++;
		PORTC = 0b00000000;
	}
}