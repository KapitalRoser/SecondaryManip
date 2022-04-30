/*

All fx that call particles begin with a call to calculate the max number of particles on screen at once.
addr: 80130c94

The two mystery objects are still particle emitter nodes, but I still don't know what they're involved in. Pausing moves/deletes them.

Pausing stabilizes pattern into 57, 68, 78 that is subject to the same set of rotations as the regular fountains. Theoretically could use idle
anim to figure out what spot you're in, but you still have the problem of framerate in the pause menu.
Tested, unfortunately it doesn't resync reliably.


801303bc PE BANK -- GREEN FUNC.




R31 loops through the fountains + 2 mystery objects
0x8100FAE0,0x8101b460,0x8102D720,0x81322D60,0x8131EB80
South, then middle, then north, then two mystery objects.

Mystery object theory -- something relating to shadows????


South fountain is 23 static, then middle is 23-34-34 and north is 11-11-21
mystery is 11,11,20. I call it a pair because I can't delete them seperately.



Tentative pattern? -- 5.0 observed. 

59-49-59-49 pattern for both eights and A's. A's last 7 frames and Eights last either 6 or 9 frames, with some variance...
current igt if sevens assumed, not counting blink igt. 
roughly a 27% IGT if only sevens are allowed. (58 eights, 61 A's)
After more tests, the pattern seems to hold up. Sevens is most common

86/500 eights, 63/500 A's, a 29% combined IGT, shrinks to a 12.5% if you go for 2/3 patterns being viable. 
Not sure what the teddy performance tradeoff is. Would want to make sure ideal teddies are still quick enough. 



What decides if a pattern is 7's, 8's or A's???

#
-23


79 89 77
56 66 54

Sevens: 
56 = 11 north, 34 middle, 11 mys
66 = 21 north, 34 middle, 11 mys 
54 = 11 north, 23 middle, 20 mys

68 79 98
45 56 75

A's
45 = 11 n, 23 mi, 11 my
56 = 11 n, 34 mi, 11 my
75 = 21 n, 34 mi, 20 my

89 68 88
66 45 65

Eights:
66 = 21 n, 34 mi, 11 my
45 = 11 n, 23 mi, 11 my
65 = 11 n, 34 mi, 20 my



78 = 21 n, 23 mi, 11 my -- These 4 don't show up???
79 = 11 n, 34 mi, 11 my
65 = 11 n, 34 mi, 20 my
87 = 21 n, 23 mi, 20 my

^ that's because the special item in middle (23) is always one frame later than the one for north fountain (21)
Therefore the two special items won't meed up to create the different comboes. 
Its all about the background objects. 




STARTUP SEQUENCE

From: 0

+4 -- Load south
23
23 -- lag occurs here on Eights?
23 + 4 -- Load Middle
23 + 23
23 + 34
23 + 34 + 4 -- Load North
23 + 23 + 11
23 + 34 + 11
23 + 34 + 21
23 + 23 + 11 + 4 -- load Michael and Jovi -- neat.
23 + 34 + 11
23 + 34 + 21
23 + 23 + 11 + 4 -- Michael moved to correct position.
23 + 34 + 11
23 + 34 + 21

10??? -- ???????? WEIRD -- Set should be 260 calls, total is 383 instead. Diff of 123 calls?
73??? --By now, camera is moved. 
265???
35???

23 + 34 + 11
23 + 34 + 21
23 + 23 + 11
23 + 34 + 11
0
136 -- + 1 call overall, (CC + CB = 135)

23 + 34 + 11
23 + 34 + 21
23 + 23 + 11
23 + 34 + 11
23 + 34 + 21

0
126??? (CC + CA + 1) up one call overall. --- On eights it does the CC first then 0 here and catches up next frame. Still +1 overall.

23 + 34 + 21 
23 + 23 + 11
23 + 34 + 11
23 + 34 + 21

THIS IS WHERE STUFF REALLY DIVERGES FOR GOOD.


ON SEVENS:

0
147 -- CA + CB + 1

23 + 23 + 11

0
69
135 -- CA + CB + CC + 1

0
147 -- CA + CB + 1

23 + 23 + 11 + (18?)




ON A's 
0
147 -- CA + CB + 1

23 + 23 + 11
23 + 34 + 11
23 + 34 + 21
23 + 23 + 11
23 + 34 + 11
23 + 34 + 21

0 -- Should be CC -- weird, are calls different here?
58 -- CC + 1 -- Should be CA
146 -- CA + CB -- Should be CB -- +1 f overall

0
126 -- CC + CA + 1

23 + 34 + 21 + (18?)


ON EIGHTS

23 + 34 + 11

0
136 -- CB + CC + 1

23 + 34 + 11

0
79 -- CB + 1 -- also: 23 + 34 + 21 + 1
125 -- CC + CA

0
135 -- CB + CC + 1

23 + 34 + 11 + (18?) ca + 18





Looks like format is

X regular

+1 call over 2 frames lag

X Regular

+1 call over 3 frames lag

+1 call over 2 frames (immediately after previous lag)

+18

Begin.

For Sevens, its 0 before, +1 after
For A's its 0 before, 6 after
For Eights its 1 before, 1 after 

And that makes the difference.

Back through random trial and error I found this but indirectly

If 7 is net +1,
then A's is +6 and Eights is +2

If we adjust and set 7 to 0,
then A's is +5 and Eights is +1

(original RNG Filter has A's as 5 despite 7's being 1, so idk what's going on there? Original is wrong, 
listen to the rest of the line, the 0 1 and 5, not the 1,2,5 at the start.)
If there were any failures then that would be why.)


Taking this a step further:

Ex. 7's begins the at 48
A's begin at 53
Eights begin at 49



 IN SHORT. IF the +18 frame starts on a CC, you get 7's. If it starts on a CB you get A's and if it starts on a CA you get Eights.
 Now if +6 gets you A's, is it possible that a +0, or a +3 would get that as well? How much variance is involved in lead pattern determination?
*/