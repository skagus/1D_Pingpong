# 1D_Pingpong
1 dimensional LED pingpong with ws2811 LED bar and AVR.

On one stripe of LED line with a button at the each end, the light runs in the stripe. 
If you push the button when the light in close enough the button, the direction of light will change.
else (miss the light) you lose and opposite gain a point.

## Program status
It programmed with timer based job scheduler + event based keyboard input.
It calls the function as the state of task.
and the tasks are 
 - Game task to desribe game play.
 - LED task that handle LEDs in stripe.
 
