//attempt at a simple blinky baremetal
#include <stdint.h>
#define RCC 0x40021000
#define RCC_CR (RCC)
#define RCC_IOPENR (RCC + 0x34)
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

#define LED_PIN 5
#define BUTTON_PIN 13

extern void EXTI415_Callback();
extern void delay();

uint8_t button_state = 0;
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

	while(1) {
			*((int*)GPIOA_ODR) |= 0x1 << LED_PIN;
		delay(1000);
			*((int*)GPIOA_BRR) |= 0x1 << LED_PIN;
			delay(1000);
	}

	return 1;
}

void EXTI415_Callback() {
	button_state = !button_state;
	*((int*)EXTI_FPR1) = 0x1 << BUTTON_PIN;
}

