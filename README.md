# Arduino Espresso Machine

Gicar box replacement for HX espresso machines with solenoid-controlled E61.

```
USE AT YOUR OWN RISK.
This project is posted for educational use only. It involves electricity, high heat, and water/steam.
I will not be held liable damages or injuries resulting from the use of this code or from copying this project.
```

* Arduino maintains water level in the boiler. Probing water level is throttled to one second to slow down electrolysis – the continuity circuit uses digital IO pin as output and an analog pin as input. Boiler doesn't refill if dispensing is in progress.
* Three SSRs control the pump, boiler refill solenoid and E61 solenoid.
* Pushbutton starts and stops dispensing by turning on the pump and E61 solenoid.
* Boiler is controlled by a pressurestat and thermo switch on a separate AC circuit, it is assumed to be always on (see TODO).
* Built for ECM Veneziano A1 (semi-auto HX machine with volumetric control).

## Resources

https://github.com/AnthonyDipilato/EspressoMachine - similar project, uses a different water level sensing mechanism and different controls

## TODO

- post hydraulic schematic and circuit diagram
- replace SSR with relays. This will allow to control both solenoids with one relay to act as a three-way valve
- turn off boiler heat when boiler water level is too low
- monitor water tank level
- add shift register to control all buttons and LEDs on the machine
- get flowmeter working for volumetric control

<img src="https://i.imgur.com/rYCZEsTl.jpg" width="40%"/>
