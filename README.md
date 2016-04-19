# beep
To my astonishment, there doesn't seem to be a port of the infamous
[beep](http://github.com/johnath/beep/) command for the * BSDs.

This is a beep implementation using `wscons(4)`, meaning it should work under
OpenBSD as well as NetBSD. It aims to be 100% compatible with the original beep
command, though it is still in early stages. 

Currently only the -r, -l, and -f options are supported, however it is good
enough to [play the piano](https://github.com/watsyurname529/linuxbeeppiano).
