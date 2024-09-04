#include "console.h"

void console_init(void) {

  // Points module
  volatile TEGRA_ModuleUart* module = TEGRA_REG_UARTA;

  // Wait for the serial port to be ready
  while ((module->LSR & (TEGRA_UART_LSR_TEMT | TEGRA_UART_LSR_TXREADY)) != (TEGRA_UART_LSR_TEMT | TEGRA_UART_LSR_TXREADY)) {}

  // Calculates baud rate divisor
  unsigned int divisor = TEGRA_UART_CLOCK_RATE / (TEGRA_UART_BAUD_RATE * 16u);
  if ((TEGRA_UART_CLOCK_RATE % (TEGRA_UART_BAUD_RATE * 16u)) >= TEGRA_UART_BAUD_RATE * 8u) {
    divisor++;
  }

  // Configures UART baud rate
  module->LCR = TEGRA_UART_LCR_DLAB;
  module->DLM = (unsigned char)(divisor >> 8);
  module->DLL = (unsigned char)(divisor & 0xFFu);

  // Clears DLAB and configures UART line
  module->LCR = (unsigned char)((TEGRA_UART_PARITY << 3) | (TEGRA_UART_STOP_BITS << 2) | ((TEGRA_UART_DATA_BITS - 5u) << 0));

  // Enables and resets FIFOs
  module->FCR = 0x0u;
  module->FCR = (unsigned char)(TEGRA_UART_FCR_FIFOENABLE | TEGRA_UART_FCR_FIFO64);

  // Configures to use FIFOs in polled mode
  module->IER = 0x0u;

  // Resets
  module->MCR = 0x0u;
}

void console_write(unsigned char *buffer, unsigned int size) {
  unsigned int index;

  // Points module
  volatile TEGRA_ModuleUart* module = TEGRA_REG_UARTA;

  // Loops through buffer size
  while (size > 0u) {

    // Waits for peripheral to be ready to transmit
    while ((module->LSR & (TEGRA_UART_LSR_TEMT | TEGRA_UART_LSR_TXREADY)) != (TEGRA_UART_LSR_TEMT | TEGRA_UART_LSR_TXREADY)) {}

    // Fills the transmission FIFO
    for (index = 0u; index < TEGRA_UART_FIFO_SIZE && size != 0u; index++, size--, buffer++) {

      // Writes byte to the transmit buffer
      module->THR = *buffer;
    }
  }
}

unsigned int console_read(unsigned char *buffer, unsigned int size) {
  unsigned int count;

  // Points module
  volatile TEGRA_ModuleUart* module = TEGRA_REG_UARTA;

  // Iterates buffer bytes
  for (count = 0u; count < size; count++) {

    // Waits for peripheral to be ready to receive
    while ((module->LSR & TEGRA_UART_LSR_RXREADY) == 0) {}

    // Reads byte from the reception FIFO
    *buffer = module->RBR;

    // Advances buffer pointer
    buffer++;
  }

  // Returns bytes read
  return count;
}

void bsp_putchar(char c) {

  // Points module
  volatile TEGRA_ModuleUart* module = TEGRA_REG_UARTA;

  // Waits for peripheral to be ready to transmit
  while ((module->LSR & (TEGRA_UART_LSR_TEMT | TEGRA_UART_LSR_TXREADY)) != (TEGRA_UART_LSR_TEMT | TEGRA_UART_LSR_TXREADY)) {}

  // Writes to transmission buffer
  module->THR = c;

  // Waits for peripheral to finish transmission
  while ((module->LSR & TEGRA_UART_LSR_TEMT) != TEGRA_UART_LSR_TEMT) {}
}
