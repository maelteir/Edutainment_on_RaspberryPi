# Edutainment_on_RaspberryPi

This repository contains the source code of an edutainment game. The game objective is to help junior students to memorize the multiplication tables. The interaction between the students and the game is performed through the Interactive Mat. It is a low-cost physical interaction technology, where the whole body is used to provide the input to the computer application.

The target platform of the game is Raspberry Pi. The game includes two main components: game engine written in Java and position tracking module written in C and OpenGL-ES. The game engine runs on the CPU and the position tracking module is offloaded to the VideoCore GPU embedded on the Raspberry Pi to achieve real-time performance.  

This code requires Raspberry Pi Camera https://projects.raspberrypi.org/en/projects/getting-started-with-picamera to be installed and probably configured, however with simple tweaks the code can run using normal webcam.

## Prerequisite libraries:
1. OpenCV Version 2.4.9
2. VideoCore Broadcom APIs from https://github.com/raspberrypi/firmware
3. Mysql
4. cmake Version 2.8 is the minimum required

## Compilation and running instructions
  1. Run `cmake` to generate the Makefile of the position tracking module 
  2. Run `make` to compile the position tracking module
  3. Run the executable file `./picamdemo`. picamdemo in addition to running the position tracking module, it runs the game engine `MFGame-pipe-2.jar` which communicates with the tracker using pipes interface. 
  4. For the game to run probably, a simple grid drawn in a paper should be located in the field of view of the camera. To select an answer, any object should be inserted in a block within the grid corresponding to the right answer.
  
 ## Hint:
  A better performance can be achieved when using sockets interprocess communication mechanism instead of pipes. To run the sockets version of the game, update the CMakeLists.txt to use picam-sockets.cpp instead of picam.cpp. Before running the executable file picamdemo, open a terminal and run the socket version of the game using `java -jar ./MFGame-orig-skt.jar`  

For more documentation, refer to our journal paper.
