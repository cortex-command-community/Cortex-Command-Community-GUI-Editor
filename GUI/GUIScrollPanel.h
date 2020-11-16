#ifndef _GUISCROLLPANEL_
#define _GUISCROLLPANEL_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            GUIScrollPanel.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     GUIScrollPanel class
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jackal@shplorb.com
//                  www.shplorb.com/~jackal


namespace RTE
{


//////////////////////////////////////////////////////////////////////////////////////////
// Class:           GUIScrollPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     A scrollbar panel class used for controls requiring a scrollbar.
// Parent(s):       Panel.
// Class history:   1/12/2004 GUIScrollPanel Created.

class GUIScrollPanel :
    public GUIPanel
{


//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:

    // Scroll panel orientation
    enum {
        Horizontal,
        Vertical
    };

    // Pre-built draw bitmaps
    enum {
        ButtonStates=0,
        KnobStates,
        Back
    };

    // Signals
    enum {
        ChangeValue=0,
        Grab,
        Release
    };


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIScrollPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIScrollPanel object in
//                  system memory.
// Arguments:       GUIManager.

    GUIScrollPanel(GUIManager *Manager);


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIScrollPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIScrollPanel object in
//                  system memory.
// Arguments:       None.

    GUIScrollPanel();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the scrollpanel
// Arguments:       Position, Size.

    void Create(int X, int Y, int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel has been destroyed.
// Arguments:       None.

    void Destroy(); 


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ChangeSkin
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the skin has been changed.
// Arguments:       New skin pointer.

    void ChangeSkin(GUISkin *Skin);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the panel
// Arguments:       Screen class

    void Draw(GUIScreen *Screen) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes down on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseDown(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes up on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseUp(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseMove
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse moves (over the panel, or when captured).
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseMove(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseHover
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse is hovering over the panel (has to be enabled)
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

    void OnMouseHover(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Resize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the size of the panel.
// Arguments:       Width, Height.

    void SetSize(int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMinimum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the minimum value for the scrollpanel
// Arguments:       Minimum value.

    void SetMinimum(int Min);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMinimum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the minimum value for the scrollpanel
// Arguments:       None.

    int GetMinimum();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMaximum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the maximum value for the scrollpanel
// Arguments:       Maximum value.

    void SetMaximum(int Max);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMaximum
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the maximum value for the scrollpanel
// Arguments:       None.

    int GetMaximum();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the current value for the scrollpanel
// Arguments:       Value.

    void SetValue(int Value);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the current value of the scrollpanel.
// Arguments:       None.

    int GetValue();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPageSize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the page size value for the scrollpanel.
// Arguments:       PageSize.

    void SetPageSize(int PageSize);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetPageSize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the size of the page.
// Arguments:       None.

    int GetPageSize();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetOrientation
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the orientation of the scrollpanel.
// Arguments:       Orientation.

    void SetOrientation(int Orientation);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetOrientation
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the orientation of the scrollpanel.
// Arguments:       None.

    int GetOrientation();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetSmallChange
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the small change value.
// Arguments:       SmallChange.

    void SetSmallChange(int SmallChange);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSmallChange
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the small change value.
// Arguments:       None.

    int GetSmallChange();


//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadProps
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Load values from a property class.
// Arguments:       Properties.

    void LoadProps(GUIProperties *Props);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SaveProps
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Save values to a property class.
// Arguments:       Properties.

    void SaveProps(GUIProperties *Props);


//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Build the bitmap.
// Arguments:       None.

    void BuildBitmap(bool UpdateSize, bool UpdateKnob);


//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

private:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildButton
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Build a button.
// Arguments:       ArrowName, Width, Height.

    void BuildButton(const std::string ArrowName, int Y, int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBackground
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Build the background.
// Arguments:       None.

    void BuildBackground();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildKnob
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Build the knob.
// Arguments:       None.

    void BuildKnob(const std::string Section, int X, int Y, int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateKnob
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Calculate the knob size and position.
// Arguments:       None.

    void CalculateKnob();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AdjustValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the value.
// Arguments:       Delta movement.

    void AdjustValue(int Delta);


// Members
    
    GUISkin                *m_Skin;
    GUIBitmap            *m_DrawBitmap[3];
    
    // User attributes
    int                    m_Orientation;
    int                    m_Minimum;
    int                    m_Maximum;
    int                    m_Value;
    int                    m_PageSize;
    int                    m_SmallChange;

    // Internal attributes
    bool                m_RebuildSize;
    bool                m_RebuildKnob;
    int                    m_ButtonSize;
    int                    m_MinimumKnobSize;
    int                    m_KnobPosition;
    int                    m_KnobLength;
    bool                m_ButtonPushed[2];
    bool                m_GrabbedKnob;
    bool                m_GrabbedBackg;
    int                    m_GrabbedPos;
    int                    m_GrabbedSide;
};


}; // namespace RTE


#endif  //  _GUILISTPANEL_