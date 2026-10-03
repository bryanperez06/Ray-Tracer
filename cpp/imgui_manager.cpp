#include "imgui_manager.h"

#include <cstdio>
#include <stdexcept>

// GLAD must be included before GLFW3
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace {
void GlfwErrorCallback(int error, const char *description) {
  std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}
} // namespace

/**
 *	The ImguiManager constructor initializes GLFW3, which provides an OpenGL
 *	context. The class also maintains the ImGui context created here, and
 *	it ties that context to OpenGL and GLFW.
 */
ImguiManager::ImguiManager(const std::string &title, int width, int height) {

  glfwSetErrorCallback(GlfwErrorCallback);
  if (!glfwInit())
    throw std::runtime_error("ImguiManager: glfwInit() failed");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("ImguiManager: glfwCreateWindow() failed");
  }

  glfwMakeContextCurrent(window_);

  // Windows exposes only OpenGL 1.1 through opengl32.dll, so every entry point
  // past that has to be resolved against the context at runtime. glad does that
  // for the whole API here, before anything else issues a GL call.
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwDestroyWindow(window_);
    glfwTerminate();
    throw std::runtime_error("ImguiManager: gladLoadGLLoader() failed");
  }

  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // Handle font scaling because the default is teeny-tiny for high-res displays
  // NOTE: I'm pretty sure the standard for doing this is about to change. I've
  // seen some discussion on the ImGui repository that ImGui will be able to
  // access the OS scaling directly (in fact it works now on a prototype
  // branch). For now (as of ImGui 1.92) I'm going through GLFW.
  float xscale, yscale;
  glfwGetWindowContentScale(window_, &xscale, &yscale);

  // I'm assuming isotropic scaling here
  float dpi_scale = xscale;
  ImGuiStyle &style = ImGui::GetStyle();
  style.FontScaleDpi = dpi_scale;
}

/**
 *	The destructor cleans up the ImGui and OpenGL contexts, and closes
 *	the GLFW3 window.
 */
ImguiManager::~ImguiManager() {

  // All windows are destroyed first, because they need the OpenGL context
  // to be destroyed.
  windows_.clear();
  pending_.clear();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

std::weak_ptr<ImguiWindow>
ImguiManager::AddWindow(std::unique_ptr<ImguiWindow> window) {
  pending_.push_back(std::move(window));
  return pending_.back();
}

bool ImguiManager::Running() const { return !glfwWindowShouldClose(window_); }

/**
 *	This function draws the ImGui interface. The function loops through all
 *	windows attached to the manager.
 */
void ImguiManager::Render() {

  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Render all windows here
  for (auto &window : windows_)
    window->Render();

  // Windows created during this frame join the list now that it is no longer
  // being
  //	iterated, and any window that closed itself is dropped.
  windows_.splice(windows_.end(), pending_);
  windows_.remove_if([](const std::shared_ptr<ImguiWindow> &window) {
    return !window->IsOpen();
  });

  ImGui::Render();

  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  glViewport(0, 0, width, height);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window_);
}
