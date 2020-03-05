//-----------------------------------------------------------------------------
// name: tt-gfx.cpp
// desc: graphics stuff for tt visualization
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#include "tt-gfx.h"
#include "tt-globals.h"
#include "tt-entity.h"
#include "tt-audio.h"
#include "x-fun.h"
#include "x-gfx.h"
#include "x-loadlum.h"
#include "x-vector3d.h"
#include <map>
#include <iostream>
#include <vector>
using namespace std;


// globals
int g_synthNoteCount = 0;

// Intro
YEntity g_introRoot;
vector<YBokeh *> g_bokehs;
YText * g_introText;

// Game
YEntity g_gameRoot;
TTLineGrid      * g_lineGrid;
TTPlayHead      * g_playHead;
TTHallStraight  * g_hallPiece;
TTTorusPathway  * g_leftTorus;
TTTorusPathway  * g_rightTorus;
TTExplosionArea * g_expBottom;
TTExplosionArea * g_expLeft;
TTExplosionArea * g_expRight;
TTSnareLineCirclePathway * g_rightLineCircle;

YEntity     * g_hallway;
TTCube      * g_testCube;
TTPianoNoteGrid * g_pianoGrid;
TTExplosionArea ** g_pianoExps;

int g_countoffCountToShow;


// TTRotated * g_rotated;

// hud
YText * g_topLeft;
YText * g_bottomRight;
YText * g_hello;
YText * g_smile;
YText * g_wink;
YText * g_wave;

YText * g_press;
YText * g_pressx;
YText * g_pressv;
YText * g_pressn;

unsigned long g_fadeHelloStartTime;

// keys
map<char,int> g_keys;
map<char,int> g_keyToBucket;

// changing velocity
float g_velocity[16] = {1,0.5,0.7,0.5,\
                        1,0.5,0.7,0.5,\
                        1,0.5,0.7,0.5,\
                        1,0.5,0.7,0.5};

// max sim step size in seconds
#define SIM_SKIP_TIME (.25)


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc();
void displayFunc();
void reshapeFunc( int width, int height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void specialFunc( int key, int x, int y );

void initialize_graphics();
void initialize_simulation();
bool initialize_data();
void loadTextures();
bool checkTexDim( int dim );
void setupTexture( const char * filename, GLuint texobj,
                  GLenum minFilter, GLenum maxFilter,
                  int useMipMaps = 0 );

void renderBackground();
void blendPane();
void updateNodeEntities();
void renderNodeEntities();



//-----------------------------------------------------------------------------
// name: tt_gfx_init( )
// desc: graphics/interaction entry point
//-----------------------------------------------------------------------------
bool tt_gfx_init( int argc, const char ** argv )
{
#ifdef __APPLE__
    // save working dir
    char * cwd = getcwd( NULL, 0 );
    // set path
    // #ifdef __VQ_RELEASE__
#if 1
    // HACK: use this hard-coded path
    Globals::path = "./";
#else
    // store this globally
    Globals::path = cwd;
#endif
    // compute the datapath
    Globals::datapath = Globals::path + Globals::relpath;
#endif
    
    // initialize GLUT
    glutInit( &argc, (char **)argv );
    
#ifdef __APPLE__
    //restore working dir
    chdir( cwd );
    free( cwd );
#endif
    
    // print about
    tt_about();
    tt_endline();

    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( Globals::windowWidth, Globals::windowHeight );
    // set the window postion
    glutInitWindowPosition( TT_WINDOW_X, TT_WINDOW_Y );
    // create the window
    glutCreateWindow( "takeTwo");
    // full screen
    if( Globals::fullscreen )
        glutFullScreen();
    
    // set the idle function - called when idle
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    // for arrow keys, etc
	glutSpecialFunc (specialFunc );
    
    // do data
    if( !initialize_data() )
    {
        // done
        return false;
    }

    // do our own initialization
    initialize_graphics();
    // simulation
    initialize_simulation();

    
    return true;
}




//-----------------------------------------------------------------------------
// name: tt_gfx_loop( )
// desc: hand off to graphics loop
//-----------------------------------------------------------------------------
void tt_gfx_loop()
{
    // let GLUT handle the current thread from here
    glutMainLoop();
}




//-----------------------------------------------------------------------------
// Name: initialize_graphics( )
// Desc: sets initial OpenGL states and initializes any application data
//-----------------------------------------------------------------------------
void initialize_graphics()
{
    // log
    cerr << "[tt]: initializing graphics system..." << endl;
    
    // reset time
    XGfx::resetCurrentTime();
    // set simulation speed
    XGfx::setDeltaFactor( 1.0f );
    // get the first
    XGfx::getCurrentTime( true );
    // random
    XFun::srand();
    
    // set the GL clear color - use when the color buffer is cleared
    glClearColor( Globals::bgColor.actual().x, Globals::bgColor.actual().y, Globals::bgColor.actual().z, 1.0f );
    // set the shading model to 'smooth'
    glShadeModel( GL_SMOOTH );
    // enable depth
    glEnable( GL_DEPTH_TEST );
    // set the front faces of polygons
    glFrontFace( GL_CCW );
    // set fill mode
    glPolygonMode( GL_FRONT_AND_BACK, Globals::fillmode );
    // enable lighting
    glEnable( GL_LIGHTING );
    // enable lighting for front
    glLightModeli( GL_FRONT_AND_BACK, GL_TRUE );
    // material have diffuse and ambient lighting
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    // enable color
    glEnable( GL_COLOR_MATERIAL );
    // normalize (for scaling)
    glEnable( GL_NORMALIZE );
    // line width
    glLineWidth( Globals::linewidth );
    
    // enable light 0
    glEnable( GL_LIGHT0 );
    
    // setup and enable light 1
    glLightfv( GL_LIGHT1, GL_AMBIENT, Globals::light1_ambient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, Globals::light1_diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, Globals::light1_specular );
    glEnable( GL_LIGHT1 );
    
    // load textures
    loadTextures();
    
    // fog
    Globals::fog_mode[0] = 0;
    Globals::fog_mode[1] = GL_LINEAR;
    // fog_mode[1] = GL_EXP; fog_mode[2] = GL_EXP2;
    Globals::fog_filter = 0;
    Globals::fog_density = .04f;
    
    // fog color
    GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};
    // fog mode
    if( Globals::fog_filter ) glFogi(GL_FOG_MODE, Globals::fog_mode[Globals::fog_filter]);
    // set fog color
    glFogfv( GL_FOG_COLOR, fogColor );
    // fog density
    glFogf( GL_FOG_DENSITY, Globals::fog_density );
    // fog hint
    glHint( GL_FOG_HINT, GL_DONT_CARE );
    // fog start depth
    glFogf( GL_FOG_START, 1.0f );
    // fog end depth
    glFogf( GL_FOG_END, 10.5f );
    // enable
    if( Globals::fog_filter ) glEnable( GL_FOG );
    
    // check global flag
    if( Globals::fog )
    {
        // fog mode
        glFogi(GL_FOG_MODE, Globals::fog_mode[Globals::fog_filter]);
        // enable
        glEnable(GL_FOG);
    }
    else
    {
        // disable
        glDisable(GL_FOG);
    }
    
    // clear the color buffer once
    glClear( GL_COLOR_BUFFER_BIT );
}


