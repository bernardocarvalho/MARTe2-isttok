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

int openSerialPort(const char *portname) {
  int fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    std::cerr << "Error opening port: " << portname << std::endl;
    return -1;
  }
  return fd;
}

bool configureSerialPort(int fd, int baudRate) {
  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if (tcgetattr(fd, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    return false;
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

  //   http://unixwiz.net/techtips/termios-vmin-vtime.html
  //    ICANON bit is turned off, a "raw mode" is selected
  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL); // Disable any special handling of received bytes
  // write(fd, packet, sizeof(packet));
  // printf("Sent : %lu bytes\n", sizeof(packet));

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g.
  // newline chars)
  tty.c_oflag &=
      ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT
  // PRESENT ON LINUX) tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars
  // (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 1; // Wait for up to 1s (10 deciseconds), returning as soon
  // as any data is received.
  tty.c_cc[VMIN] = 0;
  // tty.c_cc[VMIN] = 2;
  //  Error reading: Resource temporarily unavailable%

  // Set in/out baud rate
  cfsetispeed(&tty, baudRate);
  cfsetospeed(&tty, baudRate);

  // Save tty settings, also checking for error
  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return false;
  }
  return true;
}
