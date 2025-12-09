#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "UartLib.hpp"

// #include <pthread.h>
#include <thread>

bool run;
void repeatData(int sourceFd, int destFd, const std::string filename) {
  char buffer[8];
  std::ofstream outfile(filename); // Open file for writing
  while (run) {
    int bytesRead = read(sourceFd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
      write(destFd, buffer, bytesRead);
      outfile.write(buffer, bytesRead); // Write to file
    }
  }
  outfile.close();
}

void signalHandler(int signum) {
  std::cout << "Caught signal: " << signum << std::endl;
  run = false;
  // exit(signum);
}

int main() {
  int fdMarte1 = openSerialPort("/dev/ttyS1");
  configureSerialPort(fdMarte1, B921600);
  int fdPSU = openSerialPort("/dev/ttyS2");
  configureSerialPort(fdPSU, B921600);
  run = true;
  signal(SIGINT, signalHandler);

  std::thread t1(repeatData, fdMarte1, fdPSU, "outS1.txt");
  std::thread t2(repeatData, fdMarte1, fdPSU, "outS2.txt");

  t1.join();
  t2.join();
  // pthread_join(thread1, nullptr);
  // pthread_join(thread2, nullptr);
  return 0;
}
