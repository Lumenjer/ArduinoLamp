// ============================== SETTINGS ==================================
// ---------------- MATRIX --------------
//__________ BRIGHTNESS AND POWER ________
#define BRIGHTNESS 200        // Brightness limit (0-255)
#define CURRENT_LIMIT 2000    // mA (0 - off) 
//_____________________ SIZE _____________
#define WIDTH 16               
#define HEIGHT 16              
//_____________________ COLOR ______________
#define COLOR_ORDER GRB       
//_______________ TYPE AND DIRECTION ______
#define MATRIX_TYPE 0         // 0 - zigzag, 1 - paralel
#define CONNECTION_ANGLE 0    // 0 - bottom left, 1 - top left, 2 - top right, 3 - bottom right
#define STRIP_DIRECTION 0     // 0 - right, 1 - up, 2 - left, 3 - down
// Matrix settings guide: https://alexgyver.ru/matrix_guide/
//___________________ OTHER _____________
#define SEGMENTS 1
//#define XY_MAPPING
// ----------------- CONNECTING AND CONTROL ------------
#define LED_PIN 4
#define USE_BUTTON
#define USE_IR
#define USE_BT
#define IR_PIN 3
#define BUTTON_PIN 2
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
//-------------------- ЭФФЕКТЫ --------------
#define MODE_AMOUNT 14
byte Brightness[MODE_AMOUNT]{16,8,9,23,11,9,9,10,27,15,22,21,7};
byte Speed[MODE_AMOUNT]{142,196,217,203,19,207,180,252,186,225,180,198,240};
byte Scale[MODE_AMOUNT]{63,56,1,150,59,26,99,32,23,1,93,18};
byte currentMode = 10;
//-------------------- OTHER ---------------
//#define DEBUG                // Not done
//-------------------------------------------
//===========================================
