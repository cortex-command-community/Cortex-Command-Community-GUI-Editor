//////////////////////////////////////////////////////////////////////////////////////////
// File:            FrameMan.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the FrameMan class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "FrameMan.h"
#include "UInputMan.h"
//#include "ConsoleMan.h"
//#include "SettingsMan.h"

#include "GUI.h"
#include "AllegroBitmap.h"
#include "AllegroScreen.h"


// I know this is a crime, but if I include it in FrameMan.h the whole thing will collapse due to int redefinitions in Allegro
std::mutex ScreenRelativeEffectsMutex[MAXSCREENCOUNT];

namespace RTE
{

const string FrameMan::m_ClassName = "FrameMan";

//////////////////////////////////////////////////////////////////////////////////////////
// Callback function for the allegro set_display_switch_callback. It will be called when
// focus is swtiched away to the game window. 

void DisplaySwitchOut(void)
{
    g_UInputMan.DisableMouseMoving(true);
//    g_ActivityMan.PauseActivity();
//    g_InActivity = false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Callback function for the allegro set_display_switch_callback. It will be called when
// focus is swtiched back to the game window. It will temporarily disable positioniong of
// the mouse. This is so that when focus is swtiched back to the game window, it avoids
// having the window fly away because the user clicked the title bar of the window.

void DisplaySwitchIn(void)
{
    g_UInputMan.DisableMouseMoving(false);
//    g_UInputMan.ReInitKeyboard();
}



//////////////////////////////////////////////////////////////////////////////////////////
// Method:			Translate coordinates
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Translates coordinats from scene to this bitmap
//                  
// Arguments:       bitmap offset, scene coordinates
// Return value:    Vector with translated coordinates
Vector FrameMan::GraphicalPrimitive::TranslateCoordinates(Vector targetPos, Vector scenePos)
{
	return scenePos - targetPos;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:			TranslateCoordinates
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Translates coordinats from scene to this bitmap offset producing two coordinates 
//					for 'left' scene bitmap with negative values as if scene seam is 0,0 and
//					for 'right' dcini bitmap with positive values.
//                  
// Arguments:       Bitmap to draw on, bitmap offset, scene coordinates, translated coordinates
// Return value:    Vector with translated coordinates
void FrameMan::GraphicalPrimitive::TranslateCoordinates(Vector targetPos, Vector scenePos, Vector & drawLeftPos, Vector & drawRightPos)
{
	// Unfortunately it's hard to explain how this works. It tries to represent scene bitmap as two parts
	// with center in 0,0. Right part is just plain visible part with coordinates from [0, scenewidth]
	// and left part is imaginary bitmap as if we traversed it across the seam right-to-left with coordinates [0, -scenewidth].
	// So in order to be drawn each screen coordinates calculated twice for left and right 'bitmaps' and then one of them
	// either flies away offscreen or gets drawn on the screen.
	// When we cross the seam either left or right part is actually drawn in the bitmap, and negative coords of right
	// part are compensated by view point offset coordinates when we cross the seam right to left. I really don't know
	// how to make it simpler, becuase it has so many special cases and simply wrapping all out-of-the scene coordinates
	// don't work because this way nithing will be ever draw across the seam. You're welcom to rewrite this nightmare if you can
	// I wasted a whole week on this (I can admit that I'm just too dumb for this) )))
	drawLeftPos = scenePos;
	drawRightPos = scenePos;

	if (g_SceneMan.SceneWrapsX()) 
	{
		int sceneWidth = g_SceneMan.GetSceneWidth(); 

		if (targetPos.m_X <= sceneWidth && targetPos.m_X > sceneWidth  / 2)
			targetPos.m_X = targetPos.m_X - sceneWidth;
		 
		if (drawLeftPos.m_X > 0) 
			drawLeftPos.m_X -= sceneWidth;
		else
			drawLeftPos.m_X -= sceneWidth + targetPos.m_X;
	}

	drawLeftPos.m_X -= targetPos.m_X; 
	drawRightPos.m_X -= targetPos.m_X; 

	if (g_SceneMan.SceneWrapsY()) 
	{
		int sceneHeight = g_SceneMan.GetSceneHeight(); 

		if (targetPos.m_Y <= sceneHeight && targetPos.m_Y > sceneHeight  / 2)
			targetPos.m_Y = targetPos.m_Y - sceneHeight;
		 
		if (drawLeftPos.m_Y > 0) 
			drawLeftPos.m_Y -= sceneHeight;
		else
			drawLeftPos.m_Y -= sceneHeight + targetPos.m_Y;
	}

	drawLeftPos.m_Y -= targetPos.m_Y; 
	drawRightPos.m_Y -= targetPos.m_Y; 
} 



void FrameMan::LinePrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos; 
		Vector drawEnd = m_End - targetPos; 
		 
		line(pDrawScreen, drawStart.m_X, drawStart.m_Y, drawEnd.m_X, drawEnd.m_Y, m_Color);
	} else {
		Vector drawStartLeft;
		Vector drawEndLeft;

		Vector drawStartRight;
		Vector drawEndRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);
		TranslateCoordinates(targetPos, m_End, drawEndLeft, drawEndRight);

		line(pDrawScreen, drawStartLeft.m_X, drawStartLeft.m_Y, drawEndLeft.m_X, drawEndLeft.m_Y, m_Color);
		line(pDrawScreen, drawStartRight.m_X, drawStartRight.m_Y, drawEndRight.m_X, drawEndRight.m_Y, m_Color);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this primitive on provided bitmap
//                  
// Arguments:       Bitmap to draw on, bitmap offset
// Return value:    None
void FrameMan::BoxPrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos; 
		Vector drawEnd = m_End - targetPos; 
		 
		rect(pDrawScreen, drawStart.m_X, drawStart.m_Y, drawEnd.m_X, drawEnd.m_Y, m_Color);
	} else {
		Vector drawStartLeft;
		Vector drawEndLeft;

		Vector drawStartRight;
		Vector drawEndRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);
		TranslateCoordinates(targetPos, m_End, drawEndLeft, drawEndRight);

		rect(pDrawScreen, drawStartLeft.m_X, drawStartLeft.m_Y, drawEndLeft.m_X, drawEndLeft.m_Y, m_Color);
		rect(pDrawScreen, drawStartRight.m_X, drawStartRight.m_Y, drawEndRight.m_X, drawEndRight.m_Y, m_Color);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this primitive on provided bitmap
//                  
// Arguments:       Bitmap to draw on, bitmap offset
// Return value:    None
void FrameMan::BoxFillPrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos; 
		Vector drawEnd = m_End - targetPos; 
		 
		rectfill(pDrawScreen, drawStart.m_X, drawStart.m_Y, drawEnd.m_X, drawEnd.m_Y, m_Color);
	} else {
		Vector drawStartLeft;
		Vector drawEndLeft;

		Vector drawStartRight;
		Vector drawEndRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);
		TranslateCoordinates(targetPos, m_End, drawEndLeft, drawEndRight);

		rectfill(pDrawScreen, drawStartLeft.m_X, drawStartLeft.m_Y, drawEndLeft.m_X, drawEndLeft.m_Y, m_Color);
		rectfill(pDrawScreen, drawStartRight.m_X, drawStartRight.m_Y, drawEndRight.m_X, drawEndRight.m_Y, m_Color);
	}
}


void FrameMan::CirclePrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos; 
		 
		circle(pDrawScreen, drawStart.m_X, drawStart.m_Y, m_Radius, m_Color);
	} else {
		Vector drawStartLeft;
		Vector drawStartRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);

		circle(pDrawScreen, drawStartLeft.m_X, drawStartLeft.m_Y, m_Radius, m_Color);
		circle(pDrawScreen, drawStartRight.m_X, drawStartRight.m_Y, m_Radius, m_Color);
	}
}


void FrameMan::CircleFillPrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos; 
		 
		circlefill(pDrawScreen, drawStart.m_X, drawStart.m_Y, m_Radius, m_Color);
	} else {
		Vector drawStartLeft;
		Vector drawStartRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);

		circlefill(pDrawScreen, drawStartLeft.m_X, drawStartLeft.m_Y, m_Radius, m_Color);
		circlefill(pDrawScreen, drawStartRight.m_X, drawStartRight.m_Y, m_Radius, m_Color);
	}
}

void FrameMan::TextPrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos; 
		AllegroBitmap pPlayerGUIBitmap(pDrawScreen);

		if (m_IsSmall)
			g_FrameMan.GetSmallFont()->DrawAligned(&pPlayerGUIBitmap, drawStart.m_X, drawStart.m_Y, m_Text, m_Alignment);
		else
			g_FrameMan.GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, drawStart.m_X, drawStart.m_Y, m_Text, m_Alignment);
	} else {
		Vector drawStartLeft;
		Vector drawStartRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);

		AllegroBitmap pPlayerGUIBitmap(pDrawScreen);
		if (m_IsSmall)
		{
			g_FrameMan.GetSmallFont()->DrawAligned(&pPlayerGUIBitmap, drawStartLeft.m_X, drawStartLeft.m_Y, m_Text, m_Alignment);
			g_FrameMan.GetSmallFont()->DrawAligned(&pPlayerGUIBitmap, drawStartRight.m_X, drawStartRight.m_Y, m_Text, m_Alignment);
		}
		else
		{
			g_FrameMan.GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, drawStartLeft.m_X, drawStartLeft.m_Y, m_Text, m_Alignment);
			g_FrameMan.GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, drawStartRight.m_X, drawStartRight.m_Y, m_Text, m_Alignment);
		}
	}
}


