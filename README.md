# OpenCFE

OpenCFE is a piggyback module for your P2 Volvo's CEM that enables additional functionality by reading and writing mesages on the vehicle's CANbus network.

Feature availability depends on platform, model year, and options, but some examples include:
- Automatically close windows when locking car
- Automatically turn on heated seats in cold conditions
- Show welcome and goodbye messages on DIM LCD
- Show performance and diagnostic data on DIM LCD (No need to use DiCE box just to read and clear DTCs!)
- Gauge sweep at ignition on
- Change 'low fuel' warning threshold (And add warning message on LCD or trigger gong sound)
- Enable "flash-to-pass" turn signals
- Show indication of automatic headlight status (or headlight reminder)
- Change dealer-adjustable settings without VIDA (Auto headlights, DRL mode, etc.)
- Allow fog lights with high beams (Without changing CEM carconfig)

## Project Status (Software Only)
- [x] Begin testing basic backend system for task scheduling and subsystem control
- [ ] Basic serial CLI for testing and diagnostics (with features like Cisco IOS-style synchronus logging)
- [ ] Establish CAN communications with vehicle and test simple functions
- [ ] Finish and test CAN based wakeup and shutdown controls (Wakes up when pin is pulled low by test lead, haven't tried with CAN transciever yet, Want to enable sleep debugging to see if we are actually sleeping)
- [ ] Design abstraction for different model years of vehicle (for CAN IDs and baudrates) (Need to determine if this will be implemented with conditional compilation or automatic detection at runtime)
- [ ] Create full list of planned subsystems and which ones are supported per vehicle
- [ ] Find way to deal with K-Line keepalive on 99-04 vehicles (or bypass relay)
- [ ] Determine processor specs required for production hardware revision
- [ ] Migrate to Mbed-CE (Waiting for platformio to support it first, but old Mbed is approaching EoL)

## License and Open Source Guarantee Statement
This project is distributed with a GNU GPLv2 License. See the LICENSE file for more information.

The OpenCFE project is built to embrace the ideas of open source hardware and software to avoid situations with similar products in the past where support was dropped and rights were sold to a company that wasn't interested in continuing development. This limits what the Volvo community can gain by making the research, hardware, and software behind them private. This project is committed to staying fully open source for as long as it exists, and will not be sold off or made private.

# Development

## Setting up Build Environment
1. Install PlatformIO (Using Python 3.11 for Mbed compatability)
2. Clone this repository to a suitable location

## Building
To build the software, run the following command.

```sh
pio run
```

## Generating `compile_commands.json`
Some editors like Neovim need a file called `compile_commands.json` to tell the LSP how to build the program. To generate this file, run the following command.

```sh
pio run -t compiledb
```

## Flashing Firmware
To flash the firmware to the hardware, connect the device to your computer and run the following command.

```sh
pio run -t upload
```

## Running Unit Tests
To run the unit tests for the included libraries, run the following command.

```sh
pio test -e native
```
