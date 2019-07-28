void usartSetup (void) {
  // make sure the relevant pins are appropriately set up.
  RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;              // enable clock for GPIOA
  GPIOA_CRH   |= (0x0BUL  < < 4);                  // Tx (PA9) alt. out push-pull
  GPIOA_CRH   |= (0x04UL  << 8);                  // Rx (PA10) in floating
  RCC_APB2ENR |= RCC_APB2ENR_USART1EN;            // enable clock for USART1
  USART1_BRR  = 64000000L/115200L;                // set baudrate
  USART1_CR1 |= (USART1_CR1_RE | USART1_CR1_TE);  // RX, TX enable
  USART1_CR1 |= USART1_CR1_UE;                    // USART enable
  }

int SendChar (int ch)  {
  while (!(USART1_SR & USART1_SR_TXE));
  USART1_DR = (ch & 0xFF);
  return (ch);
}

int GetChar (void)  {
  while (!(USART1_SR & USART1_SR_RXNE));
  return ((int)(USART1_DR & 0xFF));
}