void FrameMan::BitmapPrimitive::Draw(BITMAP *pDrawScreen, Vector targetPos) 
{
	if (!m_pBitmap)
		return;

	Vector pos[2];
	int passes = 0;

	if (!g_SceneMan.SceneWrapsX() && !g_SceneMan.SceneWrapsY()) 
	{ 
		Vector drawStart = m_Start - targetPos;

		pos[0] = drawStart;
		passes = 1;

	} else {
		Vector drawStartLeft;
		Vector drawStartRight;

		TranslateCoordinates(targetPos, m_Start, drawStartLeft, drawStartRight);

		pos[0] = drawStartLeft;
		pos[1] = drawStartRight;
		passes = 2;
	}

	Matrix rotation = Matrix(m_RotAngle);

	for (int i = 0 ; i < 2 ; i++)
	{
		// Take into account the h-flipped pivot point
		pivot_scaled_sprite(pDrawScreen,
							m_pBitmap,
							pos[i].GetFloorIntX(),
							pos[i].GetFloorIntY(),
							m_pBitmap->w / 2,
							m_pBitmap->h / 2,
							ftofix(rotation.GetAllegroAngle()),
							ftofix(1.0));
	}
}




//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this FrameMan, effectively
//                  resetting the members of this abstraction level only.

void FrameMan::Clear()
{
    m_ResX = 640;
    m_ResY = 480;
    m_BPP = 8;
    m_pBackBuffer8 = 0;
	m_pBackBuffer32 = 0;
    m_pScreendumpBuffer = 0;
    m_PaletteFile.Reset();
    m_pPaletteDataFile = 0;
    m_BlackColor = 245;
    m_AlmostBlackColor = 245;
    m_Fullscreen = false;
    m_NxWindowed = 1;
    m_NxFullscreen = 1;
    m_NewNxFullscreen = 1;
    m_PostProcessing = false;
    m_PostPixelGlow = false;
    m_pYellowGlow = 0;
    m_YellowGlowHash = 0;
    m_pRedGlow = 0;
    m_RedGlowHash = 0;
    m_pBlueGlow = 0;
    m_BlueGlowHash = 0;
    m_PostScreenEffects.clear();
    m_HSplit = false;
    m_VSplit = false;
    m_HSplitOverride = false;
    m_VSplitOverride = false;
    m_pPlayerScreen = 0;
    m_PlayerScreenWidth = 0;
    m_PlayerScreenHeight = 0;
    m_PPM = 0;
    m_pFrameTimer = 0;
    m_MSPFs.clear();
    m_MSPFAverage = 0;
    m_SimSpeed = 1.0;
    m_pGUIScreen = 0;
    m_pLargeFont = 0;
    m_pSmallFont = 0;
    m_ShowPerfStats = false;

	m_NetworkFrameCurrent = 0;
	m_NetworkFrameReady = 1;

	// Delete all allocated primitives and clear the list
	ClearPrimitivesList();

    for (int i = 0; i < MAXSCREENCOUNT; ++i)
    {
        m_ScreenText[i].clear();
        m_TextDuration[i] = -1;
        m_TextDurationTimer[i].Reset();
        m_TextBlinking[i] = 0;
        m_TextCentered[i] = false;
        m_FlashScreenColor[i] = -1;
        m_FlashedLastFrame[i] = false;
        m_FlashTimer[i].Reset();

		m_ScreenRelativeEffects->clear();
    }
    m_TextBlinkTimer.Reset();
}



//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the FrameMan object ready for use, which is to be used with
//                  SettingsMan first.

