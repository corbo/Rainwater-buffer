# Rainwater-buffer

Submitted project for https://hackathon.iot2tangle.io/

[See also the Wiki](Home)

## Problem:
    Flooding in urbanized areas in case of heavy rainfall
    
## (Possible) solution:
    Controll, private, rainbarrels by emptying them before another heavy rain shower is expected in order to create a buffer for rainwater. The collective buffers will help prevent the new fallen rainwater to overload the sewer infrastructure.

## Operation:
    Waterlevel in the rainbarrel will be monitored in order to determine it's capacity, fillingstate and degree of rainfall.
    The organisation in charge of the cities sewer can remotely open selected valves to drain the filled rainwater barrels in a controlled fashion.
    
    The waterlevels and capacities are transmitted to the tangle making that information available for the sewer organisation.
    When expecting heavy rainfall in it's area the sewer organisation can select barrels and send a drain command to them, possibly with an accompanying micro payment for the services rendered by the person who installed such a barrel.

## Electronic components
    An ESP32 microcontroller is equiped with a SN-SR04T waterproof ultrasonic distance sensor 
    https://nl.aliexpress.com/item/1005001304572844.html The sensor is mounted in the lid of the rainwaterbarrel facing down measuring the distance to the water. The distance is than converted to a volume.
    A 12V valve https://nl.aliexpress.com/item/4001217800444.html is placed between a feeder hose in the barrel and a drain hose ending in the sewer. A MOSFET controlled by the ESP32 is switching the 12V power for opening the valve.
    
