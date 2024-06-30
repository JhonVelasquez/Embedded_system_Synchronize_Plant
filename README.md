# Embedded_system_Synchronize_Plant
Project of a embedded system  that synchronizes signals in a plant. There is electronic (PCB) and software design (FreeRTOS for STM32F7).

## Required Embedded System - Plant:
The embedded system in the pictures is what is needed for interacting with the Plant, it must send pulses and TCP messages in a specific pattern every time there is a new object incoming the plant.
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Diagrams/Electronic_system_requested_desgin.drawio.png)

## HARDWARE:

The following PCB collects all the electronics part of the dessigned Embedded system
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Hardware_PCB.JPEG)

Diagram of the electronics sections. The PCB was made based on it.
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Diagrams/PCB_Electronic/Electronic_diagram_general.png)

Main sub-systems of the PCB
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Diagrams/PCB_Electronic/PCB_sub_systems.drawio.png)

## SOFTWARE:

RTOS Architechture:
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Software_RTOS.png)

CLI command structure:
The CLI for interfacing the microcontroller should have the following structure.
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Diagrams/Command_structure.png)

## Integration connections:
![alt text](https://github.com/JhonVelasquez/Embedded_system_Synchronize_Plant/blob/main/Diagrams/PCB_Plant.drawio.png)

