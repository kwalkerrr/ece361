/*
This module handles all of the reading from the sensors. Each reading_t
measurement includes a timestamp (Unix-style), a temperature reading (degrees
Celsius), and a humidity reading (%RH). The latter two readings are rounded to
the nearest 0.1. This module does not print anything.
*/