//-----------------------------------------------------------------------------
// name: tt-entity.cpp
// desc: entities for tt visualization
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#include "tt-globals.h"
#include "tt-entity.h"
#include "x-fun.h"
#include <cmath> 
using namespace std;


// globals
static int * g_lineNotes[] = {Globals::snareNotes,Globals::cymbalNotes,Globals::kickNotes};

// texture coordinates
static const GLshort g_coord[ ] = { 0, 0, 1, 0, 0, 1, 1, 1 };

//-----------------------------------------------------------------------------
// name: g_squareVertices
// desc: vertices for a cube
//-----------------------------------------------------------------------------
static const GLfloat g_squareVertices[] = 
{
    // FRONT
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    // BACK
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    // LEFT
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    // RIGHT
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    // TOP
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    // BOTTOM
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,
};




//-----------------------------------------------------------------------------
// name: g_squareNormals
// desc: normals for a cube
//-----------------------------------------------------------------------------
static const GLfloat g_squareNormals[] = 
{
    // FRONT
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    // BACK
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    0, 0, -1,
    // LEFT
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    -1, 0, 0,
    // RIGHT
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    // TOP
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    // BOTTOM
    0, -1, 0,
    0, -1, 0,
    0, -1, 0,
    0, -1, 0
};



//-----------------------------------------------------------------------------
// name: set()
// desc: start with this
//-----------------------------------------------------------------------------
void TTSpark::set( int _texture, float _size, float _alpha )
{
    setTexture( _texture );
    setSize( _size );
    alpha = _alpha;
}




//-----------------------------------------------------------------------------
// name: setSize()
// desc: set size
//-----------------------------------------------------------------------------
void TTSpark::setSize( float _size )
{
    _size *= 0.5f;
    vertices[0] = -_size; vertices[1] = -_size;
    vertices[2] = _size; vertices[3] = -_size;
    vertices[6] = _size; vertices[7] = _size;
    vertices[4] = -_size; vertices[5] = _size;
}




//-------------------------------------------------------------------------------
// name: setTexture()
// desc: ...
//-------------------------------------------------------------------------------
void TTSpark::setTexture( int _texture )
{
    // set it
    texture = _texture;
}



//-------------------------------------------------------------------------------
// name: update()
// desc: ...
//-------------------------------------------------------------------------------
void TTSpark::update( YTimeInterval dt )
{
    // slew
    ALPHA.interp( dt );
    // set
    this->alpha = ALPHA.value;
}




//-------------------------------------------------------------------------------
// name: render()
// desc: ...
//-------------------------------------------------------------------------------
void TTSpark::render( )
{
    // disable depth
    glDisable( GL_DEPTH_TEST );
    // enable texture
    glEnable( GL_TEXTURE_2D );
    // set blend function
    glBlendFunc( GL_ONE, GL_ONE );
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    // enable blend
    glEnable( GL_BLEND );

    // bind to texture
    glBindTexture( GL_TEXTURE_2D, Globals::textures[this->texture] );

    // set color
    // glColor4f( col.x, col.y, col.z, alpha * ALPHA.value );

    // set vertex coordinates
    glVertexPointer( 2, GL_FLOAT, 0, this->vertices );
    glEnableClientState( GL_VERTEX_ARRAY );

    // set texture coordinates
    glTexCoordPointer( 2, GL_SHORT, 0, g_coord );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // draw stuff!
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // disable texture
    glDisable( GL_TEXTURE_2D );
    // disable blend
    glDisable( GL_BLEND );

    // disable client states
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}




//-------------------------------------------------------------------------------
// name: update()
// desc: ...
//-------------------------------------------------------------------------------
void TTTeapot::update( YTimeInterval dt )
{
    // do nothing for now
}




//-------------------------------------------------------------------------------
// name: render()
// desc: ...
//-------------------------------------------------------------------------------
void TTTeapot::render()
{
    // enable lighting
    glEnable( GL_LIGHTING );
    // set color
    glColor4f( col.x, col.y, col.z, alpha );
    // render stuff
    glutSolidTeapot( 1.0 );
    // disable lighting
    glDisable( GL_LIGHTING );
}


