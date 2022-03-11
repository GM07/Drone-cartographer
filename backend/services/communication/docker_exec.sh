#!/bin/bash

# Kill existing argos processes
docker exec -d embedded sh -c "pkill argos"
docker exec -d embedded sh -c "argos3 -c crazyflie_sensing.argos"

# We must wait 0.5 second to make sure that the argos process has time to die if needed
sleep 0.5

# Check if argos process is still running
isRunning=$(docker exec -t embedded sh -c "pgrep argos")

# Exit based on success or not
if [ ${#isRunning} -ge 1 ]; then exit 0;
else exit 1;
fi

