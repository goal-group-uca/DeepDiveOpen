# GAGI: Game Engine for Artificial General Intelligence Experimentation
 GAGI is an open-source custom 3D graphics engine written in C++ and OpenGL with the capability of letting the user define a custom game where the players are agents controlled by Artificial Intelligence (AI). It serves as a Artificial General Intelligence (AGI) testing ground, obtaining significant information about how the AI agent behaves when it tries to complete its task. Furthermore, the source code is documented and easy to understand for other researchers and developers allowing to extend the functionality of the engine

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
    * **[OpenGL Mathematics (GLM)](https://github.com/g-truc/glm)**

The “Dependencies” folder should look like the following image:

<img src='/imgs/install_deepdiveopen1.png'>

The GLEW folder should look similar to the following screenshot:

<img src='/imgs/install_deepdiveopen2.png'>

The GLFW folder should have a similar appearance to the picture below:

<img src='/imgs/install_deepdiveopen3.png'>

The GLM folder should be like the next figure:

<img src='/imgs/install_deepdiveopen4.png'>

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

This section will serve as a guide of how to access all the aspects of the project and the controls of every mode that the engine has to offer. It assumes that the project was configured successfully. In consequence, refer to the previous section if you have not installed the engine.

### Start engine
In order to start the engine, if the user wishes to **launch it with its Visual Studio project opened**, it can be done by pressing the F5 key. That will compile the project and launch it with the possibility of adding breakpoint 
and other debug utilities offered by the IDE.

On the other hand, if the user desires to **execute the engine without using Visual Studio**, then the user must navigate (assuming that the address starts from the project’s root directory) to /x64 (or the type of system that the project was compiled for by the user) and then, in the “Debug” directory, make sure that the directories “AIData”, “records”, Resources” and “saves” are all updated (as, for example, if the shader programs are missing, the engine will produce an error when attempting to load them). Finally, in order to start the engine, execute the “Deep dive open.exe” executable file.

Once the engine starts, by default, a command console will open featuring a menu with three possible options:
   1. Start the example AI game and access its main menu.
   2. Enter the level editor mode.
   3. Stop the engine's execution.

### Example AI game
If the user chooses to enter the example AI game, another menu will be printed to the console with 8 options, being the 8th one to exit the example AI game and return to the previously mentioned menu. 

1. **Initiate agent training simulations generating the AI agents with random weights** (according to the parameters given to the AI game in the code).
2. **Initiate agent training simulations loading some AI agents from an correct “.aidata” file**.
3. **Initiate agent testing simulations generating the AI agents with random weights** (according to the parameters given to the AI game in the code). In this case, the AI agents are not trained and the only output is some statistics of how well they performed when completing their tasks.
4. **Initiate agent testing simulations loading some AI agents from an correct “.aidata” file**. In this case, the AI agents are not trained and the only output is some statistics of how well they performed when completing their tasks.
5. **Generate a record of one epoch with AI agents generated with random weights** (according to the parameters given to the AI game in the code). The engine will ask for the name of the ".rec” file that will be generated containing the record of the match.
6. **Generate a record of one epoch with some AI agents loaded from an correct “.aidata” file**. The engine will ask for the name of the ".rec” file that will be generated containing the record of the match.
7. **Play previously generated record**.  The engine will ask the user the name of said record and, if found, it will start the engine’s graphical mode, load the level that is associated with said recording (inside the /saves/recordingsWorlds directory found in the same folder as where the “Deep dive open.exe” file is located). If the record’s name corresponds to one already existing in the AI game’s record file, then the engine will ask if the user desires to override the existing file or not. If not, then it will ask for another name. Once the engine’s graphical part initialises in record playback mode, the user will have a free 3D camera to observe the level and the actions that the AI agents are doing. The controls for this mode are:
   * W key for moving forward in the viewing direction.
   * S key for moving backwards in the viewing direction.
   * D key for moving to the right side in the viewing direction.
   * A key for moving to the left side in the viewing direction.
   * Move the mouse for looking around and change the viewing direction.
   * X key for stopping the recording’s playback.
   * Down arrow key for pausing the recording’s playback.
   * Left arrow key for executing the record file’s AI actions backwards.
   * Right arrow key for executing the record fikle’s AI action forward.

When training or testing AI agents, the engine will ask for a number of agents to create for the process. In training mode, said number must be even due to the genetic operators used during the simulation. When loading previously generated AI data, the engine will ask instead the path to its corresponding file and how many agents to load from the file (if the user declares to load “n” agents, the engine will load the first “n” agents found in the “.aidata” file)- Once the number of AI agents is specified (and they are loaded into the engine by using previously generated AI data), for training and testing modes, the engine will ask for a number of epochs for the simulation. An epoch is one iteration of the game or one game “match” in which the agents do their task. If there was an AI game of football, an epoch would be considered a single match.

The “.aidata” and “.rec” files are stored, respectively, in the “AIData” and “records” directories, found in the same directory where “Deep dive open.exe” is found. Inside those directories, a folder is created for each registered AI game in order not to mix files from diferent AI games as the definitions of AI data and AI actions may vary per AI games. 


### Level editor
Entering in this mode will activate the engine’s graphical mode and display to the user a GUI (Graphical User Interface) that has three options: 
   1. **LOAD**. Used to load a previously existing level saved in one of the 5 level slots provided by the engine. This load menu will have 5 buttons (one for level slot) and another one for returning to the previous menu.
   2. **NEW**. Generate a new random level and enter it
   3. **EXIT**. Stop the engine’s graphical mode and return to its initial menu.

Once the user enters a level in edit mode, it will have the same camera and controls as in record playback mode, with the exception that the X, left arrow, down arrow and right arrow are disabled as there is no record being played. However, the user has new controls, with the left mouse button, the user will destroy the block he is looking at if it is close enough. On the other side, with the right mouse button, the user will place a block where it is looking (only if it is looking at a non-null block first). Also, the user has 9 types of different blocks to place and to select them the user needs to press one of the 1 to 9 keys in the keyboard (the ones that are usually just below the function keys).

Additionally, the user can access an options menu by pressing the Escape key. It will have the following three options:
  1. **LOAD**. Used to load a previously existing level saved in one of the 5 level slots provided by the engine. This load menu will have 5 buttons (one for level slot) and another one for returning to the previous menu.
  2. **SAVE**. Save the currently loaded level into one of the 5 slots 
  3. **EXIT**. Return to the initial menu of the level editor mode without saving the current level. 

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

Thank you for contributing to GAGI. 

## License
 This project is licensed under the GPL License - see the LICENSE.md file for details. 
 Copyright © 2023
