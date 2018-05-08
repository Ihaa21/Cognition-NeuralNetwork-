# Cognition-NeuralNetwork-

This repository contains a feed forward classifier and auto encoder for MNIST digits, implemented in C++. This program is built for visual studio with the .sln files stored in build_win32.

Build: To build the program, open a command promt and navigate to the build directory. Run build.bat and the project should be built.

Controls: In win32_cognition.cpp, you can find a define for TRAIN_AUTOENCODER and TRAIN_CLASSIFIER. Setiing either of them to 1 will make the auto encoder or classifier run respectively. The learning rates are also set near the top of the file in win32_cognition.cpp.

Program Entry Point: The main entry point for the program is found at win32_cognition.cpp.
