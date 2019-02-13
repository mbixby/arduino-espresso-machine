# Arduino Espresso Machine

Gicar box replacement for HX espresso machines with solenoid-controlled E61.

```
USE AT YOUR OWN RISK.
This project is posted for educational use only. It involves electricity, high heat, and water/steam.
I will not be held liable damages or injuries resulting from the use of this code or from copying this project.
```

* Arduino maintains water level in the boiler. Probing water level is throttled to one second to slow down electrolysis – the continuity circuit uses digital IO pin as output and an analog pin as input. Boiler doesn't refill if dispensing is in progress.
* Three relays control the pump, boiler, boiler refill solenoid and E61 solenoid.
* Pushbutton starts and stops dispensing by turning on the pump and E61 solenoid.
* Low tank water level is indicated by flashing LEDs.
* Boiler is controlled by a pressurestat and thermo switch on a separate AC circuit. It is turned off when the boiler water level is low.
* Built for ECM Veneziano A1 (semi-auto HX machine with volumetric control).

## TODO

- post hydraulic schematic
- get flowmeter working for volumetric control

## Resources

https://github.com/AnthonyDipilato/EspressoMachine - similar project, uses a different water level sensing mechanism and different controls

**Wiring diagram from ECM Giotto**   
<img src="https://www.home-barista.com/forums/userpix/18943_wiring_diagram_-_rocket-giotto-premium-plus-v2-sp-2014.jpg" width="40%"/>

**Prototype board**   

<img src="https://i.imgur.com/JBID1xi.png" width="40%"/>

**ECM Veneziano**   

<img src="https://i.imgur.com/rYCZEsTl.jpg" width="40%"/>
