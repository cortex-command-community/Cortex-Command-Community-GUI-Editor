#ifndef _GUICONTROL_
#define _GUICONTROL_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            GUIControl.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     GUIControl class
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jackal@shplorb.com
//                  www.shplorb.com/~jackal


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "GUIWriter.h"


namespace RTE
{


// Forward Declarations
class GUIControlManager;


//////////////////////////////////////////////////////////////////////////////////////////
// Class:           GUIControl
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     A base class inherited by all controls.
// Parent(s):       None.
// Class history:   1/4/2004 GUIControl Created.

class GUIControl {

//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:

    // Anchor points
    enum {
        Anchor_Left = 0x01,
        Anchor_Top = 0x02,
        Anchor_Right = 0x04,
        Anchor_Bottom = 0x08
    } Anchor;


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIControl
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIControl object in
//                  system memory.
// Arguments:       None.

    GUIControl();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control has been created.
// Arguments:       Name, Position, Size

    virtual void Create(const std::string Name, int X, int Y, int Width = -1, int Height = -1);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control has been created.
// Arguments:       Properties.

    virtual void Create(GUIProperties *Props);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control has been destroyed.
// Arguments:       None.

    virtual void Destroy();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    Activate
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control is activated and ready for use.
// Arguments:       None.

    virtual void Activate();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    ChangeSkin
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the skin has been changed.
// Arguments:       New skin pointer.

    virtual void ChangeSkin(GUISkin *Skin);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddEvent
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Add a new event to the queue.
// Arguments:       Type, Message, Data.

    void AddEvent(int Type, int Msg, int Data);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control's name.
// Arguments:       None.

    std::string GetName();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetToolTip
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the control's tooltip string.
// Arguments:       The new ToolTip for this.

    void SetToolTip(std::string tip) { m_Properties.SetValue("ToolTip", tip); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetToolTip
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control's tooltip string.
// Arguments:       None.

    std::string GetToolTip();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns a string representing the control's ID
// Arguments:       None.

    std::string GetID();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetAnchor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the anchor flags.
// Arguments:       None.

    int GetAnchor();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddChild
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a child to this control
// Arguments:       Control.

    void AddChild(GUIControl *Control);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetChildren
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the children lst
// Arguments:       None.

    std::vector<GUIControl *> *GetChildren();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    GetPanel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the panel of the control.
// Arguments:       None.
// Returns:         0 if the control does not have a panel, otherwise the topmost panel.

    virtual GUIPanel * GetPanel();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the control properties.
// Arguments:       Writer.
// Returns:         True if sucessful

    bool Save(GUIWriter *W);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    StoreProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control to store the values into properties.
// Arguments:       None.

    virtual void StoreProperties();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    Move
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be moved.
// Arguments:       New position.

    virtual void Move(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    Resize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the control needs to be resized.
// Arguments:       New size.

    virtual void Resize(int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    GetControlRect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the rectangle of the control.
// Arguments:       Position, Size.

    virtual void GetControlRect(int *X, int *Y, int *Width, int *Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    SetVisible
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the visibility of the control.
// Arguments:       Visible.

    virtual void SetVisible(bool Visible);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    GetVisible
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the visibility of the control.
// Arguments:       None.

    virtual bool GetVisible();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    SetEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the enabled state of the control.
// Arguments:       Enabled.

    virtual void SetEnabled(bool Enabled);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    GetEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the enabled state of the control.
// Arguments:       None.

    virtual bool GetEnabled();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetParent
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the parent of this control.
// Arguments:       None.

    GUIControl *GetParent();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the control properties.
// Arguments:       None.

    GUIProperties *GetProperties();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:    ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Applies new properties to the control.
// Arguments:       GUIProperties.

    virtual void ApplyProperties(GUIProperties *Props);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ApplyProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the IsContainer value.
// Arguments:       None.

    bool IsContainer();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RemoveChild
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes a child based on name.
// Arguments:       Child Name.

    void RemoveChild(const std::string Name);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RemoveChildren
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Removes all the children.
// Arguments:       None.

    void RemoveChildren();


//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

protected:

    GUISkin            *m_Skin;
    int             m_SkinPreset;
    GUIProperties    m_Properties;
    GUIControl        *m_ControlParent;
    std::vector<GUIControl *>        m_ControlChildren;

    std::string        m_ControlID;

    bool            m_IsContainer;

    // For the GUI editor
    int                m_MinWidth, m_MinHeight;
    int                m_DefWidth, m_DefHeight;

    GUIControlManager    *m_ControlManager;

};


};  // namespace RTE


#endif  //  _GUICONTROL_