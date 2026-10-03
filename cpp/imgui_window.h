#pragma once

/**
 * This is the base class for any ImGui window used in volblade.
 * All renderable windows will extend this class, which contains a
 * virtual Render() function that will be called when the pane is displayed.
*/
class ImguiWindow {

public:

	virtual ~ImguiWindow() = default;

	/**
	 * This virtual function is called whenever the window pane is active
	 * and instructs the renderer to draw the pane and all associated widgets.
	*/
	virtual void Render() = 0;

	/**
	 * This function reports whether the window is still in use. A window that
	 * returns false is removed from the render list by the ImguiManager after
	 * the current frame. Windows that live for the whole session never have to
	 * override it.
	*/
	virtual bool IsOpen() const { return true; }


};
