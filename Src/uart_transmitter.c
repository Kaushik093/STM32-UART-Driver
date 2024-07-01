/*
 * Bare-metal C code for transmitting data via UART from an STM32 microcontroller to a laptop.
 */

#include "stm32f4xx.h"
#include <stdint.h>

#define UART2_CLK_EN        (1U<<17) //Enable clock for UART2 module
#define AHB1_CLK_EN			(1U<<0);

#define CR1_TE				(1U<<3)  //Enable transmitter
#define CR1_UE				(1U<<13)

#define SYS_FREQ			16000000 //Default system frequency
#define APB1_CLK			SYS_FREQ //Verify and change code
#define BAUD_RATE			115200

#define TX_EMPTY			(1U<<7)

static uint16_t compute_divisor(uint32_t PeriphClk, uint32_t BaudRate){

	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate){

	USARTx->BRR = compute_divisor(PeriphClk,BaudRate);
}

void uart_transmit(int ch){

	//Ensure transmit data register is empty
	while(!(USART2->SR & TX_EMPTY)){}

	//Write to transmit data register
	USART2->DR = (ch & 0xFF);	//Bitwise and to ensure that only 8 bits are transmitted
}

void uart2_tx_init(){

	/****Configure UART GPIO pin ****/

	//Enable clock access
	RCC->AHB1ENR |= AHB1_CLK_EN;

	//Set PA2 register mode to alternate function mode
	GPIOA->MODER &= ~(1U<<4);
	GPIOA->MODER |= (1U<<5);

	//Specify the alternate function (Alternate function low register)
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);

	/****Configure UART****/
	//Enable clock access to uart2
	RCC->APB1ENR |= UART2_CLK_EN;

	//Set baud rate
	uart_set_baudrate(USART2, APB1_CLK, BAUD_RATE);

	//Configure transfer direction (Enable transmitter)
	USART2->CR1 = CR1_TE;

	//Enable UART module
	USART2->CR1 |= CR1_UE;
}

int main(void){

	uart2_tx_init();

	while(1){
		uart_transmit('Y');
	}
}
