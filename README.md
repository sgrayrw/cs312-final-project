# Project - Super Mario Clone

A Super Mario clone implemented using OpenGL.

https://user-images.githubusercontent.com/43045651/119061801-38484400-b9a3-11eb-90dd-15d2f81f7080.mov

## Build

The build is only verified on MacOS with the 
following brew dependencies: `cmake, glew, glfw3`.

To build and run the game:
```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ../mario
```

## High-level Design
All objects (Mario, background scene, 
mushrooms, goombas, bricks, base walls) are 
implemented as particles in a single
particle system representing the game world. In each
frame update, the `update()` function of each
particle in the world will be invoked to update
its force, velocity, and position. Collision 
detection and resolution is applied at the same
time. Based on the result of collision resolution,
objects will be either updated, created, or removed.

## Data Structures
1. An unordered map `pressedKeys` that maps
   `GLFW_KEY_*` to `bool`, keeping track of the
   state of keyboard inputs. This allows the 
   game to correctly handle multiple key inputs,
   such as making Mario jump to the right when both
   up and right keys are pressed.
2. An unordered map `textures` that maps texture 
   names to `GLuint`. Textures are loaded at game
   initialization and different texture ids are used
   in each frame to allow dynamic texture loading.
3. An unordered map `particles` that map particle 
   names (e.g. `"mario", "scene"`) to a vector of 
   particles of the same type for batch update and
   rendering.
   
## Maths and Physics
1. Collision detection
   
   This is calculated as box-box 
   collision, which is the union of horizontal 
   collision and vertical collision. Formally:
   ```c++
   bool collide(const box& a, const box& b) {
       bool collisionX = a.x + a.size >= b.x && b.x + b.size >= a.x;
       bool collisionY = a.y + a.size >= b.y && b.y + b.size >= a.y;
       return collisionX && collisionY;
   }
   ```
   
2. Collision resolution

   The above `collide()` function is extended to
   calculate the direction of the collision by
   computing the angles between the vector joining 
   the centers of the two boxes and the four unit 
   direction vectors (up, down, left, right).
   Collisions in different directions are handled 
   differently between different objects. 
   For example, the `BOTTOM` collision between Mario
   and a brick will cause the brick to exert a 
   supporting force so that Mario can stand on it; 
   the `TOP` collision of Mario on a goomba will 
   kill the goomba, while any other collision 
   direction will kill Mario, etc.
   
3. Character Movement

   The horizontal movements of Mario is implemented by 
   exerting horizontal forces while the left or right
   key is pressed. A frictional force is applied to
   gradually offset the velocity to zero when keys 
   are released. The up key provides a vertical force
   in the positive `y` direction, and Mario falls 
   when the gravitational force dominates the vertical
   velocity.
   
4. Updating velocity and position

   The velocity and position of all objects are updated
   in the same way:
   ```c++
   obj.pos = obj.pos + dt * obj.vel;
   obj.vel = obj.vel + dt * obj.force / obj.mass;
   ```
   
## Results
This project is an effective clone of the original
Super Mario Bros both visually and functinonally. 
While many details in the original game is unimplemented,
the game produced by this project is a playable one without
any obvious defects.
