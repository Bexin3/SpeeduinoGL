# SpeeduinoGL
A work in progress library with graphical functions optimised for arm cortex m7. Tested on GIGA R1, with rasterization writing to registers between two lines, and basic shapes (triangles, circles and quadrilaterals) as well as Fill Screen, but also transfer of one image to another with zoom shift and rotate transformations. Support for better examples and documentation are in progress. The camera trasnfer takes about 30ms at 5x zoom, and while its slower at lower zoomes than each display pixel reading a camera ones, it progressively gets faster.
