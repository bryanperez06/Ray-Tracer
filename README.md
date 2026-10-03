# Parallel Algorithms Ray-Tracing Template
This code provides the infrastructure for a ray-tracing algorithm that we
will be developing as part of this class.

## First Steps (Important)
After pulling this repository, the first thing you should do is make a copy
of the ```pathtrace.cpp.template``` file and rename it to ```pathtrace.cpp```.
For example (on Linux):
```
cp cpp/pathtrace.cpp.template cpp/pathtrace.cpp
```
After doing this, you should be able to build and execute the application.

This will allow you to edit the ```pathtrace.cpp``` file as well as add
additional files yourself. Any updates that I make to this template can easily
be pulled using ```git pull``` without overwriting your changes.

## Initial Template
Once you are able to execute the template, you should see two ImGui windows:
- The ViewportWindow (defined in ```viewport_window.h```) displays an image
buffer directly to the screen. This image is updated every rendering pass
so any changes you make to the buffer will immediately change the image that
is displayed.
- The ControlWindow (defined in ```control_window.h```) provides a series of
controls that you can use to manipulate the camera and change the image
resolution.

There are two functions in ```pathtrace.cpp``` that you will have to modify:
- ```InitializePathTracer()``` is called any time the image resolution is
changed. All this function does right now is re-allocate the buffer and set
all of its values to zero (producing a black image).
- ```UpdatePathTracer()``` is called every render cycle. This
function currently retrieves a vector for each pixel describing a ray direction
and assigns a color to the image buffer associated with the coordinates of
that vector. You can observe this by moving the ```theta``` and ```phi```
controls, which will move the camera in spherical coordinates around the
origin. For example, setting the ```phi``` value to zero will point the camera
so that it is looking directly down the ```z``` axis. Most of the pixels in
the screen should therefore be blue (since most rays are going in the ```z```
direction).

## Phase 1: Build a Path Tracer
The first step of this class project is to implement a basic path tracer.
Feel free to add any additional source files you want to this repository,
but don't add any additional external libraries. I want to provide you with
as much flexibility as possible in your implementation so that you can use
programming techniques that you are comfortable with. For example, you can
stick with traditional C or use object-oriented C++.

### Requirements
There are a few things that we'll need your path tracer to accomplish:
- [ ] You should be able to render a scene with multiple *spheres* defined
by a position $p$ and a radius $r$. You can hard-code them or load them from
some sort of simple text file.
- [ ] The spheres will need some material properties including
```reflectivity```, 
# Ray-tracer
