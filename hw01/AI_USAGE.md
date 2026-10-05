# AI Usage Disclosure
I consulted Claude lightly throughout the development of bits.c and status.c,
but most of that code was written myself. My main usage of AI was using Claude
Code to develop the testing files and the Makefile. I also used Claude to
create the README.md file, but its information was sourced primarily from the
function documentation in the header files (which I wrote entirely myself). I
reviewed all the testing code carefully until I felt I understood it before
trusting that the tests'successes were valid. I also had it help me with
producing Doxygen-style header file documentation (more for the hover-comments
in VS Code than for actual documentation files), though in that regard it ended
up making a mistake by not correctly leaving a blank line between bullet points
for VS Code's markdown documentation interpreter. So far as I was able to tell,
the actual code it produced worked as intended.