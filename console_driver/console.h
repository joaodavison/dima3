#ifndef CONSOLE_H_
#define CONSOLE_H_

// UART base address
#define TEGRA_UARTA_BASE (0x03100000u)

// UART register set
#define TEGRA_REG_UARTA ((volatile TEGRA_ModuleUart*)TEGRA_UARTA_BASE)

/**
 * UART module.
 */
typedef struct {
  union {
    volatile unsigned char RBR; /* 0x0 - DLAB=0 - Read */
    volatile unsigned char THR; /* 0x0 - DLAB=0 - Write */
    volatile unsigned char DLL; /* 0x0 - DLAB=1 */
  };
  union {
    volatile unsigned char IER; /* 0x1 - DLAB=0 */
    volatile unsigned char DLM; /* 0x1 - DLAB=1 */
  };
  union {
    volatile unsigned char IIR; /* 0x2 - Read */
    volatile unsigned char FCR; /* 0x2 - Write */
  };
  volatile unsigned char LCR; /* 0x3 */
  volatile unsigned char MCR; /* 0x4 */
  volatile unsigned char LSR; /* 0x5 */
  volatile unsigned char MSR; /* 0x6 */
  volatile unsigned char SCR; /* 0x7 */
} TEGRA_ModuleUart;

#define TEGRA_UART_FCR_FIFOENABLE   (1u << 0)
#define TEGRA_UART_FCR_FIFO64       (1u << 5)

#define TEGRA_UART_LCR_DLAB         (1u << 7)

#define TEGRA_UART_MCR_DTRC         (1u << 0)
#define TEGRA_UART_MCR_RTSC         (1u << 1)

#define TEGRA_UART_LSR_RXREADY      (1u << 0)
#define TEGRA_UART_LSR_TXREADY      (1u << 5)
#define TEGRA_UART_LSR_TEMT         (1u << 6)

#define TEGRA_UART_MSR_CTS          (1u << 4)
#define TEGRA_UART_MSR_DSR          (1u << 5)
#define TEGRA_UART_MSR_RI           (1u << 6)
#define TEGRA_UART_MSR_DCD          (1u << 7)

#define TEGRA_UART_FIFO_SIZE        (16u)

#define TEGRA_UART_CLOCK_RATE       (407347200u)
#define TEGRA_UART_BAUD_RATE        (115200u)
#define TEGRA_UART_DATA_BITS        (8u)
#define TEGRA_UART_STOP_BITS        (0u)
#define TEGRA_UART_PARITY           (0u)

void console_init(void);
void console_write(unsigned char *buffer, unsigned int size);
unsigned int console_read(unsigned char *buffer, unsigned int size);
void bsp_putchar(char c);

#endif /* CONSOLE_H_ */
