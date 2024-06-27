// Assumed B_UART_FCR_FIFOE and  B_UART_FCR_FIFO64 are set

#define SerialRegisterBase (0x031d0000u)
#define PcdSerialClockRate (407347200u)
#define PcdSerialBaudRate (115200u)

#define BIT0 (1u)
#define BIT1 (2u)
#define BIT2 (4u)
#define BIT3 (8u)
#define BIT4 (16u)
#define BIT5 (32u)
#define BIT6 (64u)
#define BIT7 (128u)
#define BIT8 (256u)

#define false (0u)
#define true (1u)

// 16550 UART register offsets and bitfields
#define R_UART_RXBUF         0    // LCR_DLAB = 0
#define R_UART_TXBUF         0    // LCR_DLAB = 0
#define R_UART_BAUD_LOW      0    // LCR_DLAB = 1
#define R_UART_BAUD_HIGH     1    // LCR_DLAB = 1
#define R_UART_IER           1    // LCR_DLAB = 0
#define R_UART_FCR           2
#define   B_UART_FCR_FIFOE   BIT0
#define   B_UART_FCR_FIFO64  BIT5
#define R_UART_LCR           3
#define   B_UART_LCR_DLAB    BIT7
#define R_UART_MCR           4
#define   B_UART_MCR_DTRC    BIT0
#define   B_UART_MCR_RTS     BIT1
#define R_UART_LSR           5
#define   B_UART_LSR_RXRDY   BIT0
#define   B_UART_LSR_TXRDY   BIT5
#define   B_UART_LSR_TEMT    BIT6
#define R_UART_MSR           6
#define   B_UART_MSR_CTS     BIT4
#define   B_UART_MSR_DSR     BIT5
#define   B_UART_MSR_RI      BIT6
#define   B_UART_MSR_DCD     BIT7

unsigned char SerialPortReadRegister(unsigned int  Base, unsigned int  Offset) {
  return *((unsigned char*)(Base + Offset * 4u));
}

void SerialPortWriteRegister(unsigned int  Base, unsigned int  Offset, unsigned char  Value) {
  *((unsigned char*)(Base + Offset * 4u)) = Value;
}