//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTCube::render()
{
    // push
    // glPushMatrix();

    // scale
    // this->sca = size;
    // cout << size.x << endl;
    
    // translate (added)
    // this->loc = position;

    // draw it
    glutWireCube(1.0f);
    
    // pop
    // glPopMatrix();

}




//-----------------------------------------------------------------------------
// name: update()
// desc: ...
//-----------------------------------------------------------------------------
void TTCube::update( YTimeInterval dt )
{
    // interp
    // size.interp( dt );
    ialpha.interp( dt );
    // set it
    alpha = ialpha.value;

    if(Globals::scene == PIANO && Globals::now != m_lastMoved && Globals::now % 512 == 0){
        this->loc.y -= 0.1f;
        m_lastMoved = Globals::now;
        this->sca.x = 25.0f + 1*cos(2*MY_PIE*this->loc.y/4.0);
        // this->sca.z = 7.5 + 1*sin(2*MY_PIE*this->loc.y/4.0);
        if(this->loc.y <= -10.1f){
            this->loc.y = -10+2.0f*39;
        }
    }
}

void TTCube::setAlpha(const Vector3D _ialpha)
{
    ialpha = _ialpha;
}

void TTCube::showThenFade()
{
    ialpha = Vector3D(1,0,5);
}

void TTCube::init()
{
    m_lastMoved = 512;
}

//-----------------------------------------------------------------------------
// name: setSize
// desc: ...
//-----------------------------------------------------------------------------
void TTCube::setSca( const Vector3D _size )
{
    // set size
    this->sca = _size;
}


//-----------------------------------------------------------------------------
// name: pos()
// desc: ...
//-----------------------------------------------------------------------------
void TTCube::setLoc( const Vector3D _position )
{
    // set posish
    this->loc = _position;
}