int FrameMan::Create()
{
    m_pFrameTimer = new Timer();

    // Init allegro's graphics
    set_color_depth(m_BPP);

//    GFX_MODE_LIST *pList = get_gfx_mode_list(GFX_DIRECTX_ACCEL);
//    destroy_gfx_mode_list(pList);

    // Refuse windowed multiplier if the resolution is too high
    if (m_ResX > 1024)
        m_NxWindowed = 1;
/*
    // Force double virtual fullscreen res if the res is too high
    if (m_ResX >= 1280)// && m_Fullscreen && m_NxFullscreen == 1)
    {
        m_NxFullscreen = 2;
        m_NewResX = m_ResX /= m_NxFullscreen;
        m_NewResY = m_ResY /= m_NxFullscreen;
    }
    // Not oversized resolution
    else
        m_NxFullscreen = 1;
*/

	int fullscreenGfxDriver = GFX_AUTODETECT_FULLSCREEN;
	int windowedGfxDriver = GFX_AUTODETECT_WINDOWED;

#if defined(__APPLE__)
	//fullscreenGfxDriver = GFX_QUARTZ_FULLSCREEN;
	//windowedGfxDriver = GFX_QUARTZ_WINDOW
#elif defined(__unix__)
	//fullscreenGfxDriver = GFX_XWINDOWS_FULLSCREEN;
	//windowedGfxDriver = GFX_XWINDOWS;
#else
    fullscreenGfxDriver = GFX_DIRECTX_ACCEL;

	if (g_SettingsMan.ForceSoftwareGfxDriver()) 
		fullscreenGfxDriver = GFX_DIRECTX_SOFT;
	if (g_SettingsMan.ForceSafeGfxDriver()) 
		fullscreenGfxDriver = GFX_DIRECTX_SAFE;
	if (g_SettingsMan.ForceOverlayedWindowGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_OVL;
	if (g_SettingsMan.ForceNonOverlayedWindowGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_WIN;
	if (g_SettingsMan.ForceVirtualFullScreenGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_WIN_BORDERLESS;
#endif // defined(__APPLE__)


    if (set_gfx_mode(m_Fullscreen ? fullscreenGfxDriver : windowedGfxDriver, m_Fullscreen ? m_ResX * m_NxFullscreen : m_ResX * m_NxWindowed, m_Fullscreen ? m_ResY * m_NxFullscreen : m_ResY * m_NxWindowed, 0, 0) != 0)
    {
		g_ConsoleMan.PrintString("Failed to set gfx mode, trying different windowed scaling.");

		// If player somehow managed to set up a windowed 2X mode and then set a resoultion higher than physical, then disable 2X resolution
		m_NxWindowed = m_NxWindowed == 2 ? 1 : m_NxWindowed;
		if (set_gfx_mode(m_Fullscreen ? fullscreenGfxDriver : windowedGfxDriver, m_Fullscreen ? m_ResX * m_NxFullscreen : m_ResX * m_NxWindowed, m_Fullscreen ? m_ResY * m_NxFullscreen : m_ResY * m_NxWindowed, 0, 0) != 0)
	    {
			g_ConsoleMan.PrintString("Failed to set gfx mode, trying different fullscreen scaling.");

			// TODO: this is whack if we're attempting windowed
			// Oops, failed to set fullscreen mode, try a different fullscreen scaling
			m_NewNxFullscreen = m_NxFullscreen = m_NxFullscreen == 1 ? 2 : 1;
			if (set_gfx_mode(m_Fullscreen ? fullscreenGfxDriver : windowedGfxDriver, m_Fullscreen ? m_ResX * m_NxFullscreen : m_ResX * m_NxWindowed, m_Fullscreen ? m_ResY * m_NxFullscreen : m_ResY * m_NxWindowed, 0, 0) != 0)
			{
				// Oops, failed to set the resolution specified in the setting file, so default to a safe one instead
				allegro_message("Unable to set specified graphics mode because: %s!\n\nNow trying to default back to VGA...", allegro_error);
				if (set_gfx_mode(m_Fullscreen ? GFX_AUTODETECT_FULLSCREEN : GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0)
				{
					set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
					allegro_message("Unable to set any graphics mode because %s!", allegro_error);
					return 1;
				}
				// Successfully reverted to VGA, so set that as the current resolution
				m_ResX = m_NewResX = 640;
				m_ResY = m_NewResY = 480;
				m_NxWindowed = 1;
				m_NewNxFullscreen = m_NxFullscreen = 1;
			}
		}
    }

    // Clear the screen buffer so it doesn't flash pink
    if (m_BPP == 8)
        clear_to_color(screen, m_BlackColor);
    else
        clear_to_color(screen, 0);

    // Sets the allowed color conversions when loading bitmaps from files
    set_color_conversion(COLORCONV_MOST);

    // Load and set the palette
    if (!LoadPalette(m_PaletteFile.GetDataPath()))
        return -1;

    // Set the switching mode; what happens when the app window is switched to and fro
    set_display_switch_mode(SWITCH_BACKGROUND);
//    set_display_switch_mode(SWITCH_PAUSE);
    set_display_switch_callback(SWITCH_OUT, DisplaySwitchOut);
    set_display_switch_callback(SWITCH_IN, DisplaySwitchIn);

    // Create transparency color table
    PALETTE ccpal;
    get_palette(ccpal);
    create_trans_table(&m_LessTransTable, ccpal, 192, 192, 192, 0);
    create_trans_table(&m_HalfTransTable, ccpal, 128, 128, 128, 0);
    create_trans_table(&m_MoreTransTable, ccpal, 64, 64, 64, 0);
    // Set the one Allegro currently uses
    color_map = &m_HalfTransTable;

    // Create the back buffer, this is still in 8bpp, we will do any postprocessing on the PostProcessing bitmap
    m_pBackBuffer8 = create_bitmap_ex(8, m_ResX, m_ResY);
    clear_to_color(m_pBackBuffer8, m_BlackColor);



    // Create the post processing buffer if in 32bpp video mode, it'll be used for glow effects etc
    if (get_color_depth() == 32 && m_BPP == 32)
    {
        // 32bpp so we can make ze cool effects. Everyhting up to this is 8bpp, including the back buffer
        m_pBackBuffer32 = create_bitmap_ex(32, m_ResX, m_ResY);
//        clear_to_color(m_pBackBuffer32, m_BlackColor);

// TODO: Make more robust and load more glows!
        ContentFile glowFile("Base.rte/Effects/Glows/YellowTiny.bmp");
        m_pYellowGlow = glowFile.GetAsBitmap();
		m_YellowGlowHash = glowFile.GetHash();
        glowFile.SetDataPath("Base.rte/Effects/Glows/RedTiny.bmp");
        m_pRedGlow = glowFile.GetAsBitmap();
        m_RedGlowHash = glowFile.GetHash();
        glowFile.SetDataPath("Base.rte/Effects/Glows/BlueTiny.bmp");
        m_pBlueGlow = glowFile.GetAsBitmap();
        m_BlueGlowHash = glowFile.GetHash();

		m_pTempEffectBitmap_16 = create_bitmap_ex(32, 16, 16);
		m_pTempEffectBitmap_32 = create_bitmap_ex(32, 32, 32);
		m_pTempEffectBitmap_64 = create_bitmap_ex(32, 64, 64);
		m_pTempEffectBitmap_128 = create_bitmap_ex(32, 128, 128);
		m_pTempEffectBitmap_256 = create_bitmap_ex(32, 256, 256);
	}

    m_PlayerScreenWidth = m_pBackBuffer8->w;
    m_PlayerScreenHeight = m_pBackBuffer8->h;

    // Create the splitscreen buffer
    if (m_HSplit || m_VSplit)
    {
// TODO: Make this a Video-only memory bitmap: create_video_bitmap"
        m_pPlayerScreen = create_bitmap_ex(8, m_ResX / (m_VSplit ? 2 : 1), m_ResY / (m_HSplit ? 2 : 1));
        clear_to_color(m_pPlayerScreen, m_BlackColor);
        set_clip_state(m_pPlayerScreen, 1);

        // Update these to represent the split screens
        m_PlayerScreenWidth = m_pPlayerScreen->w;
        m_PlayerScreenHeight = m_pPlayerScreen->h;
    }


#if __USE_SOUND_GORILLA
	m_PerfCounterNames[PERF_SOUND] = "Sound";
#endif
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsValidResolution	
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns true if this resolution is supported
bool FrameMan::IsValidResolution(int width, int height) const
{
	int actualWidth = width;
	int actualHeight = height;

	// If width is greater than 1280, the game will switch itself in 2X mode
	// lowering actual resolution twice
	if (width >= 1280)
	{
		actualWidth = width / 2;
		actualHeight = height / 2;
	}

	if (actualWidth < 360 || actualHeight < 360)
		return false;
	else
		return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int FrameMan::ReadProperty(std::string propName, Reader &reader)
{
    if (propName == "ResolutionX")
    {
        reader >> m_ResX;
    }
    else if (propName == "ResolutionY")
    {
        reader >> m_ResY;
    }
    else if (propName == "TrueColorMode")
    {
        bool trueColor;
        reader >> trueColor;
        m_BPP = trueColor ? 32 : 8;
    }
    else if (propName == "PaletteFile")
        reader >> m_PaletteFile;
    else if (propName == "Fullscreen")
    {
        reader >> m_Fullscreen;
#ifdef FULLSCREEN_OVERRIDE
        m_Fullscreen = true;
#endif // FULLSCREEN_OVERRIDE
    }
    else if (propName == "NxWindowed")
        reader >> m_NxWindowed;
    else if (propName == "NxFullscreen")
    {
        reader >> m_NxFullscreen;
        m_NewNxFullscreen = m_NxFullscreen;
    }
    else if (propName == "PostProcessing")
        reader >> m_PostProcessing;
    else if (propName == "PostPixelGlow")
        reader >> m_PostPixelGlow;
    else if (propName == "PixelsPerMeter")
    {
        reader >> m_PPM;
        m_MPP = 1 / m_PPM;
        // Calculate the Pixle per Litre and Litre per Pixel
        float cmPP = 100 / m_PPM;
        float LVolume = 10 * 10 * 10;
        m_PPL = LVolume / (cmPP * cmPP * cmPP);
        m_LPP = 1 / m_PPL;
    }
    else if (propName == "HSplitScreen")
        reader >> m_HSplitOverride;
    else if (propName == "VSplitScreen")
        reader >> m_VSplitOverride;
    else
        // See if the base class(es) can find a match instead
        return Serializable::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ResetSplitScreens
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets new values for the split screen configuration.

void FrameMan::ResetSplitScreens(bool hSplit, bool vSplit)
{
    // Free the previous splitscreen, if any
    if (m_pPlayerScreen)
        release_bitmap(m_pPlayerScreen);

	// Override screen splitting according to settings if needed
	if ((hSplit || vSplit) && !(hSplit && vSplit) && (m_HSplitOverride || m_VSplitOverride))
	{
		hSplit = m_HSplitOverride;
		vSplit = m_VSplitOverride;
	}

    m_HSplit = hSplit;
    m_VSplit = vSplit;

    // Create the splitscreen buffer
    if (m_HSplit || m_VSplit)
    {
// TODO: Make this a Video-only memory bitmap: create_video_bitmap"
        m_pPlayerScreen = create_bitmap_ex(8, g_FrameMan.GetResX() / (m_VSplit ? 2 : 1), g_FrameMan.GetResY() / (m_HSplit ? 2 : 1));
        clear_to_color(m_pPlayerScreen, m_BlackColor);
        set_clip_state(m_pPlayerScreen, 1);

        // Update these to represent the split screens
        m_PlayerScreenWidth = m_pPlayerScreen->w;
        m_PlayerScreenHeight = m_pPlayerScreen->h;
    }
    // No splits, so set the screen dimensions equal to the back buffer
    else
    {
        m_PlayerScreenWidth = m_pBackBuffer8->w;
        m_PlayerScreenHeight = m_pBackBuffer8->h;
    }

    // Reset the flashes
    for (int i = 0; i < MAXSCREENCOUNT; ++i)
    {
        m_FlashScreenColor[i] = -1;
        m_FlashedLastFrame[i] = false;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this FrameMan with a Writer for
//                  later recreation with Create(Reader &reader);

int FrameMan::Save(Writer &writer) const
{
    Serializable::Save(writer);

    writer.NewProperty("ResolutionX");
    writer << m_ResX;
    writer.NewProperty("ResolutionY");
    writer << m_ResY;
    writer.NewProperty("TrueColorMode");
    writer << (m_BPP == 32);
    writer.NewProperty("PaletteFile");
    writer << m_PaletteFile;
    writer.NewProperty("Fullscreen");
    writer << m_Fullscreen;
    writer.NewProperty("PostProcessing");
    writer << m_PostProcessing;
    writer.NewProperty("PostPixelGlow");
    writer << m_PostPixelGlow;
    writer.NewProperty("PixelsPerMeter");
    writer << m_PPM;
    writer.NewProperty("HSplitScreen");
    writer << m_HSplitOverride;
    writer.NewProperty("VSplitScreen");
    writer << m_VSplitOverride;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the FrameMan object.

void FrameMan::Destroy()
{
    destroy_bitmap(m_pBackBuffer8);

    destroy_bitmap(m_pBackBuffer32);
    destroy_bitmap(m_pPlayerScreen);
    if (m_pPaletteDataFile)
        unload_datafile_object(m_pPaletteDataFile);
    delete m_pGUIScreen;
    delete m_pLargeFont;
    delete m_pSmallFont;
    delete m_pFrameTimer;

    g_TimerMan.Destroy();

    Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetLargeFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the large font from the GUI engine's current skin. Ownership is
//                  NOT transferred!

GUIFont * FrameMan::GetLargeFont()
{
    if (!m_pLargeFont)
    {
        if (!m_pGUIScreen)
        {
            m_pGUIScreen = new AllegroScreen(m_pBackBuffer8);
        }
        m_pLargeFont = new GUIFont("FatFont");
        m_pLargeFont->Load(m_pGUIScreen, "Base.rte/GUIs/Skins/Base/fatfont.bmp");
    }
    return m_pLargeFont;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSmallFont
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the large font from the GUI engine's current skin. Ownership is
//                  NOT transferred!

GUIFont * FrameMan::GetSmallFont()
{
    if (!m_pSmallFont)
    {
        if (!m_pGUIScreen)
        {
            m_pGUIScreen = new AllegroScreen(m_pBackBuffer8);
        }
        m_pSmallFont = new GUIFont("SmallFont");
        m_pSmallFont->Load(m_pGUIScreen, "Base.rte/GUIs/Skins/Base/smallfont.bmp");
    }
    return m_pSmallFont;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateTextHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns max text height

int FrameMan::CalculateTextHeight(std::string text, int maxWidth, bool isSmall)
{
	if (isSmall)
		return GetSmallFont()->CalculateHeight(text, maxWidth);
	else
		return GetLargeFont()->CalculateHeight(text, maxWidth);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CalculateTextWidth
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns max text height

int FrameMan::CalculateTextWidth(std::string text, bool isSmall)
{
	if (isSmall)
		return GetSmallFont()->CalculateWidth(text);
	else
		return GetLargeFont()->CalculateWidth(text);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the message to be displayed on top of each player's screen

void FrameMan::SetScreenText(const std::string &msg, int which, int blinkInterval, int displayDuration, bool centered)
{
    if (which >= 0 && which < MAXSCREENCOUNT)
    {
        // See if we can overwrite the previous message
        if (m_TextDurationTimer[which].IsPastRealMS(m_TextDuration[which]))
        {
            m_ScreenText[which] = msg;
            m_TextDuration[which] = displayDuration;
            m_TextDurationTimer[which].Reset();
            m_TextBlinking[which] = blinkInterval;
            m_TextCentered[which] = centered;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetScreenText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the message to be displayed on top of each player's screen

std::string FrameMan::GetScreenText(int which) const
{
    if (which >= 0 && which < MAXSCREENCOUNT)
		return m_ScreenText[which];
	else
		return "";
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          LoadPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Loads a palette from a .dat file and sets it as the currently used
//                  screen palette.

bool FrameMan::LoadPalette(std::string palettePath)
{
    // Look for the '#' denoting the divider between the datafile and the datafile object's name is
    // If we find one, that means we're trying to load from a datafile, otehrwise it's from an exposed bitmap
    int separatorPos = palettePath.rfind('#');

    // Check whether we're trying to load a palette from an exposed bitmap or from a datafile
    if (separatorPos == -1 || separatorPos >= palettePath.length() - 1)
    {
        // Just going to discard the bitmap, we're only interested in the palette
        BITMAP *tempBitmap;
        PALETTE newPalette;
        if (!(tempBitmap = load_bitmap(palettePath.c_str(), newPalette)))
            RTEAbort(("Failed to load palette from bitmap with following path:\n\n" + palettePath).c_str());

        // Set the current palette
        set_palette(newPalette);

        // Update what black is now with the loaded palette
        m_BlackColor = bestfit_color(newPalette, 0, 0, 0);
        m_AlmostBlackColor = bestfit_color(newPalette, 5, 5, 5);

        // Free the temp bitmap that had the palette
        destroy_bitmap(tempBitmap);
    }
    // Loading from a datafile
    else
    {
        // Get the Path only, without the object name, using the separator index as length limiter
        string datafilePath = palettePath.substr(0, separatorPos);
        // Adjusting to the true first character of the datafile object's name string.
        string objectName = palettePath.substr(separatorPos + 1);

        // Try loading the datafile from the specified path + object names.
        DATAFILE *pTempFile = load_datafile_object(datafilePath.c_str(), objectName.c_str());

        // Make sure we loaded properly.
        if (!pTempFile || !pTempFile->dat || pTempFile->type != DAT_PALETTE)
            RTEAbort(("Failed to load palette datafile object with following path and name:\n\n" + palettePath).c_str());

        // Now when we know it's valid, go ahead and replace the old palette with it
        if (m_pPaletteDataFile)
            unload_datafile_object(m_pPaletteDataFile);
        m_pPaletteDataFile = pTempFile;

        // Set the current palette
        set_palette(*((PALETTE *)m_pPaletteDataFile->dat));

        // Update what black is now with the loaded palette
        m_BlackColor = bestfit_color(*((PALETTE *)m_pPaletteDataFile->dat), 0, 0, 0);
        m_AlmostBlackColor = bestfit_color(*((PALETTE *)m_pPaletteDataFile->dat), 5, 5, 5);
    }

    // Indicate success
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FadeInPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Fades the palette in from black at a specified speed.

void FrameMan::FadeInPalette(int fadeSpeed)
{
    if (fadeSpeed < 1)
        fadeSpeed = 1;
    if (fadeSpeed > 64)
        fadeSpeed = 64;

    PALETTE pal;
    get_palette(pal);
    fade_in(pal, fadeSpeed);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FadeOutPalette
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Fades the palette out to black at a specified speed.

void FrameMan::FadeOutPalette(int fadeSpeed)
{
    if (fadeSpeed < 1)
        fadeSpeed = 1;
    if (fadeSpeed > 64)
        fadeSpeed = 64;

    fade_out(fadeSpeed);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTransTable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a specific precalculated transperency table which is used for
//                  any subsequent transparency drawing.

void FrameMan::SetTransTable(TransperencyPreset transSetting)
{
    if (transSetting == LessTrans)
        color_map = &m_LessTransTable;
    else if (transSetting == MoreTrans)
        color_map = &m_MoreTransTable;
    else
        color_map = &m_HalfTransTable;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SaveScreenToBMP
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Dumps a bitmap to a 8bpp BMP file.

int FrameMan::SaveScreenToBMP(const char *namebase)
{
    int filenumber = 0;
    char fullfilename[256];
    int maxFileTrys = 1000;

    // Make sure its not a 0 namebase
    if (namebase == 0 || strlen(namebase) <= 0)
        return -1;

    do {
        // Check for the file namebase001.bmp; if it exists, try 002, etc.
        sprintf_s(fullfilename, sizeof(fullfilename), "%s%03i.bmp", namebase, filenumber++);
        if (!exists(fullfilename)) {
            break;
        }
    } while (filenumber < maxFileTrys);

    // Save out the screen bitmap, after making a copy of it, faster sometimes
    if (screen) {
        if (!m_pScreendumpBuffer)
            m_pScreendumpBuffer = create_bitmap(screen->w, screen->h);
        blit(screen, m_pScreendumpBuffer, 0, 0, 0, 0, screen->w, screen->h);
        PALETTE palette;
        get_palette(palette);
        save_bmp(fullfilename, m_pScreendumpBuffer, palette);

        g_ConsoleMan.PrintString("SYSTEM: Screen was dumped to: " + string(fullfilename));
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ToggleFullscreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Toggles to and from fullscreen and windowed mode.

int FrameMan::ToggleFullscreen()
{
    // Save the palette so we can re-set it after the change.
    PALETTE pal;
    get_palette(pal);

    // Refuse windowed multiplier if the resolution is too high
    if (m_ResX > 1024)
        m_NxWindowed = 1;
/*
    // Force double virtual fullscreen res if the res is too high
    if (m_ResX >= 1280)// && m_Fullscreen && m_NxFullscreen == 1)
    {
        m_NxFullscreen = 2;
        m_NewResX = m_ResX /= m_NxFullscreen;
        m_NewResY = m_ResY /= m_NxFullscreen;
    }
    // Not oversized resolution
//    else
//        m_NxFullscreen = 1;
*/
    // Need to save these first for recovery attempts to work (screen might be 0)
    int resX = m_ResX;
    int resY = m_ResY;


	int fullscreenGfxDriver = GFX_AUTODETECT_FULLSCREEN;
	int windowedGfxDriver = GFX_AUTODETECT_WINDOWED;

#if defined(__APPLE__)
	//fullscreenGfxDriver = GFX_QUARTZ_FULLSCREEN;
	//windowedGfxDriver = GFX_QUARTZ_WINDOW
#elif defined(__unix__)
	//fullscreenGfxDriver = GFX_XWINDOWS_FULLSCREEN;
	//windowedGfxDriver = GFX_XWINDOWS;
#else
    fullscreenGfxDriver = GFX_DIRECTX_ACCEL;
	//windowedGfxDriver = GFX_DIRECTX_WIN;

	if (g_SettingsMan.ForceSoftwareGfxDriver()) 
		fullscreenGfxDriver = GFX_DIRECTX_SOFT;
	if (g_SettingsMan.ForceSafeGfxDriver()) 
		fullscreenGfxDriver = GFX_DIRECTX_SAFE;
	if (g_SettingsMan.ForceOverlayedWindowGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_OVL;
	if (g_SettingsMan.ForceNonOverlayedWindowGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_WIN;
	if (g_SettingsMan.ForceVirtualFullScreenGfxDriver()) 
		windowedGfxDriver = GFX_DIRECTX_WIN_BORDERLESS;
#endif // defined(__APPLE__)


    // Switch to windowed mode
    if (m_Fullscreen)
    {
        if (set_gfx_mode(windowedGfxDriver, resX * m_NxWindowed, resY * m_NxWindowed, 0, 0) != 0)
        {
            // Oops, failed to set windowed mode, so go back to fullscreen
            if (set_gfx_mode(fullscreenGfxDriver, resX * m_NxFullscreen, resY * m_NxFullscreen, 0, 0) != 0)
            {
                // Can't go back to fullscreen either! total fail
                set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
                allegro_message("Unable to set back to fullscreen mode because: %s!", allegro_error);
                return 1;
            }

            // Successfully reverted back to fullscreen
            g_ConsoleMan.PrintString("ERROR: Failed to switch to windowed mode, reverted back to fullscreen!");
            m_Fullscreen = true;
            // Reset the palette.
            set_palette(pal);
            // Make sure everything gets caught up after the switch
            rest(2000);
            return 1;
        }
        // Make sure everything gets caught up after the switch
        rest(2000);
        
        g_ConsoleMan.PrintString("SYSTEM: Switched to windowed mode");

        m_Fullscreen = false;
    }
    // Switch to fullscreen
    else
    {
        if (set_gfx_mode(fullscreenGfxDriver, resX * m_NxFullscreen, resY * m_NxFullscreen, 0, 0) != 0)
        {
            // Oops, failed to set fullscreen mode, try a different fullscreen scaling
            m_NewNxFullscreen = m_NxFullscreen = m_NxFullscreen == 1 ? 2 : 1;
            if (set_gfx_mode(fullscreenGfxDriver, resX * m_NxFullscreen, resY * m_NxFullscreen, 0, 0) != 0)
            {
                // That didn't work either, so go back to windowed
                allegro_message("Unable enter fullscreen mode because: %s!\n\nWill try to revert to windowed mode now...", allegro_error);
                if (set_gfx_mode(windowedGfxDriver, resX * m_NxWindowed, resY * m_NxWindowed, 0, 0) != 0)
                {
                    // Can't go back to windowed either! total fail
                    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
                    allegro_message("Unable to set back to windowed mode because: %s!", allegro_error);
                    return 1;
                }

                // Successfully reverted back to windowed
                g_ConsoleMan.PrintString("ERROR: Failed to switch to fullscreen mode, reverted back to windowed!");
                m_Fullscreen = false;
                // Make sure everything gets caught up after the switch
                rest(1500);
                // Reset the palette.
                set_palette(pal);
                return 1;
            }
        }
        // Make sure everything gets caught up after the switch
        rest(1500);

        g_ConsoleMan.PrintString("SYSTEM: Switched to fullscreen mode");

        m_Fullscreen = true;
    }

    // Reset the palette.
    set_palette(pal);

    // Adjust the speed of the mouse according to 2x of screen
    float mouseDenominator = IsFullscreen() ? NxFullscreen() : NxWindowed();
    // If Nx fullscreen, adjust the mouse speed accordingly
    if (g_FrameMan.IsFullscreen() && g_FrameMan.NxFullscreen() > 1)
        set_mouse_speed(1, 1);
    else
        set_mouse_speed(2, 2);
    set_mouse_range(0, 0, (GetResX() * mouseDenominator) - 3, (GetResY() * mouseDenominator) - 3);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SwitchWindowMultiplier
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets and switches to a new windowed mode multiplier.

int FrameMan::SwitchWindowMultiplier(int multiplier)
{
    // Sanity check input
    if (multiplier <= 0 || multiplier > 4 || multiplier == m_NxWindowed)
        return -1;

    // No need to do anyhting else if we're in fullscreen already
    if (m_Fullscreen)
    {
        m_NxWindowed = multiplier;
        return 0;
    }

    // Refuse windowed multiplier if the resolution is too high
    if (m_ResX > 1024)
        m_NxWindowed = 1;

    // Save the palette so we can re-set it after the switch.
    PALETTE pal;
    get_palette(pal);
    // Need to save these first for recovery attempts to work (screen might be 0)
    int resX = m_ResX;
    int resY = m_ResY;

    // Switch to new windowed mode
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, resX * multiplier, resY * multiplier, 0, 0) != 0)
    {
        // Oops, failed to set windowed mode, so go back to previous multiplier
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, resX * m_NxWindowed, resY * m_NxWindowed, 0, 0) != 0)
        {
            // Can't go back to prev mode either! total fail
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to set back to previous windowed mode multiplier because: %s!", allegro_error);
            return 1;
        }

        // Successfully reverted back to previous setting
        g_ConsoleMan.PrintString("ERROR: Failed to switch to new windowed mode multiplier, reverted back to previous setting!");
        // Reset the palette.
        set_palette(pal);
        // Make sure everything gets caught up after the switch
        rest(2000);
        return 1;
    }
    // Make sure everything gets caught up after the switch
    rest(2000);
    
    g_ConsoleMan.PrintString("SYSTEM: Switched to different windowed mode multiplier");
    // Save the new multiplier
    m_NxWindowed = multiplier;
    m_Fullscreen = false;

    // Reset the palette.
    set_palette(pal);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          PostProcess
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Takes the current state of the back buffer, copies it, and adds post
//                  processing effects on top like glows etc. Only works in 32bpp mode.

void FrameMan::PostProcess()
{
    if (!m_PostProcessing)
        return;

    // First copy the current 8bpp backbuffer to the 32bpp buffer; we'll add effects to it
    blit(m_pBackBuffer8, m_pBackBuffer32, 0, 0, 0, 0, m_pBackBuffer8->w, m_pBackBuffer8->h);

	// Set the screen blender mode for glows
//    set_alpha_blender();
    set_screen_blender(128, 128, 128, 128);

//    acquire_bitmap(m_pBackBuffer8);
//    acquire_bitmap(m_pBackBuffer32);

    // Randomly sample the entire backbuffer, looking for pixels to put a glow on
    // NOTE THIS IS SLOW, especially on higher resolutions!
    if (m_PostPixelGlow)
    {
        int x = 0, y = 0, startX = 0, startY = 0, endX = 0, endY = 0, testpixel = 0;

        for (list<Box>::iterator bItr = m_PostScreenGlowBoxes.begin(); bItr != m_PostScreenGlowBoxes.end(); ++bItr)
        {
            startX = (*bItr).m_Corner.m_X;
            startY = (*bItr).m_Corner.m_Y;
            endX = startX + (*bItr).m_Width;
            endY = startY + (*bItr).m_Height;
            testpixel = 0;

            // Sanity check a little at least
            if (startX < 0 || startX >= m_pBackBuffer8->w || startY < 0 || startY >= m_pBackBuffer8->h ||
                endX < 0 || endX >= m_pBackBuffer8->w || endY < 0 || endY >= m_pBackBuffer8->h)
                continue;

// TODO: REMOVE TEMP DEBUG
//            rect(m_pBackBuffer32, startX, startY, endX, endY, g_RedColor);

            for (y = startY; y < endY; ++y)
            {
                for (x = startX; x < endX; ++x)
                {
                    testpixel = _getpixel(m_pBackBuffer8, x, y);

                    // YELLOW
                    if ((testpixel == g_YellowGlowColor && PosRand() < 0.9) || testpixel == 98 || (testpixel == 120 && PosRand() < 0.7))// || testpixel == 39 || testpixel == 86 || testpixel == 47 || testpixel == 48 || testpixel == 116)
                        draw_trans_sprite(m_pBackBuffer32, m_pYellowGlow, x - 2, y - 2);
                    // RED
        //            if (testpixel == 13)
        //                draw_trans_sprite(m_pBackBuffer32, m_pRedGlow, x - 2, y - 2);
                    // BLUE
                    if (testpixel == 166)
                        draw_trans_sprite(m_pBackBuffer32, m_pBlueGlow, x - 2, y - 2);
                }
            }            
        }
    }
/* This one is even slower than above method
    // How many samples to make
    int samples = 640 * 480 * 0.7;

    for (int i = 0; i < samples; ++i)
    {
        x = m_pBackBuffer8->w * PosRand();
//        y = m_pBackBuffer8->h * PosRand();
        testpixel = _getpixel(m_pBackBuffer8, x, y);
        // YELLOW
        if (testpixel == g_YellowGlowColor || testpixel == 98 || testpixel == 120)// || testpixel == 39 || testpixel == 86 || testpixel == 47 || testpixel == 48 || testpixel == 116)
            draw_trans_sprite(m_pBackBuffer32, m_pYellowGlow, x - 2, y - 2);
        // RED
//            if (testpixel == 13)
//                draw_trans_sprite(m_pBackBuffer32, m_pRedGlow, x - 2, y - 2);
        // BLUE
        if (testpixel == 166)
            draw_trans_sprite(m_pBackBuffer32, m_pBlueGlow, x - 2, y - 2);
    }
*/

    // Draw all the scene screen effects accumulated this frame
    BITMAP *pBitmap = 0;
    int effectPosX = 0;
    int effectPosY = 0;
    int strength = 0;
	float angle = 0;

    for (list<PostEffect>::iterator eItr = m_PostScreenEffects.begin(); eItr != m_PostScreenEffects.end(); ++eItr)
    {
		if ((*eItr).m_pBitmap)
		{
			pBitmap = (*eItr).m_pBitmap;
			strength = (*eItr).m_Strength;
			set_screen_blender(strength, strength, strength, strength);
			effectPosX = (*eItr).m_Pos.GetFloorIntX() - (pBitmap->w / 2);
			effectPosY = (*eItr).m_Pos.GetFloorIntY() - (pBitmap->h / 2);
			angle = (*eItr).m_Angle;
			//draw_trans_sprite(m_pBackBuffer32, pBitmap, effectPosX, effectPosY);

			if (angle == 0)
			{
				draw_trans_sprite(m_pBackBuffer32, pBitmap, effectPosX, effectPosY);
			}
			else
			{
				BITMAP * pTargetBitmap;

				if (pBitmap->w < 16 && pBitmap->h < 16)
					pTargetBitmap = m_pTempEffectBitmap_16;
				else if (pBitmap->w < 32 && pBitmap->h < 32)
					pTargetBitmap = m_pTempEffectBitmap_32;
				else if (pBitmap->w < 64 && pBitmap->h < 64)
					pTargetBitmap = m_pTempEffectBitmap_64;
				else if (pBitmap->w < 128 && pBitmap->h < 128)
					pTargetBitmap = m_pTempEffectBitmap_128;
				else
					pTargetBitmap = m_pTempEffectBitmap_256;

				clear_to_color(pTargetBitmap, 0);

				//fixed fAngle;
				//fAngle = fixmul(angle, radtofix_r);

				Matrix m;
				m.SetRadAngle(angle);

				rotate_sprite(pTargetBitmap, pBitmap, 0, 0, ftofix(m.GetAllegroAngle()));
				draw_trans_sprite(m_pBackBuffer32, pTargetBitmap, effectPosX, effectPosY);
			}
		}
    }

//    release_bitmap(m_pBackBuffer32);
//    release_bitmap(m_pBackBuffer8);

    // Set blender mode back??
//    set_trans_blender(128, 128, 128, 128);

    // Clear the effects list for this frame
    m_PostScreenEffects.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FlipFrameBuffers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Flips the framebuffer.

void FrameMan::FlipFrameBuffers()
{
    if (get_color_depth() == 32 && m_BPP == 32 && m_pBackBuffer32)
    {
        if (g_InActivity)
        {
            if (!m_Fullscreen && m_NxWindowed != 1)
                stretch_blit(m_PostProcessing ? m_pBackBuffer32 : m_pBackBuffer8, screen, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h, 0, 0, SCREEN_W, SCREEN_H);
            else if (m_Fullscreen && m_NxFullscreen != 1)
                stretch_blit(m_PostProcessing ? m_pBackBuffer32 : m_pBackBuffer8, screen, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h, 0, 0, SCREEN_W, SCREEN_H);
            else
                blit(m_PostProcessing ? m_pBackBuffer32 : m_pBackBuffer8, screen, 0, 0, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h);            
        }
        // Menu is always 32bpp
        else
        {
            if (!m_Fullscreen && m_NxWindowed != 1)
                stretch_blit(m_pBackBuffer32, screen, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h, 0, 0, SCREEN_W, SCREEN_H);
            else if (m_Fullscreen && m_NxFullscreen != 1)
                stretch_blit(m_pBackBuffer32, screen, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h, 0, 0, SCREEN_W, SCREEN_H);
            else
                blit(m_pBackBuffer32, screen, 0, 0, 0, 0, m_pBackBuffer32->w, m_pBackBuffer32->h);            
        }
    }
    // 8bpp video mode
    else
    {
        if (!m_Fullscreen && m_NxWindowed != 1)
            stretch_blit(m_pBackBuffer8, screen, 0, 0, m_pBackBuffer8->w, m_pBackBuffer8->h, 0, 0, SCREEN_W, SCREEN_H);
        else if (m_Fullscreen && m_NxFullscreen != 1)
            stretch_blit(m_pBackBuffer8, screen, 0, 0, m_pBackBuffer8->w, m_pBackBuffer8->h, 0, 0, SCREEN_W, SCREEN_H);
        else
            blit(m_pBackBuffer8, screen, 0, 0, 0, 0, m_pBackBuffer8->w, m_pBackBuffer8->h);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FlippingWith32BPP
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether the 32bpp framebuffer is currnetly being used or not
//                  when flipping the frame buffers.

bool FrameMan::FlippingWith32BPP() const
{
    return get_color_depth() == 32 && m_BPP == 32 && m_pBackBuffer32 && g_InActivity && m_PostProcessing;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawText
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a text string to the bitmap of choice, using the internal
//                  fontsets.

void FrameMan::DrawText(BITMAP *pTargetBitmap,
                        const string &str,
                        const Vector &pos,
                        bool black)
{
    RTEAbort("Old fonts are not ported yet!");
/*
    BITMAP *pFont = black ? m_pBlackFont : m_pWhiteFont;
    const char *drawChar = str.c_str();
    int charWidth = pFont->GetTile()->GetBlockWidth() - 1;
    int charHeight = pFont->GetTile()->GetBlockHeight() - 1;
    int lineSep = 4;
    int line = 0;

    for (int i = 0, j = 0; i < str.size(); ++i, ++j) {
        if (*drawChar == '\n') {
            i++;
            j = 0;
            line++;
            drawChar++;
        }
        pFont->SetPos(pos.m_X + (j * charWidth), pos.m_Y + (line * (charHeight + lineSep)));
        pFont->SetFrame(*(drawChar++));
        pFont->Draw(pTargetBitmap, 0, 0, CDXBLT_TRANS);
    }
*/
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a line that can be dotted or with other effects.

int FrameMan::DrawLine(BITMAP *pBitmap, const Vector &start, const Vector &end, int color, int altColor, int skip, int skipStart, bool shortestWrap)
{
    RTEAssert(pBitmap, "Trying to draw line to null Bitmap");

//    acquire_bitmap(pBitmap);

    int hitCount = 0, error, dom, sub, domSteps, skipped = skip + (skipStart - skip);
    int intPos[2], delta[2], delta2[2], increment[2];
    bool drawAlt = false;

    // Just make the alt the same color as the main one if no one was specified
    if (altColor == 0)
        altColor = color;
/*
    // Glow intensity
    int glowIntensity = color == g_YellowGlowColor ? 55 + 200 * PosRand() : 0;
*/
    // Calculate the integer values
    intPos[X] = floorf(start.m_X);
    intPos[Y] = floorf(start.m_Y);

    // Wrap line around the scene if it makes it shorter
    if (shortestWrap)
    {
        Vector deltaVec = g_SceneMan.ShortestDistance(start, end, false);
        delta[X] = floorf(deltaVec.m_X);
        delta[Y] = floorf(deltaVec.m_Y);
    }
    // No wrap
    else
    {
        delta[X] = floorf(end.m_X) - intPos[X];
        delta[Y] = floorf(end.m_Y) - intPos[Y];
    }

    if (delta[X] == 0 &&  delta[Y] == 0)
        return 0;

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm preparation

    if (delta[X] < 0)
    {
        increment[X] = -1;
        delta[X] = -delta[X];
    }
    else
        increment[X] = 1;

    if (delta[Y] < 0)
    {
        increment[Y] = -1;
        delta[Y] = -delta[Y];
    }
    else
        increment[Y] = 1;

    // Scale by 2, for better accuracy of the error at the first pixel
    delta2[X] = delta[X] << 1;
    delta2[Y] = delta[Y] << 1;

    // If X is dominant, Y is submissive, and vice versa.
    if (delta[X] > delta[Y]) {
        dom = X;
        sub = Y;
    }
    else {
        dom = Y;
        sub = X;
    }

    error = delta2[sub] - delta[dom];

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm execution

    Vector glowPoint;
    for (domSteps = 0; domSteps < delta[dom]; ++domSteps)
    {
        intPos[dom] += increment[dom];
        if (error >= 0)
        {
            intPos[sub] += increment[sub];
            error -= delta2[dom];
        }
        error += delta2[sub];

        // Only draw pixel if we're not due to skip any
        if (++skipped > skip)
        {
            // Scene wrapping, if necessary
            g_SceneMan.WrapPosition(intPos[X], intPos[Y]);

            // Slap a regular pixel on there
            putpixel(pBitmap, intPos[X], intPos[Y], drawAlt ? color : altColor);
/*
            // Slap a glow on there in absoltue scene coordinates if enabled
            if (glowIntensity > 0)
            {
                glowPoint.SetXY(intPos[X], intPos[Y]);
                g_SceneMan.RegisterGlowDotEffect(glowPoint + targetOffset, m_pYellowEffect, glowIntensity);
            }
*/
            drawAlt = !drawAlt;
            skipped = 0;
        }
    }

//    release_bitmap(pBitmap);
    // Return the end phase state of the skipping
    return skipped;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawDotLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws a line that can be dotted with bitmaps.

int FrameMan::DrawDotLine(BITMAP *pBitmap, const Vector &start, const Vector &end, BITMAP *pDot, int skip, int skipStart, bool shortestWrap)
{
    RTEAssert(pBitmap, "Trying to draw line to null Bitmap");
    RTEAssert(pDot, "Trying to draw line of dots without specifying a dot Bitmap");

//    acquire_bitmap(pBitmap);

    int hitCount = 0, error, dom, sub, domSteps, skipped = skip + (skipStart - skip);
    int intPos[2], delta[2], delta2[2], increment[2];
    bool drawAlt = false;
    int dotHalfHeight = pDot->h / 2;
    int dotHalfWidth = pDot->w / 2;

    // Calculate the integer values
    intPos[X] = floorf(start.m_X);
    intPos[Y] = floorf(start.m_Y);

    // Wrap line around the scene if it makes it shorter
    if (shortestWrap)
    {
        Vector deltaVec = g_SceneMan.ShortestDistance(start, end, false);
        delta[X] = floorf(deltaVec.m_X);
        delta[Y] = floorf(deltaVec.m_Y);
    }
    // No wrap
    else
    {
        delta[X] = floorf(end.m_X) - intPos[X];
        delta[Y] = floorf(end.m_Y) - intPos[Y];
    }

    if (delta[X] == 0 &&  delta[Y] == 0)
        return 0;

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm preparation

    if (delta[X] < 0)
    {
        increment[X] = -1;
        delta[X] = -delta[X];
    }
    else
        increment[X] = 1;

    if (delta[Y] < 0)
    {
        increment[Y] = -1;
        delta[Y] = -delta[Y];
    }
    else
        increment[Y] = 1;

    // Scale by 2, for better accuracy of the error at the first pixel
    delta2[X] = delta[X] << 1;
    delta2[Y] = delta[Y] << 1;

    // If X is dominant, Y is submissive, and vice versa.
    if (delta[X] > delta[Y]) {
        dom = X;
        sub = Y;
    }
    else {
        dom = Y;
        sub = X;
    }

    error = delta2[sub] - delta[dom];

    /////////////////////////////////////////////////////
    // Bresenham's line drawing algorithm execution

    Vector glowPoint;
    for (domSteps = 0; domSteps < delta[dom]; ++domSteps)
    {
        intPos[dom] += increment[dom];
        if (error >= 0)
        {
            intPos[sub] += increment[sub];
            error -= delta2[dom];
        }
        error += delta2[sub];

        // Only draw pixel if we're not due to skip any
        if (++skipped > skip)
        {
            // Scene wrapping, if necessary
            g_SceneMan.WrapPosition(intPos[X], intPos[Y]);

            // Slap the dot on there
            masked_blit(pDot, pBitmap, 0, 0, intPos[X] - dotHalfWidth, intPos[Y] - dotHalfHeight, pDot->w, pDot->h);

            drawAlt = !drawAlt;
            skipped = 0;
        }
    }

//    release_bitmap(pBitmap);
    // Return the end phase state of the skipping
    return skipped;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this FrameMan. Supposed to be done every frame.

void FrameMan::Update()
{
    // Time and add the millisecs per frame reading to the buffer
    m_MSPFs.push_back(m_pFrameTimer->GetElapsedRealTimeMS());
    // Keep the buffer trimmed
    while (m_MSPFs.size() > MSPFAVERAGESAMPLESIZE)
        m_MSPFs.pop_front();

    // Calculate the average millsecs per frame over the last sampleSize frames
    m_MSPFAverage = 0;
    for (deque<int>::iterator fItr = m_MSPFs.begin(); fItr != m_MSPFs.end(); ++fItr)
        m_MSPFAverage += *fItr;
    m_MSPFAverage /= m_MSPFs.size();

    // If one sim update per frame mode, adjust the pitch of most sound effects to match the sim time over real time ratio as it fluctuates!
    if (g_TimerMan.IsOneSimUpdatePerFrame())
    {
        // Calculate the sim speed over the actual real time
        m_SimSpeed = g_TimerMan.GetDeltaTimeMS() / (float)m_MSPFAverage;
//        float simSpeed = g_TimerMan.GetDeltaTimeSecs() / m_pFrameTimer->GetElapsedRealTimeS();
        // If limited, only allow pitch to go slower, not faster
        if (g_TimerMan.IsSimSpeedLimited() && m_SimSpeed > 1.0)
            m_SimSpeed = 1.0;
        // Soften the ratio of the pitch adjustment so it's not such an extreme effect on the audio
// TODO: Don't hardcode this coefficient - although it's a good defualt
        float pitch = m_SimSpeed + (1.0f - m_SimSpeed) * 0.35;
        // Set the pitch for all other applicable sounds other than music
        g_AudioMan.SetGlobalPitch(pitch, true);
// MUSIC PITCHING IS SUCK.. ruins the songs
        // Only affect the music if it's really slow, so it doesn't sound funny and fluctuating
// TODO: Don't hardcode this threshold - although it's a good defualt
//        g_AudioMan.SetMusicPitch(pitch >= 0.50 ? 1.0 : pitch);
    }
    else
        m_SimSpeed = 1.0;

    // Clear the back buffers
//    m_pScreen->GetBack()->Fill(0); // don't do this to avoid the black lines...look into that later.

//    g_UInputMan.DisableMouseMoving(false);

	//Remove all scheduled primitives, those will be readded by updates from other entities
	ClearPrimitivesList();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawPrimitives
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws all stored primitives on the screen
// Arguments:       Bitmap to draw on, poistion to draw.
// Return value:    None.

void FrameMan::DrawPrimitives(int player, BITMAP *pTargetBitmap, const Vector &targetPos)
{
    // Count how many split screens we'll need
    //int screenCount = (m_HSplit ? 2 : 1) * (m_VSplit ? 2 : 1);
    //BITMAP *pDrawScreen = /*get_color_depth() == 8 && */screenCount == 1 ? m_pBackBuffer8 : m_pPlayerScreen;

	//Draw primitives
	for (std::list<GraphicalPrimitive *>::const_iterator it = m_Primitives.begin(); it != m_Primitives.end(); ++it)
	{
		if (player == (*it)->m_Player || (*it)->m_Player == -1)
			(*it)->Draw(pTargetBitmap, targetPos);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the current frame to the screen.

void FrameMan::Draw()
{
    // Count how many split screens we'll need
    int screenCount = (m_HSplit ? 2 : 1) * (m_VSplit ? 2 : 1);

    RTEAssert(screenCount <= 1 || m_pPlayerScreen, "Splitscreen surface not ready when needed!");
    // Choose which buffer to draw to. If there are no splitscreens and 8bit modes, draw directly to the back buffer, else use a intermediary splitscreen buffer
    char str[512];

    // Clear out the post processing screen effects list
    m_PostScreenEffects.clear();
    m_PostScreenGlowBoxes.clear();
    // These accumulate the effects for each player's screen area, and are then transferred to the above lists with the player screen offset applied
	list<PostEffect> screenRelativeEffects;
    list<Box> screenRelativeGlowBoxes;

    for (int whichScreen = 0; whichScreen < screenCount; ++whichScreen)
    {
        // Screen Update
        screenRelativeEffects.clear();
        screenRelativeGlowBoxes.clear();

		BITMAP *pDrawScreen = /*get_color_depth() == 8 && */screenCount == 1 ? m_pBackBuffer8 : m_pPlayerScreen;
		BITMAP *pDrawScreenGUI = pDrawScreen;


		AllegroBitmap pPlayerGUIBitmap(pDrawScreenGUI);



        Vector targetPos = g_SceneMan.GetOffset(whichScreen);
        // Adjust the drawing position on the target screen for if the target screen is larger than the scene in nonwrapping dimension.
        // Scene needs to be displayed centered on the target bitmap then, and that has to be adjusted for when drawing to the screen
        if (!g_SceneMan.SceneWrapsX() && pDrawScreen->w > g_SceneMan.GetSceneWidth())
            targetPos.m_X += (pDrawScreen->w - g_SceneMan.GetSceneWidth()) / 2;
        if (!g_SceneMan.SceneWrapsY() && pDrawScreen->h > g_SceneMan.GetSceneHeight())
            targetPos.m_Y += (pDrawScreen->h - g_SceneMan.GetSceneHeight()) / 2;

		// Try to move at the framebuffer copy time to maybe prevent wonkyness
		m_TargetPos[m_NetworkFrameCurrent][whichScreen] = targetPos;

        // Draw the scene
		if (!m_StoreNetworkBackBuffer/* || g_UInputMan.KeyHeld(KEY_6)*/)
		{
			g_SceneMan.Draw(pDrawScreen, pDrawScreenGUI, targetPos);
		} 
		else 
		{
			clear_to_color(pDrawScreen, g_KeyColor);
			clear_to_color(pDrawScreenGUI, g_KeyColor);
			g_SceneMan.Draw(pDrawScreen, pDrawScreenGUI, targetPos, true, true);
		}
        // Get only the scene-relative post effects that affect this player's screen
        if (m_PostProcessing && pActivity)
        {
            g_SceneMan.GetPostScreenEffectsWrapped(targetPos, pDrawScreen->w, pDrawScreen->h, screenRelativeEffects, pActivity->GetTeamOfPlayer(pActivity->PlayerOfScreen(whichScreen)));
            g_SceneMan.GetGlowAreasWrapped(targetPos, pDrawScreen->w, pDrawScreen->h, screenRelativeGlowBoxes);

        }

// TODO: Find out what keeps disabling the clipping on the draw bitmap
        // Enable clipping on the draw bitmap
        set_clip_state(pDrawScreen, 1);

		//Always draw seam in debug mode
#ifdef DEBUG_BUILD
		//DrawLinePrimitive(Vector(0,0),Vector(0,g_SceneMan.GetSceneHeight()), 5);
#endif

        ///////////////////////////////////////
        // Draw screen texts
        int yTextPos = 0;

        // Only draw this stuff for actual human players
        if (whichScreen < g_ActivityMan.GetActivity()->GetHumanCount())
        {
            // Team of current screen's player
//            int team = g_ActivityMan.GetActivity()->GetTeamOfPlayer(g_ActivityMan.GetActivity()->PlayerOfScreen(whichScreen));

            yTextPos += 12;

            // Message
            if (!m_ScreenText[whichScreen].empty())
            {
				if (m_TextCentered[whichScreen])
					yTextPos = (GetPlayerScreenHeight() / 2) - 52;
				int occOffsetX = g_SceneMan.GetScreenOcclusion(whichScreen).m_X;

				// If there's really no room to offset the text into, then don't
				if (GetPlayerScreenWidth() <= GetResX() / 2)
					occOffsetX = 0;
				// Draw blinking effect, but not of the text message itself, but some characters around it (so it's easier to read the msg)
				if (m_TextBlinking[whichScreen] && m_TextBlinkTimer.AlternateReal(m_TextBlinking[whichScreen]))
					GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, (GetPlayerScreenWidth() + occOffsetX) / 2, yTextPos, (">>> " + m_ScreenText[whichScreen] + " <<<").c_str(), GUIFont::Centre);
				else
					GetLargeFont()->DrawAligned(&pPlayerGUIBitmap, (GetPlayerScreenWidth() + occOffsetX) / 2, yTextPos, m_ScreenText[whichScreen].c_str(), GUIFont::Centre);

                yTextPos += 12;
            }

        ////////////////////////////////////////
        // If we are dealing with split screens, then deal with the fact that we need to draw the player screens to different locations on the final buffer

        // The position of the current drawscreen on the final screen
        Vector screenOffset;

        if (screenCount > 1)
        {
            // Always upper left corner
            if (whichScreen == 0)
                screenOffset.SetIntXY(0, 0);
            else if (whichScreen == 1)
            {
                // If both splits, or just Vsplit, then in upper right quadrant
                if ((m_VSplit && !m_HSplit) || (m_VSplit && m_HSplit))
                    screenOffset.SetIntXY(GetResX() / 2, 0);
                // If only hsplit, then lower left quadrant
                else
                    screenOffset.SetIntXY(0, GetResY() / 2);
            }
            // Always lower left quadrant
            else if (whichScreen == 2)
                screenOffset.SetIntXY(0, GetResY() / 2);
            // Always lower right quadrant
            else if (whichScreen == 3)
                screenOffset.SetIntXY(GetResX() / 2, GetResY() / 2);
        }

        // Flash the screen if we're supposed to
        if (m_FlashScreenColor[whichScreen] != -1)
        {
            // If set to flash for a period of time, first be solid and then start flashing slower
            float left = m_FlashTimer[whichScreen].LeftTillRealTimeLimitMS();
            if (/*left > 500 || */left < 10 || m_FlashTimer[whichScreen].AlternateReal(50))
            {
                // At most, only flash every other frame
                if (m_FlashedLastFrame[whichScreen])
                    m_FlashedLastFrame[whichScreen] = false;
                else
                {
                    // FLASH!
                    rectfill(pDrawScreenGUI, 0, 0, pDrawScreenGUI->w, pDrawScreenGUI->h, m_FlashScreenColor[whichScreen]);
                    // Show that we did indeed flash this frame
                    m_FlashedLastFrame[whichScreen] = true;
                }
            }
            // Make things go into slight slomo - DANGER
//            g_TimerMan.SetOneSimUpdatePerFrame(!m_SloMoTimer.IsPastSimTimeLimit());
//            g_TimerMan.IsOneSimUpdatePerFrame();
            // Stop with the flash after the designated period
            if (m_FlashTimer[whichScreen].IsPastRealTimeLimit())
                m_FlashScreenColor[whichScreen] = -1;
        }

        // Add the player screen's effects to the total screen effects list so they can be drawn in post processing
        if (m_PostProcessing)
        {
            int occX = g_SceneMan.GetScreenOcclusion(whichScreen).GetFloorIntX();
            int occY = g_SceneMan.GetScreenOcclusion(whichScreen).GetFloorIntY();

            // Adjust for the player screen's position on the final buffer
            for (list<PostEffect>::iterator eItr = screenRelativeEffects.begin(); eItr != screenRelativeEffects.end(); ++eItr)
            {
                // Make sure we won't be adding any effects to a part of the screen that is occluded by menus and such
                if ((*eItr).m_Pos.m_X > occX && (*eItr).m_Pos.m_Y > occY && (*eItr).m_Pos.m_X < pDrawScreen->w + occX && (*eItr).m_Pos.m_Y < pDrawScreen->h + occY)
                    m_PostScreenEffects.push_back( PostEffect((*eItr).m_Pos + screenOffset, (*eItr).m_pBitmap, (*eItr).m_BitmapHash, (*eItr).m_Strength, (*eItr).m_Angle) );
            }

            // Adjust glow areas for the player screen's position on the final buffer
            for (list<Box>::iterator bItr = screenRelativeGlowBoxes.begin(); bItr != screenRelativeGlowBoxes.end(); ++bItr)
            {
                m_PostScreenGlowBoxes.push_back(*bItr);
                // Adjust each added glow area for the player screen's position on the final buffer
                m_PostScreenGlowBoxes.back().m_Corner += screenOffset;
            }
        }
    }


    // Do postprocessing effects, if applicable and enabled
    if (m_PostProcessing && g_InActivity && m_BPP == 32)
        PostProcess();

    // Draw the console on top of everything
    if (FlippingWith32BPP())
        g_ConsoleMan.Draw(m_pBackBuffer32);

    release_bitmap(m_pBackBuffer8);

    // Reset the frame timer so we can measure how much it takes until next frame being drawn
    m_pFrameTimer->Reset();
}

void FrameMan::GetPostEffectsList(int whichScreen, list<PostEffect> & outputList)
{
	ScreenRelativeEffectsMutex[whichScreen].lock();
	outputList.clear();
	for (list<PostEffect>::iterator eItr = m_ScreenRelativeEffects[whichScreen].begin(); eItr != m_ScreenRelativeEffects[whichScreen].end(); ++eItr)
		outputList.push_back(PostEffect((*eItr).m_Pos, (*eItr).m_pBitmap, (*eItr).m_BitmapHash, (*eItr).m_Strength, (*eItr).m_Angle));

	ScreenRelativeEffectsMutex[whichScreen].unlock();
}

void FrameMan::SetPostEffectsList(int whichScreen, list<PostEffect> & inputList)
{
	ScreenRelativeEffectsMutex[whichScreen].lock();
	m_ScreenRelativeEffects[whichScreen].clear();
	for (list<PostEffect>::iterator eItr = inputList.begin(); eItr != inputList.end(); ++eItr)
		m_ScreenRelativeEffects[whichScreen].push_back(PostEffect((*eItr).m_Pos, (*eItr).m_pBitmap, (*eItr).m_BitmapHash, (*eItr).m_Strength, (*eItr).m_Angle));

	ScreenRelativeEffectsMutex[whichScreen].unlock();
}






} // namespace RTE 
