
#include <avr/io.h>
#define SERIAL_RX_RING_SIZE 16

            .text
            .lcomm   rx_ring,SERIAL_RX_RING_SIZE

            .data
            .type   rx_head, @object
            .size   rx_head, 2
rx_head:
            .word   rx_ring

            .type   rx_tail, @object
            .size   rx_tail, 2
rx_tail:
            .word   rx_ring

            .text

            .global USART_RX_vect

USART_RX_vect:
            push r1
            eor r1, r1
            push r0
            in r0, AVR_STATUS_ADDR
            push r0
            push r31
            push r30
            push r29
            push r28
            push r27
            push r26
            push r16

            ; Read the incoming data
            lds r16, UDR0

            ; Get the tail pointer into Z
            lds r30, rx_tail
            lds r31, rx_tail+1

            ; Save tail pointer in X
            movw r26, r30

            ; Increment tail pointer
            adiw r30, 1

            ; Does the tail pointer need to wrap?
            cpi r30, lo8(rx_ring + SERIAL_RX_RING_SIZE)
            brne no_wrap_tail
            cpi r31, hi8(rx_ring + SERIAL_RX_RING_SIZE)
            brne no_wrap_tail

           ; Wrap the tail pointer
           ldi r30, lo8(rx_ring)
           ldi r31, hi8(rx_ring)
no_wrap_tail:
            ; Get head pointer in Y
            lds r28, rx_head
            lds r29, rx_head+1

            ; Is buffer full (new tail == head)?
            cp r28, r30
            brne ring_not_full
            cp r29, r31
            breq ring_full

ring_not_full:
            ; Toggle PORTD7
#ifdef TOGGLE
            in r0, _SFR_IO_ADDR(PORTD)
            ldi r28, 0x80
            eor r0, r28
            out _SFR_IO_ADDR(PORTD), r0
#else
            sbi _SFR_IO_ADDR(PORTD), 7
            ldi r28, 0x04
            sts TCNT1H, r28
#endif
            ; Store the received byte
            st x, r16

            ; Store the updated tail pointer
            sts rx_tail, r30
            sts rx_tail+1, r31

ring_full:
            pop r16
            pop r26
            pop r27
            pop r28
            pop r29
            pop r30
            pop r31
            pop r0
            out AVR_STATUS_ADDR, r0
            pop r0
            pop r1
            reti

            .global serial_getc
serial_getc:
            cli

            ; Get head pointer in Z
            lds r30, rx_head
            lds r31, rx_head+1

            ; Get tail pointer in X
            lds r26, rx_tail
            lds r27, rx_tail+1

            ; Is the ring empty (head == tail)?
            cp r26, r30
            brne ring_not_empty
            cp r27, r31
            brne ring_not_empty

            ldi r24, lo8(-1)
            ldi r25, hi8(-1)
            sei
            ret

ring_not_empty:
            ; Get the input byte into pair r25:r24
            ld r24, Z
            eor r25, r25

            ; Update head pointer
            adiw r30, 1

            ; Does the head pointer need to wrap?
            cpi r30, lo8(rx_ring + SERIAL_RX_RING_SIZE)
            brne no_wrap_head
            cpi r31, hi8(rx_ring + SERIAL_RX_RING_SIZE)
            brne no_wrap_head

            ; Wrap the head pointer
            ldi r30, lo8(rx_ring)
            ldi r31, hi8(rx_ring)

no_wrap_head:
            ; Store the updated tail pointer
            sts rx_head, r30
            sts rx_head+1, r31
            sei
            ret

            ; Apparently we need this if there aren't any other globals
            .global __do_copy_data
            .global __do_clear_bss

