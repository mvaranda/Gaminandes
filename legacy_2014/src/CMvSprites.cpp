//
//  CMvSprites.cpp
//  caminandes
//
//  Created by Marcelo Varanda on 2014-03-08.
//
//

#include "CMvSprites.h"
#include "common.h"

#define     MAX_FILENAME_LEN        512
#define     MAX_NUM_TEXT_FIELD      64

#define         FRAME_DURATION          0.025 // seconds
#define         DEFAULT_FPS             40


CMvSprites::CMvSprites(char *filename_patern, int init_idx, int last_idx, float fps)
{
  int z, i, append_idx = strlen(filename_patern);

  // init All
  _current_idx = 0;
  _pause = true;
  _spriteMainBatch=NULL;
  _initialEnlapseTime = 0;
  _totaEnlapseTime = 0;
  _run_once = 0;
  _run_once_last_idx = 0;
  _fps = 0;
  _repeatLastFrame = 0;
  _finishRepeating = 0;

  _num_sprites = last_idx - init_idx + 1;
  char num_txt[MAX_FILENAME_LEN];
  char file[MAX_FILENAME_LEN];
  BackwardsProperty = false;
  if (fps == 0)
    _fps = DEFAULT_FPS;
  else
    _fps = fps;


  if (append_idx > (MAX_FILENAME_LEN - MAX_NUM_TEXT_FIELD)) {
    LOG("Need to increase fine buffer as need %d.", append_idx + MAX_FILENAME_LEN);
    ::exit(0);
  }

  if ( _num_sprites > NUM_MAX_SPRITES || _num_sprites < 0) {
    LOG("Need to increase NUM_MAX_SPRITES as we have %d sprites.", _num_sprites);
    ::exit(0);
  }
  z=0;
  strcpy(file, filename_patern);
  for (i=init_idx; i <= last_idx; i++) {
    sprintf(num_txt, "%.4d",i);
    strcpy(&file[append_idx], num_txt);
    strcat(file, ".png");
    //LOG("file: \"%s\"", file);
    _sprites[z] = SpriteBatch::create(file, NULL, 0);
    if (_sprites[z] == NULL) {
      LOG("Could not create sprite obj for file \"%s\"", file);
      ::exit(0);
    }
    z++;

  }

}

void CMvSprites::SetFps(float fps)
{
  _fps = fps;
}

int CMvSprites::GetIndex(void)
{
  if (_finishRepeating)
    return _run_once_last_idx;

  return _current_idx;
}

void CMvSprites::RepeatLastFrame(int nframes) {
  _repeatLastFrame = nframes;
}

SpriteBatch * CMvSprites::GetNextSpriteToDraw(float enlapse)
{
  float delta;
  int idx;

  if (_finishRepeating) {
    if (_repeatLastFrame > 0) {
      _repeatLastFrame--;
      return _sprites[_run_once_last_idx];
    }
    _finishRepeating = false;
    return NULL;
  }


  if (_pause) {
    return _sprites[_current_idx];
  }

  if (_initialEnlapseTime == 0) {
    return GetFirstSpriteToDraw(enlapse, _run_once);
  }
  _totaEnlapseTime += enlapse;
  delta =_totaEnlapseTime - _initialEnlapseTime;
  _current_idx = (int) ((delta/1000) * _fps) % _num_sprites;
  if (_current_idx < 0 || _current_idx > _num_sprites) {
    LOG("_current_idx overflow %d", _current_idx);
    ::exit(0);
  }

  if (_run_once) {
    if (_run_once_last_idx > _current_idx)  { //detect overlap
      _run_once = false;
      if (_repeatLastFrame > 0) {
        _repeatLastFrame--;
        _finishRepeating = true;
        return _sprites[_run_once_last_idx];
      }
      return NULL;
    }
    _run_once_last_idx = _current_idx;

    return _sprites[_current_idx];
  }
  //LOG("_current_idx=%d, _totaEnlapseTime=%f, enlapse=%f, _initialEnlapseTime=%f", _current_idx, _totaEnlapseTime, enlapse, _initialEnlapseTime);
  if (BackwardsProperty) {
    idx = (_num_sprites - _current_idx) - 1;
  } else {
    idx =_current_idx;
  }

  return _sprites[idx];
}

SpriteBatch * CMvSprites::GetFirstSpriteToDraw(float enlapse, bool run_once)
{
  _current_idx = 0;
  _initialEnlapseTime = enlapse;
  _totaEnlapseTime = enlapse;
  _run_once = run_once;
  _run_once_last_idx = 0;

  return _sprites[0];

  // _initialEnlapseTime, _totaEnlapseTime
}

SpriteBatch * CMvSprites::GetLastSpriteToDraw(float enlapse)                              // it also stops animation
{
  _current_idx = _num_sprites - 1;
  _initialEnlapseTime = enlapse;
  _totaEnlapseTime = enlapse;
  _pause = true;
  return _sprites[_current_idx];

}

void CMvSprites::Pause(void)
{
  _pause = true;
}

void CMvSprites::Resume(float enlapse)
{
  _initialEnlapseTime = enlapse;
  _totaEnlapseTime = enlapse;
  _pause = false;
}

void CMvSprites::Backward(void)
{
  BackwardsProperty = true;
}

void CMvSprites::Forward(void)
{
  BackwardsProperty = false;
}