void generateIntro(){
    //////////////
    // INTROintr ////
    ////////////
    // this part modded from  bokeh
    for( int i = 0; i < 512; i++ )
    {
        // create a spark
        YBokeh * bokeh = new YBokeh();
        // set attributes
        bokeh->set( 1.0f, 1.0f, 1.0f, 1.0f, TT_TEX_FLARE_TNG_4 );
        // set bokeh
        bokeh->setBokehParams( // initial time
                               XFun::rand2f(0,10),
                               // freq
                               XFun::rand2f(1,3),
                               // time step
                               50,
                               // location
                               Vector3D(XFun::rand2f(-5,6),XFun::rand2f(-4,4), XFun::rand2f(-1,1)),
                               // color
                               Vector3D(XFun::rand2f(.1,.7),XFun::rand2f(.3,.9), XFun::rand2f(.5,.9)) );
        // alpha
        bokeh->setAlpha( .1 );
        // add to simulation
        g_introRoot.addChild( bokeh );
        g_bokehs.push_back( bokeh );
    }

    g_introText = new YText(1);
    g_introText->set("[takeTwo]");
    g_introText->setCenterLocation(Vector3D(-0.1,0,0));
    g_introText->setWidth(3.0);
    g_introText->sca.x = 2.0f;
    g_introText->sca.y = 2.0f;
    g_introRoot.addChild( g_introText );
    Globals::sim.addChild( & g_introRoot );
    g_introText = new YText(1);
    g_introText->set("any key -> begin");
    g_introText->setCenterLocation(Vector3D(0.2,-0.4,0));
    g_introText->setWidth(3.0);
    g_introText->sca.x = 2.0f;
    g_introText->sca.y = 2.0f;
    g_introRoot.addChild( g_introText );
    Globals::sim.addChild( & g_introRoot );

}


