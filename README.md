# Description of Operation

## Overview
_text description of the operation of the final system_

## Block Diagram
_block diagram of the overall system_
```mermaid
flowchart TD
launchpad(TI Launchpad)
rslk_motor(RSLK Motors)
rslk_encoder(RSLK Encoders)
motor_driver(Motor Driver)
stepper_driver(Stepper Driver)
shooter_motor(Shooter Motor)
feeder_motor(Feeder Motor)
shooter_encoder(Shooter Encoder)
feeder_encoder(Feeder Encoder)
ir_prox(IR Proximity Sensor)
ultra(Ultrasonic)
ir1(IR Beacon Sensor 1)
ir2(IR Beacon Sensor 2)
ir3(IR Beacon Sensor 3)

subgraph ultrasonic
ultra
end
ultra-->launchpad

subgraph ir
ir1
ir2
ir3
end
ir1-->launchpad
ir2-->launchpad
ir3-->launchpad

subgraph motion
rslk_motor
rslk_encoder
end
launchpad-->rslk_motor
rslk_encoder-->launchpad
rslk_encoder-.-rslk_motor

subgraph turret
stepper_driver
stepper_motor
end
launchpad-->stepper_driver
stepper_driver-->stepper_motor

subgraph shooter
motor_driver
shooter_motor
feeder_motor
shooter_encoder
feeder_encoder
ir_prox
end
launchpad-->motor_driver
motor_driver-->shooter_motor
motor_driver-->feeder_motor
shooter_motor-.-shooter_encoder
feeder_motor-.-feeder_encoder
shooter_encoder-->launchpad
feeder_encoder-->launchpad
ir_prox-->launchpad
```

## State Machine

## Schematic