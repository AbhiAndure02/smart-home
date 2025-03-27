#include "stm8s.h"
#include <stdio.h>
#include <string.h>

#define LED_PIN    GPIO_PIN_5 // Change according to your setup
#define LED_PORT   GPIOB

#define UART_BAUD_RATE 9600

// Function to initialize GPIO for LED
void GPIO_Init_LED(void) {
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
}

// Function to initialize UART for communication with ESP8266
void UART_Init(void) {
    UART1->CR1 |= UART1_CR1_UE;    // Enable UART
    UART1->CR1 |= UART1_CR1_TE;    // Enable Transmitter
    UART1->CR1 |= UART1_CR1_RE;    // Enable Receiver
    UART1->BRR2 = (F_CPU / UART_BAUD_RATE) & 0xFF;
    UART1->BRR1 = (F_CPU / UART_BAUD_RATE) >> 8;
}

// Function to send data over UART
void UART_SendChar(char c) {
    while (!(UART1->SR & UART1_SR_TXE)) {};  // Wait for TX buffer to be empty
    UART1->DR = c;  // Send character
}

// Function to receive data over UART
char UART_ReceiveChar(void) {
    while (!(UART1->SR & UART1_SR_RXNE)) {};  // Wait for data to be received
    return UART1->DR;  // Read the received data
}

// Function to handle LED control based on received command
void handleCommand(char cmd) {
    if (cmd == '1') {
        GPIO_WriteHigh(LED_PORT, LED_PIN);  // Turn LED ON
        UART_SendChar('L');  // Send feedback to ESP8266
    }
    else if (cmd == '0') {
        GPIO_WriteLow(LED_PORT, LED_PIN);  // Turn LED OFF
        UART_SendChar('O');  // Send feedback to ESP8266
    }
}

// Main function
void main(void) {
    // Initialize system
    CLK_HSICmd(ENABLE);  // Enable high-speed internal clock
    GPIO_Init_LED();     // Initialize LED GPIO
    UART_Init();         // Initialize UART communication

    char receivedChar;
    
    while (1) {
        // Continuously check for UART input
        receivedChar = UART_ReceiveChar();
        handleCommand(receivedChar);  // Process received command
    }
}
