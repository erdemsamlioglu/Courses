#include "pragmas.h"
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 64
#define T_PRELOAD_HIGH 0x85 // High byte for 100ms
#define T_PRELOAD_LOW 0xF0  // Low byte for 100ms

#define MAX_LENGTH 10

/*Hocam zaman yok, maalesef commentleri cok aciklayici yazamadik*/

char receiver_array[MAX_LENGTH];
char buffer[BUFFER_SIZE];
unsigned int distance_remaining = 0;
unsigned int altitude_freq = 0;
unsigned int adc_value = 0;
int altitude_counter = 0;
int alt_responses_open = 0;
int manual_control_enabled = 0;
char rcvd_chr = 0;
int buffer_index = 0;
int msg_length = 0;
int transmission_length = 0;
int transmission_index = 0;
int receiver_index = 0;
int msg_started = 0;
int portb_val = 0;
int rbflag = 0;
bool man_state = false;


void clear_buffer() {
    memset(buffer, 0, BUFFER_SIZE);
    buffer_index = 0;
}

void init_adc() {
    TRISH = 0x10; // AN12 input RH4
    ADCON0 = 0x31; // Channel 12; Turn on AD Converter
    ADCON1 = 0x00; // All analog pins
    ADCON2 = 0xAA; // Right Align | 12 Tad | Fosc/32
    ADRESH = 0x00;
    ADRESL = 0x00;
    PIE1bits.ADIE = 0;  // Disable ADC interrupt initially
    PIR1bits.ADIF = 0;  // Clear ADC interrupt flag
}



void led_init(){
    // A
    LATA = 0x00; TRISA = 0x00; PORTA = 0x00;
    // B
    LATB = 0x00; TRISB = 0xF0; PORTB = 0x00;
    // C
    LATC = 0x00; TRISC = 0x00; PORTC = 0x00;
    // D
    LATD = 0x00; TRISD = 0x00; PORTD = 0x00;
}

void init_int() {
    /* configure I/O ports */
    TRISCbits.RC7 = 1; // TX1 and RX1 pin configuration
    TRISCbits.RC6 = 0;

    /* configure USART transmitter/receiver */
    TXSTA1 = 0x04;      // 8-bit transmit, transmitter enabled, asynchronous, high speed mode
    RCSTA1 = 0x90;      // 8-bit receiver, receiver enabled, serial port enabled
    BAUDCON1bits.BRG16 = 0;
        SPBRG1 = 21;       // for 40 MHz, to have 115200 baud rate, it should be 21

    /* configure the interrupts */
    INTCON = 0;         // clear interrupt register completely
    PIE1bits.TX1IE = 1; // enable USART transmit interrupt
    PIE1bits.RC1IE = 1; // enable USART receive interrupt
    PIR1 = 0;           // clear all peripheral flags

    INTCONbits.PEIE = 1; // enable peripheral interrupts
    INTCONbits.GIE = 1;  // globally enable interrupts
    INTCONbits.RBIE = 1;
}

// Timer setup for 100ms
void InitializeTimerAndInterrupts() {
    // Configure Timer0
    T0CON = 0x00;          // Reset the configuration
    T0CONbits.TMR0ON = 1;  // Turn on the timer
    T0CONbits.T08BIT = 0;  // Timer0 in 16-bit mode
    T0CONbits.T0CS = 0;    // Clock source is the internal instruction cycle clock (FOSC/4)
    T0CONbits.PSA = 0;     // Prescaler is assigned
    T0CONbits.T0PS = 0b100; // Prescaler 1:64

    // Pre-load the value for 100ms interval
    TMR0H = T_PRELOAD_HIGH; // High byte
    TMR0L = T_PRELOAD_LOW; // Low byte

    // Enable interrupts
    RCONbits.IPEN = 0;    // Disable priority levels on interrupts
    INTCON = 0x00;        // Reset the INTCON register
    INTCONbits.TMR0IE = 1;// Enable Timer0 overflow interrupt
    INTCONbits.GIE = 1;   // Global interrupt enable
    INTCONbits.PEIE = 1;  // Enable peripheral interrupts
} 

void init_system() {
    init_int(); // Initialize USART with interrupts
    init_adc(); // Initialize ADC
}


void openTransmission(int length) {
    TXSTA1bits.TXEN = 1; // Enable transmission
    transmission_length = length;
    transmission_index = 0;
}

void sendString(const char *str) {
    int length = strlen(str);
    if (length < MAX_LENGTH) {
        strcpy(receiver_array, str);
        openTransmission(length);
    }
}


