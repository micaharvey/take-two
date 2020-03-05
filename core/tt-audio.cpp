// name: tt-audio.cpp
// desc: audio stuff for tt
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#include "tt-audio.h"
#include "y-fluidsynth.h"
#include "tt-globals.h"
#include "y-fft.h"
#include "y-waveform.h"
#include "y-echo.h"
#include <iostream>
using namespace std;

// globals
GLfloat g_BPM = Globals::bpm;
GLfloat g_EPM = g_BPM * 2;
GLfloat g_EPS = g_EPM / 60;
GLfloat g_period = 1 / g_EPS;
GLfloat g_periodInSamples = g_period * TT_SRATE;

YEcho * g_echo;
int g_waterfallDepth = 30;
SAMPLE ** g_waterfallArr = NULL;

// keys
map<char,int> g_pianoKeyToBucket;


//-----------------------------------------------------------------------------
// name: audio_callback
// desc: audio callback
//-----------------------------------------------------------------------------
static void audio_callback( SAMPLE * buffer, unsigned int numFrames, void * userData )
{
    // no sound during intro
    if(Globals::scene == INTRO){
        memset( buffer, 0, sizeof(SAMPLE)*numFrames*XAudioIO::numChannels() );
        return;
    }

    // copy to global buffer
    memcpy( Globals::inputBuffer, buffer, sizeof(SAMPLE)*numFrames*XAudioIO::numChannels() );

    // keep it synthy san diego
    Globals::synth->synthesize2( buffer, numFrames );
    // say what again?
    g_echo->synthesize2(buffer, numFrames);


    // did we miss a note??? not functional yet
    if( Globals::timeSinceLastEigthNoteInSamples >= g_periodInSamples/2.5 ){
        if(!Globals::lastSnareHit && Globals::snareNotes[(Globals::eigthNotes-1)%16]){
            //Globals::currentStreak = 0;
        }

        Globals::lastSnareHit  = false;
        Globals::lastCymbalHit = false;
        Globals::lastKickHit   = false;
    }

    // eigth notes
    if( Globals::scene >= COUNTOFF){
        if( Globals::now  >= g_periodInSamples*(Globals::eigthNotes+1) + Globals::countoffStartTime){
            // book keep!
            Globals::timeSinceLastEigthNoteInSamples = 0;
            Globals::eigthNotes++;
            // cerr << Globals::eigthNotes;
            // if( Globals::now == g_periodInSamples*8 + Globals::countoffStartTime )
            //     Globals::eigthNotes = 0;
            // if( Globals::pianoNotes[ 8 ][ (Globals::eigthNotes-8)%1024 ] )
            //     cerr << Globals::eigthNotes << endl;
        }
    }

    // waveform    
    // copy to mono buffer
    int channels = TT_NUMCHANNELS;
    double sum = 0.0;
    for( int i = 0; i < numFrames; i++ )
    {
        // zero out
        sum = 0;
        // loop over channels
        for( int j = 0; j < channels; j++ )
        {
            // sum
            sum += buffer[i*channels + j];
        }
        // set
        Globals::lastAudioBufferMono[i] = sum / channels;
    }
    // window it
    for( int i = 0; i < numFrames; i++ )
    {
        // multiply
        Globals::lastAudioBufferMono[i] *= Globals::audioBufferWindow[i];
        Globals::lastAudioBufferMono[i] *= 10;
    }
    // set in the wave
    if(Globals::now / numFrames % 16 == 0 )// 1024*16 < numFrames-1)
        Globals::waveform->set( Globals::lastAudioBufferMono, numFrames );

    // // fft
    // memcpy( Globals::fftBuffer, buffer, sizeof(SAMPLE)*numFrames*XAudioIO::numChannels() );
    // rfft( Globals::fftBuffer, numFrames, FFT_FORWARD );
    // // cast the result to a buffer of complex values (re,im)
    // complex * cbuf = (complex *)Globals::fftBuffer;
    // SAMPLE* frequencyBuf = new SAMPLE[numFrames];
    // for( int i = 0; i < numFrames/2; i++){
    //     frequencyBuf[i*2] = 10*pow( cmp_abs(cbuf[i]), .5 );
    //     frequencyBuf[i*2+1] = 10*pow( cmp_abs(cbuf[i]), .5 );
    // }
    // for (int i = g_waterfallDepth-1; i >= 1 ; --i)
    // {
    //     Globals::fftWaveformWaterfall[i]->set(g_waterfallArr[i-1], numFrames);
    //     g_waterfallArr[i] = g_waterfallArr[i-1];
    // }
    // g_waterfallArr[0] = frequencyBuf;
    // Globals::fftWaveformWaterfall[0]->set(frequencyBuf, numFrames);

    // tick
    Globals::now += numFrames;
    Globals::timeSinceLastEigthNoteInSamples += numFrames;

}