//-----------------------------------------------------------------------------
// name: initialize_simulation( )
// desc: simulation setup
//-----------------------------------------------------------------------------
void initialize_simulation()
{
    generateIntro();

    /////////////
    // GAMEgme ////
    ///////////
    Globals::board = new TTGameBoard(16);
    g_gameRoot.addChild(Globals::board);


    //play head
    g_playHead = new TTPlayHead(16);
    g_gameRoot.addChild(g_playHead);
    g_playHead->active = false;

    g_pianoExps = new TTExplosionArea*[9];
    for(int i = 0; i < 9; i++){
        g_pianoExps[i] = new TTExplosionArea();
        g_pianoExps[i]->setLoc(Vector3D( -4.0f + 1.0f*i, 0.0f, 0.0f));
        switch(i){
            case 0: 
            case 7: { g_pianoExps[i]->setCol( Globals::ourRed  );  break; }

            case 1: 
            case 8: { g_pianoExps[i]->setCol( Globals::ourOrange); break; }

            case 2: { g_pianoExps[i]->setCol( Globals::ourYellow); break; }
            case 3: { g_pianoExps[i]->setCol( Globals::ourGreen ); break; }
            case 4: { g_pianoExps[i]->setCol( Globals::ourBlue  ); break; }
            case 5: { g_pianoExps[i]->setCol( Globals::ourPurple); break; }
            case 6: { g_pianoExps[i]->setCol( Globals::ourGray  ); break; }
        }
        g_pianoExps[i]->setOrix(20.0f);
        g_pianoExps[i]->spawn();
        g_gameRoot.addChild(g_pianoExps[i]);
    }

    g_hallway = new YEntity();
    // test cube
    // g_testCube = new TTCube();
    // g_testCube->setSca(Vector3D(10.0f,10.0f,10.0f));
    // g_testCube->setLoc(Vector3D(0,5.0f,0));
    // g_gameRoot.addChild(g_testCube);
    for(int i = 0; i < 40; i++){
        g_testCube = new TTCube();
        g_testCube->setSca(Vector3D(17.5f,1.0f,17.5f));
        g_testCube->setLoc(Vector3D(0,-10.0f+6.0f*i,0.0f));
        g_testCube->init();
        g_hallway->addChild(g_testCube);
    }
    g_gameRoot.addChild(g_hallway);
    g_hallway->active = false;


    // piano grid
    g_pianoGrid = new TTPianoNoteGrid();
    Globals::pianoNotes = new int*[9];
    memset(Globals::pianoNotes, 0, sizeof(int) * 9);
    for(int key = 0; key < 9; key++){
        Globals::pianoNotes[key] = new int[1024];
        memset(Globals::pianoNotes[key], 0, sizeof(int) * 1024);
    }
    Globals::pianoNotes[ g_keyToBucket['.'] ][ 0 ] = 1;
    Globals::pianoNotes[ g_keyToBucket[','] ][ 2 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['v'] ][ 4 ] = 1;

    Globals::pianoNotes[ g_keyToBucket['.'] ][ 16 ] = 1;
    Globals::pianoNotes[ g_keyToBucket[','] ][ 18 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['v'] ][ 20 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['b'] ][ 22 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['n'] ][ 26 ] = 1;

    Globals::pianoNotes[ g_keyToBucket['x'] ][ 32+0 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['.'] ][ 32+0 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['c'] ][ 32+2 ] = 1;
    Globals::pianoNotes[ g_keyToBucket[','] ][ 32+2 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['v'] ][ 32+4 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['n'] ][ 32+4 ] = 1;

    Globals::pianoNotes[ g_keyToBucket['x'] ][ 32+16 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['.'] ][ 32+16 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['c'] ][ 32+18 ] = 1;
    Globals::pianoNotes[ g_keyToBucket[','] ][ 32+18 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['v'] ][ 32+20 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['n'] ][ 32+20 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['m'] ][ 32+21 ] = 1;
    Globals::pianoNotes[ g_keyToBucket[','] ][ 32+22 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['.'] ][ 32+23 ] = 1;
    Globals::pianoNotes[ g_keyToBucket[','] ][ 32+24 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['n'] ][ 32+25 ] = 1;
    Globals::pianoNotes[ g_keyToBucket['b'] ][ 32+26 ] = 1;

    g_pianoGrid->spawn(Globals::pianoNotes);
    g_gameRoot.addChild(g_pianoGrid);
    g_pianoGrid->active = false;

    // line grid for debug
    g_lineGrid = new TTLineGrid(32);
    // g_gameRoot.addChild(g_leftTorusineGrid);



    // hall piece
    // for(int i = 0; i < 40; i++){
    //     g_hallPiece = new TTHallStraight(12.0f, Vector3D(0,0,10-2*i));
    //     g_gameRoot.addChild(g_hallPiece);
    // }

    // torus
    g_rightTorus = new TTTorusPathway(Vector3D(16.0f, 0.0f, 0.0f));
    g_rightTorus->setColor(Globals::ourBlue);
    g_gameRoot.addChild(g_rightTorus);
    g_leftTorus = new TTTorusPathway(Vector3D(-16.0f, 0.0f, 0.0f));
    g_leftTorus->setColor(Globals::ourYellow);
    // g_gameRoot.addChild(g_leftTorus);

    // lineCircles
    g_rightLineCircle  = new TTSnareLineCirclePathway( Vector3D( 16.0f, 0.0f, 0.0f));
    g_gameRoot.addChild(g_rightLineCircle);
    
    // title
    YText * title = new YText(1);
    title->set("hitMe");
    title->setCenterLocation(Vector3D(-3,1,0));
    title->setWidth(3.0);
    title->sca.x = 10.0f;
    title->sca.y = 10.0f;
    // g_gameRoot.addChild(title);

    //Explosion Areas (loc, color)
    //bottom
    g_expBottom = new TTExplosionArea();
    g_expBottom->setLoc( Vector3D( 0.0f, 0.0f, 0.0f ) );
    g_expBottom->setCol( Vector3D( 0.8f, 0.1f, 0.2f ) );
    g_expBottom->setOrix( 60.0f );
    g_expBottom->spawn();
    g_gameRoot.addChild(g_expBottom);
    //left
    g_expLeft = new TTExplosionArea();
    g_expLeft->setLoc( Vector3D( -2.0f, 0.0f, 0.0f ));
    g_expLeft->setCol( Vector3D( 0.8f, 0.8f, 0.2f ) );
    g_expLeft->setOrix( 60.0f );
    g_expLeft->spawn();
    g_gameRoot.addChild(g_expLeft);
    // right
    g_expRight = new TTExplosionArea();
    g_expRight->setLoc( Vector3D( 2.0f, 0.0f, 0.0f ));
    g_expRight->setCol( Vector3D( 0.2f, 0.2f, 0.8f ) );
    g_expRight->setOrix( 60.0f );
    g_expRight->spawn();
    g_gameRoot.addChild(g_expRight);

    g_rightTorus->active      = false;
    g_rightLineCircle->active = false;
    // g_rightLineCircle->active = false;

    Globals::sim.addChild( & g_gameRoot );

    /////////////
    // HUDhdd  ////
    ///////////
    g_topLeft = new YText(1);
    g_topLeft->set("Streak: ");
    g_topLeft->setCenterLocation(Vector3D(0,94,0));
    g_topLeft->setWidth(4.0);
    g_topLeft->sca.x = 50.0f;
    g_topLeft->sca.y = 50.0f;
    Globals::hud.addChild(g_topLeft);
    g_topLeft->active = false;

    g_bottomRight = new YText(1);
    g_bottomRight->set("Longest: ");
    g_bottomRight->setCenterLocation(Vector3D(60,1,0));
    g_bottomRight->setWidth(4.0);
    g_bottomRight->sca.x = 50.0f;
    g_bottomRight->sca.y = 50.0f;
    // Globals::hud.addChild(g_bottomRight);

    g_hello = new YText(1);
    g_hello->set("so many ways to say hello. ");
    g_hello->setCenterLocation(Vector3D(5,90,0));
    g_hello->setWidth(4.0);
    g_hello->sca.x = g_hello->sca.y = 50.0f;
    g_hello->col = Globals::ourYellow;
    Globals::hud.addChild(g_hello);

    g_smile = new YText(1);
    g_smile->set("A smile :D");
    g_smile->setCenterLocation(Vector3D(5,80,0));
    g_smile->setWidth(4.0);
    g_smile->sca.x = g_smile->sca.y = 25.0f;
    Globals::hud.addChild(g_smile);

    g_wink = new YText(1);
    g_wink->set("A wink  ;)");
    g_wink->setCenterLocation(Vector3D(5,75,0));
    g_wink->setWidth(4.0);
    g_wink->sca.x = g_wink->sca.y = 25.0f;
    Globals::hud.addChild(g_wink);
    
    g_wave = new YText(1);
    g_wave->set("A wave ");
    g_wave->setCenterLocation(Vector3D(20,66,0));
    g_wave->setWidth(4.0);
    g_wave->sca.x = g_wave->sca.y = 50.0f;
    Globals::hud.addChild(g_wave);

    g_press = new YText(1);
    g_press->set("[x]");
    g_press->setCenterLocation(Vector3D(15,3,0));
    g_press->setWidth(4.0);
    g_press->sca.x = g_press->sca.y = 50.0f;
    Globals::hud.addChild(g_press);

    g_smile->active =    false;
    g_wink->active =     false;
    g_wave->active =     false;

    g_gameRoot.active =  false;
    Globals::hud.active = false;
  }


