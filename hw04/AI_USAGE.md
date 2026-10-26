I wrote all of tally.h myself based on the assignment's guidelines, then used
Claude to generate the functions in tally.c. I also used Claude to generate the
bulk of test_tally.c, events.c, and the Makefile. The README.md was largely
generated from the tally.h documentation, though I wrote the last section on
opaque types myself. The only thing that I found Claude did "wrong" was that it
initially used memmove in the insert helper function in order to shift over
values to the right in the name array. I didn't explicitly tell it not to use
memmove, but I did tell it not to use malloc, so I was surprised it tried to
use an adjacent function. More importantly, I'm not even sure using memmove is
really much better than using a backwards-iterating for loop. After that
happened, I added a ban on direct memory-interfacing functions to CLAUDE.md.