void set_altitude_frequency(unsigned int frequency) {
    altitude_freq = frequency;
    if (altitude_freq == 0) 
    {
        alt_responses_open = 0;
        PIE1bits.ADIE = 0;
    } 
    else 
    {
        alt_responses_open = 1;
        altitude_counter = 0;
        PIE1bits.ADIE = 1;
        ADCON0bits.GODONE = 1;
    }
}



void sendAltitudeResponse(unsigned int adc_value) {
    char message[12];
    unsigned int altitude;

    if (adc_value >= 768) {
        altitude = 12000;
    } else if (adc_value >= 512) {
        altitude = 11000;
    } else if (adc_value >= 256) {
        altitude = 10000;
    } else {
        altitude = 9000;
    }

    sprintf(message, "$ALT%04X#", altitude);
    sendString(message);
}


void sendDistanceResponse() {
    char message[12];
    sprintf(message, "$DST%04X#", distance_remaining);
    sendString(message);
}

void manstart() {
    INTCONbits.RBIE = 1;
    man_state = true;
}

void manend() {
    INTCONbits.RBIE = 0;
    man_state = false;
}


void enable_manual_control(char command) {
    if (command == 1) {
        manstart();
    } else {
        manend();
    }
}

void start_simulation(unsigned int initial_distance) {
    distance_remaining = initial_distance;
    InitializeTimerAndInterrupts();
}

void stop_simulation() {
    // Actions to stop the simulation
    TXSTA1 = 0;
    T0CON = 0;
    INTCON = 0;
    RCON = 0;
    RCSTA1 = 0;
}

//update distance given speed
void update_distance(unsigned int speed)
{
    if (distance_remaining > speed)
    {
        distance_remaining -= speed;
    } 
    else
    {
        distance_remaining = 0;
    }
}



//handle led
void handle_led_command(char command) {
    if (man_state) {
        switch (command) {
            case 0x00:
                LATDbits.LATD0 = 0;
                LATCbits.LATC0 = 0; 
                LATBbits.LATB0 = 0; 
                LATAbits.LATA0 = 0; 
                break;
            case 0x01:
                LATDbits.LATD0 = 1;
                break;
            case 0x02:
                LATCbits.LATC0 = 1;
                break;
            case 0x03:
                LATBbits.LATB0 = 1;
                break;
            case 0x04:
                LATAbits.LATA0 = 1;
                break;
            default:
                break;
        }
    }
}

void handle_command(char* cmd) {
    int len = strlen(cmd);

    // handle the GO command
    if (strncmp(cmd, "$GOO", 4) == 0 && len == 9 && cmd[8] == '#') {
        unsigned int distance;
        sscanf(cmd + 4, "%04X", &distance);
        start_simulation(distance);
    }
    // handle the END command
    else if (strcmp(cmd, "$END#") == 0 && len == 5) {
        stop_simulation();
    }
    // handle the Speed command
    else if (strncmp(cmd, "$SPD", 4) == 0 && len == 9 && cmd[8] == '#') {
        unsigned int speed;
        sscanf(cmd + 4, "%04X", &speed);
        update_distance(speed);
    }
    // handle the Altitude command
    else if (strncmp(cmd, "$ALT", 4) == 0 && len == 9 && cmd[8] == '#') {
        unsigned int period;
        sscanf(cmd + 4, "%04X", &period);
        set_altitude_frequency(period);
        man_state = 0;
    }
    //handle the Manual Control command
    else if (strncmp(cmd, "$MAN", 4) == 0 && len == 7 && cmd[6] == '#') {
        char control;
        sscanf(cmd + 4, "%02X", &control);
        enable_manual_control(control);
        alt_responses_open = 0;
    }
    // handle the LED command
    else if (strncmp(cmd, "$LED", 4) == 0 && len == 7 && cmd[6] == '#') {
        char led;
        sscanf(cmd + 4, "%02X", &led);
        handle_led_command(led);
    }
    // If invalid command, ignore
}

void processReceivedChar(char ch) {
    static char command_buffer[BUFFER_SIZE];
    static int cmd_index = 0;
    static bool receiving_command = false;

    // Start of a new command
    if (ch == '$')
    {
        receiving_command = true;
        cmd_index = 0;  // Reset index
    }

    if (receiving_command)
    {
        if (cmd_index < BUFFER_SIZE - 1)
        {
            command_buffer[cmd_index++] = ch;
        }
        else 
        {
            // Buffer overflow
            receiving_command = false;
            cmd_index = 0;
        }


        if (ch == '#') {
            command_buffer[cmd_index] = '\0'; 
            handle_command(command_buffer);
            receiving_command = false;
            cmd_index = 0; 
        }
    }
}

