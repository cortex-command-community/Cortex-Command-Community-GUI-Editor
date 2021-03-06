#ifndef _GUIEVENT_
#define _GUIEVENT_

namespace RTE {

/// <summary>
/// A class to hold event information.
/// </summary>
class GUIEvent {

public:

    // Event Types
    enum {
        Command = 0,
        Notification
    } EventType;


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIEvent
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIEvent object in system
//                  memory.
// Arguments:       None.

    GUIEvent();


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIEvent
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIEvent object in system
//                  memory.
// Arguments:       Control, Event type, Msg, Data.

    GUIEvent(GUIControl *Control, int Type, int Msg, int Data);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetType
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the event type
// Arguments:       None.

    int GetType() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMsg
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the msg.
// Arguments:       None.

    int GetMsg() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetData
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the data.
// Arguments:       None.

    int GetData() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetControl
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the event control.
// Arguments:       None.

    GUIControl * GetControl();

private:

    GUIControl *m_Control;
    int m_Type;
    int m_Msg;
    int m_Data;

};
};
#endif