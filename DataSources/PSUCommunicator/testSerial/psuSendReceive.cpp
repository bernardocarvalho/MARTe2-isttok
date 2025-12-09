// C library headers
// setserial /dev/ttyS0 spd_cust
// setserial /dev/ttyS0 divisor 16
// stty -F /dev/ttyS0 921600

#include <cstdint>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

// Linux headers
#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <iostream>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include "PSUMessages.h"
#include "UartLib.hpp"

uint8_t packet[2];
uint8_t packetR[4];

// int sourceFd;
bool isStarted;

void InterpretMessage();

bool DecodeCurrentPacket() { // float &current, unsigned char packet1, unsigned
  // char packet2){
  // Validate packets
  unsigned char validation = (packetR[1] & 0xF0) ^ ((packetR[0] & 0x1E) << 3);
  if (validation != 0xF0) {
    printf("WrongMessagesReceived\n");
    return false;
  }
  short pointOfCurrent = (short)((((unsigned short)packetR[0] & 0x00E0) >> 5) |
                                 (((unsigned short)packetR[1] & 0x00FE) << 2));
  printf("pointOfCurrenti = %d\n", pointOfCurrent);
  return true;
}

bool CreateCurrentPacket(unsigned int current) {
  bool ok = true;
  // Zero current 510
  // Calculate the point in the scale of the current
  int16_t pointOfCurrent = current;

  // Saturate current
  if (pointOfCurrent < FA_SCALE_MIN)
    pointOfCurrent = FA_SCALE_MIN;
  if (pointOfCurrent > FA_SCALE_MAX)
    pointOfCurrent = FA_SCALE_MAX;

  // Build packets
  uint16_t pc = (uint16_t)pointOfCurrent;
  uint16_t nc = ~pc;
  packet[0] = (uint8_t)(0x0000 | ((nc & 0x03C0) >> 5) | ((pc & 0x0007) << 5));
  packet[1] = (uint8_t)(0x0001 | ((pc & 0x03F8) >> 2));
  /*
  nc = (uint16_t)packet[1];
  nc <<= 8;
  nc &= 0xFF00;
  nc |= packet[0];
  */
  printf("Current: %d, Packet 0x%02X 0x%02X\n", current, packet[0], packet[1]);

  return EXIT_SUCCESS;
}

int read_msg(int fd) {
  // Allocate memory for read buffer, set size according to your needs
  // char read_buf[4];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // call printf() easily.
  memset(&packetR, '\0', sizeof(packetR));
  // wait 1 ms
  usleep(1000);

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME
  int num_bytes = read(fd, &packetR, sizeof(packetR));

  // n is the number of bytes read. n may be 0 if no bytes were received, and
  // can also be -1 to signal an error.
  if (num_bytes < 0) {
    printf("Error reading: %s", strerror(errno));
    return EXIT_FAILURE;
  }

  // Here we assume we received ASCII data, but you might be sending raw bytes
  // (in that case, don't try and print it to the screen like this!)
  printf("Read %i bytes. \t", num_bytes);
  for (int i = 0; i < num_bytes; i++) {
    unsigned int val = packetR[i] & 0xFF;
    printf("%i: 0x%02X \t", i, val);
    // printf("%i: %u \n", i, val);
  }
  printf("\n");
  if (num_bytes == 2)
    InterpretMessage();
  return EXIT_SUCCESS;
}
void InterpretMessage() {
  // Operation Started message
  if ((packetR[0] == FA_STARTED_MESSAGE_1) &&
      (packetR[1] == FA_STARTED_MESSAGE_2)) {
    printf("isStarted\n");
    isStarted = true;
    return;
  }
  // Operation Stopped message
  if ((packetR[0] == FA_STOPPED_MESSAGE_1) &&
      (packetR[1] == FA_STOPPED_MESSAGE_2)) {
    printf("isStopped\n");
    return;
  }
  // Stop Error message
  if ((packetR[0] == FA_STOP_ERROR_MESSAGE_1) &&
      (packetR[1] == FA_STOP_ERROR_MESSAGE_2)) {
    printf("Stop Error message\n");
    return;
  }
  // Communication Error message
  if ((packetR[0] == FA_COMM_ERROR_MESSAGE_1) &&
      (packetR[1] == FA_COMM_ERROR_MESSAGE_2)) {
    printf("Communication Error message\n");
    return;
  }
  // printf("Other message\n");
  DecodeCurrentPacket();
}

int main() {
  // Open the serial port. Change device path as needed (currently set to an
  // standard FTDI USB-UART cable type device)
  // sourceFd = open("/dev/ttyS2", O_RDWR);
  int sourceFd = openSerialPort("/dev/ttyS2");

  configureSerialPort(sourceFd, B921600);
  // B115200; // B576000; // B460800; OK// B230400; // B9600;B921600;
  // const speed_t baud_rate = B115200; // B9600;B921600;

  // clear input port
  read_msg(sourceFd);
  isStarted = false;
  // Write to serial port
  // unsigned char msg[] = {FA_STARTOP_MESSAGE_1, 0xFF};
  packet[0] = FA_STARTOP_MESSAGE_1;
  packet[1] = FA_STARTOP_MESSAGE_2;
  write(sourceFd, packet, sizeof(packet));
  printf("Sent Start: %lu bytes\n", sizeof(packet));
  read_msg(sourceFd);
  if (isStarted) {
    usleep(1000);
    for (int i = 0; i < 10; i++) {
      CreateCurrentPacket(500 + i);
      write(sourceFd, packet, sizeof(packet));
      usleep(10);
      // printf("Sent : %lu bytes\n", sizeof(packet));
      read_msg(sourceFd);
      // Halting the execution for 100000 Microseconds (0.1 seconds)
      usleep(10000);
    }
  }
  packet[0] = FA_STOPOP_MESSAGE_1;
  packet[1] = FA_STOPOP_MESSAGE_2;
  write(sourceFd, packet, sizeof(packet));
  printf("Sent  Stop: %lu bytes\n", sizeof(packet));
  // read_msg();
  usleep(1000);
  read_msg(sourceFd);
  read_msg(sourceFd);
  close(sourceFd);
  return 0; // success
}

/**
 void repeatData(int sourceFd, int destFd) {
    char buffer[256];
    while (true) {
        int bytesRead = read(sourceFd, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            write(destFd, buffer, bytesRead);
        }
    }
}
*/
