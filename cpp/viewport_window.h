#pragma once

/**
 * @file Displays the path-traced image stored in image_buffer
 */

// C++ standard libraries
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

// Library includes
#include <glad/glad.h>
#include <imgui.h>

// Project includes
#include "imgui_window.h"

// The image buffer (interleaved 8-bit RGB) and its size, defined in display.cpp
extern std::vector<unsigned char> image_buffer;
extern size_t s_exp;

/**
 * Displays the contents of image_buffer as an image. The buffer is uploaded to
 * an OpenGL texture every frame, so changes made by the path tracer show up as
 * soon as the next frame is drawn.
 */
class ViewportWindow : public ImguiWindow {
protected:
  GLuint texture_ = 0;

  // size of the texture currently allocated on the GPU
  size_t tex_sx_ = 0;
  size_t tex_sy_ = 0;

  // Copies image_buffer into the texture, (re)allocating it if the size changed
  void UploadTexture() {
    if (texture_ == 0) {
      glGenTextures(1, &texture_);
      glBindTexture(GL_TEXTURE_2D, texture_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
      glBindTexture(GL_TEXTURE_2D, texture_);
    }

    // RGB rows are not guaranteed to be aligned to 4 bytes
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    size_t sx = std::pow(2, s_exp);
    size_t sy = sx;
    if (sx != tex_sx_ || sy != tex_sy_) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, static_cast<GLsizei>(sx),
                   static_cast<GLsizei>(sy), 0, GL_RGB, GL_UNSIGNED_BYTE,
                   image_buffer.data());
      tex_sx_ = sx;
      tex_sy_ = sy;
    } else {
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(sx),
                      static_cast<GLsizei>(sy), GL_RGB, GL_UNSIGNED_BYTE,
                      image_buffer.data());
    }
  }

public:
  ViewportWindow() = default;
  ViewportWindow(const ViewportWindow &) = delete;
  ViewportWindow &operator=(const ViewportWindow &) = delete;

  // The ImguiManager destroys its windows before the OpenGL context
  ~ViewportWindow() override {
    if (texture_ != 0)
      glDeleteTextures(1, &texture_);
  }

  void Render() override {
    ImGui::Begin("Viewport");

    size_t sx = std::pow(2, s_exp);
    size_t sy = sx;

    if (sx == 0 || sy == 0 || image_buffer.size() < sx * sy * 3) {
      ImGui::TextUnformatted("No image");
      ImGui::End();
      return;
    }

    UploadTexture();

    // scale the image to fit the window while preserving its aspect ratio
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float scale = std::min(avail.x / static_cast<float>(sx),
                           avail.y / static_cast<float>(sy));
    if (scale <= 0.0f)
      scale = 1.0f;

    ImGui::Image(
        (ImTextureID)(intptr_t)texture_,
        ImVec2(static_cast<float>(sx) * scale, static_cast<float>(sy) * scale));

    ImGui::End();
  }
};
