#include "RTEError.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool RTEAbortFunc(const char *description, const char *file, int line) {
		// Ditch the video mode so the message box appears without problems
		if (screen != 0) { set_gfx_mode(GFX_TEXT, 0, 0, 0, 0); }
		// Set title of the messagebox
		set_window_title("RTE Aborted! (x_x)");

		char message[512];

#if defined DEBUG_BUILD
		// Show message box with explanation
		std::snprintf(message, sizeof(message), "Runtime Error in file %s, line %i, because:\n\n%s\n\nThe last frame has been dumped to 'abortscreen.bmp'", file, line, description);
		allegro_message(message);
#else
		// Shortened and less confusing one. users have no use of knowing which source file and where.
		std::snprintf(message, sizeof(message), "%s\n\nThe last frame has been dumped to 'abortscreen.bmp'", description);
		allegro_message(message);
#endif
		// True so that the debugbreak code is run and the debugger goes there.
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool RTEAbortFunc(const std::string description, const char *file, int line) {
		return RTEAbortFunc(description.c_str(), file, line);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool RTEAssertFunc(bool expression, const char *description, const char *file, int line, bool &alwaysIgnore) {
		if (!expression) {
			// TODO: Make this display a box in the game asking whether to ignore or abort. For now, always abort.
			RTEAbortFunc(description, __FILE__, __LINE__);

			// True so that the debugbreak code is run and the debugger goes there.
			return true;
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool RTEAssertFunc(bool expression, std::string description, const char *file, int line, bool &alwaysIgnore) {
		return RTEAssertFunc(expression, description.c_str(), file, line, alwaysIgnore);
	}
}