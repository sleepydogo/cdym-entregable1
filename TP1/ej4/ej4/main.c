#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <math.h>

// Mascaras para los bits de control
const uint8_t display[] = {
    0b0001000, // Decenas de segundos
	0b0000100, // Segundos
	0b0000010, // Decimas de segundos
	0b0000001, // Centesimas de segundos
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

/*
    uni: variable que incrementa cada 10ms ;
    aux: copia de uni para realizar las operaciones de division y modulo ; 
    stop = 1 detiene el conteo ;
    reset = 1 detiene el conteo y setea uni a cero ;
*/
uint16_t uni = 0, aux = 0, stop = 1, reset = 0;

/* Arreglos de control para el debounce*/
uint8_t debounceStart[] = {0, 0, 0, 0, 0};
uint8_t debounceStop[] = {0, 0, 0, 0, 0};
uint8_t debounceReset[] = {0, 0, 0, 0, 0};

/* Muestra valor en el display n */
void actualizarDisplay(int valor, int ndisplay)
{
    PORTB = segment_map[valor % 10];
    PORTC = display[ndisplay];
}

/* Realiza un bucle que descompone la unidad temporal y muestra cada digito en su display correspondiente, recibe como parametro count que determina si se incrementa la unidad temporal o solo se muestra el valor actual de uni*/
void mostrarDig(uint8_t count)
{
    aux = uni;
    for (int i = 0; i <= 3; i++)
    {
        actualizarDisplay(aux % 10, i);
        _delay_ms(2.5);
        if (i != 3)
        {
            aux = uni / (pow(10, i + 1));
        }
    }
    if (count == 1)
    {
        uni++;
    }
    return;
}

/* Checkea si hay un pulso positivo en el PINX_X del puerto PINX */
int checkButton(uint8_t PINX, uint8_t PINX_X)
{
    if ((PINX & (1 << PINX_X)) == 0)
    {
        return 0;
    }
    return 1;
}

/*
    Recibe como parametro value, el cual es un valor numerico en el rango [0..4], este valor incrementa de manera circular desde 0 a 4. Se usa como indice de los vectores de debounce almacenando en ellos el valor que retorna checkButton().
    Luego se hace una suma de los valores de los vectores de debounce. Si la suma es igual a 5, significa que el boton ha sido pulsado. Por un tiempo prolongado y que esto no se debe a un efecto rebote. Se implementa la logica para cada caso.
    Esta logica consiste en resetear a 0 los vectores de debounce asociados a la operacion a realizar y poner la variable que representa el estado (stop, reset, start) en 1.  
*/
void buttonControl(uint8_t value)
{
    int totalStop = 0, totalReset = 0, totalStart = 0;

    debounceStart[value] = checkButton(PINC, PINC4); // Chequeamos si START esta presionado
    debounceStop[value] = checkButton(PINC, PINC5);  // Chequeamos si STOP esta presionado
    debounceReset[value] = checkButton(PINB, PINB7); // Chequeamos si RESET esta presionado

    for (int i = 0; i < 5; i++)
    {
        totalStart += debounceStart[i];
        totalStop += debounceStop[i];
        totalReset += debounceReset[i];
    }

    if (totalStart == 5)
    {
	    while (checkButton(PINC, PINC4) == 1);
        for (int i = 0; i < 5; i++)
        {
            debounceStart[i] = 0;
            debounceStop[i] = 0;
        }
		stop = 0;
    }
    if (totalStop == 5)
    {      
        for (int i = 0; i < 5; i++)
        {
            debounceStop[i] = 0;
        }
		stop = 1;
	}
    if (totalReset == 5)
    {
        while (checkButton(PINB, PINB7) == 1);
        for (int i = 0; i < 5; i++)
        {
            debounceStop[i] = 0;
        }
		reset = 1;
		stop = 1;
		uni = 0;
    }
}

int main(void)
{
    /* Configuracion de los puertos */
    DDRB = 0x7F;
    DDRC = 0x0F;
    PORTB = segment_map[uni];
	uint8_t controlValue = 0; // Se utiliza para brindar un valor value a la funcion buttonControl().
    while (1)
    {
        /* 
            Si stop esta en 1 se llama a la rutina mostrarDig() con argumento 0 para que se muestre el valor de uni y cada vez que termina la funcion se incrementa en 1 esta variable.
        */
        while (stop == 0)
        {
            mostrarDig(1);
            buttonControl(uni % 5);
        }
        /* 
            Si stop esta en 0 se llama a la rutina mostrarDig() con argumento 1 para que se muestre el valor de uni actual sin modificarlo.
            Luego se llama a la funcion buttonControl() para el checkeo de los botones. Esta funcion se llama cada 10ms.
            Luego se llama a la funcion buttonControl() para el checkeo de los botones. Como aqui no tenemos un incremento en uni es necesario crear otra variable que itere de 0 a 4 para pasar el valor a buttonControl(). 
        */
        while (stop == 1)
        {
			if (controlValue == 6) {
				controlValue = 0;
			}
			mostrarDig(0);
			buttonControl(controlValue);
			controlValue++;
        }
    }
}