//-------------------------------------------------------------------------------
// name: systemCascade()
// desc: trigger system wide update with time steps and draws the scene
//-------------------------------------------------------------------------------
void systemCascade()
{
    // get current time (once per frame)
    XGfx::getCurrentTime( true );

    // Timing loop
    YTimeInterval timeElapsed = XGfx::getCurrentTime() - Globals::simTime;
    Globals::simTime += timeElapsed;
    
    // special case: first update
    if( Globals::first )
    {
        // set time just enough for one update
        timeElapsed = 1.0f/30.0f;
        // set flag
        Globals::first = false;
    }
    
    // clamp it
    if( timeElapsed > SIM_SKIP_TIME )
        timeElapsed = SIM_SKIP_TIME;
    
    // update it
    // check paused
    if( !Globals::isPaused )
    {
        // update the world with a fixed timestep
        Globals::sim.updateAll( timeElapsed );
    }
    
    // redraw
    Globals::sim.drawAll();
    
    // set
    Globals::lastDelta = timeElapsed;
}


//-----------------------------------------------------------------------------
// name: initialize_data( )
// desc: load data
//-----------------------------------------------------------------------------
bool initialize_data()
{

    //drums_data
    g_keys['j'] = TT_SNARE; // clap
    g_keys['k'] = 37; // side stick
    g_keys['l'] = 38; // snare
    g_keys[';'] = 40; // snare 2
    g_keys['u'] = 48; // toms decending
    g_keys['i'] = 47; // mid
    g_keys['o'] = 45; // lowmid
    g_keys['p'] = 41; // low

    //cymbals
    g_keys['a'] = 53; // ride
    g_keys['s'] = 46; // open hihat
    g_keys['d'] = 42; // closed hihat
    g_keys['f'] = TT_HIHAT;
    g_keys['q'] = 52; // crash
    g_keys['w'] = 49; // crash
    g_keys['e'] = 55; // chinese
    g_keys['r'] = 57; // crash

    //middle randoms
    g_keys['t'] = 58; //crazy train intro
    g_keys['y'] = 65; //high timbale
    g_keys['g'] = 54; //tamborine
    g_keys['h'] = 56; //cow bell


    // g_keys['k'] = TT_HIHAT;
    // g_keys['f'] = TT_HIHAT;

    //piano
    g_keys['z'] = 55;
    g_keys['x'] = 57;
    g_keys['c'] = 59;
    g_keys['v'] = 60;
    g_keys['b'] = 62;
    g_keys['n'] = 64;
    g_keys['m'] = 65;
    g_keys[','] = 67;
    g_keys['.'] = 69;

    //keys to bucket
    g_keyToBucket['z'] = 0;
    g_keyToBucket['x'] = 1;
    g_keyToBucket['c'] = 2;
    g_keyToBucket['v'] = 3;
    g_keyToBucket['b'] = 4;
    g_keyToBucket['n'] = 5;
    g_keyToBucket['m'] = 6;
    g_keyToBucket[','] = 7;
    g_keyToBucket['.'] = 8;

    return true;
}




//-----------------------------------------------------------------------------
// name: tt_about()
// desc: ...
//-----------------------------------------------------------------------------
void tt_about()
{
    tt_line();
    fprintf( stderr, "[tt]: takeTwo\n" );
    tt_line();
    fprintf( stderr, "   | author: Micah Arvey\n" );
    fprintf( stderr, "   | Stanford University | CCRMA\n" );
    fprintf( stderr, "   | http://ccrma.stanford.edu/~marvey/\n" );
    fprintf( stderr, "   | version: %s\n", Globals::version.c_str() );
}




