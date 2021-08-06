#include <SPI.h>
#include <SD.h>
#include <bossa.h>

#define LED_PIN           33
#define SD_MOSI           15
#define SD_SS             13
#define SD_SCK            14
#define SD_MISO           2
#define SD_FREQ           1000000
#define TX_PIN            17
#define RX_PIN            16

SPIClass SPI1(HSPI);    
HardwareSerial SerialPRN(1);

class ESP32SerialPort : public SerialPort {
    public:
        ESP32SerialPort(const std::string& name) : SerialPort(name) {
        }
        bool open(int baud, int data, Parity parity, StopBit stop) {return true;}
        void close() {}
        bool initcmd() {return true;}
        bool endcmd() {return true;}
        bool isUsb() {return true;}
        int read(uint8_t* data, int size) {const int n = SerialPRN.readBytes(data,size); return n;}
        int write(const uint8_t* data, int size) {const int n = SerialPRN.write(data,size); return n;}
        int get() {int c = SerialPRN.read();Serial.print('<');Serial.println(c); return c;}
        int put(int c) {SerialPRN.write(c);SerialPRN.flush(); Serial.print('>');Serial.println(c);}

        bool timeout(int millisecs) {SerialPRN.setTimeout(millisecs);}
        void flush() {SerialPRN.flush();}
};

const int chipSelect = 4;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Open the flashing serial port
  SerialPRN.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  SerialPRN.println("Testing");

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  SPI1.begin(SD_SCK, SD_MISO, SD_MOSI, SD_SS);
  if (!SD.begin(SD_SS, SPI1, SD_FREQ)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("/firmware.bin");

  // if the file is available, write to it:
  if (dataFile) {
    std::string name = "COM";
    SerialPort::Ptr x(new ESP32SerialPort(name));
    BOSSA::flash(dataFile, x);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening firmware.bin");
  }
}

void loop() {
}
