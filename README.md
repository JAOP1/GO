# GO generalized game engine.
The target is develop a game engine wich works in different Go boardgames. 
The board are represented by graph.

## Requirements.
  * LibTorch. (you can download [here!](https://pytorch.org/))
  * SFML.

## Compiling project


  We need to create a new directory in the same project.
  ```
  mkdir build/
  cd build/
  ```
  After then, we continue with the process in the cmakefile.

  ```
  cmake ..
  make
  ```

## User interface
 Exist some problems about compatibility between libtorch and SFML, so, you have to change off Build_trainer and Build_Testing.
 After then, save the cmakelists settings and compile again. You will see the executable.
  ```
  ./visualizer --help 
  ```
 Allow us:
 * Upload graphs.
 * Save new graphs.
 * finally, play Go in every kind of board.
 
## Training phase
Enable only Buid_trainer and Build_Testing (if you want). Compile the project.
  ```
  ./trainer --help
  ```
Currently the project only works in grid graphs, but the intention is develop for every graph.


**Bugs**
- [ ] Compiler doesn't identify SFML functions when include libtorch.
- [ ] Generate issues Gtest with libtorch.  
**Notes:**
* Currently working it.
* forgive me, but some comments are in spanish.

**Future**
* Create and train a net model using pytorch geometric.
* Traduction using JIT for compatibility between python and C++.
