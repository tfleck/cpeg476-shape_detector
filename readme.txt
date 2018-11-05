# Shape Detector
This project identifies shapes in a provided image, and highlights them with a border in the output image

Dependencies:
OpenCV - sudo apt-get install opencv (or build from source)

Usage:
./shapes [options]     Outputs image with specified shape highlighted
Options:
-i <filename>          Specify input filename (32 character max)
-o <filename>          Specify output filename (32 character max)
-c <string>            Specify color for detected shape outline in hex (no hash sign), 'FF0000' is default
-s <string>            Specify shape to look for, 'square' is default
                       Valid Shapes: triangle, square, rectangle, rhombus, parallelogram, trapezoid, kite, quadrilateral pentagon, hexagon
