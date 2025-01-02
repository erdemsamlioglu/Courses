// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL // Oscillator Selection bits (HS oscillator,
                           // PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF // Fail-Safe Clock Monitor Enable bit
                           // (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF  // Internal/External Oscillator Switchover bit
                           // (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF  // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF // Brown-out Reset Enable bits (Brown-out
                           // Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT =                                                           \
    OFF // Watchdog Timer Enable bit
        // (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF // Low-Power Timer1 Oscillator Enable bit
                             // (Timer1 configured for higher power operation)
#pragma config MCLRE = ON    // MCLR Pin Enable bit (MCLR pin enabled;
                             // RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF   // Single-Supply ICSP Enable bit (Single-Supply
                           // ICSP disabled)
#pragma config XINST = OFF // Extended Instruction Set Enable bit
                           // (Instruction set extension and Indexed
                           // Addressing mode disabled (Legacy mode))

#pragma config DEBUG = OFF // Disable In-Circuit Debugger

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

// ============================ //
//             End              //
// ============================ //

#include <xc.h>
#include <stdint.h>

// ============================ //
//        DEFINITIONS           //
// ============================ //
typedef enum {
  DOT_PIECE,
  SQUARE_PIECE,
  L_PIECE,
  PIECE_TYPE_COUNT 
} PieceType;

typedef enum { FIRST, SECOND, THIRD, FOURTH, ROTATION_TYPE_COUNT } rotationType;

typedef struct {
  int x;
  int y;
  PieceType type;
  rotationType rotation;
} Piece;


// ============================ //
//          GLOBALS             //
// ============================ //

// You can write globals definitions here...
static int GRID_HEIGHT = 8;
static int GRID_WIDTH = 4;

Piece currentPiece = {0, 0, DOT_PIECE, FIRST};
int timer0Counter = 0;
int grid[4][8] = {0};
int rotationRequested = 0;
int submissionRequested = 0;
int piecesInserted = 0;
static unsigned char lastPortBState = 0;



const unsigned char seven_seg_digits[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// ============================ //
//          FUNCTIONS           //
// ============================ //

// You can write function definitions here...



#define T_PRELOAD_LOW 0xC1
#define T_PRELOAD_HIGH 0x67
#define DEBOUNCE_CHECKS 5 



static int blinkState = 1;
static int ones = 0;
static int tens = 0;
static int portAValue = 0x00;

static int rotationButtonStableHigh = 0;
static int rotationButtonPressed = 0;


void Init()
{
    
    // B
    LATB = 0x00; PORTB = 0x00; TRISB = 0x90;
    // C
    LATC = 0x00; PORTC = 0x00; TRISC = 0x00;
    // D
    LATD = 0x00; PORTD = 0x00; TRISD = 0x00;
    //D
    LATE = 0x00; PORTE = 0x00; TRISE = 0x00;
    // E
    LATF = 0x00; PORTF = 0x00; TRISF = 0x00;
    //F
}

void InitializeTimerAndInterrupts()
{

    T0CON = 0x00;
    T0CONbits.TMR0ON = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 1;
    TMR0H = T_PRELOAD_HIGH;
    TMR0L = T_PRELOAD_LOW;

    RCONbits.IPEN = 0;
    INTCON = 0x00;
    INTCONbits.TMR0IE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}


void init_display() {
  ADCON1 = 0x0F; 
  TRISH = 0x00;
  TRISJ = 0x00;
  PORTH = 0x00;
  PORTJ = 0x00;
}

int totalArea_Increase(Piece piece) {
    PieceType p_type = piece.type;
    if (p_type == DOT_PIECE) {
        return 0;
    } else if (p_type==SQUARE_PIECE) {
        return 3;
    } else if (p_type==L_PIECE) {
        return 2;
    }
return 0;
}

void display_digit(int digit, int position) {
    if(position == 1)
    {
        LATH = 0x04;
    }
    else if(position == 0)
    {
        LATH = 0x08;
    }
    else
    {
        LATH = 0x03;
    }
    
    LATJ = seven_seg_digits[digit];
}

void display_number(int num) {
  int tens = num / 10;
  int ones = num % 10;
  

  display_digit(tens,1);
  display_digit(ones, 0);
  display_digit(0, 2);

}

void getShape(PieceType type, rotationType rotation, int shape[2][2]) {
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 2; x++) {
            shape[y][x] = 0;
        }
    }

    if (type == DOT_PIECE) {
        shape[0][0] = 1;
    } else if (type == SQUARE_PIECE) {
        shape[0][0] = 1;
        shape[0][1] = 1;
        shape[1][0] = 1;
        shape[1][1] = 1;
    } else if (type == L_PIECE) {
        if (rotation == FIRST) {
            shape[0][0] = 1; shape[1][0] = 1; shape[1][1] = 1;
            
        } else if (rotation == SECOND) {
            shape[0][1] = 1; shape[1][0] = 1; shape[1][1] = 1;
        } else if (rotation == THIRD) {
            shape[0][0] = 1; shape[0][1] = 1; shape[1][1] = 1;
            
        } else if (rotation == FOURTH) {
            shape[0][0] = 1; shape[0][1] = 1; shape[1][0] = 1;
 }
}
}


int wouldCollide(Piece piece) {
    int shape[2][2];
    getShape(piece.type, piece.rotation, shape);

    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            if (shape[dx][dy]) {
                int x = piece.x + dx;
                int y = piece.y + dy;

                if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT || grid[x][y] != 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}


void updateGrid(Piece piece) {
    int shape[2][2];
    getShape(piece.type, piece.rotation, shape);

    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            if (shape[dx][dy]) {
                int x = piece.x + dx;
                int y = piece.y + dy;
                grid[x][y] = 1;
            }
        }
    }
}

