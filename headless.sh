#!/bin/bash

# Demonstrates how to create a headless display using xvfb.

# Create the display:
Xvfb :100 -ac &
PID1=$!
export DISPLAY=:100.0

# Run the application that needs the display:
xterm &
PID2=$!

# Kill the application:
kill -9 $PID2

# Remove the display:
kill -9 $PID1