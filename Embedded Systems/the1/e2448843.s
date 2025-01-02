PROCESSOR 18F8722

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
; CONFIG1H
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = OFF       ; Power-up Timer Enable bit (PWRT disabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
CONFIG DEBUG = OFF      ; Disable In-Circuit Debugger


GLOBAL var1
GLOBAL var2
GLOBAL result
GLOBAL counter

    
GLOBAL flagRE0
GLOBAL flagRE1
    
GLOBAL enableBarB
GLOBAL enableBarC
GLOBAL progressBarB
GLOBAL progressBarC
    
GLOBAL loopCounter
GLOBAL outerLoopCounter
    
GLOBAL prevRE0
GLOBAL prevRE1
    
GLOBAL temp_result
GLOBAL barB
GLOBAL barC
    
    
GLOBAL TEMP

    
    


    
    
; Define space for the variables in RAM
PSECT udata_acs
var1:
    DS 1 ; Allocate 1 byte for var1
var2:
    DS 1 
counter:
    DS 1 
temp_result:
    DS 1   
result: 
    DS 1   
flagRE0:
    DS 1 
flagRE1:
    DS 1 
barB:
    DS 1
barC:
    DS 1
loopCounter:
    DS 1 
outerLoopCounter:
    DS 1
prevRE0:
    DS 1 
prevRE1:
    DS 1
enableBarB:
    DS 1
enableBarC:
    DS 1
progressBarB:
    DS 1
progressBarC:
    DS 1
TEMP:
    DS 1




    


PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto       main

PSECT CODE
main:
    clrf var1	; var1 = 0		
    clrf result ; result = 0
    clrf var2
    clrf temp_result
    
    
    clrf flagRE0
    clrf flagRE1
    clrf barB 
    clrf barC
    clrf loopCounter
    clrf outerLoopCounter
    clrf counter
    
    
    clrf prevRE0
    clrf prevRE1
    clrf progressBarB
    clrf progressBarC
    clrf enableBarB
    clrf enableBarC
    
    clrf TEMP
    

   
    
    movlw 128
    movwf counter
    
    movlw 115
    movwf outerLoopCounter

    
    
    
    ; PORTB
    ; LATB
    ; TRISB determines whether the port is input/output
    ; set output ports
    clrf TRISB
    clrf TRISC
    clrf TRISD
    setf TRISE ; PORTE is input
    
    
    
    movlw 00001111B
    movwf TRISA
    
    setf PORTB
    setf LATC ; light up all pins in PORTC
    setf LATD
    call busy_wait_thousand
    
    clrf PORTB  ; Turn off all LEDs
    clrf LATC
    clrf LATD
    
    

    
    main_loop:
	clrf loopCounter
	
    inner_loop:
	call check_buttons
	incfsz loopCounter, F
	goto inner_loop
	
	decfsz outerLoopCounter, F
	goto main_loop
	
	call update_display 
	call toggle_rd0
	movlw 115
	movwf outerLoopCounter
	goto main_loop
	
	
	
	
toggle_rd0:
    btg LATD,0, 0 
    return
	

check_buttons:
    btfsc PORTE, 0
    goto RE0_pressed
    btfss prevRE0, 0
    goto RE0_no_change
    bsf flagRE0, 0
    bcf prevRE0, 0
    goto RE1_check

RE0_pressed:
    bsf prevRE0, 0
    goto RE1_check

RE0_no_change:

RE1_check:
    btfsc PORTE, 1
    goto RE1_pressed
    btfss prevRE1, 0
    return
    bsf flagRE1, 0
    bcf prevRE1, 0
    return

RE1_pressed:
    bsf prevRE1, 0
    return
    
    
    
    

update_display:
    btfss flagRE1, 0
    goto checkFlagRE0
    comf enableBarB, F     
    clrf flagRE1
    btfsc enableBarB, 0
    goto checkFlagRE0
    clrf PORTB                

checkFlagRE0:
    btfss flagRE0, 0
    goto checkEnableBarB
    comf enableBarC, F     
    clrf flagRE0
    btfsc enableBarC, 0
    goto checkEnableBarB
    clrf LATC                 
    

checkEnableBarB:
    btfsc enableBarB, 0
    call updateProgressBarB

checkEnableBarC:
    btfsc enableBarC, 0
    call updateProgressBarC

    return



    


updateProgressBarB:
    movf PORTB, W
    xorlw 0xFF
    btfsc STATUS, 2
    goto clearAndRestartB
    movf PORTB, W
    addwf PORTB, W
    movwf PORTB, W
    incf PORTB, F
    return

clearAndRestartB:
    clrf PORTB
    return

    
updateProgressBarC:
    movf PORTC, W
    xorlw 0xFF
    btfsc STATUS, 2   
    goto clearAndRestartC

    
    movlw 0x00 
    subwf PORTC, W
    btfss STATUS, 2
    goto check128

setupPortCStart:
    movlw 0x80
    movwf PORTC
    goto updateCExit

check128:
    movlw 0x80
    subwf PORTC, W
    btfss STATUS, 2
    goto check192
    movlw 0xC0
    movwf PORTC
    goto updateCExit

check192:
    movlw 0xC0
    subwf PORTC, W
    btfss STATUS, 2
    goto check224
    movlw 0xE0
    movwf PORTC
    goto updateCExit

check224:
    movlw 0xE0
    subwf PORTC, W
    btfss STATUS, 2
    goto check240
    movlw 0xF0
    movwf PORTC
    goto updateCExit

check240:
    movlw 0xF0
    subwf PORTC, W
    btfss STATUS, 2
    goto check248
    movlw 0xF8
    movwf PORTC
    goto updateCExit

check248:
    movlw 0xF8
    subwf PORTC, W
    btfss STATUS, 2
    goto check252
    movlw 0xFC
    movwf PORTC
    goto updateCExit

check252:
    movlw 0xFC
    subwf PORTC, W
    btfss STATUS, 2
    goto check254
    movlw 0xFE
    movwf PORTC
    goto updateCExit

check254:
    movlw 0xFE
    subwf PORTC, W
    btfss STATUS, 2
    goto updateCExit
    movlw 0xFF
    movwf PORTC
    goto updateCExit

updateCExit:
    return

clearAndRestartC:
    clrf PORTC
    return






    
    ;busy waits


busy_wait:
    ; for (var2 = 0; var 
    ; for (var1 = 255; var1 != 0; --var1)
    movlw 0
    movwf var2 ; var2 = 0
    outer_loop_start:
	setf var1 ; var1 = 255
	loop_start:
	    decf var1
	    bnz loop_start
	incfsz var2
	bra outer_loop_start
    return
    
busy_wait_helper:
    ; for (var2 = 0; var 
    ; for (var1 = 255; var1 != 0; --var1)
    movlw 100
    movwf var2 ; var2 = 128
    outer_loop_start_helper:
	setf var1 ; var1 = 255
	loop_start_helper:
	    decf var1
	    bnz loop_start
	incfsz var2
	bra outer_loop_start
    return
    
busy_wait_thousand:
    	call busy_wait
	call busy_wait
	call busy_wait
	call busy_wait
	call busy_wait
	return
	
	
busy_wait_fivehundred:
    call busy_wait
    call busy_wait_helper
    return
    	
	
	
    
end resetVec