void displayGrid() {
    LATC = LATD = LATE = LATF = 0x00;

    for (int dx = 0; dx < GRID_WIDTH; dx++) {
        for (int dy = 0; dy < GRID_HEIGHT; dy++) {
            if (grid[dx][dy] == 1) {
                if (dx == 0) {
                    LATC |= (1 << dy);
                } else if (dx == 1) {
                    LATD |= (1 << dy);
                } else if (dx == 2) {
                    LATE |= (1 << dy);
                } else if (dx == 3) {
                    LATF |= (1 << dy);
                }
            }
 }
}
}


void overlayCurrentPiece() {
    int shape[2][2];
    getShape(currentPiece.type, currentPiece.rotation, shape);

    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            if (shape[dx][dy]) {
                int x = currentPiece.x + dx;
                int y = currentPiece.y + dy;

                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    if (x == 0) {
                        LATC |= (1 << y);
                    } else if (x == 1) {
                        LATD |= (1 << y);
                    } else if (x == 2) {
                        LATE |= (1 << y);
                    } else if (x == 3) {
                        LATF |= (1 << y);
                    }
                }
            }
 }
}
}

void clearOverlayCurrentPiece() {
    int shape[2][2];
    getShape(currentPiece.type, currentPiece.rotation, shape);

    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            if (shape[dx][dy]) {
                int x = currentPiece.x + dx;
                int y = currentPiece.y + dy;

                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    if (x == 0) {
                        LATC &= ~(1 << y);
                    } else if (x == 1) {
                        LATD &= ~(1 << y);
                    } else if (x == 2) {
                        LATE &= ~(1 << y);
                    } else if (x == 3) {
                        LATF &= ~(1 << y);
                    }
                }
            }
 }
}
}


void updateDisplay() {
    displayGrid(); 
    clearOverlayCurrentPiece();
    if (blinkState) {
        overlayCurrentPiece();
    }
}

int isGridFull() {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y] == 0) {
                return 0;
            }
        }
    }
    return 1;
}







int getWidth(Piece piece) {
    if (piece.type == DOT_PIECE) {
        return 1;
    } else if (piece.type == SQUARE_PIECE) {
        return 2;
    } else if (piece.type == L_PIECE) {
        return 2;
    }
    return 1;
}

int getHeight(Piece piece) {
    if (piece.type == DOT_PIECE) {
        return 1;
    } else if (piece.type == SQUARE_PIECE) {
        return 2;
    } else if (piece.type == L_PIECE) {
        return 2;
    }
    return 1;
}




void nextPiece() {
    piecesInserted++;
    currentPiece.x = 0;
    currentPiece.y = 0;
    currentPiece.type = (currentPiece.type + 1) % PIECE_TYPE_COUNT;
    currentPiece.rotation = FIRST;
}



// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //
__interrupt(high_priority) void HandleInterrupt() {

  if (INTCONbits.TMR0IF) {


    timer0Counter++;
    blinkState = !blinkState;
    updateDisplay(); 

    if (timer0Counter >= 8) {
      if (currentPiece.y + getHeight(currentPiece) < GRID_HEIGHT) {
        currentPiece.y++;
      } else {

      }

      timer0Counter = 0;
    }

    INTCONbits.TMR0IF = 0;
    TMR0H = T_PRELOAD_HIGH;
    TMR0L = T_PRELOAD_LOW;
  
  }

if (INTCONbits.RBIF) {
    unsigned char portBValue = PORTB;

    


    if ((portBValue & 0x10) && !(lastPortBState & 0x10)) { 
        
        
        if (currentPiece.type == L_PIECE) {
            currentPiece.rotation = (currentPiece.rotation + 1) % ROTATION_TYPE_COUNT;
        }
    }
    __delay_ms(3);
    if ((portBValue & 0x80) && !(lastPortBState & 0x80)) {
        
        if (!wouldCollide(currentPiece)) {
            updateGrid(currentPiece);
            piecesInserted += totalArea_Increase(currentPiece);
            
            nextPiece();
            timer0Counter=0;
            if (isGridFull()){
                asm("goto 0x0000");
            }
        }
    }
    __delay_ms(3);
    lastPortBState = portBValue;
    INTCONbits.RBIF = 0;
}

}

static unsigned char lastPortAState = 0;


// ============================ //
//            MAIN              //
// ============================ //
void main() {

    Init();
    __delay_ms(1000);
    InitializeTimerAndInterrupts();
    init_display();

    
    updateDisplay();

  
  lastPortAState = PORTA;
  lastPortBState = PORTB;

  while (1) {
      // Poll
        tens = piecesInserted / 10;
        ones = piecesInserted % 10;
        portAValue = PORTA;
        
        __delay_ms(3);
        if (portAValue & 0x01 && !(lastPortAState & 0x01)) {
            if (currentPiece.x + getWidth(currentPiece) < GRID_WIDTH)
            {currentPiece.x++;}

        }
        display_digit(tens,1);
        __delay_ms(3);
        if (portAValue & 0x02 && !(lastPortAState & 0x02)) {
            if (currentPiece.y > 0)
            {currentPiece.y--;}

        }
        __delay_ms(3);
        if (portAValue & 0x04 && !(lastPortAState & 0x04)) { 
            if (currentPiece.y + getHeight(currentPiece) < GRID_HEIGHT)
            {currentPiece.y++;}

        }
        display_digit(ones, 0);
        __delay_ms(6);
        if (portAValue & 0x08 && !(lastPortAState & 0x08)) { 
            if (currentPiece.x > 0)
            {currentPiece.x--;}
        }

      display_digit(0, 2);
      __delay_ms(3);
      lastPortAState = portAValue;
      updateDisplay();
      
      
      

  }
}