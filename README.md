<div align="center">

# Psycho-Pass Dominator

### Arduino-powered transforming cosplay prop

[![Arduino Nano](https://img.shields.io/badge/Arduino-Nano-00979D?logo=arduino&logoColor=white)](https://docs.arduino.cc/hardware/nano/)
![Servos](https://img.shields.io/badge/Servos-3-00B8D9)
![DFPlayer Mini](https://img.shields.io/badge/Audio-DFPlayer_Mini-7B61FF)

**English** · [Русский](README_RU.md)

![Dominator animation](assets/dominator-anime.gif)

</div>

## About

A tested and fully working Arduino Nano sketch for a Psycho-Pass Dominator cosplay prop. Three servos animate the transformation mechanism, while a DFPlayer Mini provides startup, transformation, Paralyzer, and Lethal Eliminator sounds.

> The code and wiring have been tested on the completed prop.

## Features

- Smooth, non-blocking movement of three servos
- Trigger-button debounce
- Startup lock while the opening sound plays
- Four sound effects from a microSD card
- Serial commands for safe bench testing
- Operation without audio if the DFPlayer is not detected

<div align="center">

![Finished Dominator prop](assets/dominator-prop.gif)

</div>

## Hardware

| Component | Quantity | Notes |
| --- | :---: | --- |
| Arduino Nano | 1 | Main controller |
| Servo | 3 | Choose torque to suit the mechanism |
| DFPlayer Mini | 1 | Audio playback from microSD |
| Speaker and amplifier | 1 | As shown in the wiring diagram |
| Momentary button | 1 | Trigger input |
| Regulated power supply | 1 | Must handle servo peak current |
| Resistor | 1 | About 1 kΩ for Arduino TX → DFPlayer RX |

## Wiring

| Function | Arduino pin |
| --- | :---: |
| Trigger button | D4 → GND |
| Servo 1 signal | D7 |
| Servo 2 signal | D5 |
| Servo 3 signal | D6 |
| DFPlayer TX → Arduino RX | A1 |
| Arduino TX → DFPlayer RX | A2 through ~1 kΩ |

<div align="center">

![Wiring and soldering diagram](assets/wiring-diagram.png)

</div>

> Do not power the servos from the Nano's 5 V pin. Use a correctly rated regulated supply and connect all grounds together. Use protected Li-ion cells and a suitable BMS; avoid short circuits, overcharging, and deep discharge.

## Software

- `Servo` — included with Arduino IDE
- `SoftwareSerial` — included with Arduino IDE
- `DFRobotDFPlayerMini` — install through Library Manager

## Audio files

The audio files are stored in the repository's [`Mp3`](Mp3) folder. Format the microSD card as FAT32 and copy them into a lowercase `/mp3` folder on the card, as required by `playMp3Folder()`:

| File | Sound |
| :---: | --- |
| `0001.mp3` | Startup |
| `0002.mp3` | Opening/transformation |
| `0003.mp3` | Paralyzer |
| `0004.mp3` | Lethal Eliminator shot |
| `0005.mp3` | Additional sound (not used by the current sketch) |

## Upload and calibration

1. Open [`dominator_gun_psychopass.ino`](dominator_gun_psychopass.ino) in Arduino IDE.
2. Select the correct Arduino Nano board, processor, and port.
3. Install `DFRobotDFPlayerMini`, then compile and upload the sketch.
4. Disconnect the servo linkages and calibrate `CLOSED_ANGLES` and `OPEN_ANGLES` before operating the mechanism.
5. Set `STARTUP_AUDIO_MS` to the real duration of `0001.mp3`.

### Bench controls

Open Serial Monitor at **9600 baud**:

| Command | Action |
| :---: | --- |
| `p` | Close / Paralyzer mode |
| `l` | Open / Lethal Eliminator mode |
| `f` | Play the current firing sound |

## Safety

This is an electronic cosplay prop, not a weapon. Follow local rules for replica props, use an orange tip or other required markings, and never carry or display it where it could be mistaken for a real weapon.
