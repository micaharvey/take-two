//----------------------------------------------------------------------------
// name: main.cpp
// desc: tt entry point
//
// author: Micah Arvey
// date: Fall 2014
//----------------------------------------------------------------------------
#include <iostream>
#include "tt-audio.h"
#include "tt-gfx.h"
#include "tt-globals.h"
using namespace std;




//----------------------------------------------------------------------------
// name: main()
// desc: application entry point
//----------------------------------------------------------------------------
int main( int argc, const char ** argv )
{
    system( "pwd" );
    // invoke graphics setup and loop
    if( !tt_gfx_init( argc, argv ) )
    {
        // error message
        cerr << "[tt]: cannot initialize graphics/data system..." << endl;
        return -1;
    }
    
    // start real-time audio
    if( !tt_audio_init( TT_SRATE, TT_FRAMESIZE, TT_NUMCHANNELS ) )
    {
        // error message
        cerr << "[tt]: cannot initialize real-time audio I/O..." << endl;
        return -1;
    }
    
    // start audio
    if( !tt_audio_start() )
    {
        // error message
        cerr << "[tt]: cannot start real-time audio I/O..." << endl;
        return -1;
    }
    
    // graphics loop
    tt_gfx_loop();
    
    return 0;
}
