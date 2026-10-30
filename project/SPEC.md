# States
Each state is represented in the code by an enumerated data type with the prefix
"STATE_". The prefix is omitted here for the sake of readability.
- DISARMED
  - No intrusion alarm
  - Initial state
- ARMING
  - Exit delay running (30 ticks)
- ARMED
  - Alarm primed; watching door contact
- ENTRY_DELAY
  - Door opened while panel was armed
  - Entry delay running (20 ticks)
- ALARM
  - Siren on

# Events
Each event is represented in the code by an enumerated data type with the prefix
"EVENT_". The prefix is omitted here for the sake of readability.
- TIMEOUT
  - A timed state's time has run out (i.e. ARMING or ENTRY_DELAY)
- ARM
  - Rising edge detected on ARM switch
- DOOR_OPEN
  - Rising edge detected on DOOR switch
- CODE
  - Rising edge detected on CODE switch
- HEAT
  - Temperature sensor reads >= 57.0 degrees Celsius