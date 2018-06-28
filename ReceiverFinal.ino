/*
 * Vincent Lopez, Austin Erben
 * CSIS 10C Final Project
 * Laser Morse Code Receiver
 */

unsigned long signal_len,break_len,t1,t2,t3;   // timer vars
int inputPin = 0;                 // input pin for PV Cell                
String code = "";                 // string for Morse Code Data

int _speed_ = 200;                  // initialize words per minute. Should be the same as the Transceiver
int dotlen = 1200/_speed_;         // calculates length of single dot
int dashlen = 3 * dotlen;          // calculates length of single dash
int thresh = 300;                  // Threshold the laser diode breaks when it flashes onto the PV Cell. Value is read in from analogRead
void setup()

{
  Serial.begin(9600);
  pinMode(inputPin, INPUT_PULLUP); // internal pullup resistor is used to simplify the circuit
}

void loop()
{
NextDotDash:
  while (analogRead(inputPin) < thresh) {}  // While the analogRead is below the thresh keep track of time passed
  t1 = millis();                            
  
  while (analogRead(inputPin) > thresh) {} // While the analogRead is above the thresh keep track of time passed
  t2 = millis();         
                     
  signal_len = t2 - t1; // signal_len corresponds to either a dot(one unit on), dash(three units on), space between letters(one unit off), or space between words(3 units off)
  if (signal_len > (.75*dotlen))                      // Error correction if signal_len > (75% of dot length) signal_len will be sent to readio()
  {
    code += readio();                      // function to read dot or dash
    Serial.print("");
  }
  
  // Checking if end of message
  while (analogRead(inputPin) < thresh) // While the analogRead is below the thresh keep track of time passed
  {
    if((millis() - t2) > (11 * dotlen)) // If analogRead(inputPin) > 10*dotlen then end of message. Convert string code(morse) into output(alphabet & nums)
    {
       convertor(); // sending code(morse) to be decoded(alphabet & nums)
       Serial.print('\n');
       goto NextDotDash; // start reading the message again
    }
  }
  t3=millis();
  break_len = t3 - t2; // length of time that passed between the last break in thresh (t2) and newest break in thresh (t3)
  if (break_len > (2.5 * dotlen)) // if break_len is > 80% of defined length of a "space" 
  {
    readbreak();
    code = "";
  }
  goto NextDotDash;
}

char readio()
{
  if (signal_len < (1.2 * dotlen) && signal_len > (.75 * dotlen))
  {
    return '.';                        //if button press less than 0.6sec, it is a dot
  }
  else if (signal_len > (.75 * dashlen) && signal_len < (1.2 * dashlen))
  { 
    return '-';                        //if button press more than 0.6sec, it is a dash
  }
  else {
    return;
  }
}

char readbreak()
{ 
  convertor(); // decode the word
  if (break_len > (1.1 * dashlen) && break_len < (11 * dotlen)) // checking if break_len is a space and not the end of message
  {
    Serial.print(" ");                    //if break between words add space
  }
}

void convertor()
{
  // array of morse code strings starting with A in morse. letters[0] corresponds to chars[0]
  static String letters[54] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", ".----","..---","...--","....-",".....","-....","--...",
                             "---..","----.","-----",".-.-.-","--..--","..--..",".----.","-.-.--","-..-.","-.--.","-.--.-",".-...","---...","-.-.-.",
                             "-...-",".-.-.","-....-","..--.-",".-..-.","...-..-",".--.-."
                            };
  static String chars[54] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","1","2","3","4","5",
                           "6","7","8","9","0",".",",","?","'","!","/","(",")","&",":",";","=","+","-","_","'","$","@"};

  String output = "";
  String currentLetter = "";
  int len = code.length(); // to keep a static length for the loop
  
  for (int i=0; i<len; i++) //reads a letter of morse
  {  
    currentLetter += code[0];
    code = code.substring(1);
  }
  
  for (int j=0;j<54;j++){
    if (letters[j] == currentLetter) // find the index of letters that corresponds with given symbol
    {
      output += chars[j]; // using the previous index find the corresponding letter or num
      goto Done;
    }    
  }

Done:
  Serial.print(output); // print one letter at a time
}
