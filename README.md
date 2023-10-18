# Deep Dive Open Engine.
 DeepDiveOpen is an open-source custom 3D graphics engine written in C++ and OpenGL with the capability of letting
 the user define a custom game where the players are agents controlled by AI. It serves as a Artificial General Intelligence (AGI) testing ground, obtaining significant information about how the AI agent behaves when it tries to complete its task.
 Furthermore, the source code is documented and easy to understand for other researchers and developers allowing to extend the functionality of the engine

## Installation
This section will serve as a guide on installing the project that contains the engine’s code and all of its resources in order to either modify it directly or add new content such as custom defined AI games.

First of all, the following requirements should be downloaded and installed in their proper places:
* **[Visual Studio (2019)](https://visualstudio.microsoft.com/es/vs/older-downloads/)**. For opening the project that contains the engine’s code.
* **[OpenGL](https://www.khronos.org/opengl/wiki/Getting_Started#Downloading_OpenGL)**
* **[ArrayFire](https://arrayfire.com/)**. It is important to write down its installation directory as it will be used later in this document.
* **[Library stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)**. Must be installed in “\Deep dive open\code\Externalcode”, being the root directory of this address the root directory of the project. Do not remove the stb_image.cpp file found already in said directory.
* Install the following extensions of OpenGL under the "Dependencies" folder located in the rooth of the project’s directory structure.
    * **[OpenGL Extension Wrangler Library (GLEW)](https://glew.sourceforge.net/)**
    * **[GLFW](https://www.glfw.org/)**
    * **[OpenGL Mathematics (GLM](https://github.com/g-truc/glm))**

The “Dependencies” folder should look like the following image:

The GLEW folder should look similar to the following screenshot:

The GLFW folder should have a similar appearance to the picture below:

The GLM folder should be like the next figure:


Once all of this is properly done, open the .sln file found at the project’s directory.

In case the project options were not configured properly, the user needs to do the following:
1. Once the project is opened in visual studio, go to Project→Deep dive open properties→C/C++→General→Additional include directories. Then, add the
following paths:
   * Arrayfire’s installation path\v3\include(for example, in the writer’s computer this path is C:\Program Files\ArrayFire\v3\include)
   * $(SolutionDir)Dependencies\GLFW\include
   * $(SolutionDir)Dependencies\glm\gtc
   * $(SolutionDir)Dependencies\glm
   * $(SolutionDir)Dependencies\GLEW\include
   * $(SolutionDir)Dependencies\glm\gtx
2. While still being in C/C++, go to Preprocessor and add the following:
NOMINMAX;GLEW_STATIC
3. Go to C/C++→Code Generation and set the Runtime Library option to Multi-threaded (/MT) and go to C/C++→Language and set the C++ Language Standard option to ISO C++20 Standard (/std::c++20).
4. Go to Linker instead of C/C++ and in General→Additional Library Directories add the following:
   * -$(SolutionDir)Dependencies\GLEW\lib\Release\x64
   * -$(SolutionDir)Dependencies\GLFW\lib-vc2019
   * -$(AF_PATH)\v3\lib (for example, in the writer’s computer this path is C:\Program Files\ArrayFire\v3\lib)
5. In General, set the Use library dependency inputs to No.
6. In Linker→Input, add the following to Additional Dependencies:
   * glew32s.lib
   * glfw3_mt.lib
   * opengl32.li
   * User32.lib
   * Gdi32.lib
   * Shell32.lib
   * afcuda.lib

## Usage



## Contributing
 Any improvement or new functionality to the project is welcome. To contribute to the project follow these guidelines:
 * Look for previous suggestions before making a new one, as yours may be a duplicate.
 * Fork the project.
 * Create a branch.
 * Commit your changes to improve the project.
 * Push this branch to your GitHub project.
 * Open a Pull Request.
 * Discuss, and optionally continue committing.
 * Wait untill the project owner merges or closes the Pull Request.

If it is a new feature request (e.g., a new functionality), post an issue to discuss this new feature before you start coding. If the project owner approves it, assign the issue to yourself and then do the steps above.

Thank you for contributing to DeepDiveOpen. 

## License
 This project is licensed under the GPL License - see the LICENSE.md file for details. 
 Copyright © 2023
