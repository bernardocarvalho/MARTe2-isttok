// C library headers
// setserial /dev/ttyS0 spd_cust
// setserial /dev/ttyS0 divisor 16
// stty -F /dev/ttyS0 921600

#include <cstdint>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include "PSUMessages.h"

uint8_t packet[2];

int serial_port;

bool CreateCurrentPacket(unsigned int current) {
  bool ok = true;

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
  nc = (uint16_t)packet[1];
  nc <<= 8;
  nc &= 0xFF00;
  nc |= packet[0];
  printf("Current: %d, Packet 0x%02X 0x%02X\n", current, packet[0], packet[1]);

  return EXIT_SUCCESS;
}

int read_msg() {
  // Allocate memory for read buffer, set size according to your needs
  char read_buf[4];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // call printf() easily.
  memset(&read_buf, '\0', sizeof(read_buf));
  // wait 1 ms
  usleep(1000);

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME
  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

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
    unsigned int val = read_buf[i] & 0xFF;
    printf("%i: 0x%02X \t", i, val);
    // printf("%i: %u \n", i, val);
  }
  printf("\n");
  return EXIT_SUCCESS;
}

int main() {
  // Open the serial port. Change device path as needed (currently set to an
  // standard FTDI USB-UART cable type device)
  serial_port = open("/dev/ttyS2", O_RDWR);
  if (serial_port <= 0) {
    printf("Error %i from open: %s\n", serial_port, strerror(errno));
    return EXIT_FAILURE;
  }

  const speed_t baud_rate = B921600;
  // B115200; // B576000; // B460800; OK// B230400; // B9600;B921600;
  // const speed_t baud_rate = B115200; // B9600;B921600;

  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if (tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    return 1;
  }

  // tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag |= PARENB;
  tty.c_cflag |= PARODD;
  tty.c_cflag |= CSTOPB;
  // tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in
  //  communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
  tty.c_cflag |= CS8;    // 8 bits per byte (most common)
  tty.c_cflag &=
      ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |=
      CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL); // Disable any special handling of received bytes
  // write(serial_port, packet, sizeof(packet));
  // printf("Sent : %lu bytes\n", sizeof(packet));

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g.
                         // newline chars)
  tty.c_oflag &=
      ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT
  // PRESENT ON LINUX) tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars
  // (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon
                        // as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate
  cfsetispeed(&tty, baud_rate);
  cfsetospeed(&tty, baud_rate);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return 1;
  }
  // clear input port
  read_msg();
  // Write to serial port
  // unsigned char msg[] = {FA_STARTOP_MESSAGE_1, 0xFF};
  packet[0] = FA_STARTOP_MESSAGE_1;
  packet[1] = FA_STARTOP_MESSAGE_2;
  write(serial_port, packet, sizeof(packet));
  printf("Sent : %lu bytes\n", sizeof(packet));
  read_msg();
  CreateCurrentPacket(100);
  write(serial_port, packet, sizeof(packet));
  printf("Sent : %lu bytes\n", sizeof(packet));
  read_msg();

  // Halting the execution for 100000 Microseconds (0.1 seconds)
  usleep(100000);
  packet[0] = FA_STOPOP_MESSAGE_1;
  packet[1] = FA_STOPOP_MESSAGE_2;
  write(serial_port, packet, sizeof(packet));
  printf("Sent : %lu bytes\n", sizeof(packet));
  read_msg();
  close(serial_port);
  return 0; // success
};
;
