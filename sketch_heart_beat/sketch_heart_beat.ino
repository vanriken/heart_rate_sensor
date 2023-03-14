#include <stdint.h>
#include <string.h>

#define BAUD_RATE 9600
#define RECEIVE_TIMEOUT_MS 2000
#define START_BYTE 0xAA

#define SIZE_OF_COMMAND_FRAME 3
#define SIZE_OF_RESPONSE_FRAME 3

enum STATE 
{
  S01_IDLE = 0,
  S02_RECEIVE_FRAME = 1,
  S03_TRANSMIT_FRAME = 2,
};

enum ERROR_CODE
{
  NO_ERROR = 0x00,
  RECEIVE_TIMEOUT = 0x01,
};

enum FRAME_TYPE
{
  FRAME_TYPE_COMMAND = 0x01,
  FRAME_TYPE_RESPONSE = 0x02,
};

static uint8_t au8CommandFrame[SIZE_OF_COMMAND_FRAME] = {0};
static uint8_t au8ResponseFrame[SIZE_OF_RESPONSE_FRAME] = {0};

// function prototypes
void vExecuteStateMachine(void);

// setup function
void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
}

// loop function
void loop() 
{
  vExecuteStateMachine();  
  
  /* 
  if command_received: handle the command
  else nothing
  
  if output_enabled: send data
  if flag #2: do thing nr 2
  */
}

// vExecuteStateMachine: receives command and sends a response
void vExecuteStateMachine(void)
{
  static enum STATE eState = S01_IDLE;
  static uint8_t u8NrOfReceivedBytes = 0;

  uint8_t u8Byte = 0;
  static uint16_t u16Tstart = 0;
  static enum ERROR_CODE eErrorCode;
  
  switch (eState)
  {
    case S01_IDLE:
    {
      if (Serial.available() > 0)
      {
        // read one byte
        u8Byte = Serial.read();
      }

      if (u8Byte == START_BYTE)
      {
        au8CommandFrame[u8NrOfReceivedBytes++] = u8Byte;
        u16Tstart = millis();
        eState = S02_RECEIVE_FRAME;
      }
      break;
    }
	
    case S02_RECEIVE_FRAME:
    {
      if (Serial.available() > 0)
      {
        u8Byte = Serial.read();
        au8CommandFrame[u8NrOfReceivedBytes++] = u8Byte;
      }

      // check whether all bytes have been received
      if (u8NrOfReceivedBytes >= SIZE_OF_COMMAND_FRAME)
      {
        eErrorCode = NO_ERROR;
        eState = S03_TRANSMIT_FRAME;
      }
      // check whether a timeout has occurred
      else if (millis() - u16Tstart >= RECEIVE_TIMEOUT_MS)
      {
        eErrorCode = RECEIVE_TIMEOUT;
        eState = S03_TRANSMIT_FRAME;
      }
      break;
    }
	
    case S03_TRANSMIT_FRAME:
    {
      // build the response frame
      au8ResponseFrame[0] = START_BYTE;
      au8ResponseFrame[1] = FRAME_TYPE_RESPONSE;
      au8ResponseFrame[2] = eErrorCode;

      // send the response frame
      Serial.write(au8ResponseFrame, SIZE_OF_RESPONSE_FRAME);

      // reset command frame, response frame and nr of received bytes
      memset(au8CommandFrame, 0, SIZE_OF_COMMAND_FRAME);
      memset(au8ResponseFrame, 0, SIZE_OF_COMMAND_FRAME);
      u8NrOfReceivedBytes = 0;

      eState = S01_IDLE;
      
      break;
    }
	
    default:
    {
      // unknown state
      break;
    }
  }
}
