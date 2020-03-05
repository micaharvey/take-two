//-----------------------------------------------------------------------------
// name: tt-entity.h
// desc: entities for tt visualization
//
// author: Micah Arvey
//   date: Fall 2014
//-----------------------------------------------------------------------------
#ifndef __TT_ENTITY_H__
#define __TT_ENTITY_H__
using namespace std;

#include "y-entity.h"
#include "x-buffer.h"
#include "x-fun.h"
#include <vector>
#include <stdio.h>;

#define TT_LINE_SNARE  0 
#define TT_LINE_CYMBAL 1 
#define TT_LINE_KICK   2 


//-----------------------------------------------------------------------------
// name: class TTSpark
// desc: ...
//-----------------------------------------------------------------------------
class TTSpark : public YEntity
{
public:
    // constructor
    TTSpark() : texture(0), ALPHA( 1, 1, 1 ) { }

public:
    // set
    void set( int _texture, float _size, float _alpha );
    void setTexture( int _texture );
    void setSize( float _size );
    
public:
    // update
    void update( YTimeInterval dt );
    // render
    void render();
    
public:
    // which ripple texture
    GLuint texture;
    // vertices
    GLfloat vertices[8];
    // alpha ramp
    Vector3D ALPHA;
};




//-----------------------------------------------------------------------------
// name: class TTTeapot
// desc: for testing
//-----------------------------------------------------------------------------
class TTTeapot : public YEntity
{
public:
    // update
    void update( YTimeInterval dt );
    // render
    void render();
};


//-----------------------------------------------------------------------------
// name: class TTCube
// desc: for grid of buttons (mainly adding location attribute)
//-----------------------------------------------------------------------------
class TTCube : public YEntity
{
public:
    TTCube(){
        size = Vector3D( 1, 1, 1.0f );
        position = Vector3D( 1, 1, 1 );
        ialpha = Vector3D(1,1,0.5);
        this->loc = position;
        this->sca = size;
    }

public:
    // set size
    void setSca( const Vector3D _size );
    // set position
    void setLoc(const Vector3D _position);
    void setAlpha(const Vector3D _ialpha);
    void init();
    void showThenFade();

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    Vector3D size;
    Vector3D position;
    // alpha slew
    Vector3D ialpha;
    int m_lastMoved;
};


class TTLineGrid : public YEntity
{
public:
    TTLineGrid(const unsigned int _size){
        size = _size; 
        center = Vector3D(0,0,0);
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    unsigned int size;
    Vector3D center;
};

class TTItem : public YEntity
{
public:
    TTItem(){
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:

};

class TTInventory : public YEntity
{
public:
    TTInventory( const Vector3D _topCenter ){
        loc = _topCenter;
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();
    virtual void addItem(TTItem item);

public:
    Vector3D loc;
    vector<TTItem> items;

};

class TTPlayPlace : public YEntity
{
public:
    TTPlayPlace( const Vector3D _center ){
        loc = _center;
        bool k = false;
        bool s = false; 
        bool h = false; 
        bool p = false;
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    Vector3D loc;
    bool k;
    bool s; 
    bool h;
    bool p;
};


class TTExplosionArea : public YEntity
{
public:
    TTExplosionArea(){
        m_lastExploded = 0;
        //m_loc = Vector3D(0.0f, 0.0f, 0.0f);
        //m_col = Vector3D(0.0f, 0.5f, 0.5f);
    };

public:
    virtual void update( YTimeInterval dt );
    virtual void render();
    virtual void explode();
    virtual void spawn();
    virtual void setLoc(const Vector3D _loc){m_loc = _loc; this->loc = m_loc;};
    virtual void setCol(const Vector3D _col){m_col = _col; this->col = m_col;};
    virtual void setOrix(const float _orix){m_orix = _orix; this->ori.x = m_orix;};


public:
    float m_orix;
    Vector3D m_loc;
    Vector3D m_col;
    vector<YBokeh*> m_bokehs;
    unsigned int m_lastExploded;
};


class TTGameBoard : public YEntity
{
public:
    TTGameBoard(const unsigned int _size){
        m_size = _size; 
        m_center = Vector3D(0,0,0);
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    unsigned int m_size;
    Vector3D m_center;
};


class TTHallStraight : public YEntity
{
public:
    TTHallStraight(const GLfloat _size, const Vector3D _loc){
        m_size = _size; 
        m_loc = _loc;
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    GLfloat m_size;
    Vector3D m_loc;
};


class TTTorusPathway : public YEntity
{
public:
    TTTorusPathway(const Vector3D _loc){
        m_size = 16.0f; 
        m_loc  = _loc;
        m_theta = 0.0f;
        ialpha = Vector3D(0,0,0.5);
        m_color = Vector3D(1,1,1);

    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();
    virtual void setColor(const Vector3D _col);
    virtual void showThenFade();

public:
    GLfloat  m_size;
    Vector3D m_loc;
    GLfloat  m_theta;
    Vector3D ialpha;
    Vector3D m_color;

};




class TTSnareLineCirclePathway : public YEntity
{
public:
    TTSnareLineCirclePathway(const Vector3D _loc){
        m_size = 16.0f; 
        m_loc  = _loc;
        m_rot = 0.0f;
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    GLfloat  m_size;
    Vector3D m_loc;
    GLfloat  m_rot;
};

class TTPlayHead : public YEntity
{
public:
    TTPlayHead(const unsigned int _size){
        m_size = _size; 
        m_center = Vector3D(0,0,0);
    }

public:
    virtual void update( YTimeInterval dt );
    virtual void render();

public:
    unsigned int m_size;
    Vector3D m_center;
};

// --------------------
// TTNote

class TTNote : public YEntity
{
public:
    // constructor
    TTNote(){ 
        m_textChar = new YText(1);
        m_textChar->setWidth(3.0);
        m_textChar->sca.x = 20.0f;
        m_textChar->sca.y = 20.0f;
        m_textChar->ori.x = 60.0f;
        m_textChar->loc.x = -0.5f;
        m_textChar->loc.y = -0.6f;
        this->addChild(m_textChar); 
    }

public:
    // set
    void setLoc(const Vector3D);
    void setCol(const Vector3D);
    void setChar(int);
    
public:
    // update
    void update( YTimeInterval dt );
    // render
    void render();
    
public:
    YText * m_textChar;

};


class TTPianoNoteGrid : public YEntity
{
public:
    // constructor
    TTPianoNoteGrid(){
        m_offset = 0;

    }

public:
    // spawn
    void spawn(int**);

    // set
    void setLoc(const Vector3D);
    void setOrix(const float);

    // reset
    void reset();
    
public:
    // update
    void update( YTimeInterval dt );
    // render
    void render();
    
public:
    // vector< vector < int > > m_grid;
    int ** m_grid;
    vector< TTNote * > m_notes;
    unsigned long m_offset;

};

#endif