//-----------------------------------------------------------------------------
// name: tt_keys()
// desc: ...
//-----------------------------------------------------------------------------
void tt_keys()
{
    tt_line();
    fprintf( stderr, "[tt]: controls \n" );
    tt_line();
    fprintf( stderr, "  'Shift + Q' - quit\n" );
    fprintf( stderr, "  'Shift + S' - toggle fullscreen\n" );
    fprintf( stderr, "  'Shift + H' - print this help message\n" );
    fprintf( stderr, "  'Shift + A' - print about statement\n" );
}




//-----------------------------------------------------------------------------
// name: tt_help()
// desc: ...
//-----------------------------------------------------------------------------
void tt_help()
{
    tt_endline();
    tt_keys();
    tt_endline();
}




//-----------------------------------------------------------------------------
// name: tt_usage()
// desc: ...
//-----------------------------------------------------------------------------
void tt_usage()
{
    tt_line();
    fprintf( stderr, "[tt]: no command line arguments\n" );
    tt_line();

}




//-----------------------------------------------------------------------------
// name: tt_endline()
// desc: ...
//-----------------------------------------------------------------------------
void tt_endline()
{
    fprintf( stderr, "\n" );
}




//-----------------------------------------------------------------------------
// name: tt_line()
// desc: ...
//-----------------------------------------------------------------------------
void tt_line()
{
    fprintf( stderr, "---------------------------------------------------------\n" );
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( int w, int h )
{
    // save the new window size
    Globals::windowWidth = w; Globals::windowHeight = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
}




//-----------------------------------------------------------------------------
// Name: look( )
// Desc: ...
//-----------------------------------------------------------------------------
void look( )
{
    // go
    Globals::fov.interp( XGfx::delta() );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( Globals::fov.value, (GLfloat)Globals::windowWidth / (GLfloat)Globals::windowHeight, .005, 500.0 );
    
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity();
    // position the view point
    gluLookAt( 0.0f,
              Globals::viewRadius.value * sin( Globals::viewEyeY.value ),
              Globals::viewRadius.value * cos( Globals::viewEyeY.value ),
              0.0f, 0.0f, 0.0f,
              0.0f, ( cos( Globals::viewEyeY.x ) < 0 ? -1.0f : 1.0f ), 0.0f );
    
    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, Globals::light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, Globals::light1_pos );
}


//-----------------------------------------------------------------------------
// added by micah
// end Intro scene
//-----------------------------------------------------------------------------
void tt_endIntro(){
    // sim and graphics
    g_gameRoot.active = true;
    Globals::hud.active = true;
    g_introRoot.active = false;
    Globals::viewRadius.y = 10.0f;
    Globals::viewEyeY.y   = -0.9f;
    // end the welcome
    Globals::scene = PRESSX;
}

