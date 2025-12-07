//attempt at a simple blinky baremetal
#define RCC 0x40021000
#define RCC_IOPENR (RCC + 0x34)
#define GPIOA 0x50000000
#define GPIOA_MODER (GPIOA + 0x00)
#define GPIOA_ODR (GPIOA + 0x14)

#define LED_PIN 5

int main() {
	*((int*)RCC_IOPENR) = 0x1;

	*((int*)GPIOA_MODER) &= ~(0x3 << (2 * LED_PIN));
	*((int*)GPIOA_MODER) |= 0x1 << (2 * LED_PIN);

	*((int*)GPIOA_ODR) |= 0x1 << LED_PIN;
	return 1;
}

