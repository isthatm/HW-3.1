#include "mbed.h"

Thread thread_master;
Thread thread_slave;

SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D9);

SPISlave device(PD_4, PD_3, PD_1, PD_0); //mosi, miso, sclk, cs; PMOD pins

DigitalOut led(LED3);

int slave() {
  device.format(8, 3); 
  device.frequency();

  while (1) {
    if (device.receive()) {
      int mode = device.read(); // Read mode from master
      printf("mode = %0x\n", mode);
      if (mode == 0x01) {     // mode 01
        int v;
        v=0x01;
        device.reply(v);   // Make this the next reply  (to line 72 ?)
        v = device.read(); // Confirmation to master
        v = device.read(); // Read another byte (number) from master
        printf("Read from master: v = %d\n", v);
        v = v + 1;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Read again to allow master read back
        led =! led;
      } 
      else if(mode == 0x02){ // mode 02
        int v;
        v=0x02;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Confirmation to master
        v = device.read(); // Read another byte from master
        printf("Read from master: v = %d\n", v);
        v = v + 2;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Read again to allow master read back
        led =! led;
      }
      else { //Undefined mode
        printf("Default reply to master: 0x00\n");
        device.reply(0x00); // Reply default value
        int v;
        v=0x00;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Confirmation to master
        v = device.read(); // Read another byte from master
        printf("Read from master: v = %d\n", v);
        v = v + 0;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Read again to allow master read back
      };
    }
  }
}

void master(){
    int number = 10;
    int masterCommand[3] = {0x01, 0x02, 0x03};
    spi.format(8,3);
    spi.frequency();

    for (int i = 0; i < 3; i++){
        cs = 1;
        cs = 0;
        printf("THE DEVICE IS CONNECTED.\n");

        //choose the mode & get the response from slave
        spi.write(masterCommand[i]);
        ThisThread::sleep_for(100ms); // wait for slave printf debug
        int response = spi.write(masterCommand[i]); 
        cs = 1;
        ThisThread::sleep_for(100ms); // wait for slave printf debug 
        printf("First response from slave = %d\n", response);

        // Send the number
        cs = 0;
        printf("Sent number to slave: %d\n",number);
        spi.write(number); // send number to slave
        ThisThread::sleep_for(100ms);
        response = spi.write(number); //read slave's reply
        ThisThread::sleep_for(100ms);
        printf("Second response from slave = %d\n", response);
        cs = 1;
        
        number += 1;
    }

}
int main(){
    thread_master.start(master);
    thread_slave.start(slave);
}