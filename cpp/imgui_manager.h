#pragma once

// C++ standard libraries
#include <list>
#include <memory>
#include <string>

#include "imgui_window.h"

struct GLFWwindow;

class ImguiManager {
protected:
  // Windows are held by shared_ptr so that a caller can keep a weak_ptr to one
  // it
  //	created and ask whether it is still alive without owning it.
  std::list<std::shared_ptr<ImguiWindow>> windows_;

  // Windows added during a frame wait here until the render loop has finished
  //	walking windows_, and are spliced in afterwards.
  std::list<std::shared_ptr<ImguiWindow>> pending_;

  GLFWwindow *window_ = nullptr;

public:
  ImguiManager(const std::string &title = "volblade", int width = 1000,
               int height = 1000);
  ~ImguiManager();

  ImguiManager(const ImguiManager &) = delete;
  ImguiManager &operator=(const ImguiManager &) = delete;

  // Takes ownership of a window and adds it to the render list. This is safe to
  //	call from inside a window's Render(); the new window first draws on the
  // next 	frame. The returned handle expires when the window closes.
  std::weak_ptr<ImguiWindow> AddWindow(std::unique_ptr<ImguiWindow> window);

  // Returns false once the user has asked to close the main window
  bool Running() const;

  // Polls events, renders all windows, and swaps buffers for a single frame
  void Render();
};
