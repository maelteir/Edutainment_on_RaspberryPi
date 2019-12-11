# Edutainment_on_RaspberryPi

This repository contains the code of an edutainment game. The game objective is to help junior students to memorize the multiplication tables. The interaction between the students and the game is performed through the Interactive Mat. It is a low-cost physical interaction technology, where the whole body is used to provide the input to the computer application.

The target platform of the game is Raspberry Pi. The game includes two main components: game engine written in Java and position tracking module written in C and OpenGL-ES. The game engine runs on the CPU and the position tracking module is offloaded to the VideoCore GPU embedded on the Raspberry Pi to achieve real time performance.  