void transmitData() {
    if (transmission_index < transmission_length)
    {
        TXREG1 = receiver_array[transmission_index++];
    }
    else
    {
        transmission_index = 0;
        while (TXSTA1bits.TRMT == 0) {}
        TXSTA1bits.TXEN = 0; // disable transmission   
    }
}


volatile unsigned int time_elapsed = 0;

__interrupt(high_priority) void HandleInterrupt() {
    
    
    if (RCSTA1bits.OERR) {
        RCSTA1bits.CREN = 0;
        RCSTA1bits.CREN = 1;}
    
    // USART receive interrupt
    if (PIR1bits.RC1IF == 1) {
        rcvd_chr = RCREG1;
        processReceivedChar(rcvd_chr);
        PIR1bits.RC1IF = 0;
    }
    
    
    // USART transmit interrupt
    if (PIR1bits.TX1IF == 1) {
        transmitData();
    }

    
    if (INTCONbits.TMR0IF)
    {
        time_elapsed += 100;
        INTCONbits.TMR0IF = 0;
        TMR0H = T_PRELOAD_HIGH;
        TMR0L = T_PRELOAD_LOW;
        
        if (alt_responses_open)
        {
            altitude_counter += 100;
            if (altitude_counter >= altitude_freq)
            {
                sendAltitudeResponse(adc_value);
                altitude_counter = 0;
            }
            else
            {
                sendDistanceResponse(); 
            }
        }
        
        
        else if(man_state)
        {
            switch(rbflag)
            {
                case 0:
                    sendDistanceResponse();
                    break;
                case 4:
                    sendString("$PRS04#");
                    rbflag = 0;
                    break;
                case 5:
                    sendString("$PRS05#");
                    rbflag = 0;
                    break;
                case 6:
                    sendString("$PRS06#");
                    rbflag = 0;
                    break;
                case 7:
                    sendString("$PRS07#");
                    rbflag = 0;
                    break;
                default:
                    break;
            }
            
            /*if (flag4)
            {
                sendString("$PRS04#");
                flag4 = false;
            }
            else if (flag5)
            {
                sendString("$PRS05#");
                flag5 = false;
            }
            else if (flag6)
            {
                sendString("$PRS06#");
                flag6 = false;
            }
            else if (flag7)
            {
                sendString("$PRS07#");
                flag7 = false;
            }
            else
            {
                sendDistanceResponse();
            }*/
        }
        
        else
        {
            {
                sendDistanceResponse();
            }
        }
        
    }
    
    //since maybe rb0 may be lit double case for each
    // interrupt-on-change
    if (INTCONbits.RBIF == 1) {
        portb_val = PORTB;
        INTCONbits.RBIF = 0;
        
        /*Pickit problemi yuzunden case seklinde yazim hata cikartabilir ancak
         pickit bagli degilken calisiyor*/
        
        switch(portb_val){
            case 0xE0:
                rbflag = 4;
                break;
            case 0xE1:
                rbflag = 4;
                break;
                
            case 0xD0:
                rbflag = 5;
                break;
            case 0xD1:
                rbflag = 5;
                break;
                
            case 0xB0:
                rbflag = 6;
                break;
            case 0xB1:
                rbflag = 6;
                break;
                
            case 0x70:
                rbflag = 7;
                break;
            case 0x71:
                rbflag = 7;
                break;
            default:
                break;
        }
        
        /*if (PORTBbits.RB4 == 0) {
            flag4 = true;
        } else if (PORTBbits.RB5 == 0) {
            flag5 = true;
        } else if (PORTBbits.RB6 == 0) {
            flag6 = true;
        } else if (PORTBbits.RB7 == 0) {
            flag7 = true;
        }
        portb_val = PORTB;
        INTCONbits.RBIF = 0;*/
    }
    
    
    //AD Interrupt
    if(PIR1bits.ADIF == 1)
    {
        adc_value = (unsigned short)((ADRESH << 8)+ADRESL);
        PIR1bits.ADIF = 0;
        ADCON0bits.GODONE = 1;
    }

}


void main(void) {
    led_init();
    init_system();
    clear_buffer();
    
    while (1) {
    }
    return;
}