//-----------------------------------------------------------------------------
// name: tt_audio_init()
// desc: initialize audio system
//-----------------------------------------------------------------------------
bool tt_audio_init( unsigned int srate, unsigned int frameSize, unsigned int channels )
{
    // initialize
    if( !XAudioIO::init( 0, 0, srate, frameSize, channels, audio_callback, NULL ) )
        return false;

    // allocate and set
    Globals::periodInSamples = g_periodInSamples;

    Globals::snareNotes  = new int[16];
    Globals::cymbalNotes = new int[16];
    Globals::kickNotes   = new int[16];
    memset(Globals::snareNotes,  0, sizeof(int)*16);
    memset(Globals::cymbalNotes, 0, sizeof(int)*16);
    memset(Globals::kickNotes,   0, sizeof(int)*16);

    //piano
    initKeys();
    //Globals::pianoNotes = new int*[9]; //one bucket for each key
    for(int i = 0; i < 9; i++){
        //Globals::pianoNotes[i] = new int[128];
    }

    // allocate
    Globals::inputBuffer = new SAMPLE[frameSize*channels];
    Globals::fftBuffer   = new SAMPLE[frameSize*channels];
    // allocate mono buffer
    Globals::lastAudioBufferMono = new SAMPLE[frameSize];
    // allocate window buffer
    Globals::audioBufferWindow = new SAMPLE[frameSize];
    // allocate fft buffer
    Globals::fftBufferWindow = new SAMPLE[frameSize];
    // set frame size (could have changed in XAudioIO::init())
    Globals::lastAudioBufferFrames = frameSize;
    // set num channels
    Globals::lastAudioBufferChannels = channels;

    // compute the window
    hanning( Globals::audioBufferWindow, frameSize );

    // create waveform
    Globals::waveform = new YWaveform();
    // place it
    Globals::waveform->ori.x = 60.0f;
    // set the width
    Globals::waveform->setWidth( 40.0f );
    // set the height
    Globals::waveform->setHeight( 15.0f );
    // initialize it
    Globals::waveform->init( frameSize );
    // color
    Globals::waveform->col = Globals::ourGreen ;
    // active?
    Globals::waveform->active = false;
    Globals::sim.addChild( Globals::waveform );

    //waterfall buffer setup
    g_waterfallArr = new SAMPLE*[g_waterfallDepth];
    for(int i = 0; i < g_waterfallDepth; i++){
        g_waterfallArr[i] = new SAMPLE[TT_FRAMESIZE];
        memset( g_waterfallArr[i], 0, sizeof(SAMPLE)*TT_FRAMESIZE );
    }


    Globals::fftWaveformWaterfall = new YWaveform*[g_waterfallDepth];;

    // create fft waterfall
    for (int i = 0; i < g_waterfallDepth; ++i)
    {
        Globals::fftWaveform = new YWaveform();
        // place it
        Globals::fftWaveform->loc.y = -sin(i*0.05f)*2.5f;
        Globals::fftWaveform->loc.x = -cos(i*0.05f)*2.5f;
        Globals::fftWaveform->ori.x = 90.0f;
        Globals::fftWaveform->ori.z = i*1.0f;
        // set the width
        Globals::fftWaveform->setWidth( 16.0f );
        // set the height
        Globals::fftWaveform->setHeight( 6.0f );
        // initialize it
        Globals::fftWaveform->init( frameSize );
        // active?
        Globals::fftWaveform->active = true;
        Globals::fftWaveformWaterfall[i] = Globals::fftWaveform;
        //Globals::sim.addChild( Globals::fftWaveform );
    }

    initLoops();
    
    // instantiate a YFluidsynth
    Globals::synth = new YFluidSynth();
    Globals::synth->init( srate, 32 );
    Globals::synth->load( "data/sfonts/rocking8m11e.sf2", "" );
    Globals::synth->programChange( TT_MULTI_CHANNEL, 0 );
    Globals::synth->programChange( TT_SYNTH_CHANNEL, 50 );

    // YEcho
    g_echo = new YEcho(TT_SRATE);
    double d0 = 30.0/Globals::bpm;
    g_echo->setDelay( 0, d0 );
    g_echo->setDelay( 1, d0/2.0 );

    return true;
}

// printState
void printState(int beat){
    // ASCII
    
}

void initKeys(){
    g_pianoKeyToBucket['z'] = 0;
    g_pianoKeyToBucket['x'] = 1;
    g_pianoKeyToBucket['c'] = 2;
    g_pianoKeyToBucket['v'] = 3;
    g_pianoKeyToBucket['b'] = 4;
    g_pianoKeyToBucket['n'] = 5;
    g_pianoKeyToBucket['m'] = 6;
    g_pianoKeyToBucket[','] = 7;
    g_pianoKeyToBucket['.'] = 8;
}

void initLoops(){

    for(int i = 0; i < 32; i++){
        // Globals::pianoNotes[g_pianoKeyToBucket['.']][i] = 1;
    }

    // the drum loop
    Globals::kickNotes[0] =   1;
    Globals::snareNotes[1]  = 1;
    Globals::kickNotes[3] =   1;
    Globals::snareNotes[4]  = 1;

    Globals::kickNotes[6] =   1;
    Globals::kickNotes[7] =   1;
    Globals::snareNotes[9]  = 1;
    Globals::kickNotes[10]=   1;
    Globals::kickNotes[11]=   1;

    Globals::snareNotes[12] = 1;
    Globals::snareNotes[13] = 1;
    Globals::snareNotes[14] = 1;
    Globals::snareNotes[15] = 1;
    for(int i = 0; i < 8; i++)
        Globals::cymbalNotes[i*2] = 1;
}


//-----------------------------------------------------------------------------
// name: vq_audio_start()
// desc: start audio system
//-----------------------------------------------------------------------------
bool tt_audio_start()
{
    // start the audio
    if( !XAudioIO::start() )
    {
        // done
        return false;
    }
    
    return true;
}
