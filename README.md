# PIXELS - A super simple LED application layer

This protocols goal is to allow for sending pixel data (color values) to
rgb and rgbw stripes in a performant way. This is especially useful for
sending data in "realtime" to led stripes connected to an ESP32 via
WIFI/UDP.

This protocol is based on
[s - Little Stream - Embedded streaming layer for UDPX](https://github.com/IoTPanic/s)

## Message Structure

A message consists of a **header**, a **payload** and a **crc-check**:

| Header (5Bytes) | Payload (x Bytes) | CRC-Check (1 Byte) |
|:----------------|:------------------|:-------------------|


### Header

| Byte Number: | 1                     | 2         | 3              | 4-5                                                                                      |
|:-------------|:----------------------|:----------|:---------------|:-----------------------------------------------------------------------------------------|
|              | Message Type          | Sync Word | Channel Number | Length of payload                                                                        |
| Data Type    | Uint8                 | Uint8     | Uint8          | 16 bit [LSB](https://developer.mozilla.org/de/docs/Glossary/Endianness) unsigned integer |
| Example      | HEX: 0x50 / ASCII: 80 |           | 1              | 144                                                                                      |

#### Message Type

The starting byte must be `0x50` which is ascii uppercase **'P'**. (This
stands for 'pixels'). This is to verify that the message is in fact
pixel data.

There could be other protocols that always start with 0x50, but chances
are good that you'll not face that kind of problem.

#### Sync Word

This is to verify the right client is connected and the right mode is
on.

#### Channel Number

Which channel of the controller should consume the payload to be sent?

#### Length of payload

How many pixels will be in the payload?

### Payload

| Byte Number: | 1     | 2     | 3     | 4     | ... |
|:-------------|:------|:------|:------|:------|:----|
|              | R     | G     | B     | (W)   | ... |
| Data Type    | Uint8 | Uint8 | Uint8 | Uint8 | ... |
| Example      | 16    | 155   | 43    | 0     | ... |

The payload consists of RGB or RGBW values, depending on which mode is
used. **The number of values sent in the payload must match "Length of
payload" setting in the message header.** An RGB Value consists of 3
Uint8 values representing the brightness for the colors Red, Green and
Blue.

Example: this is 'red' (#ff0000)

| 255 | 0 | 0 |
|:----|:--|:--|

An RGBW Value consists of 4 Uint8 values representing the brightness for
the colors Red, Green and Blue and White. Example: this is 'pure white'
using the white led of an rgbw strip.

| 0 | 0 | 0 | 255 |
|:--|:--|:--|:----|

### CRC Check

| Byte Number: | 1     |
|:-------------|:------|
|              | CRC   |
| Data Type    | Uint8 |
| Example      | 34    |

#### example implementation:

```c++
uint8_t crc = 0x0;
for(uint i = 0; i<size;i++){
   crc = crc ^ payload[i];
}
```
