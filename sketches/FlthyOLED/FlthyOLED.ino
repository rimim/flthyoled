/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                           ///
///                               Flthy Holoprojectors v1.81 [OLED VERSION]                                   ///
///                                       by Ryan Sondgeroth                                                  ///
///                                         aka FlthyMcNsty                                                   ///
///                                           08-22-2018                                                      ///
///                                                                                                           ///
///                                                                                                           ///
///      Combines the movement and display functions of each Holoprojector into a single easy                 ///
///     to control sketch.  It utilizes 7 LED NeoPixel boards inside each HP to produce a more                ///
///        life-like representation of a hologram projection. Servo control is handled by a                   ///
///      Adafruit 16 Channel I2C Breakout board, giving increased flexibility over the I2C bus,               ///
///     as well as the ability to operate the HP Servos with Neopixel LEDs from the same sketch.              ///
///       The regular arduino servo library doesn't get along with Adafruits Neopixel library,                ///
///                  Big Happy Dude's Slow Servo Library is much better any way, :-D                          ///
///                                                                                                           ///
///                        Rhyno45 is a Wanker, in case Ya'll didn't know!                                    ///
///                                                                                                           ///
///   Version History:                                                                                        ///
///     v1.1 - Added One Time HP Twitch Function                                                              ///
///                                                                                                           ///
///     v1.2 - Fixed Center Positioning Bug and others; streamlined Servo & LED functions;                    ///
///            optimized variable use & streamlined variable containers; increased servo position             ///
///            presets to 8 directions; added status light on Serial/I2C receive; added a                     ///
///            random color option; added support for the RGBW version of the Adafruit Jewels;                ///
///            added a new HP wag function; improved the flicker behavior and color to white                  ///
///            balance on the default LED function; added speed option to Dim Pulse function;                 ///
///            corrected unwanted behavior when attemping to clear LEDs while auto sequence is                ///
///            active; added test mode; added ability to assign independent default & short circuit           ///
///            colors for each HP;                                                                            ///
///                                                                                                           ///
///     v1.3 - Added ability to send sequence run time values via command string, cleaned up some             ///
///            duplicate/unnecessary code, added 3 additional options for the HP designator values            ///
///            in command code (X,Y,Z see below), assign specific command strings to auto LED                 ///
///            twitch                                                                                         ///
///                                                                                                           ///
///     v1.4 - Updated volatile variable handling to better deal with sequence runtime/halt                   ///
///            functionality as well as processing Serial and I2C commands.                                   ///
///                                                                                                           ///
///     v1.5 - Updated mapPulselength function to be more accurate.                                           ///
///                                                                                                           ///
///     v1.6 - 1. Added the ability to use random LED sequences in addition to the assigned default.          ///
///               sequence for each light when random LED twitch is enabled. User may select                  ///
///               between these two behaviors in the settings as well as switch between them on               ///
///               the fly using I2C/Serial commands.                                                          ///
///            2. Added more extensive DEBUG feed back in Serial Monitor.                                     ///
///            3. Utilized compiler directives to optimize sketch for size.                                   ///
///            4. Utilized PROGMEM to reduce SRAM usage of global setting variables resulting in a            ///
///               more stable loop.                                                                           ///
///            5. Made some additional efficiency tweaks.                                                     ///
///            6. Fixed a bug in how timed Leia Sequence (S1) functions fixing runtime being                  ///
///               interupted.                                                                                 ///
///                                                                                                           ///
///      v1.7 - Bug fix identified by skelmir.  Thanks, for the input, my friend.                             ///
///                                                                                                           ///
///      v1.8 - Added the ability to set custom "Off Colors" per IOIIOOO's (Jason Cross) request              ///
///                                                                                                           ///
///    Modified by skelmir to add support for an OLED display to play movie during the Leia sequence          ///
///                                                                                                           ///
///                                 Original unmodified code here:                                            ///
///                         http://www.2geekswebdesign.com/FlthyHPs/Sketches                                  ///
///                                                                                                           ///
///                                      Special thanks to...                                                 ///
///   LostRebel and Knightshade for significant input on both the general functions of the system             ///
///          and this code!  It helped me make vast improvements in operation and effeciency.                 ///
///                                                                                                           ///
///                                                                                                           ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                           ///
///     Commands and Structure                                                                                ///
///                                                                                                           ///
///     DT##C or DT##CS or DT##R or DT##P                                                                     ///
///                                                                                                           ///
///     D - the HP designator                                                                                 ///
///          F - Front HP                                                                                     ///
///          R - Rear HP                                                                                      ///
///          T - Top HP                                                                                       ///
///          A - All 3 HPs                                                                                    ///
///          X - Front & Rear HPs                                                                             ///
///          Y - Front & Top HPs                                                                              ///
///          Z - Rear & Top HPs                                                                               ///
///          S - Sequences (See Below)                                                                        ///
///                                                                                                           ///
///     T - the Sequence Type is either 0-Led Fuctions and 1-Servo Functions                                  ///
///                                                                                                           ///
///    ## - the Sequence Value including leading zero if necessary, ie sequence 3 is 03                       ///
///                                                                                                           ///
///     C - (Optional), the Color integer value from list below:                                              ///
///        Basic Color Integer Values                                                                         ///
///           1 = Red                                                                                         ///
///           2 = Yellow                                                                                      ///
///           3 = Green                                                                                       ///
///           4 = Cyan (Aqua)                                                                                 ///
///           5 = Blue                                                                                        ///
///           6 = Magenta                                                                                     ///
///           7 = Orange                                                                                      ///
///           8 = Purple                                                                                      ///
///           9 = White                                                                                       ///
///           0 = Random                                                                                      ///
///                                                                                                           ///
///     S - (Optional), Speed setting integer for the Dim Pulse LED function below (0-9)                      ///
///                                                                                                           ///
///     R - (Optional), Random State for clearing LED displays                                                ///
///        Random State Integer Values                                                                        ///
///           1 = Use Default Sequences                                                                       ///
///           2 = Use Random Sequences                                                                        ///
///                                                                                                           ///
///     P - (Optional), the Position integer value from list below:                                           ///
///        Preset Position Integer Values                                                                     ///
///           0 = Down                                                                                        ///
///           1 = Center                                                                                      ///
///           2 = Up                                                                                          ///
///           3 = Left                                                                                        ///
///           4 = Upper Left                                                                                  ///
///           5 = Lower Left                                                                                  ///
///           6 = Right                                                                                       ///
///           7 = Upper Right                                                                                 ///
///           8 = Lower Right                                                                                 ///
///                                                                                                           ///
///     D001    - Leia Sequence, Random shades of blue to mimic Leia Hologram                                 ///
///     D002C   - Color Projector Sequence, Like Leia above but using color command value                     ///
///     D003CS  - Dim Pulse Sequence, Color slowly pulses on and off                                          ///
///     D004C   - Cycle Sequence, using color command value                                                   ///
///     D005C   - Toggles Color, Simply sets LEDs tp solid color value.                                       ///
///     D006    - Rainbow Sequence                                                                            ///
///     D007C   - Short Circuit, Led flashes on and off with interval slowing over time                       ///
///     D096    - Clears LED, Disables Auto LED Sequence & "Off Color"                                        ///
///     D0971   - Clears LED, Enables Auto LED Sequence,Enables Default Sequences, Disables "Off Color"       ///
///     D0972   - Clears LED, Enables Auto LED Sequence,Enables Random Sequences, Disables "Off Color" .      ///
///     D098    - Clears LED, Disables Auto LED Sequence, enables "Off Color"                                 ///
///     D0991   - Clears LED, Enables Auto LED Sequence,Enables Default Sequences, Enables "Off Color"        ///
///     D0992   - Clears LED, Enables Auto LED Sequence,Enables Random Sequences, Enables "Off Color"         ///
///                                                                                                           ///
///     D101P   - Sends HP to a Preset Position*                                                              ///
///     D102    - Enables RC Control on HP (Left/Right)*                                                      ///
///     D103    - Enables RC Control on HP (Up/Down)*                                                         ///
///     D104    - Sends HP to a Random Position                                                               ///
///     D105    - Wags HP Left/Right 5 times*                                                                 ///
///     D106    - Wags HP Up/Down 5 times*                                                                    ///
///     D198    - Disables Auto HP Twitch                                                                     ///
///     D199    - Enables Auto HP Twitch                                                                      ///
///                                                                                                           ///
///       S1    - Leia Mode (Front HP in Down Position, Leia LED Sequence, all other HPs disabled)*           ///
///       S2    - Play R2 Cartoon [OLED]                                                                      ///
///       S3    - Play Deathstar Plans movie [OLED]                                                           ///
///       S4    - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence, Disables Off Color       ///
///       S5    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ default seqs.),         ///
///               Disables Off Color                                                                          ///
///       S9    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ random seqs.),          ///
///               Disables Off Color                                                                          ///
///       S7    - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence, Enables Off Color        ///
///       S8    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ default seqs.),         ///
///               Enables Off Color                                                                           ///
///       S9    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ random seqs.),          ///
///               Enables Off Color                                                                           ///
///                                                                                                           ///
///   * Function disabled or severely limited when Basic HP Positioning in enabled.                           ///
///     I recomend using Preset Position Coordinates                                                          ///
///                                                                                                           ///
///    Runtime values can be added to any command string by appending a pipe (|) followed by a                ///
///     numeric value indicating the desired time in seconds you wish the sequence to run.                    ///
///                                                                                                           ///
///       ie.  A007|25 would run the Rainbow Sequence on all 3 HPs for 25 seconds then clear each             ///
///            one, returning to the system's last known auto twitch mode.                                    ///
///                                                                                                           ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    OLED SPECIFIC                                                                                          ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                           ///
///    These commands have been added because extracting the SD card can be difficult once the Holo has       ///
///    been installed. The [filename] is 7-characters maximum and should have no extension. The extensions    ///
///    BD2 will be added automatically. The filename is not case-sensitive since the SD card is a Fat32       ///
///    file system.                                                                                           ///
///                                                                                                           ///
///    "OD[filename]": Download movie via serial to SD card                                                   ///
///    "OP[filename]": Play movie with the specified name                                                     ///
///    "OX[filename]": Delete movie of the specifid name if it exists                                         ///
///                                                                                                           ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  Required Libraries                                              ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>                        //
#include <Adafruit_NeoPixel.h>           // Source: https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_PWMServoDriver.h>     // Source: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Servos.h>                      // Source: https://drive.google.com/file/d/0B5B8A65frsBgZ3VpeGxpM1lzaFE/edit  Thanks BHD!


