# Arduino Espresso Machine

Gicar box replacement for HX espresso machines with solenoid-controlled E61.

*IN PROGRESS* - please don't use this code for anything other than reference, I have not tested it enough. 

```
USE AT YOUR OWN RISK.
This project is posted for educational use only. It involves electricity, high heat, and water/steam.
I will not be held liable damages or injuries resulting from the use of this code or from copying this project. Please make sure you understand the hydraulics and electronics of your machine in detail before adapting this code.
```

* Built for ECM Veneziano A1 (semi-auto HX machine). The inputs are: 4 buttons and 4 LEDs for volumetric control, chasis pin (to sense continuity from boiler water level probe and water tank probe), flowmeter. The outputs are: boiler switch, pump switch, boiler fill solenoid and E61 solenoid (both on AC current).
* Arduino maintains water level in the boiler. If the water level is low, the boiler stays off to prevent heating element damage. Additionally, the boiler is controlled by a pressurestat and thermo switch as is normal for every HX espresso machine.
* Probing of water level in boiler/tank is throttled to one second to slow down electrolysis – the continuity circuit uses two digital IO pin as output and an analog pin as input.
* Low tank water level is indicated by flashing LEDs.
* Three relays control the pump, boiler, boiler refill solenoid and E61 solenoid. One of these relays controls two solenoids – this determines which path the flow takes, either through E61 or to fill the boiler.
* Pushbutton starts and stops dispensing by turning on the pump and E61 solenoid.
* Boiler never refills if dispensing is in progress.
* Code-wise, the `loop()` function has three phases: it first gathers inputs (buttons, sensors), then determines what is the next active state (`IDLE`, `FILLING_BOILER`, `DISPENSING`, `NEEDS_TANK_REFILL`) and then persists that state by controlling outputs.
* You can replace `main.cpp` with `raw.cpp` when first debugging the machine. This is a very naive interface to control the machine, it can be used to verify that the hardware works before sorting out the software. For example you can turn off the boiler while testing the pump only.
* Make sure your board connections are solid if your pump is rattling and test that the machine is grounded properly. The worst case scenario could be overfilling the boiler which will spray hot water or steam inside the chasis, causing shorts, damaging circuitry and electrocuting user.

## TODO

- Improve water level sensor. See [this thread on Arduino forums](https://forum.arduino.cc/index.php?topic=585723.0). The sensing circuit should be isolated from Arduino with optocouplers. It should be shielded from AC noise. It could also use AC to prevent electrolysis.
- I ran into an intermittent issue where the pump and solenoid relays couldn't switch at the same time. This only happened when the boiler, pump and solenoids were connected to relays. In `raw.cpp` I added a small delay between switches which hotfixed the issue so I'll look into it later.
- Get flowmeter working for volumetric control

## Resources

https://github.com/AnthonyDipilato/EspressoMachine - similar project, uses a different water level sensing mechanism and different controls

**Wiring diagram from ECM Giotto**   
<img src="https://www.home-barista.com/forums/userpix/18943_wiring_diagram_-_rocket-giotto-premium-plus-v2-sp-2014.jpg" width="40%"/>

**Water flow path with parts cut in half**
https://www.youtube.com/watch?v=xmSHw1Fudpg
https://www.youtube.com/watch?v=E4p4kfw5BLI
https://www.youtube.com/watch?v=E4p4kfw5BLI

**Prototype board**   

<img src="https://i.imgur.com/JBID1xi.png" width="40%"/>

**ECM Veneziano**   

<img src="https://i.imgur.com/rYCZEsTl.jpg" width="40%"/>
