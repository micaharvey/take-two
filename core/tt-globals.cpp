//-----------------------------------------------------------------------------
// name: tt-globals.cpp
// desc: global stuff
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#include "tt-globals.h"


// defaults
#define DEFAULT_FULLSCREEN    TRUE
#define DEFAULT_WINDOW_WIDTH  1280
#define DEFAULT_WINDOW_HEIGHT 720
#define DEFAULT_BLENDSCREEN   FALSE
#define DEFAULT_FOG           FALSE
#define DEFAULT_VERSION       "1.0.0"

// Sim Stuff
YEntity Globals::sim;
YTimeInterval Globals::simTime;
YTimeInterval Globals::lastDelta;
bool Globals::first = true;
bool Globals::isPaused = false;

YEntity Globals::hud;

unsigned long Globals::now = 0;
unsigned long Globals::bpm = 100;
float Globals::periodInSamples = 0;
unsigned long Globals::eigthNotes = 0;
unsigned long Globals::pianoStartTime = 0;
unsigned long Globals::countoffStartTime = 0;
unsigned long Globals::drumsStartTime = 0;
unsigned long Globals::drumsOffset = 0;
unsigned long Globals::countoffOffset = 0;

YFluidSynth * Globals::synth;
ttScene Globals::scene = INTRO;
int Globals::numEvents = 0;

unsigned long Globals::timeSinceLastEigthNoteInSamples = 0;

unsigned int Globals::currentStreak = 0;
unsigned int Globals::longestStreak = 0;

YWaveform * Globals::waveform = NULL;
YWaveform * Globals::fftWaveform = NULL;
YWaveform ** Globals::fftWaveformWaterfall = NULL;

SAMPLE * Globals::inputBuffer = NULL;
SAMPLE * Globals::fftBuffer = NULL;
SAMPLE * Globals::lastAudioBufferMono = NULL;
SAMPLE * Globals::audioBufferWindow = NULL;
SAMPLE * Globals::fftBufferWindow = NULL;
unsigned int Globals::lastAudioBufferFrames = 0;
unsigned int Globals::lastAudioBufferChannels = 0;


bool Globals::lastSnareHit = false;
bool Globals::lastCymbalHit= false;
bool Globals::lastKickHit  = false;

TTGameBoard * Globals::board;
int         * Globals::snareNotes;
int         * Globals::cymbalNotes;
int         * Globals::kickNotes;
int         ** Globals::pianoNotes;

GLsizei Globals::windowWidth = DEFAULT_WINDOW_WIDTH;
GLsizei Globals::windowHeight = DEFAULT_WINDOW_HEIGHT;
GLsizei Globals::lastWindowWidth = Globals::windowWidth;
GLsizei Globals::lastWindowHeight = Globals::windowHeight;

GLboolean Globals::fullscreen = DEFAULT_FULLSCREEN;
GLboolean Globals::blendScreen = DEFAULT_BLENDSCREEN;
GLboolean Globals::renderWaveform = TRUE;

Vector3D Globals::blendAlpha( 1, 1, .5f );
GLfloat Globals::blendRed = 0.0f;
GLenum Globals::fillmode = GL_FILL;
iSlew3D Globals::bgColor( .5 );
Vector3D Globals::viewRadius( 5, 2, 3 );
Vector3D Globals::viewEyeY( 1, 0, 2.5f );
Vector3D Globals::fov( 120, 80, .2f );

GLuint Globals::textures[TT_MAX_TEXTURES];

GLfloat Globals::light0_pos[4] = { 2.0f, 0.0f, 0.0f, 1.0f };
GLfloat Globals::light1_ambient[4] = { .2f, .2f, .2f, 1.0f };
GLfloat Globals::light1_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat Globals::light1_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat Globals::light1_pos[4] = { -2.0f, 0.0f, 0.0f, 1.0f };
GLfloat Globals::linewidth = 1.0f;
GLboolean Globals::fog = DEFAULT_FOG;
GLuint Globals::fog_mode[4];
GLuint Globals::fog_filter;
GLfloat Globals::fog_density;

// colors
Vector3D Globals::ourWhite( 1, 1, 1 );
Vector3D Globals::ourRed( 1, .5, .5 );
Vector3D Globals::ourBlue( 102.0f/255, 204.0f/255, 1.0f );
Vector3D Globals::ourOrange( 1, .75, .25 );
Vector3D Globals::ourGreen( .7, 1, .45 );
Vector3D Globals::ourGray( .4, .4, .4 );
Vector3D Globals::ourYellow( 1, 1, .25 );
Vector3D Globals::ourSoftYellow( .7, .7, .1 );
Vector3D Globals::ourPurple( .6, .25, .6 );

std::string Globals::path = "";
std::string Globals::relpath = "data/texture/";
std::string Globals::datapath = "";
std::string Globals::version = DEFAULT_VERSION;
