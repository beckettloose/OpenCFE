# OpenCFE

OpenCFE is a piggyback module for your P2 Volvo's CEM that enables additional functionality by reading and writing mesages on the vehicle's CANbus network.

Feature availability depends on platform, model year, and options, but some examples include:
- Automatically close windows when locking car
- Automatically turn on heated seats in cold conditions
- Show welcome and goodbye messages on DIM LCD
- Gauge sweep at ignition on
- Enable "flash-to-pass" turn signals
- Allow fog lights with high beams

## Project Status (Software Only)
- [x] Begin testing basic backend system for task scheduling and subsystem control
- [ ] Establish CAN communications with vehicle and test simple functions
- [ ] Finish and test CAN based wakeup and shutdown controls
- [ ] Design abstraction for different model years of vehicle (for CAN IDs and baudrates)
- [ ] Create full list of planned subsystems and which ones are supported per vehicle
- [ ] Find way to deal with K-Line keepalive on 99-04 vehicles (or bypass relay)
- [ ] Determine processor specs required for production hardware revision

## License and Open Source Guarantee Statement
This project is distributed with a GNU GPLv2 License. See the LICENSE file for more information.

The OpenCFE project is built to embrace the ideas of open source hardware and software to avoid situations with similar products in the past where support was dropped and rights were sold to a company that wasn't interested in continuing development. This limits what the Volvo community can gain by making the research, hardware, and software behind them private. This project is committed to staying fully open source for as long as it exists, and will not be sold off or made private.
