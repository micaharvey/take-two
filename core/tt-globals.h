//-----------------------------------------------------------------------------
// name: tt-globals.h
// desc: global stuff
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#ifndef __TT_GLOBALS_H__
#define __TT_GLOBALS_H__


#include "x-def.h"
#include "x-audio.h"
#include "x-gfx.h"
#include "x-vector3d.h"
#include "y-fluidsynth.h"
#include "y-waveform.h"
#include "tt-entity.h"
using namespace std;

// c++
#include <string>
#include <map>
#include <vector>
#include <utility>

// defines
#define TT_SRATE        44100
#define TT_FRAMESIZE    256
#define TT_NUMCHANNELS  2
#define TT_MAX_TEXTURES 32

#define MY_PIE 3.14159265358

#define DRUM_CHANNEL 9
#define TT_SYNTH_CHANNEL 2
#define TT_MULTI_CHANNEL 0


#define TT_KICK  35 
#define TT_SNARE 39
#define TT_HIHAT 44


#define TT_WINDOW_X 73
#define TT_WINDOW_Y 79

//-----------------------------------------------------------------------------
// name: enum BokehTextureNames
// desc: texture names
//-----------------------------------------------------------------------------
enum BokehTextureNames
{
    TT_TEX_NONE = 0,
    TT_TEX_FLARE_TNG_1,
    TT_TEX_FLARE_TNG_2,
    TT_TEX_FLARE_TNG_3,
    TT_TEX_FLARE_TNG_4,
    TT_TEX_FLARE_TNG_5
};

//-----------------------------------------------------------------------------
// name: enum sceneNames
// desc: scene names
//-----------------------------------------------------------------------------
enum ttScene
{ 
    INTRO = 0, 
    ADDSYNTH,
    PRESSX,
    PRESSV,
    PRESSN,
    FADEHELLO,
    COUNTOFF,
    PIANO,
    DRUMS,
    GAMEOVER
};


// forward reference
class TTSim;



//-----------------------------------------------------------------------------
// name: class Globals
// desc: the global clabal
//-----------------------------------------------------------------------------
class Globals
{
public:
    // top level root simulation
    static YEntity sim;
    // current "now"
    static YTimeInterval simTime;
    // the delta time between last two render frames
    static YTimeInterval lastDelta;
    // is this the first cascade?
    static bool first;
    // are we paused?
    static bool isPaused;

    // top level only Yentity outside of sim
    static YEntity hud;

    // Now
    static unsigned long now;
    // BPM
    static unsigned long bpm;
    // periodInSamples
    static float periodInSamples;
    // eigthNote counter
    static unsigned long eigthNotes;
    // startPiano Grid
    static unsigned long pianoStartTime;
    // countdown start
    static unsigned long countoffStartTime;
    // countoff offset 
    static unsigned long countoffOffset;
    // drums start
    static unsigned long drumsStartTime;
    // drums offset
    static unsigned long drumsOffset;
    // static unsigned long drumsOffset;




    // fluid synth
    static YFluidSynth * synth;
    // last audio buffer, input
    static SAMPLE * inputBuffer;
    static SAMPLE * fftBuffer;
    static SAMPLE * lastAudioBufferMono;
    static SAMPLE * audioBufferWindow;
    static SAMPLE * fftBufferWindow;
    static unsigned int lastAudioBufferFrames;
    static unsigned int lastAudioBufferChannels;

    // self explanatory
    static unsigned long timeSinceLastEigthNoteInSamples;

    // for scenes (scenes are progressions in the game)
    static ttScene scene;
    // to drums or to piano
    static int numEvents;

    // streaks
    static unsigned int currentStreak;
    static unsigned int longestStreak;

    // for missed note detection
    static bool lastSnareHit;
    static bool lastCymbalHit;
    static bool lastKickHit;

    // game board
    static TTGameBoard * board;
    // notes
    static int * snareNotes;
    static int * cymbalNotes;
    static int * kickNotes;
    static int ** pianoNotes;

    // waveform
    static YWaveform * waveform;
    static YWaveform * fftWaveform;
    static YWaveform ** fftWaveformWaterfall;

    // path
    static std::string path;
    // path to datapath
    static std::string relpath;
    // datapath
    static std::string datapath;
    // version
    static std::string version;


    // width and height of the window
    static GLsizei windowWidth;
    static GLsizei windowHeight;
    static GLsizei lastWindowWidth;
    static GLsizei lastWindowHeight;

    // graphics fullscreen
    static GLboolean fullscreen;
    // render waveform
    static GLboolean renderWaveform;
    // blend pane instead of clearing screen
    static GLboolean blendScreen;
    // blend screen parameters
    static Vector3D blendAlpha;
    static GLfloat blendRed;
    // fill mode
    static GLenum fillmode;
    // background color
    static iSlew3D bgColor;
    // view stuff
    static Vector3D viewRadius;
    static Vector3D viewEyeY;
    static Vector3D fov;
    
    // textures
    static GLuint textures[];
    
    // light 0 position
    static GLfloat light0_pos[4];
    // light 1 parameters
    static GLfloat light1_ambient[4];
    static GLfloat light1_diffuse[4];
    static GLfloat light1_specular[4];
    static GLfloat light1_pos[4];
    // line width
    static GLfloat linewidth;
    // do
    static GLboolean fog;
    static GLuint fog_mode[4];   // storage for three/four types of fog
    static GLuint fog_filter;    // which fog to use
    static GLfloat fog_density;  // fog density
    
    // colors
    static Vector3D ourWhite;
    static Vector3D ourRed;
    static Vector3D ourBlue;
    static Vector3D ourOrange;
    static Vector3D ourGreen;
    static Vector3D ourGray;
    static Vector3D ourYellow;
    static Vector3D ourSoftYellow;
    static Vector3D ourPurple;
};




#endif
