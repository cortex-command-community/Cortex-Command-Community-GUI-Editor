
//////////////////////////////////////////////////////////////////////////////////////////
// File:            main
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Entry point file for the GUI Library editor
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jboett@internode.on.net


#include "TimerMan.h"
#include "GUIEditorApp.h"
#include "GUIEditorLib.h"

#include "allegro.h"
#include "winalleg.h"


extern "C" { FILE __iob_func[3] = { *stdin, *stdout, *stderr }; }


GUIEditorApp g_GUIEditor;

extern HINSTANCE g_hInstance = 0;
extern HWND g_hWnd = 0;

using namespace GUI;
GUIEditorApp g_GUIEditor;

using namespace RTE;

int main(int argc, char *argv[]) {
	if (!g_GUIEditor.Initialize()) { return -1; }

	// Get windows settings
	g_hWnd = win_get_window();
	g_hInstance = (HINSTANCE)GetWindowLong(g_hWnd, GWL_HINSTANCE);

	// Run editor loop
	while (true) {
		bool bContinue = g_GUIEditor.Update();
		if (!bContinue) { break; }
	}
	return 0;
}
END_OF_MAIN()