# GO generalized game engine.
The target is develop a game engine wich works in different Go boardgames. 
The board are represented by graph.

## Requirements.
  * LibTorch. (you can download [here!](https://pytorch.org/))
  * SFML.

## Using

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

  And finally, we only execute.
  ```
  ./visualizer
  ```

**Bugs**
- [ ] Compiler doesn't identify SFML functions when include libtorch.
 

**Notes:**
* Currently working it.
* forgive me, but some comments are in spanish.

