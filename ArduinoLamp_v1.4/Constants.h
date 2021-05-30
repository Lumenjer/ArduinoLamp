// ============================== SETTINGS ==================================
// ---------------- MATRIX --------------
//__________ BRIGHTNESS AND POWER ________
#define BRIGHTNESS 200        // Brightness limit (0-255)
#define CURRENT_LIMIT 2000    // mA (0 - off) 
//_____________________ SIZE _____________
#define WIDTH 16               
#define HEIGHT 8              
//_____________________ COLOR ______________
#define COLOR_ORDER GRB       
//_______________ TYPE AND DIRECTION ______
#define MATRIX_TYPE 1         // 0 - zigzag, 1 - paralel
#define CONNECTION_ANGLE 2    // 0 - bottom left, 1 - top left, 2 - top right, 3 - bottom right
#define STRIP_DIRECTION 3     // 0 - right, 1 - up, 2 - left, 3 - down
// Matrix settings guide: https://alexgyver.ru/matrix_guide/
//___________________ OTHER _____________
#define SEGMENTS 1
//#define XY_MAPPING
// ----------------- CONNECTING AND CONTROL ------------
#define LED_PIN 4
#define USE_BT
#define MOSFET_PIN 5
#define MOSFET_LEVEL HIGH
//___________________ BUTTON _____________
//#define USE_BUTTON
#define BUTTON_PIN 2
#define BUT_PULL HIGH_PULL
//_______________________ IR _____________
//#define USE_IR
#define IR_PIN 3
#define IR_ON 0x1AED14B5         // Remote Code for On / Off
#define IR_NEXT 0xA21606B5       // remote control code for Next Effect
#define IR_PREVIOUS 0xA52ACBB5   // Remote Code for Previous Effect
#define IR_SAVE 0x8D1B67B5       // remote control code to save effect settings
#define IR_DEMO 0xB50F59B5       // remote control code to enable or disable the demo
#define IR_BRIGHT_DOWN 0xB1AE17B5// Brightness control Code
#define IR_BRIGHT_UP 0x78BBC7B5  // Brightness control Code
#define IR_SPEED_DOWN 0xC2F8F2B5 // Speed control code
#define IR_SPEED_UP 0xBFE42DB5   // Speed ​​control code
#define IR_SCALE_DOWN 0x2F4CB4B5 // Scale control code
#define IR_SCALE_UP 0xD7F391B5   // Scale control Code
#define IR_PALETTE 0x21631BB5    // remote control code for Palettes
#define IR_ERASE 0x1E4E56B5      // remote control code to set default settings
//________________________________________

// -------------------- DEMO ----------------
bool isDemo = 0;               // Demo from start
#define DEMOT (1*20)           // min*sec Demo time
#define RANDOM_DEMO            // Random effect in demo
//#define RANDOM_EFF           // Random settings in demo
// ---------------- INDICATOR ---------------
#define INDICATOR
#define numHold_Time (1*1000)
#define ROTARE 1               // 0 - from left to right 1 - from down to up
#define IND_POS 4
//-------------------- EFFECTS --------------
#define MODE_AMOUNT 14
byte Brightness[MODE_AMOUNT]{16,8,9,23,11,9,9,10,27,15,22,21,7};
byte Speed[MODE_AMOUNT]{142,196,217,203,19,207,180,252,186,225,180,198,240};
byte Scale[MODE_AMOUNT]{63,56,1,150,59,26,99,32,23,1,93,18};
//--------------------RUNNING STRING --------
#define TEXT_DIRECTION 1
#define MIRR_V 0
#define MIRR_H 0
#define TEXT_HEIGHT -1
#define LET_WIDTH 5
#define LET_HEIGHT 8
#define SPACE 1
#define COL 2             // (0хRRGGBB/1- rainbow 2 - all different)
byte currentMode = 10;
//-------------------- OTHER ---------------
//#define DEBUG                // Not done
//-------------------------------------------
//===========================================
