//
//  CMvSprites.h
//  caminandes
//
//  Created by Marcelo Varanda on 2014-03-08.
//
//

#ifndef __caminandes__CMvSprites__
#define __caminandes__CMvSprites__

#include "gameplay.h"

#define     NUM_MAX_SPRITES     64

using namespace gameplay;


class CMvSprites {
public:

  CMvSprites();

  CMvSprites(char *filename_patern, int init_idx, int last_idx, float fps);
  SpriteBatch * GetNextSpriteToDraw(float enlapse);
  SpriteBatch * GetFirstSpriteToDraw(float enlapse, bool run_once);
  SpriteBatch * GetLastSpriteToDraw(float enlapse);                              // it also stops animation
  int GetIndex(void);
  void SetFps(float fps);
  void RepeatLastFrame(int nframes);
  void Pause(void);
  void Resume(float enlapse);
  void Backward(void);
  void Forward(void);

  // public properties
  bool                BackwardsProperty;
  SpriteBatch*        _sprites[NUM_MAX_SPRITES];


private:
  int                 _current_idx;
  int                 _num_sprites;
  bool                _pause;
  SpriteBatch*        _spriteMainBatch;
  float               _initialEnlapseTime;
  float               _totaEnlapseTime;
  bool                _run_once;
  int                 _run_once_last_idx;
  float               _fps;
  int                 _repeatLastFrame;
  bool                _finishRepeating;

};


//----------------------------------------------------------------
#endif /* defined(__caminandes__CMvSprites__) */
