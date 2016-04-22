# beep
To my astonishment, there doesn't seem to be a port of the infamous
[beep](http://github.com/johnath/beep/) command for the \*BSDs.

This is a re-implementation of the beep command using `wscons(4)`, meaning it
should work under OpenBSD as well as NetBSD. It aims to be 100% compatible with
the original, so if you find any behavioral differences, please open an issue
or submit a pull request.

All options from the original, both short and long, have been implemented. This
means you can [play the piano](https://github.com/watsyurname529/linuxbeeppiano).

License is 3-clause BSD.
