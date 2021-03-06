#ifndef _GUIMANAGER_
#define _GUIMANAGER_

namespace RTE {

class Timer;

/// <summary>
/// The main manager that handles all the panels and inputs.
/// </summary>
class GUIManager {

public:


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIManager
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIManager object in system
//                  memory.
// Arguments:       Input Interface

    explicit GUIManager(GUIInput *input);


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      GUIManager
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a GUIManager object.
// Arguments:       None.

    ~GUIManager();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the manager.
// Arguments:       None.

    void Clear();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a panel to the list.
// Arguments:       Pointer to a panel.

    void AddPanel(GUIPanel *panel);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the GUI.
// Arguments:       Whether keyboard events should be ignored or not. Used to avoid conflicts when custom keyboard handling for GUI elements is preset.

    void Update(bool ignoreKeyboardEvents = false);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draw all the panels
// Arguments:       Screen.

    void Draw(GUIScreen *Screen);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          EnableMouse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Enables and disables the mouse completely for this.
// Arguments:       Enable?

    void EnableMouse(bool enable = true) { m_MouseEnabled = enable; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CaptureMouse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets up capturing a mouse for a panel.
// Arguments:       Panel.

    void CaptureMouse(GUIPanel *Panel);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReleaseMouse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Releases a mouse capture.
// Arguments:       None.

    void ReleaseMouse();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetPanelID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a unique ID for a panel.
// Arguments:       None.

    int GetPanelID();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetInputController
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the input controller object
// Arguments:       None.

    GUIInput * GetInputController() { return m_Input; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          TrackMouseHover
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets up the manager to enable/disable hover tracking of this panel
// Arguments:       Panel, Enabled, Delay (milliseconds)

    void TrackMouseHover(GUIPanel *Pan, bool Enabled, int Delay);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Give focus to a panel.
// Arguments:       Panel.

    void SetFocus(GUIPanel *Pan);


private:

	std::vector<GUIPanel *> m_PanelList;
	GUIPanel *m_CapturedPanel;
	GUIPanel *m_FocusPanel;
	GUIPanel *m_MouseOverPanel;

	GUIInput *m_Input;
	bool m_MouseEnabled;
	int m_OldMouseX;
	int m_OldMouseY;

	int m_DoubleClickTime;
	int m_DoubleClickSize;
	int m_DoubleClickButtons;
	float m_LastMouseDown[3];
	GUIRect m_DoubleClickRect;

	bool m_HoverTrack;
	GUIPanel *m_HoverPanel;
	float m_HoverTime;

	bool m_UseValidation;
	int m_UniqueIDCount;

	Timer *m_pTimer;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FindBottomPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Goes through the panel list and selects the bottommost
//                  ('first', render wise) panel on a specific point.
// Arguments:       Mouse Position.

    GUIPanel *FindBottomPanel(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FindTopPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Goes through the panel list and selects the topmost ('last', render
//                  wise) panel on a specific point.
// Arguments:       Mouse Position.

    GUIPanel *FindTopPanel(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MouseInRect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if the mouse point is inside a rectangle.
// Arguments:       Rectangle, Mouse position.

    bool MouseInRect(const GUIRect *Rect, int X, int Y);

};
};
#endif