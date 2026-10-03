// Standard library includes
#include <vector>

// Project includes
#include "control_window.h"
#include "imgui_manager.h"
#include "viewport_window.h"

std::vector<unsigned char> image_buffer;
size_t s_exp = 8;

// Declare path tracing functions
void InitializePathTracer();
void UpdatePathTracer();

int main(int argc, char **argv) {

  // initialize OpenGL, GLFW and ImGui
  ImguiManager manager;

  // Add the ViewportWindow to the manager that will display the path-traced
  // result stored in the image_buffer array.
  manager.AddWindow(std::make_unique<ViewportWindow>());

  // Add the ControlWindow, which positions the camera
  manager.AddWindow(std::make_unique<ControlWindow>());

  // Initialize the path tracer
  InitializePathTracer();

  // enter the render loop
  while (manager.Running()) {

    // Render the ImGui interface (displaying the image_buffer)
    manager.Render();

    // Update the image_buffer
    UpdatePathTracer();
  }
}
