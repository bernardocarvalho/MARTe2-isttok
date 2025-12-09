// C library headers
// setserial /dev/ttyS0 spd_cust
// setserial /dev/ttyS0 divisor 16
// stty -F /dev/ttyS0 921600

int openSerialPort(const char *portname);
bool configureSerialPort(int fd, int baudRate);
