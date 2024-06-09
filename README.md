# rulaura
_screen **rul**er by **aura**_

Terrible screen ruler that uses canny edge detection to measure horizontal and vertical spans in pixels.
Vaguely based on the PowerToys screen measuring tooling, but scratched together in a few hours.
Slightly cleaned up (this was all one monolithic main.cpp at first) to be thrown on github.

## Building

```
$ ./build.sh
```

## Usage

```
$ ./main
```

# Requirements

Obviously you'll need the dependencies to build (opencv, libcairo, X11).

Needs a compositor to work correctly, otherwise the screen will be obscured by the 'background' of the overlay. (e.g. `picom`)

# Demonstration

![image](https://github.com/nhammond129/rulaura/assets/2228057/5e60aec0-e36e-4c33-8d58-eb281fb2fb38)