#define VERSION 8
#define HPCOUNT 3
#define HPPOSITIONS 9

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  Start User Adjustable Settings                                  ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HP_FRONT_LED_PIN 45
#define HP_REAR_LED_PIN 2
#define HP_TOP_LED_PIN 3
#define RADAR_EYE_LED_PIN 4

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
/* Enable OLED support on the Mega */
#define OLED
#endif

//////////////////////////////////////////////////////////////////////////////////
///*****                       Enable Debug Mode                          *****///
///*****                                                                  *****///
///*****    Uncomment the following line to enable debugging mode where   *****///
///*****  additional sketch information is printed to the Serial Monitor. *****///
///*****  This is strictly for debugging and shouldn't be enabled during  *****///
///*****                    normal droid operation.                       *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define DEBUG true

//////////////////////////////////////////////////////////////////////////////////
///*****                 Assign Arduino IC2 Address Below                 *****///
///*****                                                                  *****///
///*****  An I2C address of 25 has been standardized for other HP devices *****///
///*****       so we will use it to. It can be changed here if needed.    *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define I2CADDRESS 0x19  // 25 in Hexadecmal


//////////////////////////////////////////////////////////////////////////////////
///*****               Assign Servo Board IC2 Address Below               *****///
///*****                                                                  *****///
///*****           The I2C address of the servo breakout board can        *****///
///*****       be set below, and corresponds with the address selected    *****///
///*****          on the board using the following jumper settings:       *****///
///*****                                                                  *****///
///*****      Address= 0x40 (64) Offset = binary 00000 (no jumpers)       *****///
///*****      Address= 0x41 (65) Offset = binary 00001 (A0)               *****///
///*****      Address= 0x42 (66) Offset = binary 00010 (A1)               *****///
///*****      Address= 0x43 (67) Offset = binary 00011 (A0 & A1)          *****///
///*****      Address= 0x44 (68) Offset = binary 00100 A2)                *****///
///*****      Address= 0x45 (69) Offset = binary 00101 (A2 & A0)          *****///
///*****      Address= 0x46 (70) Offset = binary 00110 (A2 & A1)          *****///
///*****      Address= 0x47 (71) Offset = binary 00111 (A2, A1, & A0)     *****///
///*****                 ...So on and so forth                            *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define SERVOI2CADDRESS 0x40  // 64 in Hexadecmal


//////////////////////////////////////////////////////////////////////////////////
///*****               Arduino Digital Pin Assignments for LEDs           *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
const byte LEDpins[HPCOUNT]   = {HP_FRONT_LED_PIN,HP_REAR_LED_PIN,HP_TOP_LED_PIN};

//////////////////////////////////////////////////////////////////////////////////////////
///*****                         Servo Board Pin Assignments                      *****///
///*****                                                                          *****///
///*****                    HPpins[HPCOUNT][2] = {{Front 1, Front 2},             *****/// 
///*****                                          {Rear 1, Rear 2},               *****///
///*****                                          {Top 1, Top 2}};                *****///
///*****                                                                          *****///
//////////////////////////////////////////////////////////////////////////////////////////
const byte HPpins[HPCOUNT][2] = {{0,1},           // Front HP Pins
                                 {2,3},           // Rear HP Pins
                                 {4,5}};          // Top HP Pins


//////////////////////////////////////////////////////////////////////////////////
///*****       Arduino Digital Pin Assignment for Output Enable (OE)      *****///
///*****                                                                  *****///
///*****  Output Enable (OE) is used to disable/enable servos between     *****///
///*****          movements to minimize unnecessary hum/noise.            *****///
///*****                                                                  *****///
///*****    The only reasons you would want to disable this is if...      *****///
///*****    1. You wish to use an additional microcontroller to send      *****///
///*****       servo control commands via the Adafruit library.           *****///
///*****    2. You want to use additional servos on the same servo        *****///
///*****        breakout board as your HP servos.                         *****///
///*****                                                                  *****///
///*****       Neither scenario is recommended because the increased      *****///
///*****   complexity far outweighs the costs savings of not purchasing   *****///
///*****                   an additional servo board.                     *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define OUTPUT_ENABLED_PIN 10       
#define OUTPUT_ENABLED_ON true   


//////////////////////////////////////////////////////////////////////////////////
///*****                        Enable RC Mode                            *****///
///*****                                                                  *****///
///*****    If you plan on using the RC functions to control your HPs,    *****///
///*****  uncomment the following line.  Otherwise, leaving it commented  *****///
///*****               out frees up some system resources.                *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
//#define ENABLERC true

#ifdef ENABLERC    //Do Not Edit This Line!
//////////////////////////////////////////////////////////////////////////////////
///*****                                                                  *****///
///*****            Arduino Digital Pin Assignment for RC Input           *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define RCPIN 9

//////////////////////////////////////////////////////////////////////////////////
///*****             Pulse Width Range of RC Stick/Controller             *****///
///*****               assigned to control HP movement.                   *****///
//////////////////////////////////////////////////////////////////////////////////
const unsigned int RCRange[2] = {1250, 1750};     // {Min, Max}

#endif    //Do Not Edit This Line!


//////////////////////////////////////////////////////////////////////////////////
///*****                    Adafruit Jewel Board Version                  *****///
///*****                                                                  *****///
///*****       The Adafruit Jewels now come in RGB and RGBW versions,     *****///
///*****        uncomment the following if you have the RGBW version.     *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define NEO_JEWEL_RGBW

//////////////////////////////////////////////////////////////////////////////////
///*****                          LED Brightness                          *****///
///*****                                                                  *****///
///*****                 Adjust LED Brightness Level (0-255)              *****///
//////////////////////////////////////////////////////////////////////////////////
#define BRIGHT 100  // Set LED Brightness Level (0-255)

//////////////////////////////////////////////////////////////////////////////////
///*****         Enable/Disable LED Auto Twitch and Random Sequences      *****///
///*****                                                                  *****///
///*****                           Auto Disable = 0                       *****///
///*****                 Auto Enabled w/ Default Sequence = 1             *****///
///*****                 Auto Enabled w/ Random Sequences = 2             *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
byte enableTwitchLED[HPCOUNT]  = {1,1,1};              // Leds:   Front, Rear, Top


//////////////////////////////////////////////////////////////////////////////////
///*****                    Default LED Auto Twitch Commands              *****///
///*****                                                                  *****///
///*****  List the command you wish to perform during the random twitch   *****/// 
///*****                              for each HP.                        *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
const byte defaultLEDTwitchCommand[3][3] = {{1,5,0},    // FRONT HP: Sequence, Color, Speed      
                                           {1,5,0},    // REAR HP: Sequence, Color, Speed 
                                           {1,5,0}};   // TOP HP: Sequence, Color, Speed   

//////////////////////////////////////////////////////////////////////////////////
///*****                   Enable/Disable Servo Auto Twitch               *****///
///*****                                                                  *****///
///*****                   Enabled = true, Disabled = false               *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
boolean enableTwitchHP[HPCOUNT]   = {true,true,true};  // Leds:   Front, Rear, Top 



///////////////////////////////////////////////////////////////////////////////////
///*****               LED & Servo Auto Twitch Interval Ranges             *****///
///*****                                                                   *****///
///*****      Use these values to select the range (min, max) in           *****///
///*****     seconds to randomly select the next auto twitch interval.     *****///
///*****      This helps give the illusion these displays & movements      *****///
///*****                     are totally autonomous.                       *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////  
    
const unsigned int LEDTwitchInterval[HPCOUNT][2] = {{45,180},            // (45secs-3mins) Enter min and max seconds Front HP Led Twitches  
                                                    {45,180},            // (45secs-3mins) Enter min and max seconds Rear HP Led Twitches 
                                                    {45,180}};           // (45secs-3mins) Enter min and max seconds Top HP Led Twitches    
    
const unsigned int HPTwitchInterval[HPCOUNT][2] = {{45,120},             // (45s-2mins) Enter min and max seconds Front HP Servo Twitches  
                                                   {60,180},             // (1min-3mins) Enter min and max seconds Rear HP Servo Twitches
                                                   {60,180}};            // (1min-3mins) Enter min and max seconds Top HP Servo Twitches  

///////////////////////////////////////////////////////////////////////////////////
///*****                  LED Auto Twitch Run Time Ranges                  *****///
///*****                                                                   *****///
///*****    Use these values to select the range (min, max) in seconds     *****///
///*****   to randomly select how long the sequence runs each time the     *****///
///*****                      LED twitch is executed.                      *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////  
const unsigned int LEDTwitchRunInterval[HPCOUNT][2] = {{5,25},   // (5-25s) Front LED Runtime
                                                       {5,25},   // (5-25s) Rear LED Runtime
                                                       {5,25}};  // (5-25s) Top LED Runtime  

//////////////////////////////////////////////////////////////////////////////////
///*****                           Servo Speed                            *****///
///*****                                                                  *****///
///*****      Tweak these value to adjust the range of movement speeds    *****///
///*****     for your HP servos. Values in ms, Lower values results in    *****///
///*****      faster movement.   By using a range, the servos will not    *****///
///*****     always move at the same speed giving the subtle illusion     *****/// 
///*****           that each movement is purposeful and unique.           *****///
///*****                   They say its the little things.                *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////       
const unsigned int SERVO_SPEED[2] = {150, 400};



//////////////////////////////////////////////////////////////////////////////////
///*****                     Basic HP Servo Positioning                   *****///
///*****                                                                  *****///
///*****    The HP kits ship with this mode enabled, so they are some     *****/// 
///*****    what plug and play, but it is highly suggested you use the    *****/// 
///*****             Preset HP Servo Positioning covered below.           *****///
///*****                                                                  *****///
///*****   Uncomment the following if you wish to use this basic mode.    *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define ENABLEBASICHP true


#ifdef ENABLEBASICHP  //Do Not Edit This Line!
//////////////////////////////////////////////////////////////////////////////////
///*****                  Basic HP Servo Position Values                  *****///
///*****                                                                  *****///
///*****   If you find yourself needing to edit these values, you are     *****/// 
///*****  honesty way better off using the Preset HP Servo Positioning    *****/// 
///*****                           covered below.                         *****///
///*****                                                                  *****///  
//////////////////////////////////////////////////////////////////////////////////  
 const PROGMEM uint16_t HPposBasic[2][3] = {{1500,1300,1700},        // HP Servo 1 Values for Basic Mode (Center, Min, Max)
                                            {1500,1300,1700}};       // HP Servo 2 Values for Basic Mode (Center, Min, Max)

