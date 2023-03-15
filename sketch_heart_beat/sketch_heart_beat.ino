/*************************************
 * Filename:  sketch_heart_beat.ino
 * Author:    Alexandros Rikos
 *************************************/
 
#include <stdint.h>
#include <string.h>

#define BAUD_RATE 9600
#define RECEIVE_TIMEOUT_MS 500
#define START_BYTE 0xAA

#define SIZE_OF_COMMAND_FRAME 3
#define SIZE_OF_RESPONSE_FRAME 3

#define POSITION_OF_START_BYTE 0
#define POSITION_OF_FRAME_TYPE 1
#define POSITION_OF_COMMAND_ID 2
#define POSITION_OF_ACK_CODE 2

enum STATE 
{
  S01_IDLE = 0,
  S02_RECEIVE_FRAME,
  S03_WAIT_FOR_RESPONSE,
  S04_TRANSMIT_FRAME,
};

enum ERROR_CODE
{
  NO_ERROR = 0x00,
  RECEIVE_TIMEOUT,
  UNKNOWN_FRAME_TYPE,
  UNKNOWN_COMMAND,
};

enum FRAME_TYPE
{
  FRAME_TYPE_COMMAND = 0x01,
  FRAME_TYPE_RESPONSE,
};

static uint8_t au8CommandFrame[SIZE_OF_COMMAND_FRAME] = {0};
static uint8_t au8ResponseFrame[SIZE_OF_RESPONSE_FRAME] = {0};

static bool blFrameReceived = false;
static bool blSendResponse = false;

// -------------------
// function prototypes
// -------------------
void vExecuteStateMachine(void);
void vHandleReceivedFrame(void);
void vPrepareResponseFrame(enum ERROR_CODE eErrorCode);

/* setup
 * -----
 * The setup function is executed once after startup.
 * 
 * param[in]: None
 * param[out]: None
 */
void setup() {
  Serial.begin(BAUD_RATE);
}

/* loop
 * ----
 * The loop function is be executed again and again.
 * 
 * param[in]: None
 * param[out]: None
 */
void loop() 
{
  vExecuteStateMachine();  

  // if a frame has been received, handle the frame
  if (blFrameReceived == true)
  {
    blFrameReceived = false;
    vHandleReceivedFrame();
    // ready to send a response
    blSendResponse = true;
  }
}

/* vExecuteStateMachine
 * --------------------
 * Executes state machine that receives frames and sends responses. 
 * 
 * param[in]: None
 * param[out]: None
 */
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
        blFrameReceived = true;
        eState = S04_TRANSMIT_FRAME;
      }
      // check whether a timeout has occurred
      else if (millis() - u16Tstart >= RECEIVE_TIMEOUT_MS)
      {
        vPrepareResponseFrame(RECEIVE_TIMEOUT);
        eState = S04_TRANSMIT_FRAME;
      }
      break;
    }

    case S03_WAIT_FOR_RESPONSE:
    {
      if (blSendResponse == true)
      {
        eState = S04_TRANSMIT_FRAME;
      }
    }
	
    case S04_TRANSMIT_FRAME:
    {
      // send the response frame
      Serial.write(au8ResponseFrame, SIZE_OF_RESPONSE_FRAME);
      blSendResponse = false;

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

/* vHandleReceivedFrame
 * --------------------
 * Handles the received frame and prepares the response frame.
 * 
 * param[in]: None
 * param[out]: None
 */
void vHandleReceivedFrame(void)
{
  enum ERROR_CODE eErrorCode = NO_ERROR;

  if (au8CommandFrame[POSITION_OF_FRAME_TYPE] == FRAME_TYPE_COMMAND)
  {
    switch (au8CommandFrame[POSITION_OF_COMMAND_ID])
    {
      case 0x01:
      {
        break;
      }
      case 0x02:
      {
        break;
      }
      default:
      {
        eErrorCode = UNKNOWN_COMMAND;
      }
    }
  }
  else
  {
    eErrorCode = UNKNOWN_FRAME_TYPE;
  }

  vPrepareResponseFrame(eErrorCode);
}

/* vPrepareResponseFrame
 * ---------------------
 * Prepares the response frame.
 * 
 * param[in] eErrorCode: The acknowledgment code that will be written in the response frame.
 * param[out]: None
 */
void vPrepareResponseFrame(enum ERROR_CODE eErrorCode)
{
  au8ResponseFrame[POSITION_OF_START_BYTE] = START_BYTE;
  au8ResponseFrame[POSITION_OF_FRAME_TYPE] = FRAME_TYPE_RESPONSE;
  au8ResponseFrame[POSITION_OF_ACK_CODE] = eErrorCode;

  // a response is ready to be sent
  blSendResponse = true;
}
