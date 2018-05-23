# NetworkingProjectSFML
### Using SFML and Box2D

[YouTube Video](https://youtu.be/jkqCUnILRnQ)

My very first try to build a multiplayer game from scratch. It's far from perfect, could use some refactoring but it works fine.

Features:
* Interpolation - game is rendering state from 100ms before current to ensure there are always packets to interpolate between
* Extrapolation - quite basic way of guessing where object should be now if there's no packets to interpolate between
* Player prediction - client knows his own input and predicts his own movement instead of waiting for packets from the server
* Delta compression - sending update packets only for objects which moved since last frame
* Compressing floats - added just to demonstrate the techinique but it's not really worth it. It's changing floats to ints when sending to save some bandwith. It's the main reason of problems with the app because of lost information.
* Congestion control - sending less packets if long RTT is detected to help when router flooding occurs
* Lag compensation - server knows about 100ms rendering delay(used for interp) and uses game state from that time to check if player was killed
