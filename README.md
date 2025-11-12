# OpenCFE

OpenCFE is a piggyback computer module for your P2 Volvo that uses custom software to enhance many vehicle functions.

At the moment, this project is only compatible with Volvos built on the P2 platform. Support may eventually be added for P1 and P3 cars, but don't count on it.

Feature availability depends on model year and options, but some examples include:
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
- [x] Migrate to Mbed-CE
- [x] Begin testing basic backend system for task scheduling and subsystem control
- [ ] Basic serial CLI for testing and diagnostics (similar to Cisco IOS)
    - [x] Execute code in a way that is PSM compatible (allows system to sleep properly)
    - [x] Accept commands and call appropriate functions
    - [x] Shorthand auto-match (`caf -> caffeinate`)
    - [x] Tab completion on multiple levels
    - [x] Basic logging system with log level, module name, and message
    - [ ] Commands to filter output by log level and module
    - [ ] Synchronus logging that prevents log messages from clobbering your prompt
    - [ ] Show help on `?` key, filter based on what's already typed
    - [ ] Logging timestamp?
    - [ ] Wakeup event triggered by Serial RX pin
- [ ] Additional utility functions
    - [x] Caffeinate mode (prevent auto sleep)
    - [ ] Monitor a CAN message
    - [ ] Get and set values in persistent storage
    - [ ] Reset different parts of the software stack
- [ ] Establish CAN communications with vehicle and test simple functions
- [ ] Finish and test CAN based wakeup and shutdown controls
    - [x] Works when shorting wire to ground
    - [ ] Works when connected to an actual CAN transceiver
    - [ ] Verify functionality with Sleep Debugging Mode
- [ ] Design abstraction for different model years of vehicle (different CAN IDs, baud rates, and message data) (Current plan is avoid conditional compilation and dynamically determine the modules to be loaded at runtime based on a configuration).
- [ ] Figure out how to handle arbitration of multi-parameter D2 IO controls between different modules. (Turn Signals, Headlights, and Wipers all share one message) (Maybe use a mutex for locking, then track ownership of the mutex to determine if the mask bit should be set? We will definitely want to avoid unnecessary releases of this lock as it requires the whole message to be refreshed.)
- [ ] Create full list of planned subsystems and which ones are supported per vehicle
- [ ] Find way to deal with K-Line keepalive on 99-04 vehicles (or bypass relay)
- [ ] Determine processor specs required for production hardware revision

## License and Open Source Guarantee Statement
This project is distributed with a GNU GPLv2 License. See the LICENSE file for more information.

The OpenCFE project is committed to staying completely open-source forever. This should avoid a potential situation where support for OpenCFE is dropped and the rights get sold to a company that isn't interested in continuing development. There is very little reason for a project like this to be private, as it limits what the Volvo community can gain from the research and development efforts of this project. Additionally, it allows the community to contribute their own changes and improvements to make OpenCFE better for everyone.

# Development
When cloning the repo, be sure to either clone with `--recurse-submodules` or run the following commands if you have already cloned the repo.

```sh
git submodule init
git submodule update
```

## Setting up Build Environment
1. Ensure you have `cmake` and `ninja-build` installed.
2. Install the gcc-arm compiler from the [Arm Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) page.

To configure the repository run the following commands from the repo root directory.

```sh
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Develop -DMBED_TARGET=NUCLEO_F767ZI
```

If you use an editor or LSP that requires a `compile_commands.json` file, you can generate it by adding `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to the end if the cmake command above.

## Building
To build the software, run the following command in the `build/` directory

```sh
ninja
```

## Flashing Firmware
To flash the firmware to the hardware, connect the device to your computer and run the following command from the build directory.

```sh
ninja flash-OpenCFE
```

## Running Unit Tests

Warning: unit tests have not been migrated from platformio yet!

To run the unit tests for the included libraries, run the following command.

```sh
:
```
