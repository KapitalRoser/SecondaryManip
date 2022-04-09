/*

All fx that call particles begin with a call to calculate the max number of particles on screen at once.
addr: 80130c94

The two mystery objects are still particle emitter nodes, but I still don't know what they're involved in. Pausing moves/deletes them.

Pausing stabilizes pattern into 57, 68, 78 that is subject to the same set of rotations as the regular fountains. Theoretically could use idle
anim to figure out what spot you're in, but you still have the problem of framerate in the pause menu.
Tested, unfortunately it doesn't resync reliably.




*/