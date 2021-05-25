# Arduino Nano Visual Wake Words Example

Example repository for person detection using the TinyMLPerf Visual Wake Words Dataset and Edge Impulse for the HarvardX Arduino TinyML Kit.

- [Visual Wake Words Dataset](https://github.com/mlcommons/tiny/tree/master/v0.1/training/visual_wake_words)
- [Public Edge Impulse Project](https://studio.edgeimpulse.com/public/29439/latest)

## Pre-requisites

- Purchase an [Arduino Tiny Machine Learning Kit](https://store.arduino.cc/usa/tiny-machine-learning-kit)
- Install [Arduino IDE](https://www.arduino.cc/en/software)
- Install Arduino Nano 33 BLE Sense Board: Open the Arduino boards manager in `Tools -> Board -> Boards Manager` and search for `Arduino MBED OS Boards`
- Install HarvardX Arduino Library: Open the Arduino library manager in
`Tools -> Manage Libraries...` and search for `Harvard_TinyMLx`


## Compile & Flash

1. Clone this repo: `git clone https://github.com/edgeimpulse/example-arduino-nano-visual-wake-words.git`
1. Open the `visual_wake_words_ei/visual_wake_words_ei.ino` sketch in the Arduino IDE
1. Install the `ei-visual-wake-words---tinymlperf-arduino-1.0.1.zip` library from the main directory of this repository in the Arduino library manager: `Sketch -> Include Library -> Add .ZIP Library...`
1. Plug your TinyML Kit into your computer
1. Select the Arduino Nano 33 BLE Sense Board from the Arduino boards manager: `Tools -> Board -> Arduino Mbed OS Boards -> Arduino Nano 33 BLE`
1. Select the port your board is connected to: `Tools -> Port` 
1. Select the **Upload** button