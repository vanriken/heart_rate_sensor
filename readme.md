# Heart-rate Sensor

## Overview
This project uses an Arduino with the MAX32664 Biometric Sensor Hub and the MAX30101 Pulse Oximetry and Heart Rate Module.
The user can interact directly with the Arduino by using a serial monitor program such as [HTERM](https://www.der-hammer.info/pages/terminal.html).
In the future, it will also be possible to interact with the sensor by using a GUI on the PC.


## Communication Protocol
A communication protocol is used to communicate with the Arduino. The protocol defines two frame types: **command frames** and **response frames**. Command frames are sent from a central control unit (CCU) to the Arduino. Response frames are sent from the Arduino to the CCU, as a response to a command frame.

| Frame Type | Value |
| :- | :-:|
| Response Frame | 0x00 |
| Command Frame | 0x01 |

### Command Frame
Command frames consist of three fields:

| | 1 | 2 | 3 |
| :- | :-: | :-: | :-: |
|**Field Name** | Start Byte | Frame Type | Command ID |
|**Field Value** | 0xAA | 0x01 | 0xXX |

### Response Frame
Response frames consist of four fields:

| | 1 | 2 | 3 | 4 |
| :- | :-: | :-: | :-: | :-: |
|**Field Name** | Start Byte | Frame Type | Command ID | Error Code |
|**Field Value** | 0xAA | 0x00 | 0xXX | 0xXX |

The "Command ID" in the response frame is the "Command ID" from the command frame. <br>
The field "Error Code" contains one of the following codes:
|Error Code | Interpretation |
| :-: | :-: |
| 0x00 | No Error |
| 0x01 | Unknown Frame Type |
| 0x02 | Unknown Command ID |
| 0x03 | Receive Timeout |

### Command IDs
To be defined.