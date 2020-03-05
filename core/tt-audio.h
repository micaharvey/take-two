//-----------------------------------------------------------------------------
// name: tt-audio.h
// desc: audio stuff
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#ifndef __TT_AUDIO_H__
#define __TT_AUDIO_H__




// init audio
bool tt_audio_init( unsigned int srate, unsigned int frameSize, unsigned int channels );
// start audio
bool tt_audio_start();

void initLoops();
void initKeys();

// ascii
void printState(int beat);



#endif