unsigned int Tegra16550SerialPortWrite(unsigned char *Buffer, unsigned int NumberOfBytes) {
  unsigned int  Result;
  unsigned int  Index;
  unsigned int  FifoSize;

  if (NumberOfBytes == 0) {
    // Flush the hardware

    // Wait for both the transmit FIFO and shift register empty.
    while ((SerialPortReadRegister(SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) {}

    return 0;
  }

  // Compute the maximum size of the Tx FIFO
  //if ((PcdGet8(PcdSerialFifoControl) & B_UART_FCR_FIFOE) != 0) {
    //if ((PcdGet8(PcdSerialFifoControl) & B_UART_FCR_FIFO64) == 0) {
      FifoSize = 16;
    //} else {
    //  FifoSize = PcdGet32(PcdSerialExtendedTxFifoSize);
    //}
  //}

  Result = NumberOfBytes;
  while (NumberOfBytes != 0) {
    // Wait for the serial port to be ready, to make sure both the transmit FIFO
    // and shift register empty.
    while ((SerialPortReadRegister(SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) {
    }

    // while ((SerialPortReadRegister(SerialRegisterBase, R_UART_LSR) & B_UART_LSR_TEMT) == 0);

    // Fill then entire Tx FIFO
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--, Buffer++) {
      // Write byte to the transmit buffer.
      SerialPortWriteRegister(SerialRegisterBase, R_UART_TXBUF, *Buffer);
    }
  }

  return Result;
}

unsigned int Tegra16550SerialPortRead(unsigned char *Buffer, unsigned int NumberOfBytes) {
  unsigned int  Result;

  for (Result = 0; NumberOfBytes-- != 0; Result++, Buffer++) {
    // Wait for the serial port to have some data.
    while ((SerialPortReadRegister(SerialRegisterBase, R_UART_LSR) & B_UART_LSR_RXRDY) == 0) {}

    // Read byte from the receive buffer.
    *Buffer = SerialPortReadRegister(SerialRegisterBase, R_UART_RXBUF);
  }

  return Result;
}

void Tegra16550SerialPortSetAttributes(void) {
  unsigned int   Divisor;
  unsigned char   Lcr;
  unsigned char   LcrData;
  unsigned char   LcrParity = 0u;
  unsigned char   LcrStop = 0u;
  unsigned char   DataBits = 8u;

  // Map 5..8 to 0..3
  LcrData = (DataBits - 5u);
  LcrParity = 0;
  LcrStop = 0;

  // Calculate divisor for baud generator
  //    Ref_Clk_Rate / Baud_Rate / 16
  Divisor = PcdSerialClockRate / (PcdSerialBaudRate * 16);
  if ((PcdSerialClockRate % (PcdSerialBaudRate * 16)) >= PcdSerialBaudRate * 8) {
    Divisor++;
  }

  // Configure baud rate
  SerialPortWriteRegister(SerialRegisterBase, R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister(SerialRegisterBase, R_UART_BAUD_HIGH, (unsigned char)(Divisor >> 8));
  SerialPortWriteRegister(SerialRegisterBase, R_UART_BAUD_LOW, (unsigned char)(Divisor & 0xff));

  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from line control value
  Lcr = (unsigned char)((LcrParity << 3) | (LcrStop << 2) | LcrData);
  SerialPortWriteRegister(SerialRegisterBase, R_UART_LCR, (unsigned char)(Lcr & 0x3F));
}

void console_init(void) {
  unsigned int   Divisor;
  unsigned int   CurrentDivisor;

  // Calculate divisor for baud generator
  //    Ref_Clk_Rate / Baud_Rate / 16
  Divisor = PcdSerialClockRate / (PcdSerialBaudRate * 16);
  if ((PcdSerialClockRate % (PcdSerialBaudRate * 16)) >= PcdSerialBaudRate * 8) {
    Divisor++;
  }

  SerialPortWriteRegister(SerialRegisterBase, R_UART_LCR, (unsigned char)(SerialPortReadRegister(SerialRegisterBase, R_UART_LCR) | B_UART_LCR_DLAB));
  CurrentDivisor  =  SerialPortReadRegister(SerialRegisterBase, R_UART_BAUD_HIGH) << 8;
  CurrentDivisor |= (unsigned int)SerialPortReadRegister(SerialRegisterBase, R_UART_BAUD_LOW);
  SerialPortWriteRegister(SerialRegisterBase, R_UART_LCR, (unsigned char)(SerialPortReadRegister(SerialRegisterBase, R_UART_LCR) & ~B_UART_LCR_DLAB));

  // Wait for the serial port to be ready.
  // Verify that both the transmit FIFO and the shift register are empty.
  while ((SerialPortReadRegister(SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) {}

  // Configure baud rate
  SerialPortWriteRegister(SerialRegisterBase, R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister(SerialRegisterBase, R_UART_BAUD_HIGH, (unsigned char)(Divisor >> 8));
  SerialPortWriteRegister(SerialRegisterBase, R_UART_BAUD_LOW, (unsigned char)(Divisor & 0xff));

  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from PcdSerialLineControl
  //SerialPortWriteRegister(SerialRegisterBase, R_UART_LCR, (unsigned char)(PcdGet8(PcdSerialLineControl) & 0x3F));
  Tegra16550SerialPortSetAttributes();

  // Enable and reset FIFOs
  // Strip reserved bits from PcdSerialFifoControl
  SerialPortWriteRegister(SerialRegisterBase, R_UART_FCR, 0x00);
  SerialPortWriteRegister(SerialRegisterBase, R_UART_FCR, (unsigned char)(B_UART_FCR_FIFOE | B_UART_FCR_FIFO64));

  // Set FIFO Polled Mode by clearing IER after setting FCR
  SerialPortWriteRegister(SerialRegisterBase, R_UART_IER, 0x00);

  // Put Modem Control Register(MCR) into its reset state of 0x00.
  SerialPortWriteRegister(SerialRegisterBase, R_UART_MCR, 0x00);
}

void bsp_putchar(char c) {
  // Wait for the serial port to be ready, to make sure both the transmit FIFO and shift register empty.
  while ((SerialPortReadRegister(SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) {}
  SerialPortWriteRegister(SerialRegisterBase, R_UART_TXBUF, c);
}
