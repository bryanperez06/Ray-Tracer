// Standard library includes
#include <algorithm>
#include <cmath>
#include <vector>

// Project includes
#include <tira/graphics/camera.h>

extern std::vector<unsigned char> image_buffer;
extern size_t s_exp;

tira::graphics::Camera camera;

/**
 * This function is called when the program is loaded. Use it to initialize
 * the path tracer as necessary. For example, you can load models (like
 * spheres), initialize data structures, etc.
 */
void InitializePathTracer() {

  // allocate the image buffer and clear it (set it to zero)
  if (s_exp > 0) {
    image_buffer.resize(pow(2, s_exp) * pow(2, s_exp) * 3);
    std::fill(image_buffer.begin(), image_buffer.end(), 0);
  }
}

/**
 * This function is called every time the user interface is rendered. When
 * this function returns, the value of the image buffer will be displayed.
 * Make sure that it returns in a reasonable time so that you have some
 * visual feedback ever frame.
 */
void UpdatePathTracer() {

  size_t s = std::pow(2, s_exp);
  float dx = 1.0f / s;

  // iterate across all pixels in the image buffer
  for (size_t yi = 0; yi < s; yi++) {
    for (size_t xi = 0; xi < s; xi++) {

      // this gives you a vector from the camera through a pixel
      glm::vec3 direction = camera.Ray(-0.5f + xi * dx, -0.5f + yi * dx);

      // this gives you the position of the camera
      glm::vec3 start = camera.Position();

      // set the associated pixel color based on the ray direction
      image_buffer[yi * s * 3 + xi * 3 + 0] = std::abs(direction.x) * 255;
      image_buffer[yi * s * 3 + xi * 3 + 1] = std::abs(direction.y) * 255;
      image_buffer[yi * s * 3 + xi * 3 + 2] = std::abs(direction.z) * 255;
    }
  }
}
