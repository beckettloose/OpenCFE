# OpenCFE Features

Below is a list of all planned features and their development status. Applicable features also describe which vehicles are compatible (will be done in future, currently only have 2004 V70R to test on)

# Doors and Locks

## Auto-Locking Doors
- Development Status: NOT STARTED
- Notes:
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - Automatically lock doors above certain vehicle speed
    - Unlock doors when front doors are opened or shifted in to park or handbrake engaged

# Mirrors

## Auto-Folding Mirrors
- Development Status: BLOCKED
- Notes: Need a car with electronic mirror folding to test on
- Compatability:
    - Platforms: P2 (2005-)
    - Required Options: Power-Folding Mirrors
- Features:
    - Fold and unfold mirrors when vehicle is locked and unlocked

## Auto Mirror Tilt
- Development status: NOT STARTED
- Notes: Uses driver preset 3 trick
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Power mirror adjustments (not sure if all p2s had this, may remove in the future)
- Features:
    - Tilt mirrors down when engaging reverse gear

# Windows and Wipers

## Intermittent wipers on rain sensor cars
- Development Status: NOT STARTED
- Notes: May not implement because you can just unplug the rain sensor and the car will default to intermittent wipers. Will be integrated in to the Enhanced Wipers subsystem if we want it.
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - Time based intermittent wipers on cars with a rain sensor. (because the rain sensor on -04 cars is apparently not very good)

## Enhanced Windshield Wipers
- Development Status: NOT STARTED
- Notes:
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - Delayed extra wipe after windshield washing to avoid striping

## Enahnced Power Windows
- Development Status: NOT STARTED
- Notes:
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Power Sunroof (to use sunroof functionality)
- Features:
    - Use key fob to open and close windows/sunroof
    - Double press rear window switches to activate auto-up/down (still respects window lock state)

## Auto Glass Heating
- Development Status: NOT STARTED
- Notes: May move to climate controls category as this function is managed by the CCM
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Heated mirror and rear window glass (again, I'm not sure if this was an option or not)
- Features:
    - Automatically enable mirror and rear glass heating based on outside temperature

# Exterior Lighting

## "Flash-to-pass" Turn Signals
- Development Status: NOT STARTED
- Notes: I don't think this makes sense to roll in to any other subsystem, so keeping it separate for now
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - When turn signal stalk is activated for a short time, blink signal 3 times

## Enhanced Headlight Control
- Development Status: NOT STARTED
- Notes: This part is pretty complicated, need to make sure we get it right because I don't want to rewrite anything.
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Daytime Running Lights (for DRL options), Fog Lights (for fog light options), Approach Lighting (for approach lighting settings, not sure if this was an option), Automatic Headlights (for any option that requires exterior brightness measurements, not sure if this was an option)
- Features:
    - Configure auto-headlight settings
        - Change brightness threshold for automatic headlights
        - Turn headlights on or show dashboard indicator as reminder (indicator can optionally be canceled if headlights are manually switched on)
        - Use dashboard indicator to show when auto headlights are active (using the trailer turn signal indicator, beacuse it vaguely resembles a modern headlight indicator)
        - Use parking lights as DRLs
        - Activate with Approach Lighting
    - Configure DRL Settings
        - Switch between DRL Modes (Always off, Auto, Always on, Follow High Beam)
        - For auto mode, use threshold from automatic headlights
        - Activate with Approach Lighting
    - Configure Fog Light Settings
        - Activate with high beams (Always or when switch is active)
        - Activate with reverse lights (front and/or rear, always or only when dark)
        - Activate with Approach Lighting
    - Configure Approach Lighting Activation
        - Trigger with normal button
        - Trigger when unlock pressed
        - Trigger when key removed from ignition
        - Trigger when door opened
        - Only activate if dark out (also follow auto-headlight threshold probably)

# Climate Controls

## Auto-Heated Seats
- Development Status: NOT STARTED
- Notes:
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Heated Front Seats
- Features:
    - Automatically turn on heated seats when below temperature threshold
    - Separate temp thresholds for low and high heat settings
    - Enable passenger heated seat if seatbelt is buckled (Not sure if these seats have weight sensors)

## Auto Windshield Defrost
- Development Status: NOT STARTED
- Notes:
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - Automatically turn on windshield defrost when below temperature threshold

# Instruments

## Gauge Sweep
- Development Status: NOT STARTED
- Notes: This is definitely possible because I did it with a DHA script.
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - Sweep gauge needles from min to max values at ignition-on (really just a party trick, but still cool)

## Enhanced DIM LCD
- Development Status: NOT STARTED
- Notes: Figured out how to write to LCD with ascii text, but haven't tested the OpenCFE implementation yet. Already established some basic logic for welcome message to validate subsystem architecture. Still needs to be rolled in to the final Enhanced LCD subsystem.
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Trip Computer (for menus, uses reset button to toggle right now)
- Features:
    - Display welcome and goodbye messages
    - Menus controlled by power mirror switch (need to figure out alternative to using trip reset button though)
    - Display various performance data
    - Read and clear DTCs
    - Display low fuel warning

## Enhanced Low Fuel Warning
- Development Status: NOT STARTED
- Notes: need to see if we can overwrite gas warning light, and get trip computer DTE estimate
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Trip Computer (for triggering warning based on distance to empty)
- Features:
    - Change threshold for low-fuel warning (physical level or DTE calculation)
    - Don't override gas warning to off state as fallback in case we screw up the math/logic
    - Play gong when warning is triggered

## Auto Time Set
- Development Status: NOT STARTED
- Notes: Would need to add GPS reciever and antenna, as well as dealing with time zones
- Compatability:
    - Platforms: P2 (2004)
- Features:
    - Use GPS Clock to automatically set DIM time

# Misc

## Enhanced Cruise Control
- Development Status: NOT STARTED
- Notes:
- Compatability:
    - Platforms: P2 (2004)
    - Required Options: Cruise Control
- Features:
    - Automatically enable cruise control when ignition is turned on
