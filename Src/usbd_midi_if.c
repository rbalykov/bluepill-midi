/**
  ******************************************************************************
  * @file           : usbd_midi_if.c
  * @brief          :
  ******************************************************************************

    (CC at)2016 by D.F.Mac. @TripArts Music

*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi.h"
#include "usdb_midi_if.h"
#include "stm32f1xx_hal.h"
#include "queue32.h"
#include "main.h"
#include "midi-uart.h"

// basic midi rx/tx functions
static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
//static uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);

// from mi:muz (Internal)
stB4Arrq rxq;

void (*cbNoteOff)(uint8_t ch, uint8_t note, uint8_t vel);
void (*cbNoteOn)(uint8_t ch, uint8_t note, uint8_t vel);
void (*cbCtlChange)(uint8_t ch, uint8_t num, uint8_t value);

static int checkMidiMessage(uint8_t *pMidi);

USBD_MIDI_ItfTypeDef hUsbClassMidi_CB =
{
	MIDI_DataRx
	//,MIDI_DataTx
};

static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length)
{
	uint16_t cnt;
	uint16_t msgs = length / 4;
	uint16_t chk = length % 4;
	if(chk == 0)
	{
		for(cnt = 0;cnt < msgs;cnt ++)
		{
			b4arrq_push(&rxq,((uint32_t *)msg)+cnt);
		}
	}
	return 0;
}

void sendMidiMessage(uint8_t *msg, uint16_t size)
{
	if(size == MIDI_USB_MSG_SIZE)
	{
		MIDI_Message_Ring_Push(msg);
	}
}



// from mi:muz (Interface functions)

static uint8_t buffer[4];

void mimuz_init(void){
  b4arrq_init(&rxq);
}

void setHdlNoteOff(void (*fptr)(uint8_t ch, uint8_t note, uint8_t vel)){
  cbNoteOff = fptr;
}

void setHdlNoteOn(void (*fptr)(uint8_t ch, uint8_t note, uint8_t vel)){
  cbNoteOn = fptr;
}

void setHdlCtlChange(void (*fptr)(uint8_t ch, uint8_t num, uint8_t value)){
  cbCtlChange = fptr;
}

void sendNoteOn(uint8_t ch, uint8_t note, uint8_t vel){
  buffer[0] = MIDI_PREAMBLE(MIDI_EVENT_NOTE_ON);
  buffer[1] = MIDI_EVENT_HEADER(MIDI_EVENT_NOTE_ON,ch);
  buffer[2] = MIDI_TRIM(note);
  buffer[3] = MIDI_TRIM(vel);
  sendMidiMessage(buffer,MIDI_USB_MSG_SIZE);
}

void sendNoteOff(uint8_t ch, uint8_t note){
  buffer[0] = MIDI_PREAMBLE(MIDI_EVENT_NOTE_OFF);
  buffer[1] = MIDI_EVENT_HEADER(MIDI_EVENT_NOTE_OFF,ch);
  buffer[2] = MIDI_TRIM(note);
  buffer[3] = 0;
  sendMidiMessage(buffer,MIDI_USB_MSG_SIZE);
}

void sendCtlChange(uint8_t ch, uint8_t num, uint8_t value){
  buffer[0] = MIDI_PREAMBLE(MIDI_EVENT_CONTROL);
  buffer[1] = MIDI_EVENT_HEADER(MIDI_EVENT_CONTROL,ch);
  buffer[2] = MIDI_TRIM(num);
  buffer[3] = MIDI_TRIM(value);
  sendMidiMessage(buffer,MIDI_USB_MSG_SIZE);
}

static int checkMidiMessage(uint8_t *pMidi){
  if(((*(pMidi + 1) & 0xf0)== 0x90)&&(*(pMidi + 3) != 0)){
    return 2;
  }else if(((*(pMidi + 1) & 0xf0)== 0x90)&&(*(pMidi + 3) == 0)){
    return 1;
  }else if((*(pMidi + 1) & 0xf0)== 0x80){
    return 1;
  }else if((*(pMidi + 1) & 0xf0)== 0xb0){
    return 3;
  }else{
    return 0;
  }
}

void processMidiMessage(){
  uint8_t *pbuf;
  uint8_t kindmessage;
  // Rx
  if(rxq.num > 0){
    pbuf = (uint8_t *)b4arrq_pop(&rxq);
    kindmessage = checkMidiMessage(pbuf);
    if(kindmessage == 1){
      if(cbNoteOff != NULL){
        (*cbNoteOff)(*(pbuf+1)&0x0f,*(pbuf+2)&0x7f,*(pbuf+3)&0x7f);
      }
    }else if(kindmessage == 2){
      if(cbNoteOn != NULL){
        (*cbNoteOn)(*(pbuf+1)&0x0f,*(pbuf+2)&0x7f,*(pbuf+3)&0x7f);
      }
    }else if(kindmessage == 3){
      if(cbCtlChange != NULL){
        (*cbCtlChange)(*(pbuf+1)&0x0f,*(pbuf+2)&0x7f,*(pbuf+3)&0x7f);
      }
    }
  }
  USBD_MIDI_DumpRingBuffer();
}

