//Listing 1 - Opening a serial port.

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */


int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf("error %d setting term attributes", errno);
}

char* create_packet (char* command) {
    int i;
    i = 1;
    return 0;

}

int main(int argc, char *argv[]) {
  char *portname = "/dev/tty.usbmodem1411";
  int fd;
  int wlen;

  char buf[1024];
  char command[1024];

  //open a read/write port
  //O_NOCTTY specifies that this is not the controlling terminal for this port
  //O_NDELAY means we do not care about the DCD port - subject to change
  fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
  //check whether the port was made or not, exit program if not made
  if (fd < 0) {
      printf("Error opening %s: %s\n", portname, strerror(errno));
      return -1;
  }
  /*baudrate 115200, 8 bits, no parity, 1 stop bit */
  set_interface_attribs(fd, B115200, 0);

  // set no blocking
  set_blocking (fd, 0);                


  while (1) {
    gets(command);

    //send 7 character greeting
    write (fd, "hello!\n", 7);           

    // sleep enough to transmit the 7 plus 
    usleep ((7 + 25) * 100);             
                                     
    // receive 25:  approx 100 uS per char transmit
    char buf [100];
    // read up to 100 characters if ready to read
    int n = read (fd, buf, sizeof buf);  
    }
  }


