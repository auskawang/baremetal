//attempt at a simple blinky baremetal
#include <stdint.h>
#define RCC 0x40021000
#define RCC_CR (RCC)
#define RCC_IOPENR (RCC + 0x34)
#define RCC_APBENR2 (RCC + 0x40)
#define GPIOA 0x50000000
#define GPIOA_MODER (GPIOA + 0x00)
#define GPIOA_ODR (GPIOA + 0x14)
#define GPIOA_BRR (GPIOA + 0x28)

#define GPIOC 0x50000800
#define GPIOC_MODER (GPIOC + 0x00)
#define GPIOC_PUPDR (GPIOC + 0x0C)

#define EXTI 0x40021800
#define EXTI_EXTICR_4 (EXTI + 0x060 + 0x4 * (4 - 1))
#define EXTI_FTSR1 (EXTI + 0x004)
#define EXTI_FPR1 (EXTI + 0x010)
#define EXTI_IMR1 (EXTI + 0x080)

#define NVIC_ISER 0xE000E100

#define MPU_CTRL 0xE000ED94

#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018

#define TIM14 0x40002000
#define TIM14_CR1 (TIM14 + 0x0)
#define TIM14_SR (TIM14 + 0x10)
#define TIM14_DIER (TIM14 + 0xC)
#define TIM14_CNT (TIM14 + 0x24)
#define TIM14_PSC (TIM14 + 0x28)
#define TIM14_ARR (TIM14 + 0x2C)

#define SHPR3 0xE000ED20
#define NVIC_IPR4 0xE000E410

#define LED_PIN 5
#define BUTTON_PIN 13

extern void EXTI415_Callback();
extern void TIM14_Callback();
extern void delay();
void set_brightness(int);
uint8_t button_state = 0;

const uint8_t sine_lut[200] = {

		50, 52, 53, 55, 56, 58, 59, 61, 62, 64, 65, 67, 68, 70, 71, 73, 74, 75, 77, 78, 79, 81, 82, 83, 84, 85, 86, 88, 89, 90, 90, 91, 92, 93, 94, 95, 95, 96, 96, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 99, 99, 98, 98, 98, 97, 96, 96, 95, 95, 94, 93, 92, 91, 90, 90, 89, 88, 86, 85, 84, 83, 82, 81, 79, 78, 77, 75, 74, 73, 71, 70, 68, 67, 65, 64, 62, 61, 59, 58, 56, 55, 53, 52, 50, 48, 47, 45, 44, 42, 41, 39, 38, 36, 35, 33, 32, 30, 29, 27, 26, 25, 23, 22, 21, 19, 18, 17, 16, 15, 14, 12, 11, 10, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 4, 4, 5, 5, 6, 7, 8, 9, 10, 10, 11, 12, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 29, 30, 32, 33, 35, 36, 38, 39, 41, 42, 44, 45, 47, 48
};

int main() {



	*((int*)RCC_IOPENR) = 0x1;

	*((int*)GPIOA_MODER) &= ~(0x3 << (2 * LED_PIN));
	*((int*)GPIOA_MODER) |= 0x1 << (2 * LED_PIN);



	//configure pc13 as input
	*((int*)RCC_IOPENR) |= 0x4;
	*((int*)GPIOC_MODER) &= ~(0x3 << (2 * BUTTON_PIN));

	//configure PC13 as EXTI interrupt falling edge
	*((int*)EXTI_FTSR1) |= 0x2000;
	*((int*)EXTI_EXTICR_4) |= (0x2 << 8);
	*((int*)EXTI_IMR1) |= 0x2000;

	//i know i have privilege, MPU is disabled, word accessed and memory aligned,
			__asm volatile("":::"memory");
			*((volatile uint32_t*)NVIC_ISER) |= (uint32_t)0x80;	//hard faults when accessing bits after bit 6
			__asm volatile("":::"memory");

	//sysclk stuff based off hsi 48mhz (each interrupt should fire every 1ms)
	*((int*)STK_RVR) = 0x5DB; //reload of 47999 -> 5999 -> 1499
	*((int*)STK_CVR) = 1;	//reset current value
	*((int*)STK_CSR) |= 0x3;


	//set tim14 priority to 1
	*((int*)NVIC_IPR4) |= 0x40000000;
	//tim14 clock based off of 48mhz/4
	*((int*)RCC_APBENR2) |= 0x8000; //enable tim14
	*((int*)TIM14_DIER) |= 0x1;	//enable interrupt generation
	*((int*)NVIC_ISER) |= 0x80000; //enable interrupt for NVIC
	*((int*)TIM14_PSC) = 0x0; //PS of 1
	*((int*)TIM14_ARR) = 0x258;
	*((int*)TIM14_CR1) |= 0x1; //start the counter

	int i = 0;
	while(1) {
		for (int i = 0; i < 200; i++) {
			set_brightness(sine_lut[i]);
		delay(8);
		}

//		if (button_state) *((int*)GPIOA_ODR) |= 0x1 << LED_PIN;
//		else *((int*)GPIOA_BRR) |= 0x1 << LED_PIN;
	}

	return 1;
}

void EXTI415_Callback() {
	button_state = !button_state;
	*((int*)EXTI_FPR1) = 0x1 << BUTTON_PIN;
}
uint8_t brightness = 0;
//called every 5.46ms
void TIM14_Callback() {
	static int tim_counter = 0;
	const int max_count = 100;
	if (tim_counter < brightness)
		*((int*)GPIOA_ODR) |= 0x1 << LED_PIN;
	else
		*((int*)GPIOA_BRR) |= 0x1 << LED_PIN;
	if (tim_counter == max_count) {
		tim_counter = 0;
	}
	tim_counter++;

	//clear interrupt flags
	*((int*)TIM14_SR) &= ~0x3;
}

//sets the brightness of the onboard led from configurable from 0-100
void set_brightness(int param_brightness) {
	brightness = param_brightness;
}
