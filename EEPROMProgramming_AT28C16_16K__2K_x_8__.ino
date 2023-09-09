#define WE 39
#define OE 41
#define CE 43


const int DATA_PINS[] = {23, 25, 27, 29, 31, 33, 35, 37};
const int ADDRESS_PINS[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42};
bool Reformatting = false;

//------------------------------------------------------------------------------------------------------------
//Sets Desired Address On Address Pins
void setAddress(int address, bool outputEnable)
{
digitalWrite( OE, HIGH );

for (int i = 0; i < 11; i++)
{
int b = (address >> i) & 1; // Extract each bit from addressValue
digitalWrite(ADDRESS_PINS[i], b); // Set the pin according to the bit value  
}

digitalWrite(OE, outputEnable);

}
//------------------------------------------------------------------------------------------------------------
//A Way To Lump All Data Pins Together
void setDataMode( bool state )
{

for ( int i = 0; i < 8 ; i++ )
    pinMode( DATA_PINS[i], state );

}
//------------------------------------------------------------------------------------------------------------
//Funcion to reverse the byte because the wiring may or may not be wrong on the board
byte reverse(byte x) {
    byte y = 0;
    for (int i = 0; i < 8; i++) {
        y <<= 1;
        y |= (x & 1);
        x >>= 1;
    }
    return y;
}
//------------------------------------------------------------------------------------------------------------
//Returns The Data At Given Address
int readEEPROM(int address)
{

    // set all the DATA_PINS GPIO pins to input
    setDataMode( INPUT );

    setAddress(address, LOW);
    delayMicroseconds(3);

    int data = 0;

    for ( int i = 7; i >= 0; i-- )
    {
    data = (data << 1) + digitalRead( DATA_PINS[i] ); // Shift Left and add the next bit to the LSB position
    }

    //data = reverse(data);
    return data;
}
//------------------------------------------------------------------------------------------------------------
//Writes Byte Of Data To Given Address
void writeEEPROM(int address, int data)
{
    //data = reverse(data);
    //address = address - 1;      --WRITES TO THE ADDRESS ONE ABOVE OF WHAT YOU SET
    // Set all the DATA_PINS GPIO pins to output
    setDataMode( OUTPUT );

    setAddress(address, HIGH);

    for ( int i = 0 ; i < 8 ; i++ )
    {
    digitalWrite( DATA_PINS[i], data & 1 /*Getting Least Significant Bit*/ );
    data = data >> 1;
    }

    //Pulsing Write Enable (has a max pulse width of 1000 nanoseconds btw)
    digitalWrite(WE, LOW);

    delayMicroseconds(10);
    
    digitalWrite(WE, HIGH);

    // Writing only starts when WE goes high. The AT28 series EEPROMs have a way of polling
    // the data pins to check if the write has been completed.

    // First, OE needs to go LOW
    digitalWrite(OE, LOW);

    // Next, set the data pins to inputs
    setDataMode( INPUT );

    // We want to check the MSB of the data pins against the MSB of our data byte
    bool msb = bitRead( data, 7);

//if ( Reformatting == false){
    // bit 7 will be in an inverted state until writing has been completed.
    // This while loop will run until it's back to it's non-inverted state.
    Serial.print('\n');
    Serial.print("Programming EEPROM");
    while( digitalRead( DATA_PINS[7] ) != msb ){ Serial.print(".."); }
    Serial.println("\nDone\n");
//}
    digitalWrite(OE, HIGH);
    
}
//------------------------------------------------------------------------------------------------------------
//Reads Entire EEPROM
void DumpContents()
{
    char buf[10];
    int LN = 0;
    int i = 1;
    //2048 for entire chip
    for ( int address = 0; address < 2048; address++ )
    {
        if( (address % 16) == 0 )   // If the address divided by 16 has no remainder
        {
        LN = (i*16);
        sprintf(buf,"\n%03x",address);
        Serial.print(buf);  // print the value at the current address
        Serial.print(" : ");
        Serial.print(LN);
        Serial.print(" -");
        i++;
        }
        
        int b = readEEPROM(address);
        
        sprintf(buf," %02x", b );
        Serial.print(buf);
    }

    Serial.println("");

}
//------------------------------------------------------------------------------------------------------------
//Erases Entire EEPROM
void WipeEEPROM(){
  Reformatting = true;
  Serial.print("Erasing EEPROM");
  for (int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 128 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" Finished Erasing");
  delayMicroseconds(250);
  Reformatting = false;
}
//------------------------------------------------------------------------------------------------------------
//Fills Entire EEPROM With Specified Value For Testing
void FillEEPROM(int FILL){
  Reformatting = true;
  Serial.print("Filling EEPROM");
  for (int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, FILL);

    if (address % 128 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" Finished Filling");
  delayMicroseconds(250);
  Reformatting = false;
}
//------------------------------------------------------------------------------------------------------------
void setup()
{

// Set all address pins as outputs

for (int i = 0; i < 11; i++)
{
    pinMode(ADDRESS_PINS[i], OUTPUT);
}


pinMode(WE, OUTPUT);
pinMode(OE, OUTPUT); 
pinMode(CE, LOW);
pinMode(CE, OUTPUT);
pinMode(CE, LOW);
digitalWrite(WE, HIGH);
digitalWrite(OE, HIGH);

Serial.begin(57600);

//-----------------Start Program Here--------------------------

DumpContents();

}

void loop()
{

}
