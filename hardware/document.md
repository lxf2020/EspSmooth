# RosSmoothBoard Description
 ## 1、ESP32 PIN Plan
  - 1 RS232 ==> UART1(RXD->GPIO18,TXD->GPIO19)
  - 1 USB TO RS232 ==> UART2(CH340C,RXD->GPIO16,TXD->GPIO17)
  - 1 USB flash disk OR SD card TO SPI ==> HSPI(CH376S,MOSI->GPIO12,MISO->GPIO13,CLK->GPIO14,CS->GPIO15) 
  - 2 Setpper motor ==> TMC2208(M1STEP->GPIO33,M2STEP->GPIO32,M1DIR、M1EN、M1_PDN_UAT、M2DIR、M2EN、M2_PDN_UAT->MCP23017)
  - 1 PWM ==> GPIO27
  - 4 Input AND 3 Output ==> MCP23017(I2C,SCK->GPIO25,SDA->GPIO26) 
 ## 2、[SD卡总结-SPI模式:管脚定义](https://www.cnblogs.com/mr-bike/p/3546228.html)
 ## 3、[ESP32 管脚参考](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
