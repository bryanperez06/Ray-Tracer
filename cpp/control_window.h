#pragma once

// C++ standard libraries
#include <algorithm>
#include <cmath>
#include <numbers>

// Library includes
#include <glm/glm.hpp>
#include <imgui.h>

// Project includes
#include "imgui_window.h"
#include <tira/graphics/camera.h>
#include <tira/math/geometry.h>

// This class has to be able to re-initialize the image when the user
// updates the image size
void InitializePathTracer();

// The camera used by the path tracer, defined in pathtrace.cpp
extern tira::graphics::Camera camera;

/**
 * @brief Provides ImGui widgets that place the camera on a sphere centered at
 * the origin and set its field of view.
 *
 * The camera position is specified in spherical coordinates (r, theta, phi),
 * where theta is the azimuthal angle measured from the +x axis and phi is the
 * polar angle measured from the +z axis. The camera always looks at the origin.
 */
class ControlWindow : public ImguiWindow {
protected:
  float theta_ = 0.0f;                           ///< azimuthal angle (radians)
  float phi_ = std::numbers::pi_v<float> / 2.0f; ///< polar angle (radians)
  float r_max_ = 10.0f; ///< maximum value of the distance slider
  float r_ = 5.0f;      ///< distance from the camera to the origin
  float fov_ = std::numbers::pi_v<float> / 3.0f; ///< field of view (radians)

  /**
   * @brief Converts the spherical coordinates to a Cartesian position and
   * updates the camera so that it is at that position looking at the origin
   * with the selected field of view.
   */
  void UpdateCamera() {
    // a camera at the origin can't look at the origin, so keep r above zero
    const float r = std::max(r_, 1e-4f);
    const glm::vec3 position =
        tira::geometry::SphericalToCartesian(glm::vec3(r, theta_, phi_));

    // use the "north" tangent of the sphere as the up vector, which is always
    // orthogonal to the view direction (a fixed up vector would be parallel to
    // the view direction at some positions)
    const glm::vec3 up(-std::cos(theta_) * std::cos(phi_),
                       -std::sin(theta_) * std::cos(phi_), std::sin(phi_));

    // the position has to be set first because LookAt calculates the view
    // direction from it
    camera.Position(position);
    camera.LookAt(glm::vec3(0.0f), up);

    // Camera::FieldOfView takes degrees. A zero field of view puts the image
    // plane at infinity and makes every ray NaN, so keep it slightly above zero
    camera.FieldOfView(glm::degrees(std::max(fov_, 1e-3f)));
  }

public:
  /**
   * @brief Creates the window and initializes the camera to match the default
   * widget values.
   */
  ControlWindow() { UpdateCamera(); }

  /**
   * @brief Draws the camera position and field of view widgets and updates
   * the camera if any of them changed.
   */
  void Render() override {
    const float pi = std::numbers::pi_v<float>;

    ImGui::Begin("Camera");

    bool changed = false;
    changed |= ImGui::SliderFloat("theta", &theta_, -pi, pi);
    changed |= ImGui::SliderFloat("phi", &phi_, 0.0f, pi);

    if (ImGui::InputFloat("r max", &r_max_)) {
      r_max_ = std::max(r_max_, 1e-3f); // keep the slider range valid
      r_ = std::min(r_, r_max_);        // keep r within the new range
      changed = true;
    }

    changed |= ImGui::SliderFloat("r", &r_, 0.0f, r_max_);
    changed |= ImGui::SliderFloat("fov", &fov_, 0.0f, pi / 2.0f);

    if (changed)
      UpdateCamera();

    ImGui::End();
  }
};
