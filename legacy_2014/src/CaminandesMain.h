#ifndef CaminandesMain_H_
#define CaminandesMain_H_
/*********************************************************************
 *
 *              C A M I N A N D E S    T H E   G A M E
 *
 *
 *
 * Copyrights: Marcelo Varanda
 * Feb: 2014
 *
 *********************************************************************
 */

#include "gameplay.h"
#include "CMvSprites.h"
#include "tables.h"
#include "common.h"

#if defined(__QNX__) || defined(__ANDROID__)
#include <curl/curl.h>
#endif


using namespace gameplay;

typedef enum {
  FENCE_NONE = 0,
  FENCE_BACK,
  FENCE_AHEAD,
} fence_colligion_t;

typedef struct BushArray_st {
  Node *  bushNodeF;
  Node *  bushNodeB;
  float   pos;
  bool    visible;
} BushArray_t;

class CaminandesMain;

class fsAutoBindingResolver : RenderState::AutoBindingResolver
{
public:
  CaminandesMain * _caminandesMain;
  fsAutoBindingResolver(CaminandesMain* caminandes_main);
  bool resolveAutoBinding(const char* autoBinding, Node* node, MaterialParameter* parameter);
};

/**
 * Main game class.
 */
#ifdef USE_GP_COLLISION
class CaminandesMain: public Game, public AnimationClip::Listener, public PhysicsCollisionObject::CollisionListener
#else
class CaminandesMain: public Game, Control::Listener
#endif
{
public:

  /**
   * Constructor.
   */
  CaminandesMain();

  /**
   * @see Game::keyEvent
   */
  void keyEvent(Keyboard::KeyEvent evt, int key);

  /**
   * @see Game::touchEvent
   */
  void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);
  void gestureSwipeEvent(int x, int y, int direction);
  void gestureTapEvent(int x, int y);


  Vector3 getLightDirection0() const;
  Vector3 getLightColor0() const;

  /**
   * @see AnimationClip::Listener::animationEvent
   */
  void animationEvent(AnimationClip* clip, AnimationClip::Listener::EventType type);


  /**
   * @see PhysicsCollisionObject::CollisionListener::collisionEvent
   */
  void collisionEvent(PhysicsCollisionObject::CollisionListener::EventType type,
      const PhysicsCollisionObject::CollisionPair& collisionPair,
      const Vector3& contactPointA = Vector3::zero(),
      const Vector3& contactPointB = Vector3::zero());

protected:

  /**
   * @see Game::initialize
   */
  void initialize();

  /**
   * @see Game::finalize
   */
  void finalize();

  /**
   * @see Game::update
   */
  void update(float elapsedTime);

  /**
   * @see Game::render
   */
  void render(float elapsedTime);

  void controlEvent(Control* control, EventType evt);


private:
  /**
   * User-defined light color
   */
  Vector3 _directionalLightVector;

  /**
   * Draws the scene each frame.
   */
  bool drawScene(Node* node);
  bool drawSceneTransparent(Node* node);

  void drawSprite(SpriteBatch* sprite, Rectangle *dst, Rectangle *src);
  int  detectColligion(fence_colligion_t *coll_type, float *fence_pos);
  float GetFenceAhead(void);
  int   GetCurrentFence(void);
  void processLight(int lapseInMilli);
  void CloneBush(void);
  void updateBushes(bool first_time);
  int  detectBush(void);
  bool isBushVisible(Node * bush_node);
  void cloneFences(void);
  void Play(AudioSource * s);
  void drawLives (void);
  void initForm (void);
  void GetConfig (void);
  void updateConfig (time_t ad_time);
  bool fetchAd(void);
#if defined(__QNX__) || defined(__ANDROID__)
  bool fetch(CURL *curl_handle, char * url, char * local_header, char * local_doc);
#endif
  bool GetDeviceInfo(void);
  void refreshAds(bool from_init);
  void drawSplash(void* param);
  void stateControl(float elapsedTime, float char_pos);
  void updateFencesPos (void);
  void resetMountainsPos (void);
  void openURL(char * url);





  fsAutoBindingResolver* _resolver;


  Scene* _scene;
  int _state;
  float _stateTimerStart;
  float _stateTimerAcc;
  float _stateTimerExpiring;
  Font* _font;
  bool _wireframe;
  Light* _directionalLight;
  Node * _rootNode;
  Node * _terrainNode_1;
  Node * _terrainNode_2;
  Node * _terrainNode_3;
  Node * _terrainNode_4;
  Node * _mainSpriteNode;
  Node * _mountainsNode;
  Node * _cameraNode;
  Node * _cBoxCharacterNode;
  Node * _cBoxFenceNode;
  Node * _fenceNodes[NUM_FENCES];
  int     _activeMove;
  float   _terrainLen;
  float   _middleRangeLen;
  bool    _resumeFlag;
  bool    _pauseFlag;
  bool    _backwards;
  bool    _jumpFlag;
  bool    _isJumpingAction;
  bool    _fenceCollision;
  bool    _isFallingAction;
  bool    _goodJump;
  bool    _snapFlag;
  bool    _isSnappingAction;


  CMvSprites * _activeMvSprite;

  CMvSprites * _spritesWalk1;
  CMvSprites * _spritesGoodJump;
  CMvSprites * _spritesFall;
  CMvSprites * _spritesEFall;
  CMvSprites * _spritesSnap;


  SpriteBatch* _spriteMainBatch;
  SpriteBatch* _spriteTerrainMiddle;
  Node* _mountainsMiddleRangeNode_1;
  Node* _mountainsMiddleRangeNode_2;
  Node* _mountainsMiddleRangeNode_3;

  BushArray_t _bushArray[NUM_BUSHES];
  int         _bushEaten;

  const int   *_currentFlashPattern;
  int     _periodIdx;
  int     _currentFenceAhead;
  int     _currentGamePhase;
  bool    _lightOnFlag;
  int     _lightTimer;

  int     _touchX;
  int     _touchY;
  bool    _touching;

  AudioSource * _soundSong1;
  AudioSource * _soundCollide;
  AudioSource * _soundFinal;
  AudioSource * _songShock;
  AudioSource * _soundSnap;
  AudioSource * _soundClaps;

  bool    _fpsFlag;
  int     _score;
  int     _lives;
  bool    _turbo;

  char    _device_id[64];
  char    _device_location[64];
  char    _device_language[64];

  Form    *_mainForm;
  bool    _mainFormVisible;

  Form    *_finishForm;
  Form    *_gameOverForm;

  bool    _restartFlag;

  config_t    _cfg;
  SpriteBatch* _ad1_sprite;
  SpriteBatch* _ad2_sprite;
  SpriteBatch*  _setupIcon;
  int   _lastBushPosition;
  float   _mountainTranslations;


  int     _liveBonusCounter;
  float   _play_time;

  //statistics
//  int   _new_version_available;
//  bool  _ad_files_ok;

};

#endif
