#include "spi.h"

// SPI Configuration
void SPIConfig(void)
{
    // Enable SPI1 Clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Disable SPI1 before configuring
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // CPOL = 0, CPHA = 0
    SPI1->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);

    // Master mode
    SPI1->CR1 |= SPI_CR1_MSTR;

    // Baud rate = fPCLK/16 (BR[2:0] = 011), PCLK2 = 90MHz, SPI clk = 5MHz
    SPI1->CR1 |= (3U << SPI_CR1_BR_Pos);

    // MSB first (clear LSBFIRST)
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;

    // Software slave management (SSM=1, SSI=1)
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

    // Full duplex (RXONLY = 0)
    SPI1->CR1 &= ~SPI_CR1_RXONLY;

    // Data frame format: 8-bit (DFF = 0)
    SPI1->CR1 &= ~SPI_CR1_DFF;

    // Clear SPI_CR2 (optional, but good practice)
    SPI1->CR2 = 0;

    // Enable SPI1
    SPI1->CR1 |= SPI_CR1_SPE;
}

// SPI Transmit Function
void SPI_MasterTransmit(SPI_TypeDef *pSPIx, uint8_t *pBuffer, int32_t len)
{
    while (len > 0)
    {
        // Wait until TXE (Transmit buffer empty) is set
        while (!(pSPIx->SR & SPI_SR_TXE))
            ;

        // Load data into DR register
        pSPIx->DR = *pBuffer;
        pBuffer++;
        len--;

        // Optional: Handle overrun flag if needed
        if (pSPIx->SR & SPI_SR_OVR)
        {
            // Clear the Overrun flag by reading DR and SR
            (void)pSPIx->DR;
            (void)pSPIx->SR;
        }
    }

    // Wait until BSY (Busy flag) is cleared
    while (pSPIx->SR & SPI_SR_BSY)
        ;
}

// GPIO Configuration for SPI
void SPIGPIOConfig(void)
{
    // Enable GPIOA and GPIOB clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    // Configure GPIOA pins PA5, PA6, PA7 as Alternate Function (AF5)
    GPIOA->MODER &= ~((3 << 10) | (3 << 12) | (3 << 14)); // Clear mode bits
    GPIOA->MODER |= (2 << 10) | (2 << 12) | (2 << 14);    // Set AF mode

    GPIOA->AFR[0] &= ~((0xF << 20) | (0xF << 24) | (0xF << 28)); // Clear AF bits
    GPIOA->AFR[0] |= (5 << 20) | (5 << 24) | (5 << 28);          // Set AF5 (SPI1)

    // Configure PB6 as Output for CS
    GPIOB->MODER &= ~(3 << 12); // Clear mode bits for PB6
    GPIOB->MODER |= (1 << 12);  // Set as output

    // Optional: Set PB6 as Pull-Up/Down if needed (not necessary for output mode)
    GPIOA->PUPDR |= (1 << 10); // No pull-up, no pull-down for PB6
    GPIOB->PUPDR &= ~(3 << 12); // No pull-up, no pull-down for PB6

    // Set output speed for SPI pins to high
    GPIOA->OSPEEDR |= (3 << 10) | (3 << 12) | (3 << 14); // High speed
}

// Chip Select (CS) Enable
void CS_Enable(void)
{
    GPIOB->BSRR |= (1 << (6 + 16)); // Reset PB6 (active low)
}

// Chip Select (CS) Disable
void CS_Disable(void)
{
    GPIOB->BSRR |= (1 << 6); // Set PB6 (inactive high)
}
