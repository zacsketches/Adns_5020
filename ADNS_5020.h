#ifndef ADNS_5020_h
#define ADNS_5020_h JUL_2014

/*
    ADNS_5020 is an introductory optical tracking mouse sensor.
    This library allows an arduino to directly connect to the ADNS
    5020.  Default values in the constructor are for the Arduino UNO
    I primarily use with this library.  Construct the object with
    your desired pins for a different Arduino board.
    
    HARDWARE NOTE: The ADNS_5020 does NOT implement SPI in the standard
    way.  In the psuedo-standard SPI (SPI is not an ISO or IEEE Standard)
    requires 4 wires and is full duplex.
         On the ADNS 5020 the timing is SPI compliant, but the chip
    consolidates the MOSI and MISO pins into a single SDIO (Serial Data
    Input/Output) pin.  Therefore, in order to use the Arduino SPI
    library to handle the communicaiton timing users will have to run two
    wires from the Arduino MOSI and MISO to the same pin on the ADNS 5020.
    Then when the sensor is communicating back to the Arduino, YOU MUST
    TAKE THE MOSI PIN TO INPUT MODE IN ORDER TO TRI-STATE THE PIN AND
    PUT IT IN A HIGH IMPEDENCE STATE.  If this precaution is not 
    taken then the MOSI pin may present a ground for signals coming
    toward it from the sensor.
    
    STYLE NOTE: The Arduino "compiler" dialect of C++ allows byte and char
    as 8 bit integers.  However, I've chosen to use the more specific 
    int8_t and uint8_t to specify when the byte is signed and when it 
    is not.
*/

#include <arduino.h>
#include <SPI.h>

class Adns_5020 {
private:
    uint8_t nReset;
    uint8_t nCS;
    uint8_t MOSI;
    uint8_t MISO;
    uint8_t SCLK;

    const static uint8_t mor = 0x02;    //motion register
    const static uint8_t dxr = 0x03;    //dx register
    const static uint8_t dyr = 0x04;    //dy register
    const static uint8_t sqr = 0x05;    //sensor quality register
    const static uint8_t pxr = 0x0b;    //pixel start register 
    
    
    //write the data byte to the sensor register address
    void write(uint8_t address, uint8_t data);
    
    //read a register byte from the sensor
    uint8_t read(uint8_t address);
    
public:
    Adns_5020(uint8_t nReset_pin, 
              uint8_t nCS_pin, 
              uint8_t MOSI_pin = 11, 
              uint8_t MISO_pin = 12,
              uint8_t SCLK_pin = 13)
        :nReset(nReset_pin), nCS(nCS_pin), MOSI(MOSI_pin), MISO(MISO_pin), SCLK(SCLK_pin)
    {
        //set pin I/O direction
        pinMode (nReset, OUTPUT);
        pinMode (nCS, OUTPUT);

        //put the device in reset not chip selected
        digitalWrite(nReset,LOW);
        digitalWrite(nCS,HIGH);
        
        // See wikipedio on SPI for a good description of clock phase and polarity
        // then see comm diagrams on datasheet to determine that SPI_MODE is is 3 for 
        // the ADNS 5020
        SPI.begin();
        SPI.setDataMode(SPI_MODE3);     //Clock polarity and Clock phase 1
        delay(1000);    //let SPI start up
        
        //enable the ADNS sensor
        digitalWrite(nReset, HIGH);
    }
    
    // check the SPI connection by polling fixed data in the sensor to ensure
    // it is ready to work.  If the sensor fails to reply or the the wrong 
    // data comes back then the sensor is malfunctioning
    bool attached();
    
    //Toggle the reset pin, leaving the chip ready to work
    //Introduces 1.5 second delay
    void reset();
    
    // The sensor doesn't directly compute distance.  Instead it computes
    // counts per inch.  This function returns the counts per inch the 
    // system is configured to measure.
    int config();
    
    // read ADNS register to determine if motion has occurred since the last read
    bool motion_detected();
  
    // read the change in position for the x and y axis.
    int8_t dx();
    int8_t dy();
    
    // read ADNS Surface Quality.  See datasheet for details, but taking a moving 
    // average on this value will provide a good indication of of how well the 
    // optical sensor is focused on the surface beneath it. Max value is 144 and
    // min is 0.  Higher value represents more recognized features in the frame.  Squal
    // near 0 when there is no discernable surface beneath the sensor.
    uint8_t squal();
    
    // access to the pixel image the sensor sees on the suraface
    // users must handle memory for this method and pass a pointer to 
    // block of 125 uint_8 bytes.  The function will fill this array of bytes
    // with the values for the 125 pixels in the sensor image.
    /*
        TODO need to think of a way to monitor health on this series of 225 read
        opaertion and return
        a bool instead of void to show success or failure.
    */    
    void pixels(uint8_t* buffer); 
            
  
};

#endif