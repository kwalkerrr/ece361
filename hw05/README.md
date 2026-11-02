# Transition Table
|        | E_RIGHT          | E_WRONG          | E_CLOSE          | E_TIMEOUT       |
|--------|------------------|------------------|------------------|-----------------|
| LOCKED | GOT1, A_NONE     | LOCKED, A_BEEP   | LOCKED, A_NONE   | LOCKED, A_LOCK  |
| GOT1   | GOT2, A_NONE     | LOCKED, A_BEEP   | GOT1, A_NONE     | LOCKED, A_LOCK  |
| GOT2   | OPEN, A_UNLOCK   | LOCKED, A_BEEP   | GOT2, A_NONE     | LOCKED, A_LOCK  |
| OPEN   | OPEN, A_NONE     | OPEN, A_NONE     | LOCKED, A_LOCK   | LOCKED, A_LOCK  |