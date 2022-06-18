/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
#include "stm32f4xx.h"


extern void USART2_Init(uint32_t baud);//en sorte que l'on utilise les initialisation de l'USART2

GPIO_TypeDef *PA = GPIOA;
USART_TypeDef *UA = USART2;//type of struct


int _io_putchar(int ch)
{
	while (!(UA->SR & USART_SR_TXE));//attendre le drapeau du registre d'état à 1
	UA->DR = ch;					 //copier la donnée dans le registre de donnée
	while (!(UA->SR & USART_SR_TC)); //vérifier que tous les bytes ont été transformées
	return ch;						 //USART_SR_TXE, USART_SR_TC sont dans les registres d'état
}

int  __io_getchar(void)
{
	int out = 0;

	while (!(UA->SR & USART_SR_RXNE));//Lorsque ce drapeau est 1, une donnée est présente dans DR
	out = UA->DR;					  //recevoir des données

//	while(UA->SR & USART_SR_RXNE);	  //vérifier qu'il n'y pas de donnée restée mais pas utile

	return out;
}

void USART2_Transmit(uint8_t * data, uint32_t len)
{
	for(int i=0; i<len; i++)			 //copier les données jusqu'à la fin
	{
		while (!(UA->SR & USART_SR_TXE));//attendre le drapeau du registre d'état à 1
		UA->DR = data[i];				 //copier la donnée dans le registre de donnée
	}
}

int32_t USART2_Receive(uint8_t * data, uint32_t len)
{
	int32_t i = 0;
	int timeout = 10000000;				 //mettre un time-out pour détecter les fins de trame.

	while((i<len) && (timeout>0))	     //deux conditions obligatoires
	{
		while (!(UA->SR & USART_SR_RXNE))//Lorsque ce drapeau est 1, une donnée est présente dans DR
			timeout--;
		if(timeout > 0)
			data[i++] = UA->DR;			 //recevoir des données
	}
	return (i-1);						 //renvoie le nombre de données effectivement reçues.
}

int main(void)
{
	uint8_t *data = "hello world";//une trame à envoyer
	uint32_t len = 11;			  //le longue de la trame

	USART2_Init(115200);		  //initialisation de l'USART2 : baud
	//génération de la clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN| RCC_APB1ENR_USART2EN;

	//pin PA2 en sortie
	PA->MODER &= ~GPIO_MODER_MODER2_Msk;
	PA->MODER |= GPIO_MODER_MODER2_1;//utiliser ..._1 parce que (0x2UL << GPIO_MODER_MODER2_Pos)
									 //Alternate function mode:  10  (reference manual)
	//pin PA3 en sortie
	PA->MODER &= ~GPIO_MODER_MODER3_Msk;
	PA->MODER |= GPIO_MODER_MODER3_1;

	/* Connect  pins to Alternate function */
	PA->AFR[0] |= (7 << 8); //AFL0-AFL7 correspond à GPIOx0-GPIOx7
	PA->AFR[0] |= (7 << 12);


	_io_putchar('4');//on doit utiliser '' pour entrer un integer ici

	__io_getchar();

	USART2_Transmit(data, len);

	USART2_Receive(data, len); //les fonctions demandées

	return 0;

}