#else  //Do Not Edit This Line!                                       
  //////////////////////////////////////////////////////////////////////////////////
  ///*****                Preset HP Servo Position Coordinates              *****///
  ///*****                                                                  *****///
  ///*****   Since each Builder's droid is assembled differently, we found  *****///
  ///*****   using position coordinates generated randomly on the fly was   *****///
  ///*****   problematic due to the occasional servo movement attempting    *****///
  ///*****   to travel beyond the outer cowl path.  Therefore we use a set  *****///
  ///*****   of 9 position pairs for each HP, set to ensure they lie with   *****///
  ///*****    in a freely accessible position. These coordinates consist    *****///
  ///*****     of the servo position of each servo using its pulse width    *****///
  ///*****    value in microseconds. I suggest using a servo tester with    *****///
  ///*****    a digital display to find these position pairs while the HPs  *****///
  ///*****                      are mounted in the dome.                    *****///
  ///*****                                                                  *****///
  ///*****     The first coordinate pair in each row references the DOWN    *****/// 
  ///*****    position of the HP, the second pair is the CENTER position,   *****/// 
  ///*****    and the 3rd pair the UP position.  LEFT, UPPER LEFT & LOWER   *****///
  ///*****      LEFT are plotted using the fourth, fifth and sixth pairs    *****///
  ///*****      while RIGHT, UPPER RIGHT & LOWER RIGHT are plotted using    *****/// 
  ///*****               the seventh, eight and ninth pairs.                *****/// 
  ///*****                                                                  *****///
  ///*****     Random twitch movement is then chosen to randomly travel     *****/// 
  ///*****     between these 9 safe points minimizing the chance of the     *****/// 
  ///*****            movement being impeded causing excessive and          *****/// 
  ///*****                     unpleasant servo noise                       *****/// 
  ///*****                                                                  *****///
  ///*****          If you would rather do it the simple way, set           *****///
  ///*****   enableBasicHP to true and it will use the Center, Min and Max  *****///
  ///*****    values set in the HPposBasic array. These are based on the    *****///
  ///*****      assumed default center value of a normal servo so it is     *****///
  ///*****    important that your servo linkage is installed in a manner    *****///
  ///*****       where the center servo position corresponds with the       *****///
  ///*****       orientation you want your HP to be in when centered.       *****///
  ///*****                                                                  *****///
  ///*****          RC mode requires position coordinates to work well,     *****///
  ///*****           so Basic Positioning must be disabled by setting       *****///
  ///*****                        enableBasicHP to false.                   *****///
  ///*****                                                                  *****///
  ///*****     IT IS IMPORTANT TO NOTE...if you disassemble your HPs and    *****///
  ///*****     servo linkage after setting the following values, you will   *****///
  ///*****    most likely need to recheck and reset them as the chances of  *****///
  ///*****       reinstalling them exactly the same way is approximately    *****///
  ///*****                             3,720 to 1.                          *****///
  ///*****                                                                  *****///
  ///*****   To get the most out of this sytem, it is recomended that you   *****///
  
  ///*****    disable basic positioning and use the 9 custom locations for  *****///
  ///*****                         for each of your HPs                     *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////     
  const PROGMEM uint16_t HPpos[HPCOUNT][HPPOSITIONS][2] = {{{1377,1706},   // Front HP Down
                                                            {1477,1426},   // Front HP Center
                                                            {1422,1161},   // Front HP Up
                                                            {1611,1425},   // Front HP Left
                                                            {1611,1155},   // Front HP Upper Left                                                          
                                                            {1611,1739},   // Front HP Lower Left
                                                            {1255,1434},   // Front HP Right 
                                                            {1255,1115},   // Front HP Upper Right
                                                            {1255,1729}},  // Front HP Lower Right
                                                           {{1582,1330},   // Rear HP Down
                                                            {1500,1512},   // Rear HP Center
                                                            {1466,1773},   // Rear HP Up
                                                            {1705,1598},   // Rear HP Left
                                                            {1705,1797},   // Rear HP Upper Left
                                                            {1705,1389},   // Rear HP Lower Left
                                                            {1286,1460},   // Rear HP Right
                                                            {1286,1602},   // Rear HP Upper Right
                                                            {1285,1283}},  // Rear HP Lower Right
                                                           {{1507,1208},   // Top HP Down
                                                            {1501,1467},   // Top HP Center
                                                            {1443,1680},   // Top HP Up
                                                            {1624,1468},   // Top HP Left
                                                            {1624,1667},   // Top HP Upper Left
                                                            {1653,1272},   // Top HP Lower Left
                                                            {1267,1468},   // Top HP Right
                                                            {1258,1756},   // Top HP Upper Right
                                                            {1395,1176}}}; // Top HP Lower right
                    
#endif  //Do Not Edit This Line! 

///////////////////////////////////////////////////////////////////////////////////
///*****                      Default Color Settings                       *****///
///*****      `                                                            *****///
///*****     Select Default Colors for certain LED sequences using the     *****///
///*****                       integer values below:                       *****///
///*****                                                                   *****///
///*****             Basic Color Integer Values                            *****///
///*****               1 = Red                                             *****///
///*****               2 = Yellow                                          *****///
///*****               3 = Green                                           *****///
///*****               4 = Cyan (Aqua)                                     *****///
///*****               5 = Blue                                            *****///
///*****               6 = Magenta                                         *****///
///*****               7 = Orange                                          *****///
///*****               8 = Purple                                          *****///
///*****               9 = White                                           *****///
///*****               0 = Random                                          *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////        
const uint8_t defaultColor[HPCOUNT] = {5,5,5};      // {Front, Rear , Top} 5 = Blue, Color integer value for the hue of default sequence. 
                                                    // This is to appease MKelly and MKarp's whiney little asses, lol
const uint8_t shortColor[HPCOUNT] = {7,7,7};        // {Front, Rear , Top} 7 = Orange, Color integer value for the hue of ShortCircuit Message. 



///////////////////////////////////////////////////////////////////////////////////
///*****                    Optional Default "Off" Color                   *****///
///////////////////////////////////////////////////////////////////////////////////
///*****      `                                                            *****///
///*****     The default state of the LEDs is unlit, ie Black.  If you     *****///
///*****     would rather have the off state be a static color, set the    *****///
///*****     color integer from the list above for the corresponding HP    *****///
///*****     below.  Use the value -1 for off.  You will see this color .  *****///
///*****     after initial twitch.                                         *****///
///*****                                                                   *****///
///////////////////////////////////////////////////////////////////////////////////
const int offcolor[HPCOUNT] = {-1,-1,-1};      // {Front, Rear , Top}   Off (Default) = -1

//////////////////////////////////////////////////////////////////////////////////
///*****               Dim Pulse Speed Range and Default Speed            *****///
///*****                                                                  *****///
//////////////////////////////////////////////////////////////////////////////////
#define DIMPULSESPEED 5                         // Deafault speed if no value is given
const uint8_t dimPulseSpeedRange[2] = {5, 75};      // Range used to map to value options 0-9, Lower is faster.


////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  End User Adjustable Settings                                    ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SD.h>
#include <SPI.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// REQUIRED PIN ASSIGNMENT
////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

////////////////////////////////////////////////////////////
// Using https://www.adafruit.com/product/684
// OLED Breakout Board - 16-bit Color 0.96" w/microSD holder
////////////////////////////////////////////////////////////
// GND (G) - Gnd (Black Wire)
// VCC (+) - 5v (Red Wire)
// SDCS (SC) - Digital #53 (Gray Wire)
// OCS (OC) - Digital #47 (Orange Wire)
// RST (R) - Digital #49 (Green Wire)
// D/C (DC) - Digital #48 (Brown Wire)
// SCK (CK) - Digital #52 (White Wire)
// MOSI (SI) - Digital #51 (Blue Wire)
// MISO (SO) - Digital #50 (Purple Wire)
// CD (CD) - skip

#define miso 50
#define mosi 51
#define sclk 52
#define cs   47
#define dc   48
#define rst  49
#define SD_CS 53
#else  // defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

/* It is possible to run the OLED and everything else on the Mini Pro but you need to strip all the libraries */

// GND (G) - Gnd (Black Wire)
// VCC (+) - 5v (Red Wire)
// SDCS (SC) - Digital #4.(Gray Wire)
// OCS (OC) - Digital #10 (Orange Wire)
// RST (R) - Digital #9 (Green Wire)
// D/C (DC) - Digital #8 (Brown Wire)
// SCK (CK) - Digital #13 (White Wire)
// MOSI (SI) - Digital #11 (Blue Wire)
// MISO (SO) - Digital #12 (Purple Wire)
// CD (CD) - skip

#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8
#endif

#ifdef OLED
// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

// Option 1: use any pins but a little slower
Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, rst);  

File bd2File;
boolean bd2Download;
uint32_t bd2FrameCount;
uint32_t bd2Timestamp;
uint32_t bd2Timestart;
uint32_t bd2FrameCountInitial;
uint32_t bd2FramesPerSec;

static void oledDisplayClear()
{
   display.fillScreen(BLACK);
}

static void bd2CloseMovie()
{
  if (bd2File != NULL)
  {
    bd2File.close();
  }
  bd2FrameCount = 0;
  bd2FrameCountInitial = 0;
  bd2Timestamp = 0;
  bd2Timestart = 0;
  oledDisplayClear();
  restoreFrontHoloBrigthness();
}

