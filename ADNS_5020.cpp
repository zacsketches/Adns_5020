/*
    See ADNS.h for usage info.

    Implements functions to control an optial mouse sensor via
    an Arduino.

    I use this sensor to provide input to an optical flow odometer
    on a mobile robot.
    
    Zac Staples zacstaples(at)mac.com
    July 2014
*/

//--------------Write------------------------------//
void Adns_5020::write(uint8_t address, uint8_t data){
    // take the CS pin low to select the chip:
    digitalWrite(nCS,LOW);
    //  send in the address and value via SPI:
    SPI.transfer(address);
    SPI.transfer(data);
    // take the SS pin high to de-select the chip:
    digitalWrite(nCS,HIGH); 
}

//--------------Read------------------------------//
uint8_t Adns_5020::read(uint8_t address){
    uint8_t data;
    // take the CS pin low to select the chip:
    digitalWrite(nCS,LOW);
    //  send in the address via SPI:
    SPI.transfer(address);
    /*
        TODO use assembly language code from Simon Monk's going beyond 
        sketches to speed up this pinMode change.
    */
    // put the MOSI pin into high 'Z' while we communicate on the MISO pin
    pinMode(MOSI, INPUT);
    // make the transfer
    data = SPI.transfer(0x00);
    // return the MOSI pin to its OUTPUT state so it is ready to communicate
    pinMode(MOSI, OUTPUT);
    // take the SS pin high to de-select the chip:
    digitalWrite(nCS,HIGH);
    
    /*
        TODO I made a format change in the return statement to take the 
        parens out...make sure this works.
    */ 
    return data;

}

//--------------Attached-------------------------//
bool attached() {
    res = true;
    /*
        TODO implement attached.  Read the Vendor ID
        register and compare against default values.
    */
    return res;
}

//--------------Reset----------------------------//
void Adns_5020::reset() {
    //put the sensor into reset
    digitalWrite(nReset, LOW);
    delay(750);
    //bring it back online
    digitalWrite(nReset, HIGH);
    delay(750);
    /*
        TODO consider adding a call to the chip reset via SPI on register 0x39
    */
}

//-------------Config----------------------------//
int Adns_5020::config() {
    //see datasheet page 23 for more details on the values returned by reading mcr
    int8_t conf = read(mcr);
    return (conf==0) ? 500 : 1000;
}

//-------------Motion_detected-------------------//
bool Adns_5020::motion_detected() {
    uint8_t motion = read(mor);
    return (motion > 127) ? true : false;
}

//-------------Dx--------------------------------//
int8_t Adns_5020::dx() {
    uint8_t dx_raw = ADNS_read(dxr);
    return int8_t(dx_raw);
}

//-------------Dy--------------------------------//
int8_t Adns_5020::dy() {
    uint8_t dy_raw = ADNS_read(dyr);
    return int8_t(dr_raw);
}

//-------------Squal-----------------------------//
uint8_t Adns_5020::squal() {
    return read(sqr);
}

//-------------Pixels----------------------------//
void Adns_5020::pixels(uint8_t* buffer) {
    //loop control variables
    uint8_t pixel_count = 0;
    
    //reset the pixel grab counter
    write(pxr, 0x00);
    
    for(pixel_count = 0; pixel_count < 225; ++i) {
        buffer[pixel_count] = read(pxr);
    }
}

