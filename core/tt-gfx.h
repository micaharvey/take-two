//-----------------------------------------------------------------------------
// name: tt-gfx.h
// desc: graphics stuff
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#ifndef __TT_GFX_H__
#define __TT_GFX_H__

#include "x-def.h"
#include "x-gfx.h"
#include <string>


// entry point for graphics
bool tt_gfx_init( int argc, const char ** argv );
void tt_gfx_loop();
void tt_about();
void tt_keys();
void tt_help();
void tt_usage();
void tt_endline();
void tt_line();
bool tt_initTexture( const std::string & filename, XTexture * tex );
XTexture * tt_loadTexture( const std::string & filename );




#endif
