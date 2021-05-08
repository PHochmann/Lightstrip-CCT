# Lightstrip-CCT
Arduino Nano firmware to dim 12V CCT LED strip with cold white and warm white LEDs using a 433 MHz remote

## Receiver
Arduino Nano that receives 433 MHz signals from remote.
Drives two MOSFETs to control the cold white and warm white LEDs of a CCT Lightstrip.

## Transmitter
Arduino Pro Mini that sends input from two potentiometers via 433 MHz to receiver.
Left pot controls brightness, right pot controls color temperature.
Additionally switches a 433 MHz RC plug on and off based on a treshold on the brightness.