void bd2DrawFrame(File fd)
{
  uint16_t sdbuffer[96];
  int frameType = (char)fd.read();
  if (frameType == -1)
  {
    /* Keyframe */
    for (int y = 0; y < 64; y++)
    {
      int num = fd.read(sdbuffer, sizeof(sdbuffer));
      if (num != sizeof(sdbuffer))
      {
      #ifdef DEBUG
        Serial.println(F("Error reading key frame"));
        Serial.print(F("Incomplete scanline: "));
        Serial.println(y);
      #endif
        bd2CloseMovie();
        break;
      }
      display.setAddrWindow(0, y, 96, 1);
      display.writePixels(sdbuffer, 96);
      display.endWrite();
    }
  }
  else if (frameType == 1)
  {
    /* Incremental */
    int x = fd.read();
    int y = fd.read();
    int w = fd.read();
    int h = fd.read();
    while (h-- > 0)
    {
      int num2 = fd.read(sdbuffer, w*sizeof(uint16_t));
      if (num2 != w*sizeof(uint16_t))
      {
      #ifdef DEBUG
        Serial.println(F("Error reading frame"));
        Serial.print(F("Incomplete scanline: "));
        Serial.println(y);
      #endif
        bd2CloseMovie();
        break;
      }
      display.setAddrWindow(x,y,w,1);
      display.writePixels(sdbuffer, w);
      display.endWrite();
      y++;
    }
  }
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

static void bd2PlayMovie(const char* filename)
{
  bd2CloseMovie();
#ifdef DEBUG
  Serial.print(F("bd2PlayMovie: "));
  Serial.println(filename);
#endif
  if ((bd2File = SD.open(filename)) == NULL) {
  #ifdef DEBUG
    Serial.print(F("File not found: "));
    Serial.println(filename);
  #endif
    return;
  }

  Serial.print(F("Playing movie: "));
  Serial.println(filename);
  // Set front holo to half-bright during movie play
  dimFrontHoloBrigthness();
  bd2FrameCount = read32(bd2File);
  bd2FrameCountInitial = bd2FrameCount;
  bd2FramesPerSec = read32(bd2File);
  bd2Timestamp = 0;
  bd2Timestart = millis();
}

static void bd2Idle()
{
  if (bd2FrameCount == 0)
  {
    return;
  }
  if (millis() - bd2Timestamp >= 100)
  {
    bd2Timestamp = millis();
    bd2DrawFrame(bd2File);
    bd2FrameCount--;
    if (!bd2FrameCount)
    {
      Serial.print("Time elapsed: ");
      Serial.print(bd2Timestamp-bd2Timestart);
      Serial.print("ms ");
      Serial.print((float)bd2FrameCountInitial / ((bd2Timestamp-bd2Timestart)/1000.0));
      Serial.println(" fps");
      bd2CloseMovie();
    }
  }
}

// This code is optional if size is a problem. It's convenience only.
#define OLED_DOWNLOAD
#ifdef OLED_DOWNLOAD
static const unsigned char pr2six[256] =
{
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static int bd2DownloadFile(File file, unsigned char* buffer, uint32_t bufferSize, uint32_t fileLength)
{
  uint32_t receivedFileSize = 0;
  while (receivedFileSize < fileLength)
  {
    boolean ack = false;
    uint32_t sum = 0;
    if (!Serial.find("ACK"))
    {
      Serial.print("NAK received=");
      Serial.println(receivedFileSize);
      return -1;
    }
    sum = Serial.readStringUntil('\n').toInt();
    unsigned char outch = 0;
    unsigned char* bufin = buffer;
    unsigned char* bufout = buffer;
    int nprbytes = Serial.readBytesUntil('\n', buffer, bufferSize);
    uint32_t sum2 = 0;
    for (int i = 0; i < nprbytes; i++)
    {
       sum2 += buffer[i];
    }
    if (sum == sum2)
    {
      receivedFileSize += nprbytes;
      showDownloadProgress(receivedFileSize, fileLength);
      while (nprbytes > 4) {
        *bufout++ = (unsigned char)((unsigned int) (pr2six[*bufin] << 2) | (unsigned int)pr2six[bufin[1]] >> 4);
        *bufout++ = (unsigned char)((unsigned int) (pr2six[bufin[1]] << 4) | (unsigned int)pr2six[bufin[2]] >> 2);
        *bufout++ = (unsigned char)((unsigned int) (pr2six[bufin[2]] << 6) | (unsigned int)pr2six[bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
      }
      if (nprbytes > 1) {
        *bufout++ = (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
      }
      if (nprbytes > 2) {
        *bufout++ = (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
      }
      if (nprbytes > 3) {
        *bufout++ = (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
      }
      file.write(buffer, bufout - buffer);
      Serial.println("ACK");
    }
    else
    {
      Serial.print("NAK:");
      Serial.print("badchecksum: ");
      Serial.print(sum2);
      Serial.print("expected: ");
      Serial.println(sum);
    }
  }
  showDownloadProgress(0, 0);
  file.flush();
  return (receivedFileSize > 0) ? receivedFileSize : -1;
}
#endif

static void oledInit()
{
    /* Set SD chip select to OUTPUT */
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    display.begin();
  #ifdef DEBUG
   oledDisplayTestPattern();
    Serial.print(F("Initializing SD card..."));
  #else
    oledDisplayClear();
  #endif

    if (!SD.begin(SD_CS)) {
    #ifdef DEBUG
      Serial.println(F("failed!"));
    #endif
      return;
    }
  #ifdef DEBUG
    debugStartupMovie();
  #endif
}

#ifdef DEBUG
void oledDisplayTestPattern(void)
{
  uint32_t x,y;
  
  for(y=0;y<64;y++)
  {
    display.setAddrWindow(0, y, 96, 1);
    uint16_t color = BLACK;
    if(y>55) color = WHITE;
    else if(y>47) color = BLUE;
    else if(y>39) color = GREEN;
    else if(y>31) color = CYAN;
    else if(y>23) color = RED;
    else if(y>15) color = MAGENTA;
    else if(y>7) color = YELLOW;
    display.writeColor(color, 96);
    display.endWrite();
  }
}
#endif
#endif

//////////////////////////////////////////////////////////////////////
///*****  Sequence/Function Varaiables, Containers & Counters *****///
//////////////////////////////////////////////////////////////////////

// General counters and timers shared across more than one animation
unsigned long tCounter[HPCOUNT] = {0, 0, 0};
unsigned int  interval[HPCOUNT] = {100, 100, 100};


// Short Circuit Animation
#define SCMAXLOOPS 20
byte         SCloop[HPCOUNT] = {0, 0, 0};   // Loop counter for short circuit animation.
boolean      SCflag[HPCOUNT] = {false, false, false};
unsigned int SCinterval[HPCOUNT] = {10, 10, 10};

// Cycle Animation
byte Frame[HPCOUNT] = {0, 0, 0};

// Wag Animation
int WagCount[HPCOUNT] = {-1,-1,-1};  // Use of negative value to determine first loop 
unsigned long WagTimer[HPCOUNT] = {0,0,0};

int RCinputCh1;

unsigned long twitchLEDTime[HPCOUNT] = {3800, 3900, 4000}; //LEDS Start up ~4 seconds after boot  Use Unique values for each
unsigned long twitchHPTime[HPCOUNT]  = {4000, 4000, 4000}; //HPs Start 4 seconds after boot
unsigned long twitchLEDRunTime[HPCOUNT];

unsigned long LEDHaltTime[HPCOUNT];
unsigned long HPHaltTime[HPCOUNT];   

const byte startEnableTwitchLED[HPCOUNT] = {enableTwitchLED[0],enableTwitchLED[1],enableTwitchLED[2]};

unsigned long ledtimer = 0;      

// Output enabled timers and flag
unsigned long OETime = 0;  // The time at which output was enabled
boolean       OEFlag = false;

//////////////////////////////////////////////////////////////////////
///*****        Command Varaiables, Containers & Flags        *****///
//////////////////////////////////////////////////////////////////////

#define INPUTBUFFERLEN 10
#define MAXCOMMANDLENGTH 12
char inputBuffer[INPUTBUFFERLEN];  
String inputString = "";                       // a string to hold incoming data
volatile boolean stringComplete = false;       // whether the serial string is complete

int halt = -1;
int pipeIndex = -1;

byte typeState;         // Hold variable that identifies if it is a LED, HP, or Sequence command
byte functionState;      // Holds variable that identifies the function within the type above
int optionState = -1;  // Holds variable that identifies the option value for those commands that use it. (color, position, etc)
int optionState2 = -1;

typedef struct LEDCmdStruct 
{
   byte LEDFunction = -1;
   byte LEDOption1 = -1;
   byte LEDOption2 = -1;
   int LEDHalt = -1;
} LEDCmd;

LEDCmd LED_command[HPCOUNT];

typedef struct HPCmdStruct 
{
   byte HPFunction = -1;
   byte HPOption1 = -1;
   int  HPHalt = -1;
} HPCmd;

HPCmd HP_command[HPCOUNT];

boolean offcoloroverride[HPCOUNT] = {false,false,false};


//////////////////////////////////////////////////////////////////////
///*****                  Color Values & Labels               *****///
//////////////////////////////////////////////////////////////////////
   
#define C_OFF      0x000000
#define C_RED      0xFF0000
#define C_ORANGE   0xFF8000
#define C_YELLOW   0xFFFF00
#define C_GREEN    0x00FF00
#define C_CYAN     0x00FFFF
#define C_BLUE     0x0000FF
#define C_MAGENTA  0xFF00FF
#define C_PURPLE   0x800080
#define C_WHITE    0xFFFFFF

//
// The colors in the table are repeated several time to give them a
// frequency of being shown. Same is true for the OFF state.
// This is done to give the HPLeds a more realistic light show.
// 
const uint32_t basicColors[10][10] = {{     C_RED,  C_YELLOW,   C_GREEN,   C_CYAN,   C_BLUE, C_MAGENTA, C_PURPLE, C_WHITE, C_OFF, C_OFF},    // Random
                                      {     C_RED,     C_RED,     C_RED,  C_WHITE, 0xFFA0A0,  0xFD5555, 0xFFD3D3,   C_OFF, C_OFF, C_OFF},    // Red
                                      {  C_YELLOW,  C_YELLOW,  C_YELLOW,  C_WHITE, 0xFDFD43,  0xFFFF82, 0xFFFFBA,   C_OFF, C_OFF, C_OFF},    // Yellow
                                      {   C_GREEN,   C_GREEN,   C_GREEN,  C_WHITE, 0x57FC57,  0x80FC80, 0xBDFFB1,   C_OFF, C_OFF, C_OFF},    // Green
                                      {    C_CYAN,    C_CYAN,    C_CYAN,  C_WHITE, 0x38FFFF,  0x71FDFD, 0xA4FDFD,   C_OFF, C_OFF, C_OFF},    // Cyan
                                      {    C_BLUE,    C_BLUE,    C_BLUE,  C_WHITE, 0xACACFF,  0x7676FF, 0x5A5AFF,   C_OFF, C_OFF, C_OFF},    // Blue
                                      { C_MAGENTA, C_MAGENTA, C_MAGENTA,  C_WHITE, 0xFB3BFB,  0xFD75FD, 0xFD9EFD,   C_OFF, C_OFF, C_OFF},    // Megenta
                                      {  C_ORANGE,  C_ORANGE,  C_ORANGE,  C_WHITE, 0xFB9B3A,  0xFFBE7D, 0xFCD2A7,   C_OFF, C_OFF, C_OFF},    // Orange
                                      {  C_PURPLE,  C_PURPLE,  C_PURPLE,  C_WHITE, 0xA131A1,  0x9B449B, 0xBD5FBD,   C_OFF, C_OFF, C_OFF},    // Purple
                                      {   C_WHITE,   C_WHITE,   C_WHITE,  C_WHITE, 0xB7B6B6,  0x858484, 0xA09F9F,   C_OFF, C_OFF, C_OFF}};   // White



//////////////////////////////////////////////////////////////////////
///*****               Initialize NeoPixel Strips             *****///
///*****               code suggested by LostRebel            *****///
//////////////////////////////////////////////////////////////////////
#ifdef OLED
const byte neoJewelLEDS[HPCOUNT] = { 12, 7, 7 };
#define NEO_JEWEL_LEDS(i) neoJewelLEDS[i]
#else
#define NEO_JEWEL_LEDS(i) 7
#endif

#ifdef NEO_JEWEL_RGBW
 #define HP_NEO_TYPE (NEO_GRBW + NEO_KHZ800)
#else
 #define HP_NEO_TYPE (NEO_GRB + NEO_KHZ800)
#endif

Adafruit_NeoPixel neoStrips[HPCOUNT];


//////////////////////////////////////////////////////////////////////
///*****                 Initialize Servo Board               *****///
//////////////////////////////////////////////////////////////////////
Servos servos(SERVOI2CADDRESS);

//#define STATUSLED
#if STATUSLED
#define STATUSLEDMSECS 200                 // Amount of time to flash the led pin
#define STATUSLEDPIN 13                    // The builtin LED on the Arduino mini is on pin 13
#endif


////////////////////////////////////////////////////////////
/// Setup and initialization                             ///
////////////////////////////////////////////////////////////

void setup() { 
#ifdef OLED
  Serial.begin(19200);                     // Starts Serial with a baudrate of 19200
#else
  Serial.begin(9600);                      // Starts Serial with a baudrate of 9600
#endif
  inputString.reserve(10);                 // reserve 10 bytes for the inputString

  Wire.begin(I2CADDRESS);                  // Connects to I2C Bus and establishes address.
  Wire.onReceive(i2cEvent);                // Register event so when we receive something we jump to i2cEvent();

  randomSeed(analogRead(3));               // Seeds psuedo-random number generator to make random more randomy.

#ifdef OLED
  oledInit();
#endif
  Serial.print(F("\nFlthyHPs, Sketch Version 1.")); 
  Serial.println(VERSION);
  #ifdef DEBUG
    Serial.println(F("Debugging Enabled")); 
  #else
    Serial.println(F("Debugging Disabled"));
  #endif
  #ifdef ENABLEBASICHP
    Serial.print(F("Basic")); 
  #else
    Serial.print(F("Pre-Set"));
  #endif
  Serial.println(F(" HP Positioning Enabled"));
  #ifndef ENABLEBASICHP
    #ifdef ENABLERC
      Serial.println(F("RC Control Enabled")); 
    #else
      Serial.println(F("RC Control Disabled"));
    #endif
  #endif
  #ifdef DEBUG
  Serial.println(F("Entering Setup..."));
  #endif

  #ifdef STATUSLED
  //////////////////////////////////////////////////////
  ///                Status LED Setup                ///
  //////////////////////////////////////////////////////
  pinMode(STATUSLEDPIN, OUTPUT);
  #endif
  
  //////////////////////////////////////////////////////
  ///                  RC Input Setup                ///
  //////////////////////////////////////////////////////
  #ifdef ENABLERC
    pinMode(RCPIN, INPUT);
  #endif
  
  //////////////////////////////////////////////////////
  ///                     OE Setup                   ///
  //////////////////////////////////////////////////////
  #ifdef OUTPUT_ENABLED_PIN
  if(OUTPUT_ENABLED_ON) {pinMode(OUTPUT_ENABLED_PIN, OUTPUT);}
  #endif

  //////////////////////////////////////////////////////
  ///                 HP Servo Setup                 ///
  //////////////////////////////////////////////////////
  for (int i=0; i<HPCOUNT; i++)
    positionHP(i,1,300);             // Centers Each HP

  //////////////////////////////////////////////////////
  ///                  HP LED Setup                  ///
  //////////////////////////////////////////////////////    
  for (int i=0; i<HPCOUNT; i++)
  {
    neoStrips[i].updateType(HP_NEO_TYPE);
    neoStrips[i].updateLength(NEO_JEWEL_LEDS(i));
    neoStrips[i].setPin(LEDpins[i]);
    neoStrips[i].begin(); 
    neoStrips[i].setBrightness(BRIGHT);
    neoStrips[i].show();
    twitchLEDRunTime[i] = (1000L*random(LEDTwitchRunInterval[i][0],LEDTwitchRunInterval[i][1]));  // Randomly sets initial LED Twitch Run Time value
  }
  #ifdef DEBUG
  Serial.println(F("Setup Complete!\n"));
  Serial.println(F("Starting Main Loop..."));
  Serial.println(F("Command Strings can now be entered in serial monitor...\n"));
  #endif
  #ifdef OLED
  Serial.println(F("READY"));
  #endif
}

void restoreFrontHoloBrigthness()
{
  // Restore front holo brightness
  neoStrips[0].setBrightness(BRIGHT);
}

void dimFrontHoloBrigthness()
{
  // Restore front holo brightness
  neoStrips[0].setBrightness(BRIGHT/4);
}

void debugStartupMovie()
{
  stringComplete = true;
  switch (random(3))
  {
    case 0:
      inputString = "S1|40";
      break;
    case 1:
      inputString = "S2";
      break;
    case 2:
      inputString = "S3";
      break;
  }
}

//////////////////////////////////////////////////////////////////////
///                        Main Loop                               ///
//////////////////////////////////////////////////////////////////////

void loop(){
  int commandLength;
#ifdef OLED
  bd2Idle();
#endif
  Servos::move(millis());  // this performs the actual moves

  statusLEDCheck();        // see if the status led should be turned off
 
  if(stringComplete) {
    inputString.trim();    // Trim any white space off off inputString, including \r and \n as they are unecessary now.

    #ifdef DEBUG
    Serial.print(F("Received Command: "));
    Serial.println(inputString);
    #endif
    
    pipeIndex = inputString.indexOf('|');
    halt = -1;
    if(pipeIndex >= 1) { 
        halt = inputString.substring(pipeIndex+1).toInt();
        inputString = inputString.substring(0, pipeIndex);
        pipeIndex = -1;
    } 

    commandLength = inputString.length();
    inputString.toCharArray(inputBuffer, INPUTBUFFERLEN);
    inputString="";       // If a complete Command string has been flagged, write the string to array to process.
    if( inputBuffer[0]=='F' ||      // Front HP
        inputBuffer[0]=='R' ||      // Rear HP
        inputBuffer[0]=='T' ||      // Top HP
        inputBuffer[0]=='X' ||      // Front & Rear HPs
        inputBuffer[0]=='Y' ||      // Front & Top HPs
        inputBuffer[0]=='Z' ||      // Rear & Top HPs
        inputBuffer[0]=='A' )       // All three HP
    {  
      if(commandLength >= 2)
        typeState = inputBuffer[1]-'0';
      if(commandLength >= 4)
        functionState = (inputBuffer[2]-'0')*10+(inputBuffer[3]-'0');

      if(commandLength >= 5)
      {
        optionState = inputBuffer[4]-'0';
      }
      else if(typeState == 1)
        optionState = 1;         // Set Center as the fallback position
      
      if(commandLength >= 6) {optionState2 = inputBuffer[5]-'0';}

      if(inputBuffer[0]=='F' || inputBuffer[0]=='X' || inputBuffer[0]=='Y' || inputBuffer[0]=='A')
        processCommands(0,typeState);
      if(inputBuffer[0]=='R' || inputBuffer[0]=='X' || inputBuffer[0]=='Z' || inputBuffer[0]=='A')
        processCommands(1,typeState);
      if(inputBuffer[0]=='T' || inputBuffer[0]=='Y' || inputBuffer[0]=='Z' || inputBuffer[0]=='A')
        processCommands(2,typeState);
    }
    #ifdef OLED
    else if (inputBuffer[0]=='O')                      // OLED Sequences
    {
      char filename[12];
      char* fp = filename;
      const char* ch = (commandLength >= 3) ? &inputBuffer[2] : "";
      while (*ch != '\0')
      {
        unsigned char c = (unsigned char)*ch++;
        *fp++ = (char)((c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c);
      }
      *fp = '\0';
      if (filename != fp)
      {
        *fp++ = '.'; *fp++ = 'B'; *fp++ = 'D'; *fp++ = '2'; *fp = '\0';
      #ifdef OLED_DOWNLOAD
        if(inputBuffer[1] == 'D')
        {
          uint32_t len = 0;
          bd2Download = true;
          Serial.println("ACK");
          if (Serial.find("LEN"))
          {
            len = Serial.readStringUntil('\n').toInt();
          }
          uint32_t bytesReceived = 0;
          /* Remove old file */
          unsigned char* buffer = (unsigned char*)malloc(4098);
          if (buffer != NULL)
          {
            if (SD.exists(filename))
              SD.remove(filename);
            File file = SD.open(filename, FILE_WRITE);
            bytesReceived = bd2DownloadFile(file, buffer, 4098, len);
            file.close();
            if (bytesReceived <= 0)
              SD.remove(filename);
            free((char*)buffer);
          }
          bd2Download = false;
          if (bytesReceived > 0)
          {
            Serial.print("Received: ");
            Serial.print(filename);
            Serial.print(" ");
            Serial.print(bytesReceived);
            Serial.println(" bytes");
          }
        }
      #endif
        else if(inputBuffer[1] == 'X')
        {
          SD.remove(filename);
        }
        else if(inputBuffer[1] == 'P')
        {
          bd2PlayMovie(filename);
        }
      }
    }
    #endif
    else if (inputBuffer[0]=='S')                      // Major Sequences (Both LEDS and HP Servos)
    {
      if(commandLength >= 2)
      {
        functionState = (inputBuffer[1]-'0');

        switch (functionState)
        { 
               case 1: for (int i=0; i<HPCOUNT; i++) {
                          varResets(i);                                                 // Resets the Variables for LEDs on all 3 HPs
                          flushCommandArray(i, 0);                                      // Flushes LED Command Arrays
                        }                                              
                       positionHP(0,0,SERVO_SPEED[0]);                                  // Moves Front HP to Down Position 
                       HP_command[0].HPFunction = 0;                                    // Then Set Front HP to Do Nothing                                      
                       LED_command[1].LEDFunction = 0;                                  // Set Rear HP LED to Do Nothing
                       HP_command[1].HPFunction = 0;                                    // Set Rear HP to Do Nothing
                       LED_command[2].LEDFunction = 0;                                  // Set Top HP LED to Do Nothing
                       HP_command[2].HPFunction = 0;                                    // Set Top HP to Do Nothing
                       LED_command[0].LEDFunction = 1;                                  // Set Leia LED Sequence to run each loop.
                       #ifdef OLED
                       bd2PlayMovie("Leia.bd2");
                       #endif
                       if(halt >= 1) {
                                  for (int i=0; i<HPCOUNT; i++) {     
                                      LED_command[i].LEDHalt = halt;                    // Set LED Halt timeout if passed via command.
                                      LEDHaltTime[i] = millis();                        // and note start time
                                      HP_command[i].HPHalt = halt;                      // Set HP Halt timeout if passed via command.
                                      HPHaltTime[i] = millis();                         // and note start time
                                   }
                              }                     
                       break;
               #ifdef OLED
               case 2: bd2PlayMovie("R2.bd2");
                       break;
               case 3: bd2PlayMovie("Plans.bd2");
               #endif

               case 4: for (int i=0; i<HPCOUNT; i++) {
                                enableTwitchHP[i]=false;                                 // Disables Auto HP Twith on all HPs
                                offcoloroverride[i] = true;                              // Overrides any set "Off Color" so LEDs are truely Off
                                flushCommandArray(i, 0);                                 // Flushes LED Command Array for all HPs
                                flushCommandArray(i, 1);                                 // Flushes HP Command Array for all HPs
                                ledOFF(i);                                               // Turns Off LEDs on all HPs
                                enableTwitchLED[i]=0;                                    // Disables Auto LED Twith on all HPs
                       }
                       break;
               case 5: for (int i=0; i<HPCOUNT; i++) {
                                enableTwitchHP[i]=true;                                  // Enables Auto HP Twith on all HPs
                                offcoloroverride[i] = true;                              // Overrides any set "Off Color" so LEDs are truely Off
                                flushCommandArray(i, 0);                                 // Flushes LED Command Array for all HPs
                                flushCommandArray(i, 1);                                 // Flushes HP Command Array for all HPs
                                ledOFF(i);                                               // Turns Off LEDs on all HPs
                                enableTwitchLED[i]=1;                                    // Enables Auto LED Twith on all HPs using assigned default sequences
                       }
                       break;
               case 6: for (int i=0; i<HPCOUNT; i++) {
                                enableTwitchHP[i]=true;                                  // Enables Auto HP Twith on all HPs
                                offcoloroverride[i] = true;                              // Overrides any set "Off Color" so LEDs are truely Off
                                flushCommandArray(i, 0);                                 // Flushes LED Command Array for all HPs
                                flushCommandArray(i, 1);                                 // Flushes HP Command Array for all HPs
                                ledOFF(i);                                               // Turns Off LEDs on all HPs
                                enableTwitchLED[i]=2;                                    // Enables Auto LED Twith on all HPs using random sequences
                       }
                       break;
               case 7: for (int i=0; i<HPCOUNT; i++) {
                                enableTwitchHP[i]=false;                                 // Disables Auto HP Twith on all HPs 
                                offcoloroverride[i] = false;                             // Removes Any Override to "Off Color" if set
                                flushCommandArray(i, 0);                                 // Flushes LED Command Array for all HPs
                                flushCommandArray(i, 1);                                 // Flushes HP Command Array for all HPs
                                ledOFF(i);                                               // Turns Off LEDs on all HPs
                                enableTwitchLED[i]=0;                                    // Disables Auto LED Twith on all HPs
                       }
                       break;
               case 8: for (int i=0; i<HPCOUNT; i++) {
                                enableTwitchHP[i]=true;                                  // Enables Auto HP Twith on all HPs 
                                offcoloroverride[i] = false;                              // Removes Any Override to "Off Color" if set
                                flushCommandArray(i, 0);                                 // Flushes LED Command Array for all HPs
                                flushCommandArray(i, 1);                                 // Flushes HP Command Array for all HPs
                                ledOFF(i);                                               // Turns Off LEDs on all HPs
                                enableTwitchLED[i]=1;                                    // Enables Auto LED Twith on all HPs using assigned default sequences
                       }
                       break;
                case 9: for (int i=0; i<HPCOUNT; i++) {
                                enableTwitchHP[i]=true;                                  // Enables Auto HP Twith on all HPs 
                                offcoloroverride[i] = false;                              // Removes Any Override to "Off Color" if set 
                                flushCommandArray(i, 0);                                 // Flushes LED Command Array for all HPs
                                flushCommandArray(i, 1);                                 // Flushes HP Command Array for all HPs
                                ledOFF(i);                                               // Turns Off LEDs on all HPs
                                enableTwitchLED[i]=2;                                    // Enables Auto LED Twith on all HPs using random sequences
                       }
                       break;
                default: break;
        }
      }
    }

    ///***  Clear States and Reset for next command.  ***///
    String inputString = "";
    stringComplete = false;
    char inputBuffer[INPUTBUFFERLEN]; 
    optionState = -1;  
    optionState2 = -1; 
    int halt = -1;      
  }

  for(int i=0; i<HPCOUNT; i++) {
    
    if(LED_command[i].LEDHalt) {
       if(millis()-LEDHaltTime[i]>=(LED_command[i].LEDHalt*1000L)) {
          flushCommandArray(i, 0);
          ledOFF(i);                                 // Clears Any Remaining Lit LEDs
          resetLEDtwitch(i); 
       }
    }
    switch (LED_command[i].LEDFunction) {
      case 0: break;       // Do Nothing
      case 1: colorProjectorLED(i,5); break;       // Leia Sequence (Blue)
      case 2: colorProjectorLED(i,LED_command[i].LEDOption1); break; 
      case 3: dimPulse(i, LED_command[i].LEDOption1, LED_command[i].LEDOption2); break;
      case 4: cycle(i,LED_command[i].LEDOption1); break;    
      case 5: ledColor(i,LED_command[i].LEDOption1); break;
      case 6: rainbow(i); break;
      case 7: ShortCircuit(i,LED_command[i].LEDOption1); break; 
      case 96: enableTwitchLED[i]=0; resetLEDtwitch(i); offcoloroverride[i] = true; flushCommandArray(i,0); break;   // Clear Function, Disable Random LED Twitch, enable off color override, and random sequences (if enabled)
      case 97: if(LED_command[i].LEDOption1 == 1 || LED_command[i].LEDOption1 ==2) { enableTwitchLED[i]=LED_command[i].LEDOption1; }
               else { enableTwitchLED[i]=startEnableTwitchLED[i]; }
               resetLEDtwitch(i); offcoloroverride[i] = true; flushCommandArray(i,0); break;   // Clear Function, Enable Random LED Twitch using random LED sequences, enable off color override. 
      case 98: enableTwitchLED[i]=0; resetLEDtwitch(i); offcoloroverride[i] = false; flushCommandArray(i,0); break;   // Clear Function, Disable Random LED Twitch, random sequences (if enabled) and disable off color override,
      case 99: if(LED_command[i].LEDOption1 == 1 || LED_command[i].LEDOption1 ==2) { enableTwitchLED[i]=LED_command[i].LEDOption1; }
               else { enableTwitchLED[i]=startEnableTwitchLED[i]; }
               resetLEDtwitch(i); offcoloroverride[i] = false; flushCommandArray(i,0); break;   // Clear Function, Enable Random LED Twitch, using random LED sequences, disable off color override,
      case 100: ledColor(i,LED_command[i].LEDOption1); break;
      default: break;
     }


    if(HP_command[i].HPHalt) {
       if(millis()-HPHaltTime[i]>=(HP_command[i].HPHalt*1000L)) {
          flushCommandArray(i, 1);
          WagCount[i] = -1; 
       }
     }
     switch (HP_command[i].HPFunction) {
      case 0: break; // Do Nothing
      case 1: positionHP(i,HP_command[i].HPOption1,SERVO_SPEED[0]); flushCommandArray(i,1); break;  
      case 2: RCHP(i,1); break;
      case 3: RCHP(i,2); break;
      case 4: twitchHP(i,0); flushCommandArray(i,1); break;
      case 5: wagHP(i,1); break;  // Wags HP Left/Right   
      case 6: wagHP(i,2); break;  // Wags HP Up/Down  
      case 98: enableTwitchHP[i]=false; flushCommandArray(i,1); break;   // Clear Function, Disable Servo Random Twitch
      case 99: enableTwitchHP[i]=true; flushCommandArray(i,1); break;   // Clear Function, Enable Servo Random Twitch
      default: break;
     } 
  }     
  
  for (int i=0; i<HPCOUNT; i++)
    if (millis() > twitchLEDTime[i] && enableTwitchLED[i]>=1 && LED_command[i].LEDFunction>99) {
      twitchLEDTime[i] = (1000L*random(LEDTwitchInterval[i][0],LEDTwitchInterval[i][1]))+millis();
      twitchLEDRunTime[i] = random(LEDTwitchRunInterval[i][0],LEDTwitchRunInterval[i][1]);
      flushCommandArray(i, 0);
      LEDHaltTime[i] = millis();
      varResets(i);
      if(enableTwitchLED[i]==2) {      
          LED_command[i].LEDFunction = random(2,7);
          LED_command[i].LEDOption1 = random(1,9);
          LED_command[i].LEDOption2 = random(1,9);
          LED_command[i].LEDHalt = twitchLEDRunTime[i];         
        }
      else {
         LED_command[i].LEDFunction = defaultLEDTwitchCommand[i][0];
         LED_command[i].LEDOption1 = defaultLEDTwitchCommand[i][1];
         LED_command[i].LEDOption2 = defaultLEDTwitchCommand[i][2];
         LED_command[i].LEDHalt = twitchLEDRunTime[i];
      }
      #ifdef DEBUG
        Serial.print(F("Received Random Command: "));
        if(i==0) {Serial.print(F("F00"));}
        else if(i==1) {Serial.print(F("R00"));}
        else if(i==2) {Serial.print(F("T00"));}
        Serial.print(LED_command[i].LEDFunction);
        Serial.print(LED_command[i].LEDOption1);
        Serial.print(LED_command[i].LEDOption2);
        Serial.print(F("|"));
        Serial.println(LED_command[i].LEDHalt);
      #endif    
    }


  for (int i=0; i<HPCOUNT; i++) {
    if (millis() > twitchHPTime[i] && enableTwitchHP[i] && HP_command[i].HPFunction>99) {
      twitchHP(i,1);
      resetHPtwitch(i);
    } 
  }

  // If OutputEnabled and the time since last move is greater that the speed allowance,
  // then turn off the servos to keep them from humming.
  #ifdef OUTPUT_ENABLED_PIN
  if(OEFlag && (millis() - OETime) >= SERVO_SPEED[1]) {            
              digitalWrite(OUTPUT_ENABLED_PIN, HIGH);       
              OEFlag = false;                               
  }
  #endif
     
} // Close of Main Loop


////////////////////////////////////////////////////////////
// This needs an explanation                             /// 
////////////////////////////////////////////////////////////
int mapPulselength(double microseconds) {
  // The Adafruit PWM Driver library overshoots the frequency by a factor of 1/0.9 
  // therefore we must multiply the frequency value of 60 by 0.9 to correct for this
  // resulting in a more actuate map
  double pulselength = 4.521;  // (1000000/(60.00*0.9))/4096
  int pulse;
  pulse = microseconds/pulselength;
  return pulse;
}


//////////////////////////////////////////////////////////////////////////////////////////
///*****                       OE Servo Enable Function                           *****///
//////////////////////////////////////////////////////////////////////////////////////////

void enableServos() {
  #ifdef OUTPUT_ENABLED_PIN
  if(OUTPUT_ENABLED_ON) {                        // If OE control of the Servo Break Out Board is enabled
    digitalWrite(OUTPUT_ENABLED_PIN, LOW);       // Set OE Pin to low to enable Servo Movement 
    OEFlag = true;                               // Set OE Flag so loop knows servos are enabled
    OETime = millis();                           // Set OE Time to current millis so we can eventually run disableServos after movement is complete 
  }
  #endif
}



//////////////////////////////////////////////////////////////////////////////////////////
///*****                     HP Twitch/Movement Functions                         *****///
//////////////////////////////////////////////////////////////////////////////////////////

void resetHPtwitch(byte hp) {
  twitchHPTime[hp] = (1000*random(HPTwitchInterval[hp][0],HPTwitchInterval[hp][1])) + millis();  
}

void positionHP(byte hp, byte pos, int speed) {
  int pos1;
  int pos2;
  #ifdef ENABLEBASICHP
  if(pos == 1) {
    pos1 = mapPulselength(pgm_read_word(&(HPposBasic[0][0])));
    pos2 = mapPulselength(pgm_read_word(&(HPposBasic[1][0])));
    enableServos();
    servos.moveTo(HPpins[hp][0],speed,pos1);
    servos.moveTo(HPpins[hp][1],speed,pos2);
    #ifdef DEBUG
      if(hp==0) {Serial.print(F("Front"));}
      else if(hp==1) {Serial.print(F("Rear"));}
      else if(hp==2) {Serial.print(F("Top"));}
      Serial.println(F(" HP moved to the Center position."));
    #endif
  }
  else {
    #ifdef DEBUG
      Serial.print(F("Only "));
      if(hp==0) {Serial.print(F("Front"));}
      else if(hp==1) {Serial.print(F("Rear"));}
      else if(hp==2) {Serial.print(F("Top"));}
      Serial.println(F(" HP centering is possible when Basic Positioning is enabled"));
    #endif
  }
  #endif
  
  #ifndef ENABLEBASICHP
    pos1 = mapPulselength(pgm_read_word(&(HPpos[hp][pos][0])));
    pos2 = mapPulselength(pgm_read_word(&(HPpos[hp][pos][1])));
    enableServos();
    servos.moveTo(HPpins[hp][0],speed,pos1);
    servos.moveTo(HPpins[hp][1],speed,pos2);
    #ifdef DEBUG
      if(hp==0) {Serial.print(F("Front"));}
      else if(hp==1) {Serial.print(F("Rear"));}
      else if(hp==2) {Serial.print(F("Top"));}
      Serial.print(F(" HP moved to the "));
      switch(pos) {
        case 0: Serial.print(F("Down"));break; 
        case 1: Serial.print(F("Center"));break; 
        case 2: Serial.print(F("Up"));break; 
        case 3: Serial.print(F("Left"));break; 
        case 4: Serial.print(F("Upper Left"));break; 
        case 5: Serial.print(F("Lower Left"));break; 
        case 6: Serial.print(F("Right"));break; 
        case 7: Serial.print(F("Upper Right"));break; 
        case 8: Serial.print(F("Lower Right"));break;
        default:break;
      }
      Serial.println(F(" position."));
   #endif  
  #endif
}

void twitchHP(byte hp, byte randtwitch) {
  int speed = random(SERVO_SPEED[0],SERVO_SPEED[1]);
  #ifdef DEBUG
    if(randtwitch==1) {    
      Serial.print(F("Random "));
      if(hp==0) {Serial.print(F("Front"));}
      else if(hp==1) {Serial.print(F("Rear"));}
      else if(hp==2) {Serial.print(F("Top"));}
      Serial.println(F(" HP twitch triggered...."));    
    }
  #endif
  enableServos();   
  #ifdef ENABLEBASICHP
    servos.moveTo(HPpins[hp][0],speed,mapPulselength(random(pgm_read_word(&(HPposBasic[0][1])),pgm_read_word(&(HPposBasic[0][2])))));
    servos.moveTo(HPpins[hp][1],speed,mapPulselength(random(pgm_read_word(&(HPposBasic[1][1])),pgm_read_word(&(HPposBasic[1][2])))));
    #ifdef DEBUG
      if(hp==0) {Serial.print(F("Front"));}
      else if(hp==1) {Serial.print(F("Rear"));}
      else if(hp==2) {Serial.print(F("Top"));}
      Serial.println(F(" HP moved to random basic position."));
    #endif
  #else
    positionHP(hp,random(0,9),speed);
  #endif
}

void RCHP(byte hp, byte type) {     
  #ifdef ENABLERC
  unsigned int servo1;
  unsigned int servo2;
    #ifndef ENABLEBASICHP
      RCinputCh1 = pulseIn(RCPIN, HIGH); // each channel
      //Serial.println(RCinputCh1);
      if(RCinputCh1>0) {
        enableServos();
        if(type==1) {                                                                                // RC Controls Left-Right Movement
          servo1 = map(RCinputCh1, RCRange[0], RCRange[1], pgm_read_word(&(HPpos[hp][3][0])), pgm_read_word(&(HPpos[hp][6][0])));
          servo2 = map(RCinputCh1, RCRange[0], RCRange[1], pgm_read_word(&(HPpos[hp][3][1])), pgm_read_word(&(HPpos[hp][6][1])));
        }
        else if(type==2) {                                                                           // RC Controls Up-Down Movement
          servo1 = map(RCinputCh1, RCRange[0], RCRange[1], pgm_read_word(&(HPpos[hp][0][0])), pgm_read_word(&(HPpos[hp][2][0])));
          servo2 = map(RCinputCh1, RCRange[0], RCRange[1], pgm_read_word(&(HPpos[hp][0][1])), pgm_read_word(&(HPpos[hp][2][1])));
        }
        servos.moveTo(HPpins[hp][0],0,mapPulselength(servo1));servos.moveTo(HPpins[hp][1],0,mapPulselength(servo2));
      }
      else { Serial.println(F("No RC Input Detected!")); }
    #else
      #ifdef DEBUG
        if(hp==0) {Serial.print(F("Front HP: "));}
        else if(hp==1) {Serial.print(F("Rear HP: "));}
        else if(hp==2) {Serial.print(F("Top HP: "));}
        Serial.println(F("RC mode is not possible when Basic Positioning is enabled."));
      #endif
    #endif
  #else
    #ifdef DEBUG
        if(hp==0) {Serial.print(F("Front HP: "));}
        else if(hp==1) {Serial.print(F("Rear HP: "));}
        else if(hp==2) {Serial.print(F("Top HP: "));}
        Serial.println(F("RC Mode is currently disabled."));
        flushCommandArray(hp,1);
    #endif
  #endif      
}

void wagHP(byte hp, byte type){
  #ifndef ENABLEBASICHP
   int speed = 250;
    if(WagCount[hp] < 0) {WagCount[hp] = 0;WagTimer[hp]=millis();}                  
    if(millis()-WagTimer[hp]>=400) {
      WagCount[hp]++;
      WagTimer[hp]=millis();          
      if (WagCount[hp] % 2) {
        if(type==1) {positionHP(hp, 3, speed);}    //  On Odd Count, Wag HP Left
        if(type==2) {positionHP(hp, 0, speed);}    //  On Odd Count, Wag HP Down
      }
      else {
        if(type==1) {positionHP(hp, 6, speed);}    //  On Even Count, Wag HP Right  
        if(type==2) {positionHP(hp, 2, speed);}    //  On Even Count, Wag HP Up 
      } 
    }
  #else
      #ifdef DEBUG
        if(hp==0) {Serial.print(F("Front HP: "));}
        else if(hp==1) {Serial.print(F("Rear HP: "));}
        else if(hp==2) {Serial.print(F("Top HP: "));}
        Serial.println(F("Wag mode is not possible when Basic Positioning is enabled."));
        flushCommandArray(hp,1);
      #endif
  #endif
}
    
//////////////////////////////////////////////////////////////////////////////////////////
///*****                            HP LED Functions                              *****///
//////////////////////////////////////////////////////////////////////////////////////////

void ledOFF(byte hp) {
  for(int i=0; i<NEO_JEWEL_LEDS(hp); i++) {neoStrips[hp].setPixelColor(i,C_OFF);} 
  neoStrips[hp].show();
}

      
void ledColor(byte hp, int c) {
  for(int i=0; i<NEO_JEWEL_LEDS(hp); i++) {neoStrips[hp].setPixelColor(i,basicColors[c][0]);} 
  neoStrips[hp].show();
}

void resetLEDtwitch(int hp) {
  ledOFF(hp);
  twitchLEDTime[hp] = (1000L*random(LEDTwitchInterval[hp][0],LEDTwitchInterval[hp][1]))+millis(); 
}

//////////////////////////////////////////////////////////
///*****        Color Projector Function          *****///
//////////////////////////////////////////////////////////
void colorProjectorLED(byte hp, int c) {
  if ((millis() - tCounter[hp]) > interval[hp]) {
    for(int i=0; i<NEO_JEWEL_LEDS(hp); i++) { neoStrips[hp].setPixelColor(i,basicColors[c][random(0,10)]); }
    neoStrips[hp].show();
    tCounter[hp]=millis();
    interval[hp]=random(50,150);
  }
}
        
/////////////////////////////////////////////////////////
///*****           Dim Pulse Function            *****///
/////////////////////////////////////////////////////////
        
void dimPulse(byte hp, int c, int setting) {
  int inter = map(setting, 0, 9, dimPulseSpeedRange[1], dimPulseSpeedRange[0]);
  long elapsed;
  int frames;
  if ((millis() - tCounter[hp]) > interval[hp]) {
    elapsed = millis() - tCounter[hp];
    frames = elapsed/inter;
    if(frames >= 64) {tCounter[hp] = millis();}     
    if (frames > 32) {frames = 32-(frames - 32);}
    if(elapsed>=inter) {
      for(int i=0; i<NEO_JEWEL_LEDS(hp); i++)
        neoStrips[hp].setPixelColor(i, dimColorVal(c,(frames*8)));
      neoStrips[hp].show();
    }
  }
}

void ShortCircuit(byte hp, int c) {
  if(SCloop[hp] <= SCMAXLOOPS) {
    if ((millis() - tCounter[hp]) > SCinterval[hp]) {
      if(SCflag[hp] == false) {
        for(int i=0; i<NEO_JEWEL_LEDS(hp); i++)
          neoStrips[hp].setPixelColor(i,C_OFF);
        SCflag[hp] = true;
        SCinterval[hp] = 10+(SCloop[hp]*random(15,25));
      } 
      else {
        for(int i=0; i<NEO_JEWEL_LEDS(hp); i++)
          neoStrips[hp].setPixelColor(i,basicColors[c][random(0,10)]);
        neoStrips[hp].show();
        SCflag[hp] = false;
        SCloop[hp]++;
      }
      tCounter[hp] = millis();
      neoStrips[hp].show();
    }  
  }   
}

            
/////////////////////////////////////////////////////////
///*****             Cycle Function              *****///
/////////////////////////////////////////////////////////

void cycle(byte hp, int c) { 
  const unsigned int inter = 75;
  if ((millis() - tCounter[hp]) > inter) {
    tCounter[hp]=millis();
    if (NEO_JEWEL_LEDS(hp) == 7)
    {
      /* 7 pixels with one center */
      neoStrips[hp].setPixelColor(0, C_OFF);     // Center LED is always off
      if (Frame[hp] >= NEO_JEWEL_LEDS(hp)) {Frame[hp]=0;}
      for (int i = 1; i<NEO_JEWEL_LEDS(hp); i++) {
        if (i == Frame[hp]) {neoStrips[hp].setPixelColor(i, basicColors[c][0]);}
        else {neoStrips[hp].setPixelColor(i, C_OFF);}                      
      }
    }
    else
    {
      /* 12 pixels no center */
      if (Frame[hp] >= NEO_JEWEL_LEDS(hp)) {Frame[hp]=0;}
      for (int i = 0; i<NEO_JEWEL_LEDS(hp); i++) {
        if (i == Frame[hp]) {neoStrips[hp].setPixelColor(i, basicColors[c][0]);}
        else {neoStrips[hp].setPixelColor(i, C_OFF);}                      
      }
    }
    neoStrips[hp].show(); 
    Frame[hp]++;
  }
}




///////////////////////////////////////////////////////
///*****           Rainbow Functions           *****///
///////////////////////////////////////////////////////
      
void rainbow(byte hp) {
  const unsigned int inter = 10;
  long elapsed;
  unsigned int frames;  
  elapsed = millis() - tCounter[hp];
  frames = elapsed/inter;
  if(frames > 256*5) {
    tCounter[hp]=millis();
  }
  else {        
    for(int i=0; i<NEO_JEWEL_LEDS(hp); i++) {
      neoStrips[hp].setPixelColor(i, Wheel(((i * 256 / NEO_JEWEL_LEDS(hp)) + frames) & 255));
    }
    if(elapsed>=inter) {neoStrips[hp].show();}
  }              
}


/////////////////////////////////////////////////////////
///*****             Wheel Function              *****///
/////////////////////////////////////////////////////////
/// Input a value 0 to 255 to get a color value. The  ///   
/// colours are a transition r - g - b - back to r.   ///
/////////////////////////////////////////////////////////
      
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
} 
      
/////////////////////////////////////////////////////////
///*****          Dim Color Function 1           *****///
/////////////////////////////////////////////////////////
/// Input a value 0 to 255 to get a color value at a  ///   
/// specific brightness.                              ///
/// Takes and int color value and returns an          ///
/// uint32_tcolor value.                              ///
/// Used for soft fade in/outs.                       ///
/////////////////////////////////////////////////////////  
      
uint32_t dimColorVal(int c, int brightness) {
  switch (c) {                                                
  case 1: return Color(255/brightness, 0, 0); break;
  case 2: return Color(255/brightness, 255/brightness, 0);break;
  case 3: return Color(0, 255/brightness, 0); break;
  case 4: return Color(0, 255/brightness, 255/brightness);break;
  case 5: return Color(0, 0, 255/brightness); break;
  case 6: return Color(255/brightness, 0, 255/brightness); break;
  case 7: return Color(255/brightness, 180/brightness, 0); break;
  case 8: return Color(255/brightness, 255/brightness, 255/brightness); break;
  case 9: return C_OFF; break;
  default: return C_OFF; break;                                          
  }   
}  

/////////////////////////////////////////////////////////
///*****    RGB to Hex Color Value Converter      *****///
/////////////////////////////////////////////////////////
///    Converts and returns a color's 3 seperat RGB   /// 
///       value as an uint32_t value.                 ///   
/////////////////////////////////////////////////////////        

uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
} 


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                                    Constructs Command Arrays                                  /////
/////                                                                                               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////// 
void processCommands(byte hp, byte type) {
  flushCommandArray(hp, typeState);          // Flushes Command Array
  if (type == 0) {
    LED_command[hp].LEDFunction = functionState;
    if(optionState >= 0) {
      if(optionState==0)
        LED_command[hp].LEDOption1=random(0,9);
      else
        LED_command[hp].LEDOption1 = optionState;
    }
    else {
      if(functionState==7)
          LED_command[hp].LEDOption1 = shortColor[hp];
        else
          LED_command[hp].LEDOption1 = defaultColor[hp];
    }
    if(optionState2 >= 0 && functionState == 3)
      LED_command[hp].LEDOption2 = optionState2;
    else if (optionState2 < 0 && functionState == 3)
      LED_command[hp].LEDOption2 = DIMPULSESPEED;
    if(halt) { LED_command[hp].LEDHalt = halt; }
    LEDHaltTime[hp] = millis();
    varResets(hp);
  }
  else if (type == 1) {
    HP_command[hp].HPFunction = functionState;
    HP_command[hp].HPOption1 = optionState;
    if(halt) { HP_command[hp].HPHalt = halt; }
    HPHaltTime[hp] = millis();
    WagCount[hp] = -1;
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                   Consolidates Flushing Command Arrays into Single Function                   /////
/////                                                                                               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////// 
void flushCommandArray(byte hp, byte type) {
  if (type==0) {
    int color = -1;
    if (offcolor[hp] >= 0 && !offcoloroverride[hp]) {
       LED_command[hp].LEDFunction = 100; color = offcolor[hp];
    }
    else {
       LED_command[hp].LEDFunction = -1;
    }
    LED_command[hp].LEDOption1 = color;
    LED_command[hp].LEDOption2 = -1;
    LED_command[hp].LEDHalt = -1;
  }
  else if(type==1) {
   HP_command[hp].HPFunction = -1;
   HP_command[hp].HPOption1 = -1;
   HP_command[hp].HPHalt = -1;
   if(enableTwitchHP[hp]) { resetHPtwitch(hp); }  // Resets HP Twitch time so random twich doesn't immediately occur following external position command.  
  } 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                          Consolidates Variable Reset in Single Function                       /////
/////                                                                                               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////            
             
void varResets(byte hp) {
  Frame[hp]=0;
  SCflag[hp] = false;
  SCloop[hp] = 0;
  SCinterval[hp] = 10;
  ledOFF(hp);                                    // Clears Any Remaining Lit LEDs 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                            Onboard LED Communication Status Trigger                           /////
/////                                                                                               /////
///// statusLEDOn - Turn on the LED                                                                 /////
///// statusLEDCheck - Check if enough time has passed and we should turn off the LED               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////// 
void statusLEDOn() {
#ifdef STATUSLED
  digitalWrite(STATUSLEDPIN, HIGH);
  ledtimer = millis();
#endif
}

void statusLEDCheck() {
#ifdef STATUSLED
  if(millis() - ledtimer >= STATUSLEDMSECS)
    digitalWrite(13, LOW);
 #endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                             Serial & I2C Communication Functions                              /////
/////                                                                                               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////  
    
      
/////////////////////////////////////////////////////////
///*****          Serial Event Function          *****///
/////////////////////////////////////////////////////////
/// This routine is run between loop() runs, so using ///
/// delay inside loop can delay response.  Multiple   ///
/// bytes of data may be available.                   ///
/////////////////////////////////////////////////////////
      
void serialEvent() {
#ifdef OLED_DOWNLOAD
  if (bd2Download) return;
#endif
  while (Serial.available()) {                         // Loop through all incoming bytes
    char inChar = (char)Serial.read();                 // Receive each byte as a character        
    if (inChar == '\r' || inChar == '\n') {            // if the incoming character is a carriage return (\r) or newline (\n), it denotes end of command
      stringComplete = true;                           // Once done, set a flag so the main loop can do something about it. 
    } 
    else { 
      inputString += inChar;                           // Add each Character to inputString     
    }
    statusLEDOn();
  }
}
/////////////////////////////////////////////////////////
///*****            I2C Event Function           *****///
/////////////////////////////////////////////////////////
///  This routine is run when an onRecieve event is   ///
///     triggered.  Multiple bytes of data may be     ///
///                    available.                     ///
/////////////////////////////////////////////////////////            
      
void i2cEvent(int howMany)
{  
  inputString = "";                                     // Ensures inputString is empty
  int i=0;
  while(Wire.available())  {                            // loop through all incoming bytes
    char inChar = (char)Wire.read();                    // Receive each byte as a character
    if(i<MAXCOMMANDLENGTH) {
       inputString += inChar;                           // Add each Character to inputString
    }
    i++;
  }
  stringComplete = true;                                // Once done, set a flag so the main loop can do something about it. 
  statusLEDOn();
}

#ifdef OLED_DOWNLOAD
void showDownloadProgress(uint32_t receivedFileSize, uint32_t fileLength)
{
    int hp = 0;
    uint32_t siz = 0;
    int val = (fileLength > 0) ? (int)(255 * NEO_JEWEL_LEDS(hp) * (double)receivedFileSize / (double)fileLength) : 0;
    for (int i = 0; i<NEO_JEWEL_LEDS(hp); i++) {
      if (val >= 255)
      {
        neoStrips[hp].setPixelColor(i, 0, 0, 255);
        val -= 255;
      }
      else if (val > 0) {
        neoStrips[hp].setPixelColor(i, 0, 0, val);
        val = 0;
      }
      else {
        neoStrips[hp].setPixelColor(i, C_OFF);
      }
    }
    neoStrips[hp].show();
}
#endif