void updateStreaks(){
    g_topLeft->set("Streak: " + to_string(Globals::currentStreak));
    g_bottomRight->set("Longest: " + to_string(Globals::longestStreak));
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    //cout << "[tt] [gfx_keyboardFunc] : char pressed: " << key << endl;
    // Quit and Screen resize are most important
    switch( key )
    {
        case 'Q':
        {
            tt_endline();
            tt_line();
            tt_endline();
            exit(0);
        }
        case 'S':
        case 27: //esc
        {
            if( !Globals::fullscreen ){
                Globals::lastWindowWidth = Globals::windowWidth;
                Globals::lastWindowHeight = Globals::windowHeight;
                glutFullScreen();
            }else{
                glutReshapeWindow( Globals::lastWindowWidth, Globals::lastWindowHeight );
                glutPositionWindow( TT_WINDOW_X, TT_WINDOW_Y );
            }
            
            Globals::fullscreen = !Globals::fullscreen;
            //fprintf( stderr, "[tt]: fullscreen:%s\n", Globals::fullscreen ? "ON" : "OFF" );
            return;
        }   
        case 'A':
        {
            tt_endline();
            tt_about();
            tt_endline();
            return;
        }
        case 'G':
        {
            Globals::sim.dumpSceneGraph();
        }
        case 'H':
        {
            tt_help();
            return;
        }
    }
    
    // Then check to end intro
    if(Globals::scene <= INTRO){
        tt_endIntro();
        return;
    }

    // pianoInput
    switch( key )
    {
        case 'z':
        case 'x':
        case 'c':
        case 'v':
        case 'b':
        case 'n':
        case 'm':
        case ',':
        case '.':
        {
            // first presses
            if(Globals::scene <= PRESSN){
                if(key == 'x' && Globals::scene == PRESSX){
                    Globals::synth->noteOn( TT_SYNTH_CHANNEL, g_keys[key], 60);
                    Globals::scene = PRESSV;
                    g_press->set("[v]");
                    //was Vector3D(15,3,0)
                    g_press->setCenterLocation(Vector3D(35,3,0));
                    g_smile->active = true;
                }
                if(key == 'v' && Globals::scene == PRESSV){
                    Globals::synth->noteOn( TT_SYNTH_CHANNEL, g_keys[key], 60);
                    Globals::scene = PRESSN;
                    g_press->set("[n]");
                    g_press->setCenterLocation(Vector3D(55,3,0));
                    g_wink->active = true;
                }
                if(key == 'n' && Globals::scene == PRESSN){
                    Globals::synth->noteOn( TT_SYNTH_CHANNEL, g_keys[key], 60);
                    Globals::scene = FADEHELLO;
                    g_fadeHelloStartTime = Globals::now;
                    // g_playHead->active = true;
                    g_press->set("");
                    g_wave->active = true;
                    Globals::waveform->active = true;
                }
                return;
            }
            // normal
            // if(Globals::scene == PIANO || Globals::scene == COUNTOFF){
            if( (Globals::pianoNotes[ g_keyToBucket[key] ][ ( Globals::eigthNotes - 8 )%1024 ] &&  // just after
                  (Globals::timeSinceLastEigthNoteInSamples < Globals::periodInSamples/2.0 ) ) ||
                (Globals::pianoNotes[ g_keyToBucket[key] ][ ( Globals::eigthNotes - 8 + 1)%1024 ] &&  // just before
                 (Globals::timeSinceLastEigthNoteInSamples + Globals::periodInSamples/3.0) > Globals::periodInSamples) )
            {
                g_pianoExps[ g_keyToBucket[key] ]->explode();
                Globals::synth->noteOff ( TT_MULTI_CHANNEL, g_keys[key] );
                Globals::synth->noteOn( TT_MULTI_CHANNEL, g_keys[key], 127); 
                // incr current steak   
                if(++Globals::currentStreak > Globals::longestStreak)
                    Globals::longestStreak = Globals::currentStreak;
            } else { //bad hit :(
                Globals::currentStreak = 0;
            }
            updateStreaks();
            // }
            break;
        }
    }

    // skip all other input
    if( !(Globals::scene == DRUMS) )
        return;

    //  // change instrument section
    // switch( key )
    // {
    //     //numbers
    //     case '1':
    //     case '2':
    //     case '3':
    //     case '4':
    //     case '5':
    //     case '6':
    //     case '7':
    //     case '8':
    //     case '9':
    //     case '0':
    //         //cout << "[tt hax]: " << key - '0' << endl;
    //         Globals::synth->programChange( TT_MULTI_CHANNEL, key-'0' );
    //         break;
    // }

    // switch( key )
    // {
    //     case ']':
    //         Globals::viewEyeY.y -= .1f;
    //         //cout << Globals::viewEyeY.y << endl;
    //         break;
    //     case '[':
    //         Globals::viewEyeY.y += .1f;
    //         break;
    //     case '=':
    //         Globals::viewRadius.y = .975 * Globals::viewRadius.y;
    //         if( Globals::viewRadius.y < .001 ) Globals::viewRadius.y = .001;
    //         break;
    //     case '-':
    //         Globals::viewRadius.y = 1.025 * Globals::viewRadius.y;
    //         break;
    //     case '_':
    //     case '+':
    //         Globals::viewRadius.y = Globals::viewRadius.x + .7*(Globals::viewRadius.y-Globals::viewRadius.x);
    //         break;
    // }

    // drums section
    switch( key )
    {

        //bottom (spacebar) kick
        case 32 : 
            //successful kick
            if( (Globals::kickNotes[ (Globals::eigthNotes-8)%16] &&  // just after
                    Globals::timeSinceLastEigthNoteInSamples < Globals::periodInSamples/2.5) ||
                (Globals::kickNotes[(Globals::eigthNotes-8+1)%16] &&  // just before
                    (Globals::timeSinceLastEigthNoteInSamples + Globals::periodInSamples/4.0) > Globals::periodInSamples)
            ){
                Globals::lastKickHit = true;
                g_expBottom->explode();
                // g_rightTorus->showThenFade();
                Globals::synth->noteOff( DRUM_CHANNEL, TT_KICK );
                Globals::synth->noteOn(  DRUM_CHANNEL, TT_KICK, 127);
                //increment and set
                if(++Globals::currentStreak > Globals::longestStreak)
                    Globals::longestStreak = Globals::currentStreak;
            }else{ //bad kick
                Globals::currentStreak = 0;
            }
            updateStreaks();
            break;
        // left cybmals
        case 'q':
        case 'w':
        case 'e':
        case 'r':
        case 'a':
        case 's':
        case 'd':
        case 'f':
            // success cymbal
            if( (Globals::cymbalNotes[(Globals::eigthNotes-8)%16] &&  // just after
                    Globals::timeSinceLastEigthNoteInSamples < Globals::periodInSamples/2.5) ||
                (Globals::cymbalNotes[(Globals::eigthNotes-8+1)%16] &&  // just before
                    (Globals::timeSinceLastEigthNoteInSamples + Globals::periodInSamples/4.0) > Globals::periodInSamples)
            ){
                Globals::lastCymbalHit = true;
                g_expLeft->explode();
                // g_rightTorus->showThenFade();
                Globals::synth->noteOff( DRUM_CHANNEL, g_keys[key] );
                Globals::synth->noteOn(  DRUM_CHANNEL, g_keys[key], 127);
                //increment and set
                if(++Globals::currentStreak > Globals::longestStreak)
                    Globals::longestStreak = Globals::currentStreak;
            }else{// bad cymbal
                Globals::currentStreak = 0;
            }
            updateStreaks();
            break;
        // right drums
        case 'u':
        case 'i':
        case 'o':
        case 'p':
        case 'j':
        case 'k':
        case 'l':
        case ';':
            //successful snare
            if( (Globals::snareNotes[(Globals::eigthNotes-8)%16] &&  // just after
                    Globals::timeSinceLastEigthNoteInSamples < Globals::periodInSamples/2.5) ||
                (Globals::snareNotes[(Globals::eigthNotes-8+1)%16] &&  // just before
                    (Globals::timeSinceLastEigthNoteInSamples + Globals::periodInSamples/4.0) > Globals::periodInSamples)
            ){
                Globals::lastSnareHit = true;
                g_expRight->explode();
                // g_rightTorus->showThenFade();
                Globals::synth->noteOff( DRUM_CHANNEL, g_keys[key] );
                Globals::synth->noteOn(  DRUM_CHANNEL, g_keys[key], 127 );
                //increment and set
                if(++Globals::currentStreak > Globals::longestStreak)
                    Globals::longestStreak = Globals::currentStreak;
            }else{ //bad hit :(
                Globals::currentStreak = 0;
            }
            updateStreaks();
            break;
        // middle
        case 't':
        case 'y':
        case 'g':
        case 'h':
            Globals::synth->noteOff( DRUM_CHANNEL, g_keys[key] );
            Globals::synth->noteOn(  DRUM_CHANNEL, g_keys[key], 127);
            break;
    }
    
    
    // do a reshape since viewEyeY might have changed
    reshapeFunc( Globals::windowWidth, Globals::windowHeight );
    // post redisplay
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: specialFunc( )
// Desc: handles arrow stuff
//-----------------------------------------------------------------------------
void specialFunc( int key, int x, int y )
{
    // check
    bool handled = false;
    
    // if not handled
    if( !handled )
    {
        switch( key )
        {
            case GLUT_KEY_LEFT:
                break;
            case GLUT_KEY_RIGHT:
                break;
            case GLUT_KEY_UP:
                break;
            case GLUT_KEY_DOWN:
                break;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}


void switchToOrtho()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 100, 0, 100, -5, 1);       
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void switchBackToFrustum()
{
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawHud()
{
    // Timing loop
    YTimeInterval timeElapsed = XGfx::getCurrentTime() - Globals::simTime;
    
    // clamp it
    if( timeElapsed > SIM_SKIP_TIME )
        timeElapsed = SIM_SKIP_TIME;
    
    // check paused
    if( !Globals::isPaused )
        Globals::hud.updateAll( timeElapsed );
    
    // redraw
    Globals::hud.drawAll();

}


//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{

    if(Globals::first){
        // print keys
        tt_endline();
        tt_keys();
        tt_line();
        tt_endline();    
    }

    // get current time (once per frame)
    XGfx::getCurrentTime( true );

    // update
    Globals::bgColor.interp( XGfx::delta() );
    Globals::blendAlpha.interp( XGfx::delta() );

    // ----------------------------------------------
    // ugupdate
    g_hello->sca.x = 50 + 2*cos(   ( Globals::now * (360.0/16.0) / Globals::periodInSamples )  / (MY_PIE*2*3)   );
    g_hello->sca.y = 50 + 5*sin(   ( Globals::now * (360.0/16.0) / Globals::periodInSamples )  / (MY_PIE*2*3)   );
    if(Globals::scene == FADEHELLO){
        if(Globals::now > ( g_fadeHelloStartTime + Globals::periodInSamples * 16) ){
            Globals::scene = COUNTOFF;
            Globals::countoffStartTime = Globals::now;
            g_rightTorus->showThenFade();
            g_pianoGrid->active = true;
            g_pianoGrid->hidden = true;
            g_topLeft->active   = true;
        }
        // silence  
    }

    if(Globals::scene == COUNTOFF){
        if( Globals::countoffOffset / (Globals::periodInSamples*2) >= 0.0f)
            if( Globals::numEvents % 2 == 0){
                // piano
                g_rightTorus->active = false;
            }
            else{ // drums
                g_pianoGrid->hidden = true;
            }
            g_hello->active = false;

        if( Globals::countoffOffset / (Globals::periodInSamples*2) >= 1.0f){
            if( Globals::numEvents % 2 == 0){
                // piano
                g_rightLineCircle->active = false;
                g_pianoGrid->hidden = false;
            }
            else{ // drums

            }
            g_smile->active = false;
            g_wink->active = false;
        }
        if( Globals::countoffOffset / (Globals::periodInSamples*2) >= 2.0f){
            if( Globals::numEvents % 2 == 0){
                // piano
                Globals::viewEyeY.y   = -0.4f;
                Globals::waveform->ori.x = 0.0f;
            }
            else{ // drums
                Globals::viewEyeY.y   = -0.9f;
                Globals::waveform->ori.x = 60.0f;
                g_rightTorus->active = true;
                // g_pianoGrid->active = false;
            }
        }
        if( Globals::countoffOffset / (Globals::periodInSamples*2) >= 3.0f){
            if( Globals::numEvents % 2 == 0){
                 // piano
            }
            else{ // drums
                g_rightLineCircle->active = true;
            }
            g_playHead->active = true;
        }

        if(  Globals::now > Globals::countoffStartTime + Globals::countoffOffset ){

            // bookkeep
            g_countoffCountToShow++;
            g_wave->set( " " + to_string(g_countoffCountToShow) );
            Globals::countoffOffset += Globals::periodInSamples*2;

            // end on fifth downbeat
            if( g_countoffCountToShow == 5 )
            {
                g_wave->set( " " );
                if( Globals::numEvents % 2 == 0){
                    Globals::scene = PIANO;
                    Globals::pianoStartTime = Globals::now;
                }else{
                    Globals::scene = DRUMS;
                    Globals::drumsStartTime = Globals::now;
                }
                //cleanup
                Globals::numEvents++;
                Globals::countoffOffset = 0;
                g_countoffCountToShow = 0;
                
            }else {
                // hihat
                Globals::synth->noteOff( DRUM_CHANNEL, TT_HIHAT );
                Globals::synth->noteOn(  DRUM_CHANNEL, TT_HIHAT, 127);
            }

        }        
    }

    if(Globals::scene == PIANO){
        // cerr << ".";
        if(Globals::eigthNotes == 64+8){
            Globals::eigthNotes = 0;
            Globals::scene = COUNTOFF;
            Globals::countoffStartTime = Globals::now;
        }
    }

    if(Globals::scene == DRUMS){
        //cout << ".";
        if(Globals::eigthNotes == 32+8){
            Globals::eigthNotes = 0;
            Globals::scene = COUNTOFF;
            Globals::countoffStartTime = Globals::now;
            g_pianoGrid->setLoc(Vector3D(0,0,0));
            g_pianoGrid->reset();
            Globals::synth->programChange( TT_MULTI_CHANNEL, 5 );
        }
    }


    // ----------------------------------------

    // clear or blend
    if( Globals::blendScreen && Globals::blendAlpha.value > .0001 )
    {
        // clear the depth buffer
        glClear( GL_DEPTH_BUFFER_BIT );
        // blend screen
        blendPane();
    }
    else
    {
        // set the GL clear color - use when the color buffer is cleared
        glClearColor( Globals::bgColor.actual().x, Globals::bgColor.actual().y, Globals::bgColor.actual().z, 1.0f );
        // clear the color and depth buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
    
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    
    // save state
    glPushMatrix();

    // slew
    Globals::viewEyeY.interp( XGfx::delta());
    Globals::viewRadius.interp( XGfx::delta() );
    look();
    
    // cascade simulation
    systemCascade();
    
    // pop state
    glPopMatrix();

    switchToOrtho();
    drawHud();
    switchBackToFrustum();

    // flush gl commands
    glFlush();
    // swap the buffers
    glutSwapBuffers();
}




//-----------------------------------------------------------------------------
// name: blendPane()
// desc: blends a pane into the current scene
//-----------------------------------------------------------------------------
void blendPane()
{
    // enable blending
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // disable lighting
    glDisable( GL_LIGHTING );
    // disable depth test
    glDisable( GL_DEPTH_TEST );
    // blend in a polygon
    glColor4f( Globals::bgColor.actual().x, Globals::bgColor.actual().y, Globals::bgColor.actual().z, Globals::blendAlpha.value );
    // glColor4f( Globals::blendRed, Globals::blendRed, Globals::blendRed, Globals::blendAlpha );
    // reduce the red component
    // Globals::blendRed -= .02f;
    // if( Globals::blendRed < 0.0f ) Globals::blendRed = 0.0f;
    
    GLfloat h = 10;
    GLfloat d = -1;
    
    // draw the polyg
    glBegin( GL_QUADS );
    glVertex3f( -h, -h, d );
    glVertex3f( h, -h, d );
    glVertex3f( h, h, d );
    glVertex3f( -h, h, d );
    glEnd();
    
    // enable lighting
    glEnable( GL_LIGHTING );
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    // disable blending
    glDisable( GL_BLEND );
}




//-----------------------------------------------------------------------------
// name: renderBackground()
// desc: ...
//-----------------------------------------------------------------------------
void renderBackground()
{
    // save the current matrix
    glPushMatrix( );
    
    // restore
    glPopMatrix( );
}




//-------------------------------------------------------------------------------
// name: loadTexture()
// desc: load textures
//-------------------------------------------------------------------------------
void loadTextures()
{
    char filename[256];
    GLenum minFilter, maxFilter;
    int i;
    
    // log
    fprintf( stderr, "[tt]: loading textures...\n" );
    
    // set store alignment
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    
    // set filter types
    minFilter = GL_LINEAR;
    maxFilter = GL_LINEAR;
    
    // load tng flares
    for( i = TT_TEX_FLARE_TNG_1; i <= TT_TEX_FLARE_TNG_5; i++ )
    {
        glGenTextures( 1, &(Globals::textures[i]) );
        sprintf( filename, "%sflare-tng-%d.bw", Globals::datapath.c_str(), i - TT_TEX_FLARE_TNG_1 + 1 );
        setupTexture( filename, Globals::textures[i], minFilter, maxFilter );
    }
}



//--------------------------------------------------------------------------------
// name: setupTexture()
// desc: ...
//--------------------------------------------------------------------------------
void setupTexture( const char * filename, GLuint texobj,
                  GLenum minFilter, GLenum maxFilter, int useMipMaps )
{
    unsigned char * buf = NULL;
    int width = 0, height = 0, components = 0;
    
    glBindTexture( GL_TEXTURE_2D, texobj );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter );
    
    // log
    // fprintf( stderr, "[tt]: loading %s...\n", filename );
    
    // load luminance
    buf = loadLuminance( filename, &width, &height, &components );
    
    // log
    // fprintf( stderr, "[tt]: '%s' : %dx%dx%d\n", filename, width, height, components);
    
    // build mip maps
    if( useMipMaps )
    {
        gluBuild2DMipmaps( GL_TEXTURE_2D, 1, width, height,
                          GL_LUMINANCE, GL_UNSIGNED_BYTE, buf );
    }
    else
    {
        glTexImage2D( GL_TEXTURE_2D, 0, 1, width, height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, buf );
    }
    
    free(buf);
}




//-----------------------------------------------------------------------------
// name: checkTexDim( )
// desc: checks to see if a dim is a valid opengl texture dimension
//-----------------------------------------------------------------------------
bool checkTexDim( int dim )
{
    if( dim < 0 ) 
        return false;
    
    int i, count = 0;
    
    // count bits
    for( i = 0; i < 31; i++ )
        if( dim & ( 0x1 << i ) )
            count++;
    
    // this is true only if dim is power of 2
    return count == 1;
}