void TTLineGrid::update( YTimeInterval dt )
{
    // do nothing
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTLineGrid::render(  )
{
    //glRotatef(  60.0f,  //angle
    //            -1,0,0); //vector
    // draw 'plus' at each diagonal point
    for (int row = 0; row <= size; row++){
        for (int col = 0; col <= size; col++){
            if(row!=col) continue;
            //color
            if(row == size/2.0 && col == size/2.0)
                glColor3f( 1,1,1 );
            else if (abs(row-size/2.0) == size/4.0 || abs(col-size/2.0) == size/4.0)
                glColor3f(0.5,0.9,0.5);
            else
                glColor3f( 0.1, 0.5, 0.1 );
            // vertical
            glBegin( GL_LINE_STRIP );
                glVertex2f( col-size/2.0, 0.0-size/2.0 );
                glVertex2f( col-size/2.0, 0.0+size/2.0 );
            glEnd();
            // horizontal
            glBegin( GL_LINE_STRIP );
                glVertex2f( 0.0-size/2.0, row -size/2.0 );
                glVertex2f( 0.0+size/2.0, row -size/2.0 );
            glEnd();
        }
    }

    //debug
    //glutSolidTeapot(1);
}

void TTInventory::update( YTimeInterval dt ){
    //nada
}

void TTInventory::render( ){

    glTranslatef( loc.x, loc.y, loc.z);

    glPushMatrix();
    glColor3f( 0.7, 0.1, 0.1 );
    glTranslatef( 0, -2, 0);
    glutSolidCube(0.5);
    glPopMatrix();


    glPushMatrix();
    glColor3f( 0.3, 0.3, 0.9 );
    glTranslatef( 0, -4, 0);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f( 0.7, 0.7, 0.1 );
    glTranslatef( 0, -6, 0);
    glutSolidCube(0.5);
    glPopMatrix();


    glPushMatrix();
    glColor3f( 0.7, 0.8, 0.6 );
    glTranslatef( 0, -8, 0);
    glutSolidCube(0.5);
    glPopMatrix();
    
}

void TTInventory::addItem( TTItem item ){
    //nada
}

void TTItem::update( YTimeInterval dt ){
    //nada
}

void TTItem::render( ){
    //nada
}

void TTPlayPlace::update( YTimeInterval dt ){

}

void TTPlayPlace::render( ){

    // print for debug
    // glPushMatrix();
    // glColor3f( 0.1, 0.5, 0.1 );
    // glTranslatef( loc.x, loc.y, loc.z);
    // glutSolidCube(1);
    // glPopMatrix();

    if(k){
        glPushMatrix();

        glColor3f( 0.7, 0.1, 0.1 );
        glTranslatef( loc.x, loc.y-0.5, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
    if(s){
        glPushMatrix();

        glColor3f( 0.3, 0.3, 0.9 );
        glTranslatef( loc.x+0.25, loc.y, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
    if(h){
        glPushMatrix();

        glColor3f( 0.7, 0.7, 0.1 );
        glTranslatef( loc.x-0.25, loc.y, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
    if(p){
        glPushMatrix();

        glColor3f( 0.7, 0.8, 0.6 );
        glTranslatef( loc.x, loc.y+0.5, loc.z);
    
        // draw it
        glutSolidCube(0.5);
    
        glPopMatrix();
    }
}



void TTGameBoard::update( YTimeInterval dt )
{
    // do nothing
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTGameBoard::render(  )
{
    //debug teapot to show its there
    //glutSolidTeapot(1);
}


void TTExplosionArea::update( YTimeInterval dt )
{
    // do nothing
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTExplosionArea::render(  )
{
    // only explode for a quarter second
    if(Globals::now - m_lastExploded > TT_SRATE/4)
        this->active = false;
}

void TTExplosionArea::explode( )
{
    // activate
    this->active = true;
    //explode :D
    for( int i = 0; i < m_bokehs.size(); i++ )
    {
        m_bokehs[i]->iLoc.updateSet(m_loc);
        m_bokehs[i]->iLoc.update( Vector3D(m_loc.x+XFun::rand2f(-12,12),
                                           m_loc.y+XFun::rand2f(3,35),
                                           m_loc.z+XFun::rand2f(-10,10) ) );
    }
    // book keep
    this->m_lastExploded = Globals::now;
}

void TTExplosionArea::spawn(  )
{
    this->active = false;
    for( int i = 0; i < 256; i++ )
    {
        // create a spark
        YBokeh * bokeh = new YBokeh();
        // set attributes
        bokeh->set( 1.0f, 1.0f, 1.0f, 1.0f, 4 );
        bokeh->iLoc.updateSet(m_loc);
        // set bokeh
        bokeh->setBokehParams( // initial time
                               XFun::rand2f(0,10),
                               // freq
                               XFun::rand2f(1,3),
                               // time step
                               50,
                               // location
                               Vector3D(m_loc.x+XFun::rand2f(-20,20),m_loc.y+XFun::rand2f(1,25),m_loc.z+XFun::rand2f(-3,5)),
                               // color
                               Vector3D(m_col.x+XFun::rand2f(-0.1f,0.1f),m_col.y+XFun::rand2f(-0.1f,0.2f), m_col.z + XFun::rand2f(-0.1f,0.3f)) );
        // alpha
        bokeh->setAlpha( 1.8  );
        // add to simulation
        this->addChild( bokeh );
        m_bokehs.push_back( bokeh );
    }
}



void TTHallStraight::update( YTimeInterval dt )
{
    m_loc.z += 4*dt;
    if(m_loc.z > 10)
        m_loc.z = -60;
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTHallStraight::render(  )
{
    // color
    this->col = Vector3D( XFun::rand2f(0.1f,0.2f), XFun::rand2f(0.1f,0.2f),  XFun::rand2f(0.7f,0.8f) );
    this->loc = m_loc;

    // walkway only
    glBegin( GL_LINE_STRIP );
        glVertex2f( -m_size, -m_size/2.0f );
        glVertex2f(  m_size, -m_size/2.0f );
    glEnd();

}





void TTTorusPathway::update( YTimeInterval dt )
{
    // interp
    ialpha.interp( dt );
    // rotates
    // m_rot += 100.0*dt;
    // m_rot = Globals::now*22.5;
    m_theta = Globals::now * (360.0/16.0) / Globals::periodInSamples;
    // set it
    alpha = ialpha.value;
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTTorusPathway::render(  )
{
    // rotate
    this->ori.z = m_theta;
    // translate
    this->loc   = m_loc;
    float scax = 1 + 0.05*cos(m_theta/(MY_PIE*2*3));
    float scay = 1 - 0.05*cos(m_theta/(MY_PIE*2*3));
    //cout << scax << endl;
    this->sca.x = scax;
    this->sca.y = scay;


    glutWireTorus ( 8.0,   //GLdouble innerRadius,
                    16.0,  //GLdouble outerRadius,
                    8, 16);//GLint nsides, GLint rings

    // // for vertical
    // this->ori.x = 90.0f;
    // this->ori.z = 90.0f;
    // this->ori.y = m_theta;


}

void TTTorusPathway::showThenFade()
{
    ialpha = Vector3D(1,1,2.9);
}


void TTTorusPathway::setColor(const Vector3D _col)
{
    m_color = _col;
    this->col = m_color;
}



void TTSnareLineCirclePathway::update( YTimeInterval dt )
{
    if(Globals::scene != DRUMS)
        return;
    m_rot = ( Globals::now - Globals::drumsStartTime) * (180.0/8.0) / Globals::periodInSamples;
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTSnareLineCirclePathway::render(  )
{
    // translate
    this->loc = m_loc;
    // rotate
    this->ori.z = m_rot;

    // if(Globals::scene >= COUNTOFF){
    //     this->active = true;
    // }

    float rotInc = -360.0f/16.0f; 
    for(int i = 0; i < 16; i++){
        // color
        glColor3f( 0.25f, 0.25f, 1.0f );
        // line lead
        glBegin( GL_LINE_STRIP );
            glVertex2f( 10*cos(rotInc*i*MY_PIE/180.0f), 10*sin(rotInc*i*MY_PIE/180.0f) );
            glVertex2f( 14*cos(rotInc*i*MY_PIE/180.0f), 14*sin(rotInc*i*MY_PIE/180.0f) );
        glEnd();
         // color
        glColor3f(  1.0f, 0.0f, 0.0f);
        // line lead
        glBegin( GL_LINE_STRIP );
            glVertex2f( 14*cos(rotInc*i*MY_PIE/180.0f), 14*sin(rotInc*i*MY_PIE/180.0f) );
            glVertex2f( 18*cos(rotInc*i*MY_PIE/180.0f), 18*sin(rotInc*i*MY_PIE/180.0f) );
        glEnd();
         // color
        glColor3f(1.0f, 1.0f, 0.0f );
        // line lead
        glBegin( GL_LINE_STRIP );
            glVertex2f( 18*cos(rotInc*i*MY_PIE/180.0f), 18*sin(rotInc*i*MY_PIE/180.0f) );
            glVertex2f( 22*cos(rotInc*i*MY_PIE/180.0f), 22*sin(rotInc*i*MY_PIE/180.0f) );
        glEnd();
    }


    rotInc = -360.0f/16.0f; 
    for(int i = 0; i < 16; i++){
        // color
        glColor3f( 0.25f, 0.25f, 1.0f );
        // line lead
        glBegin( GL_LINE_STRIP );
            glVertex2f( 10*cos(rotInc*i*MY_PIE/180.0f), 10*sin(rotInc*i*MY_PIE/180.0f) );
            glVertex2f( 14*cos(rotInc*i*MY_PIE/180.0f), 14*sin(rotInc*i*MY_PIE/180.0f) );
        glEnd();
         // color
        glColor3f(  1.0f, 0.0f, 0.0f);
        // line lead
        glBegin( GL_LINE_STRIP );
            glVertex2f( 14*cos(rotInc*i*MY_PIE/180.0f), 14*sin(rotInc*i*MY_PIE/180.0f) );
            glVertex2f( 18*cos(rotInc*i*MY_PIE/180.0f), 18*sin(rotInc*i*MY_PIE/180.0f) );
        glEnd();
         // color
        glColor3f(1.0f, 1.0f, 0.0f );
        // line lead
        glBegin( GL_LINE_STRIP );
            glVertex2f( 18*cos(rotInc*i*MY_PIE/180.0f), 18*sin(rotInc*i*MY_PIE/180.0f) );
            glVertex2f( 22*cos(rotInc*i*MY_PIE/180.0f), 22*sin(rotInc*i*MY_PIE/180.0f) );
        glEnd();
        // place notes
        // enable lighting
        glEnable( GL_LIGHTING );
        // color
        glColor3f( 0.25f, 0.25f, 1.0f );
        // cube size
        float cubeSize = 1.1f;
        if(Globals::snareNotes[(i+8)%16]){
            glPushMatrix();
                glTranslatef(12*cos(rotInc*i*MY_PIE/180.0f), 12*sin(rotInc*i*MY_PIE/180.0f), 0);
                // render stuff
                glutWireTeapot( cubeSize );
            glPopMatrix();
        }
        // color
        glColor3f( 1.0f, 0.0f, 0.0f );
        if(Globals::kickNotes[(i+8)%16]){
            glPushMatrix();
                glTranslatef(16*cos(rotInc*i*MY_PIE/180.0f), 16*sin(rotInc*i*MY_PIE/180.0f), 0);
                // render stuff
                glutWireTeapot( cubeSize );
            glPopMatrix();
        }
        // color
        glColor3f( 1.0f, 1.0f, 0.0f );
        if(Globals::cymbalNotes[(i+8)%16]){
            glPushMatrix();
                glTranslatef(20*cos(rotInc*i*MY_PIE/180.0f), 20*sin(rotInc*i*MY_PIE/180.0f), 0);
                // render stuff
                glutWireTeapot( cubeSize );
            glPopMatrix();
        }
        // disable lighting
        glDisable( GL_LIGHTING );

    }

    // // debug teapot to show its there
    // glPushMatrix();
    // // color
    // glColor3f( 0.1,0.3,0.5 );
    // // translate
    // glTranslatef(4, 4, 0);
    // // rotate
    // glRotatef(  60.0f,  //angle
    //             -1,0,0); //vector
    // glutSolidTeapot(1);
    // glPopMatrix();
}


void TTPlayHead::update( YTimeInterval dt )
{
    // do nothing
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTPlayHead::render(  )
{
    // color
    glColor3f( 0.8, 0.8, 0.4 );

    // // Left
    // glBegin( GL_LINE_STRIP );
    //     glVertex2f( 0.0-m_size/2.0, 0.0+m_size/2.0 );
    //     glVertex2f( 0.0-m_size/2.0, 0.0-m_size/2.0 );
    // glEnd();
    // Bottom
    glBegin( GL_LINE_STRIP );
        glVertex2f( 0.0-m_size/2.0, 0.0);
        glVertex2f( 0.0+m_size/2.0, 0.0);
    glEnd();
    // // Right
    // glBegin( GL_LINE_STRIP );
    //     glVertex2f( 0.0+m_size/2.0, 0.0-m_size/2.0 );
    //     glVertex2f( 0.0+m_size/2.0, 0.0+m_size/2.0 );
    // glEnd();

    //debug teapot to show its there
    //glutSolidTeapot(1);
}


void TTNote::update( YTimeInterval dt )
{
    // do nothing
    this->sca.z =  1 - 0.5*cos(   ( Globals::now * (360.0/16.0) / Globals::periodInSamples )  / (MY_PIE*2*3)   );
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTNote::render(  )
{
    // void glutWireSphere(GLdouble radius,
    //                 GLint slices, GLint stacks);

    glutWireSphere(0.75f, 8, 8);
}

//-----------------------------------------------------------------------------
// name: set()
// desc: start with this
//-----------------------------------------------------------------------------
void TTNote::setLoc(const Vector3D _loc)
{
    this->loc = _loc;
}

void TTNote::setCol(const Vector3D _col)
{
    this->col = _col;
}

void TTNote::setChar(const int _keyNum)
{
    switch(_keyNum){
        case 0: {m_textChar->set( "z" ); break;}
        case 1: {m_textChar->set( "x" ); break;}
        case 2: {m_textChar->set( "c" ); break;}
        case 3: {m_textChar->set( "v" ); break;}
        case 4: {m_textChar->set( "b" ); break;}
        case 5: {m_textChar->set( "n" ); break;}
        case 6: {m_textChar->set( "m" ); break;}
        case 7: {m_textChar->set( "," ); break;}
        case 8: {m_textChar->set( "." ); break;}
    }
    
}


void TTPianoNoteGrid::update( YTimeInterval dt )
{

    if( Globals::scene >= COUNTOFF && Globals::now >= Globals::countoffStartTime + m_offset ){
        m_offset += Globals::periodInSamples/16;
        this->loc.y -= 0.25f;
        // this->sca.x = 7.5 + 1*cos(2*MY_PIE*this->loc.y/4.0);
        // // this->sca.z = 7.5 + 1*sin(2*MY_PIE*this->loc.y/4.0);
        // if(this->loc.y <= -10.1f){
        //     this->loc.y = -10+2.0f*39;
        // }
    }
}

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
void TTPianoNoteGrid::render(  )
{

    // m_notes.active = true;

}

//-----------------------------------------------------------------------------
// name: spawn()
// desc: ...
//-----------------------------------------------------------------------------
void TTPianoNoteGrid::spawn( int ** _grid)
{
    m_grid = _grid;
    TTNote * note = new TTNote();

    //     // colors
    // static Vector3D ourWhite;
    // static Vector3D ourRed;
    // static Vector3D ourBlue;
    // static Vector3D ourOrange;
    // static Vector3D ourGreen;
    // static Vector3D ourGray;
    // static Vector3D ourYellow;
    // static Vector3D ourSoftYellow;
    // static Vector3D ourPurple;
    Vector3D color;

    for(int key = 0; key < 9; key++){
        // for(int count = 0; count < 8; count++){
        //     note = new TTNote();
        //     note->setLoc(Vector3D( -8.0f + 2.0f*key, 0.0f + 4.0f*count, 0.0f ));
        // }
        for(int n = 0; n < 128; n++){
            if(m_grid[key][n]){
                note = new TTNote();
                note->setLoc(Vector3D( -8.0f + 2.0f*key,  (4.0f * 8 ) + 4.0f*n, 0.0f )); // add to y 
                switch(key){
                    case 0: 
                    case 7: { color = Globals::ourRed; break; }

                    case 1: 
                    case 8: { color = Globals::ourOrange; break; }

                    case 2: { color = Globals::ourYellow; break; }
                    case 3: { color = Globals::ourGreen; break; }
                    case 4: { color = Globals::ourBlue; break; }
                    case 5: { color = Globals::ourPurple; break; }
                    case 6: { color = Globals::ourGray; break; }
                }
                note->setCol(color);
                note->setChar(key);
                // cout << '.';
                // m_notes.push_back(note);
                this->addChild(note);
            }
        }
    }
}


//-----------------------------------------------------------------------------
// name: set()
// desc: start with this
//-----------------------------------------------------------------------------
void TTPianoNoteGrid::setLoc(const Vector3D _loc)
{
    this->loc = _loc;
}

//-----------------------------------------------------------------------------
// name: set()
// desc: start with this
//-----------------------------------------------------------------------------
void TTPianoNoteGrid::setOrix(const float _orix)
{
    this->ori.x = _orix;
}

//-----------------------------------------------------------------------------
// name: reset()
// desc: ...
//-----------------------------------------------------------------------------
void TTPianoNoteGrid::reset()
{
    m_offset = 0;
}








