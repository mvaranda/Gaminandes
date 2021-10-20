/*********************************************************************
 *
 *              G A M I N A N D E S    T H E   G A M E
 *
 *
 *
 * Copyrights: Marcelo Varanda
 * Feb: 2014
 *
 *********************************************************************
 */

#include "CaminandesMain.h"
#include "common.h"
#include "crc.h"
#include <stdio.h>


#define PREMIUM

#define   SUPPORT_URL   "http://www.cglabs.ca/gaminandes.html"
//#define   TEST_MODE

enum {
  MOVE_NONE = 0,
  MOVE_RIGHT,
  MOVE_LEFT,
  MOVE_JUMP,
};

// final bonus = (10 min / play_time) * 4
#define BONUS_REF_TIME   10 // 10 minutes
#define BONUS_DIVISOR  ( ( 60 * BONUS_REF_TIME) * 4 )

// -------------- test switches ------------
//#define   IGNORE_COLLIGION
#define   FIRST_LEVEL  1  // 1, 2 and 3
//#define   NEVER_DIE
//#define   SHOW_FPS

#ifdef __ANDROID__
  #define VERSION_VAL   0x00010100
  #define VERSION_TEXT  "AN-1.1.0"
#elif defined(__QNX__)
  #define VERSION_VAL   0x00010100
  #define VERSION_TEXT  "BB-1.1.0"
#elif defined (WIN32)
  #define VERSION_VAL   0x00010100
  #define VERSION_TEXT  "WD-1.1.0"
#else
  #define VERSION_VAL   0x00010100
  #define VERSION_TEXT  "MC-1.1.0"
#endif

#ifdef __QNX__
#include <bps/navigator_invoke.h>
#include <bps/deviceinfo.h>
#include <bps/locale.h>
#define   TIME_TO_FETCH   (5 * 60 * 60 * 24) // 5 days
#define CONFIG_FILENAME         "app/native/gamin.dat"

#else
#ifdef WIN32
#define   TIME_TO_FETCH   (2 * 60 * 60 * 24) // 2 days for desktop
#define CONFIG_FILENAME         "gamin.dat"

#else
#define   TIME_TO_FETCH   (2 * 60 * 60) // 2 minutes testing
#define CONFIG_FILENAME         "gamin.dat"
#endif
#endif

#define USER_AGENT      "Gaminandes v1.0 - BB10"

#define   LIVE_BONUS    7

#define   FPS_X   10
#define   FPS_Y   680

#ifndef   TEST_MODE // real mode
  #define   LAST_PHASE              2
  #define   LIMIT_WALK_LEFT         -5
  #define   LIMIT_WALK_RIGHT        52
  #ifdef    PREMIUM
    #define   RESTART_AD_TIME         3000  // 3 seconds for PREMIUM
  #else
    #define   RESTART_AD_TIME         6000  // 6 seconds
  #endif
  #define   SPLASH_MIN_TIME         3000L  // 3 sec
  #define   NUM_LIVES               5


#else // test mode
#define   LAST_PHASE              2
#define   LIMIT_WALK_LEFT         -5
#define   LIMIT_WALK_RIGHT        52
#define   RESTART_AD_TIME         500  // .5 seconds
#define   SPLASH_MIN_TIME         500L
#define   NUM_LIVES               5


#endif

#if 1
#define URL_AD        "http://www.cglabs.ca/gamin/img.php"
#define URL_AD_CRC    "http://www.cglabs.ca/gamin/info.php"
#else // Test
#define URL_AD        "http://10.10.10.210/sgamin/img.php"
#define URL_AD_CRC    "http://10.10.10.210/sgamin/info.php"
#endif

#ifdef  __QNX__
#define LOCAL_HEADER            "app/native/header.txt"
#define LOCAL_FILE_AD_TMP_1     "app/native/ad1.png"
#define LOCAL_FILE_CRC_1        "app/native/info1.bin"
#define LOCAL_FILE_AD_TMP_2     "app/native/ad2.png"
#define LOCAL_FILE_CRC_2        "app/native/info2.bin"
#define ACTIVE_FILE_AD_1        "app/native/ad1_active.png"
#define ACTIVE_FILE_AD_2        "app/native/ad2_active.png"
#define ACTIVE_SPRITE_FILE_AD_1  "ad1_active.png"
#define ACTIVE_SPRITE_FILE_AD_2  "ad2_active.png"
#elif __ANDROID__
#define LOCAL_HEADER            "header.txt"
#define LOCAL_FILE_AD_TMP_1     "ad1.png"
#define LOCAL_FILE_CRC_1        "info1.bin"
#define LOCAL_FILE_AD_TMP_2     "ad2.png"
#define LOCAL_FILE_CRC_2        "info2.bin"
#define ACTIVE_FILE_AD_1        "ad1_active.png"
#define ACTIVE_FILE_AD_2        "ad2_active.png"
#define ACTIVE_SPRITE_FILE_AD_1  "ad1_active.png"
#define ACTIVE_SPRITE_FILE_AD_2  "ad2_active.png"
#else
#define LOCAL_HEADER            "header.txt"
#define LOCAL_FILE_AD_TMP_1     "ad1.png"
#define LOCAL_FILE_CRC_1        "info1.bin"
#define LOCAL_FILE_AD_TMP_2     "ad2.png"
#define LOCAL_FILE_CRC_2        "info2.bin"
#define ACTIVE_FILE_AD_1        "ad1_active.png"
#define ACTIVE_FILE_AD_2        "ad2_active.png"
#define ACTIVE_SPRITE_FILE_AD_1  "ad1_active.png"
#define ACTIVE_SPRITE_FILE_AD_2  "ad2_active.png"

#endif


#ifdef __ANDROID__
  #include <android_native_app_glue.h>
  extern struct android_app* __state;
  #include <sys/errno.h>
  inline FILE * FOPEN(char *a, char *b) {
    FILE * r;
    ANativeActivity* nativeActivity = __state->activity;
    const char* internalPath = nativeActivity->internalDataPath;
    char filename[128];
    sprintf(filename, "%s/%s", internalPath, a);
    LOG("open file: \"%s\"",filename );
    r = fopen(filename,b);
    if (r == NULL) {
      LOG("fail to open %s", filename);
    }
    return r;
  }

  inline int REMOVE(char *a) {
    ANativeActivity* nativeActivity = __state->activity;
    const char* internalPath = nativeActivity->internalDataPath;
    char filename[128];
    sprintf(filename, "%s/%s", internalPath, a);
    LOG("removing %s", filename);
    return ( remove(filename) );
  }

  inline int FILECOPY(char * a, char * b) {
    char buf[512];

    FILE *fa;
    FILE *fb;
    int total = 0;
    fa = fopen(a, "rb");
    if (!fa) {
      LOG("RENAME error, could not open file \"%s\" to read", a);
      return errno;
    }
    fb = fopen(b, "wb");
    if (!fb) {
      LOG("RENAME error, could not open file to write %d", -1);
      fclose(fa);
      return -1;
    }
    int nread, nwrite;
    while ((nread = fread(buf, 1, sizeof(b), fa)) > 0) {
      nwrite = fwrite(buf, 1, nread, fb);
      if (nwrite != nread ) {
        LOG("WRITE ERROR %d", nwrite);
        fclose(fa);
        fclose(fb);
        return -1;
      }
      total += nread;
    }
    fclose(fa);
    fclose(fb);
    LOG("file renamed, size = %d", total);
    return 0;
  }

  inline int RENAME(char *a, char *b) {
    ANativeActivity* nativeActivity = __state->activity;
    const char* internalPath = nativeActivity->internalDataPath;
    char filename_a[128];
    char filename_b[128];
    sprintf(filename_a, "%s/%s", internalPath, a);
    sprintf(filename_b, "%s/%s", internalPath, b);
    LOG("rename %s to %s", filename_a, filename_b);
#if 0
    return ( rename(a,b) );
#else
    return (FILECOPY(filename_a, filename_b));
//    char buf[512];
//    FILE *fa;
//    FILE *fb;
//    int total = 0;
//    fa = fopen(filename_a, "rb");
//    if (!fa) {
//      LOG("RENAME error, could not open file to read %d", -1);
//      return errno;
//    }
//    fb = fopen(filename_b, "wb");
//    if (!fb) {
//      LOG("RENAME error, could not open file to write %d", -1);
//      fclose(fa);
//      return -1;
//    }
//    int nread, nwrite;
//    while ((nread = fread(buf, 1, sizeof(b), fa)) > 0) {
//      nwrite = fwrite(buf, 1, nread, fb);
//      if (nwrite != nread ) {
//        LOG("WRITE ERROR %d", nwrite);
//        fclose(fa);
//        fclose(fb);
//        return -1;
//      }
//      total += nread;
//    }
//    fclose(fa);
//    fclose(fb);
//    LOG("file renamed, size = %d", total);
//    return 0;
#endif
  }

  inline int FILE_EXISTS(char * f) {
    FILE * r;
    int ret = 0;
    ANativeActivity* nativeActivity = __state->activity;
    const char* internalPath = nativeActivity->internalDataPath;
    char filename[128];
    sprintf(filename, "%s/%s", internalPath, f);
    LOG("open for FILE_EXISTS: \"%s\"",filename );
    r = fopen(filename,"rb");
    if (r) {
      LOG("File exists %s", filename);
      ret = 1;
      fclose(r);
    }
    else {
      LOG("file NOT found %s", filename);
    }
    return ret;

  }

#else
  #define FOPEN(a,b)  fopen(a,b)
  #define REMOVE(a) remove(a)
  #define RENAME(a,b) remove(a,b)
  #define FILE_EXISTS(a) file_exists(a)

#endif

//#define FOPEN(a,b)  fopen(a,b)


#define     WALK_NORMAL_SPEED           40
#define     WALK_TURBO_SPEED            60

#define     MUSIC_GAIN                  0.4

#define     MAIN_FORM                   "res/form_main.form"
#define     FINISH_FORM                 "res/form_finish.form"
#define     GAME_OVER_FORM              "res/form_game_over.form"


#define     SCENE_NAME                  "res/camin.scene"
#define     NODE_SPRITE_MAIN            "sprite_main"
#define     NODE_TERRAIN_1              "terrain_1"
#define     NODE_TERRAIN_2              "terrain_empty_1"
#define     NODE_TERRAIN_3              "terrain_3"
#define     NODE_TERRAIN_4              "terrain_4"
#define     NODE_MIDDLE_RANGE           "mountains_mi"
#define     NODE_BUSH                   "bush_"
#define     NODE_MIDDLE_RANGE_1         "mountains_middle_range_1"
#define     NODE_MIDDLE_RANGE_2         "mountains_middle_range_2"
#define     NODE_MIDDLE_RANGE_3         "mountains_middle_range_3"
#define     NODE_MOUNTAINS_NODE         "mountains"
#define     NODE_CBOX_CHARACTER         "cbox_character"
#define     NODE_CBOX_FENCE             "cbox_fence"
#define     NODE_BUSH_FRONT             "bush_front"
#define     NODE_BUSH_BACK              "bush_back"

// sounds
#define     SOUND_SONG1                 "res/sounds/gaminandes_background.ogg"
#define     SOUND_COLLIDE               "res/sounds/gaminandes_batida_1.ogg"
#define     SOUND_FINAL                 "res/sounds/gaminandes_final.ogg"
#define     SOUND_SHOCK                 "res/sounds/gaminandes_shock.ogg"
#define     SOUND_SNAP                  "res/sounds/gaminandes_snap.ogg"
#define     SOUND_CLAPS                 "res/sounds/claps.ogg"

#define     LIGHT_ON_NAME               "light_on"

#define     BUSH_THRESHOLD_BEFORE       0.15
#define     BUSH_THRESHOLD_AFTER        0.15
#define     BUSH_CHARACTER_OFFSET       0.42
#define     SPRITES_HIDE_BUSH_IDX       4

#ifdef    PREMIUM
  #define     SPRITES_AD1_NAME            "res/sprites/singles/level1_completed.png"
  #define     SPRITES_AD2_NAME            "res/sprites/singles/level2_completed.png"
#else
  #define     SPRITES_AD1_NAME            "res/sprites/singles/ad_default.png"
  #define     SPRITES_AD2_NAME            "res/sprites/singles/ad_default.png"
#endif
#define     SPRITES_SPLASH_NAME         "res/sprites/singles/splash_default.png"
#define     SPRITES_SETUP_NAME          "res/sprites/singles/setup_icon.png"


#define     SPRITES_WALK1_NAME          "res/sprites/walk1/walk1_"
#define     SPRITES_WALK1_INIT          462
#define     SPRITES_WALK1_LAST          477

#define     SPRITES_GOOD_JUMP_NAME      "res/sprites/good_jump/good_jump_"
#define     SPRITES_GOOD_JUMP_INIT     423
#define     SPRITES_GOOD_JUMP_LAST     452
#define     SPRITES_GOOD_JUMP_TAKEOFF_IDX 6
#define     SPRITES_GOOD_JUMP_LANDING_IDX 18

#define     SPRITES_EFALL_NAME          "res/sprites/efall/fall_"

#define     SPRITES_FALL_NAME          "res/sprites/fall/fall_"
#define     SPRITES_FALL_INIT          759
#define     SPRITES_FALL_LAST          769
#define     SPRITES_FALL_NFRAMES_TO_REPEAT  50   // num times the last frame must repeat.
#define     SPRITES_FALL_STOP_MOVING_IDX    6

#define     SPRITES_SNAP_NAME          "res/sprites/snap/snap_"
#define     SPRITES_SNAP_INIT          1
#define     SPRITES_SNAP_LAST          7


#define     GOOD_JUMP_THRESHOUD_HIGH    2.23
#define     GOOD_JUMP_THRESHOUD_LOW     1.3
#define     FENCE_BACK_THRESHOUD        1.0
#define     FENCE_AHEAD_THRESHOUD       0.6


#define     MOVE_SPEED_NORMAL           1
#define     MOVE_SPEED_FAST             5
#define     MOVE_SPEED_JUMPING          6
#define     MOVE_SPEED_FALLING          6
#define     MOVE_MIDDLE_RANGE_FACTOR    0.5

#define     CBOX_CHARACTER_X            0.49374
#define     CBOX_CHARACTER_Z            0.326086
#define     CBOX_CHARACTER_Y            1.259527

#define     CBOX_FENCE_X                0.124607
#define     CBOX_FENCE_Z                1.023745
#define     CBOX_FENCE_Y                1.259527

#define     POINTS_GOOD_SNAP            50
#define     POINTS_BAD_SNAP             -20
#define     POINTS_COLLIDE              -20

#ifndef WIN32
extern const float fence_pos_array[];
#else
extern float fence_pos_array[];
#endif

bool file_exists(char * file);

enum {
  ST_START = 0,
  ST_DISPLAYING_RESTART_AD,
  ST_DISPLAYING_BETWEEN_PHASES_AD,
  ST_PLAYING,
  ST_GAME_OVER,
  ST_FINISHED,
};


//#include "fences_loc.h"
// Declare our game instance
CaminandesMain game;


CaminandesMain::CaminandesMain()
: _directionalLight(NULL), _scene(NULL), _wireframe(false), _activeMove(0), _resumeFlag(false), _pauseFlag(true),
  _backwards(false), _fenceCollision(false), _isFallingAction(false), _isJumpingAction(false), _jumpFlag(false), _font(NULL),
  _goodJump(false), _periodIdx(0), _currentFenceAhead(0), _currentGamePhase(FIRST_LEVEL-1), _lightOnFlag(false), _fpsFlag(true), _score(0),
  _lives(NUM_LIVES), _turbo(false), _mainFormVisible(false), _ad1_sprite(NULL), _ad2_sprite(NULL), /*_new_version_available(0),
  _ad_files_ok(false), */_state(0), _restartFlag(false), _mountainTranslations(0), _liveBonusCounter(0), _play_time(0)



{
}

void CaminandesMain::drawSplash(void* param)
{
  Rectangle src(0, 0, 1024, 1024);

  clear(CLEAR_COLOR_DEPTH, Vector4(0, 0, 0, 1), 1.0f, 0);

  SpriteBatch* batch = SpriteBatch::create(SPRITES_SPLASH_NAME);
  batch->start();
  drawSprite(batch, NULL, &src);
  //  batch->draw(this->getWidth() * 0.5f, this->getHeight() * 0.5f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f, 1.0f, 0.0f, Vector4::one(), true);
  //  batch->draw(this->getWidth(), this->getHeight(), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, Vector4::one(), true);
  batch->finish();
  SAFE_DELETE(batch);
}

void CaminandesMain::initialize()
{
  // Display the gameplay splash screen during loading, for at least 2 second.
  displayScreen(this, &CaminandesMain::drawSplash, NULL, SPLASH_MIN_TIME);

  char const * res_path = FileSystem::getResourcePath  (   ) ;
  LOG("res_path \"%s\"", res_path);


  if (!GetDeviceInfo()) {
    LOG("Sponsored version only run if unique ID is provided. %d", 1);
    exit();
  }
  GetConfig();
  refreshAds(1);

  //_resolver = new fsAutoBindingResolver(this);
  LOG("MV Loading scene %d", 1);
  // Load game scene from file
  _scene = Scene::load(SCENE_NAME);


  LOG("MV so far so good %d", 1);

  // Load the font.
  //    _font = Font::create("res/ui/arial.gpb");
  _font = Font::create("res/ui/Silom.gpb");

  _currentFlashPattern = flashing_control_array[_currentGamePhase][_currentFenceAhead];

  Camera * cam = _scene->getActiveCamera();
  if (cam) {
    // Set the aspect ratio for the scene's camera to match the current resolution
    cam->setAspectRatio(getAspectRatio());
    cam->getNode()->rotateY(MATH_DEG_TO_RAD(-90));
  }

  // Try to use any significant light source for the scene light vector.
  // This is just a temporary hack until I manage to tap into the
  // material file from the python addon.
  Node * light = _scene->findNode("Spot") ? _scene->findNode("Spot") :
      _scene->findNode("Area") ? _scene->findNode("Area") :
          _scene->findNode("Sun") ? _scene->findNode("Sun") : 0;
  if (light) _directionalLightVector = light->getForwardVector();

  _terrainNode_1 = _scene->findNode(NODE_TERRAIN_1);
//  _terrainNode_2 = _scene->findNode(NODE_TERRAIN_2);
  //    _terrainNode_3 = _scene->findNode(NODE_TERRAIN_3);
  //    _terrainNode_4 = _scene->findNode(NODE_TERRAIN_4);
  _mountainsMiddleRangeNode_1 = _scene->findNode(NODE_MIDDLE_RANGE_1);
  _mountainsMiddleRangeNode_2 = _scene->findNode(NODE_MIDDLE_RANGE_2);
  _mountainsMiddleRangeNode_3 = _scene->findNode(NODE_MIDDLE_RANGE_3);
  _mainSpriteNode = _scene->findNode(NODE_SPRITE_MAIN);

  // get terrain and middle range mountains length
  Vector3 t1 = _terrainNode_1->getTranslationWorld();
//  Vector3 t2 = _terrainNode_2->getTranslationWorld();
//  _terrainLen = t2.x - t1.x;
//  LOG("terrain length = %f", _terrainLen);
  // get terrain and middle range mountains length
//  t1 = _mountainsMiddleRangeNode_1->getTranslationWorld();
//  t2 = _mountainsMiddleRangeNode_2->getTranslationWorld();
//  _middleRangeLen = t2.x - t1.x;
//  LOG("terrain length = %f", _middleRangeLen);

  // let add a node to have the Camera and mountains to follow
  _rootNode = _scene->addNode("root");
  _rootNode->setTranslation(0, 0, 0);
  _cameraNode = _scene->getActiveCamera()->getNode();
  _rootNode->addChild(_cameraNode);

  // Mountains goes with the camera
  // let add a node to have the Camera looking at
  _mountainsNode = _scene->findNode(NODE_MOUNTAINS_NODE);
  _rootNode->addChild(_mountainsNode);

  _cBoxCharacterNode  = _scene->findNode(NODE_CBOX_CHARACTER);
  _cBoxFenceNode = _scene->findNode(NODE_CBOX_FENCE);
  _rootNode->addChild(_cBoxCharacterNode);


  // Set the aspect ratio for the scene's camera to match the current resolution
  //cam->setAspectRatio(getAspectRatio());

  _spritesGoodJump = new CMvSprites((char *) SPRITES_GOOD_JUMP_NAME, SPRITES_GOOD_JUMP_INIT, SPRITES_GOOD_JUMP_LAST, 25.0);
  _spritesFall = new CMvSprites((char *) SPRITES_FALL_NAME, SPRITES_FALL_INIT, SPRITES_FALL_LAST, 20.0);
  _spritesEFall = new CMvSprites((char *) SPRITES_EFALL_NAME, SPRITES_FALL_INIT, SPRITES_FALL_LAST, 20.0);
  _spritesWalk1 = new CMvSprites((char *)SPRITES_WALK1_NAME, SPRITES_WALK1_INIT, SPRITES_WALK1_LAST, WALK_NORMAL_SPEED);
  _spritesSnap = new CMvSprites((char *) SPRITES_SNAP_NAME, SPRITES_SNAP_INIT, SPRITES_SNAP_LAST, 10.0);

#ifndef PREMIUM
  if (FILE_EXISTS(ACTIVE_FILE_AD_1)) {
#ifdef __ANDROID__
  FILE * fd1 = FOPEN(ACTIVE_FILE_AD_1, "rb");
  if (!fd1) {
    LOG("FAIL to open %s", ACTIVE_FILE_AD_1);
  }
  _ad1_sprite = SpriteBatch::createOnFS(fd1);
#else
    _ad1_sprite = SpriteBatch::create(ACTIVE_SPRITE_FILE_AD_1, NULL, 0);
#endif //#ifdef __ANDROID__
  }
  else {
    _ad1_sprite = SpriteBatch::create(SPRITES_AD1_NAME, NULL, 0);

  }
  if (FILE_EXISTS(ACTIVE_FILE_AD_2)) {
#ifdef __ANDROID__
  FILE * fd2 = FOPEN(ACTIVE_FILE_AD_2, "rb");
  if (!fd2) {
    LOG("FAIL to open %s", ACTIVE_FILE_AD_2);
  }
  _ad2_sprite = SpriteBatch::createOnFS(fd2);
#else
    _ad2_sprite = SpriteBatch::create(ACTIVE_SPRITE_FILE_AD_2, NULL, 0);
#endif // __ANDROID__
  }
  else {
    _ad2_sprite = SpriteBatch::create(SPRITES_AD2_NAME, NULL, 0);

  }
#else
  _ad1_sprite = SpriteBatch::create(SPRITES_AD1_NAME, NULL, 0);
  _ad2_sprite = SpriteBatch::create(SPRITES_AD2_NAME, NULL, 0);
#endif

  _setupIcon = SpriteBatch::create(SPRITES_SETUP_NAME, NULL, 0);

  _activeMvSprite = _spritesWalk1;

  CloneBush();
  updateBushes(true);

  cloneFences();

  _soundSong1 = AudioSource::create(SOUND_SONG1);
  _soundCollide = AudioSource::create(SOUND_COLLIDE);
  _soundFinal = AudioSource::create(SOUND_FINAL);
  _soundClaps = AudioSource::create(SOUND_CLAPS);
  _songShock = AudioSource::create(SOUND_SHOCK);
  _soundSnap = AudioSource::create(SOUND_SNAP);

  _soundClaps->setGain(0.5);
  _soundSong1->setGain(MUSIC_GAIN);
  _soundSong1->setLooped(true);
  //  if (_cfg.mute_all == 0 && _cfg.mute_song == 0)
  //    _soundSong1->play();

  initForm();

#ifdef USE_GP_COLLISION
  PhysicsRigidBody::Parameters params;
  params.mass = 10.0f;
  Vector3 cbox_c = Vector3(CBOX_CHARACTER_X, CBOX_CHARACTER_Y, CBOX_CHARACTER_Z);
  Vector3 cbox_f = Vector3(CBOX_FENCE_X, CBOX_FENCE_Y, CBOX_FENCE_Z);

  _cBoxCharacterNode->setCollisionObject(PhysicsCollisionObject::GHOST_OBJECT,
      PhysicsCollisionShape::box(cbox_c), &params);
  _cBoxFenceNode->setCollisionObject(PhysicsCollisionObject::GHOST_OBJECT,
      PhysicsCollisionShape::box(cbox_f), &params);
  _cBoxCharacterNode->getCollisionObject()->addCollisionListener(this);
#endif //#ifdef USE_GP_COLLISION

  // hack
  //_rootNode->translateLeft(-41.0);
}

void CaminandesMain::finalize()
{
  SAFE_RELEASE(_scene);
}

void CaminandesMain::update(float elapsedTime)
{
  float secs = elapsedTime / 1000.0;
  float speed = MOVE_SPEED_NORMAL * secs;
  float middle_range_speed = speed * MOVE_MIDDLE_RANGE_FACTOR;
  bool run_once_action = false;
  fence_colligion_t coll_type;
  float fence_pos;
  Vector3 pos_char = _cBoxCharacterNode->getTranslationWorld();


  stateControl(elapsedTime, pos_char.x);

  if (_state == ST_FINISHED) {
    _finishForm->update(elapsedTime);
    return;
  }

  if (_state == ST_GAME_OVER) {
    _gameOverForm->update(elapsedTime);
    return;
  }


  if (_mainFormVisible && _mainForm != NULL) {
    _mainForm->update(elapsedTime);
    return;
  }

  int fence_collision = -1;

  if (!_isFallingAction) {
    fence_collision = detectColligion(&coll_type, &fence_pos);
  }

  processLight(elapsedTime);

  if (fence_collision >=0 && (_goodJump == false || _lightOnFlag) )
    _fenceCollision = true;

  if (_fenceCollision == true) {
    _fenceCollision = false;
    _isFallingAction = true;
    if (_lightOnFlag) {
      _activeMvSprite = _spritesEFall;
      Play(_songShock);
#ifndef NEVER_DIE
      if (_lives ) _lives--;
#endif
    }
    else {
      Play(_soundCollide);
      _activeMvSprite = _spritesFall;
      _score += POINTS_COLLIDE;
    }
    _activeMvSprite->RepeatLastFrame(SPRITES_FALL_NFRAMES_TO_REPEAT);
    _activeMvSprite->GetFirstSpriteToDraw(elapsedTime, true);
    _activeMvSprite->Resume(elapsedTime);
    _activeMove = MOVE_NONE;
    //_pauseFlag = true; // may not be effective here
    //        _spritesWalk1->Pause();
    _jumpFlag = false;
    _isJumpingAction = false;
  }


  if (_jumpFlag == true) {
    _jumpFlag = false;
    _isJumpingAction = true;
    _activeMvSprite = _spritesGoodJump;
    _activeMvSprite->GetFirstSpriteToDraw(elapsedTime, true);
    _activeMvSprite->Resume(elapsedTime);

  }

  if (_snapFlag == true) {
    _snapFlag = false;
    _isSnappingAction = true;
    _activeMvSprite = _spritesSnap;
    _activeMvSprite->GetFirstSpriteToDraw(elapsedTime, true);
    _activeMvSprite->Resume(elapsedTime);
  }


  if (_isFallingAction == true) {
    int idx = _activeMvSprite->GetIndex();
    if (idx < SPRITES_FALL_STOP_MOVING_IDX ) {
      speed = MOVE_SPEED_FALLING * secs;
      _rootNode ->translateLeft(speed);
      _mountainTranslations += middle_range_speed;
      _mountainsMiddleRangeNode_1->translateLeft(middle_range_speed);
      _mountainsMiddleRangeNode_2->translateLeft(middle_range_speed);
      _mountainsMiddleRangeNode_3->translateLeft(middle_range_speed);
    }
    return;

  }
  if (_isSnappingAction) {
    int idx = _activeMvSprite->GetIndex();
    if (_bushEaten >= 0 && idx >= SPRITES_HIDE_BUSH_IDX) {
      _bushArray[_bushEaten].visible = false;
      Play(_soundSnap);
      _score += POINTS_GOOD_SNAP;
      _bushEaten = -1;
    }
    return;     // no translation but need to hide bush
  }

  if (_isJumpingAction == true) {
    int idx = _activeMvSprite->GetIndex();
    if (idx >= SPRITES_GOOD_JUMP_TAKEOFF_IDX && idx <= SPRITES_GOOD_JUMP_LANDING_IDX)
      speed = MOVE_SPEED_JUMPING * secs;
    _rootNode ->translateLeft(-speed);
    _mountainTranslations -= middle_range_speed;
    _mountainsMiddleRangeNode_1->translateLeft(-middle_range_speed);
    _mountainsMiddleRangeNode_2->translateLeft(-middle_range_speed);
    _mountainsMiddleRangeNode_3->translateLeft(-middle_range_speed);
    return;

  }

  if (_resumeFlag) {
    _activeMvSprite->Resume(elapsedTime);
  }
  else if (_pauseFlag) {
    _activeMvSprite->Pause();
  }
  _pauseFlag = _resumeFlag = 0;

  if (_turbo) {
    speed *= 3;
    middle_range_speed *= 3;
    _spritesWalk1->SetFps(WALK_TURBO_SPEED);
  }
  else
    _spritesWalk1->SetFps(WALK_NORMAL_SPEED);

  if (_state == ST_PLAYING) {
    switch (_activeMove)
    {
      case MOVE_RIGHT:
        if (pos_char.x < LIMIT_WALK_RIGHT) {
        _rootNode ->translateLeft(-speed);
        _mountainTranslations -= middle_range_speed;
        _mountainsMiddleRangeNode_1->translateLeft(-middle_range_speed);
        _mountainsMiddleRangeNode_2->translateLeft(-middle_range_speed);
        _mountainsMiddleRangeNode_3->translateLeft(-middle_range_speed);
        }

        break;

      case MOVE_LEFT:
        if (pos_char.x > LIMIT_WALK_LEFT) {
          _rootNode ->translateLeft(speed);
          _mountainTranslations -= middle_range_speed;
          _mountainsMiddleRangeNode_1->translateLeft(middle_range_speed);
          _mountainsMiddleRangeNode_2->translateLeft(middle_range_speed);
          _mountainsMiddleRangeNode_3->translateLeft(middle_range_speed);
        }

        break;

      default:break;

    }
  }

}

#define PRESET_TIMER(v) _stateTimerAcc = 0.0; _stateTimerExpiring = v
#define HAS_TIMER_EXPIRED() (_stateTimerAcc > _stateTimerExpiring)
void CaminandesMain::stateControl(float elapsedTime, float char_pos)
{
  _stateTimerAcc += elapsedTime;

  if(_restartFlag) {  // restart game
    _restartFlag = false;

    _mainForm->setEnabled(false);
    _mainForm->setVisible(false);
    _mainFormVisible = false;

    _finishForm->setEnabled(false);
    _finishForm->setVisible(false);

    _gameOverForm->setEnabled(false);
    _gameOverForm->setVisible(false);

    _currentGamePhase = FIRST_LEVEL - 1;
    _currentFenceAhead = 0;

   // _rootNode->setIdentity();
    _rootNode->setTranslationX(0.0);
    
    _lives = NUM_LIVES;
    _state = ST_START;
    _score = 0;
    _lightTimer = 0;

    //for (int z=0; z<1000; z++) { _currentGamePhase =  z & 1; updateBushes(false);}
    //_currentGamePhase = 0;
    LOG("Playtime = %f min", _play_time / 60000 );

//    updateBushes(false);
//    fetchAd();
    return;

  }

  switch(_state) {
    case ST_START:
      // we finished displaying splash screen. Lets display
      // restart ad
#ifndef PREMIUM
      PRESET_TIMER(RESTART_AD_TIME);
#endif
      updateBushes(false);
      refreshAds(false);
      _state = ST_DISPLAYING_RESTART_AD;
      break;

    case ST_DISPLAYING_RESTART_AD:
      if (HAS_TIMER_EXPIRED()) {
        _cfg.num_plays++;
        _state = ST_PLAYING;
        resetMountainsPos();
        _lightTimer = 0;
        // start song
        if (_cfg.mute_all == 0 && _cfg.mute_song == 0) {
          _soundSong1->rewind();
          _soundSong1->play();
        }

      }
      break;

    case ST_PLAYING:
      // Check lifes
      if (_lives == 0) {
        // game is over
        _state = ST_GAME_OVER;
        _gameOverForm->setEnabled(true);
        _gameOverForm->setVisible(true);
        _soundSong1->pause();
        touchEvent(Touch::TOUCH_RELEASE, 0, 0, 0);
        break;
      }

      if (_mainFormVisible == false)
        _play_time += elapsedTime;

      if (char_pos >= LIMIT_WALK_RIGHT) { // end of a phase
        // is the final phase?
        LOG("finishes phase %d", _currentGamePhase);
        _soundSong1->pause();
        if (_currentGamePhase == LAST_PHASE) {
          // **************** play victory ****************
          _isFallingAction = false;
          _isJumpingAction = false;
          _isSnappingAction = false;
          _activeMvSprite = _spritesWalk1;
          
          Play(_soundFinal);
          _state = ST_FINISHED;
          _finishForm->setEnabled(true);
          _finishForm->setVisible(true);
          touchEvent(Touch::TOUCH_RELEASE, 0, 0, 0);

          // calculate bonus
          float bonus_factor = BONUS_DIVISOR / (_play_time / 1000);
          LOG("Playtime %fBONUS = %f", _play_time, bonus_factor);
          if (bonus_factor > 1.0) {
            LOG("_score = %d", _score);
            LOG("BONUS = %f", bonus_factor);
            _score *= bonus_factor;
            LOG("_score witn bonus= %d", _score);
          }


        }
        else {
          // ************** New Phase ****************
          _isFallingAction = false;
          _isJumpingAction = false;
          _isSnappingAction = false;
          _activeMvSprite = _spritesWalk1;
          
          _currentGamePhase++;

          _mainForm->setEnabled(false);
          _mainForm->setVisible(false);
          _mainFormVisible = false;

          _finishForm->setEnabled(false);
          _finishForm->setVisible(false);

          _gameOverForm->setEnabled(false);
          _gameOverForm->setVisible(false);

          _currentFenceAhead = 0;

          _rootNode->setIdentity();
          if (_lives <= NUM_LIVES)_lives++;
          _state = ST_DISPLAYING_BETWEEN_PHASES_AD;

          updateBushes(false);
          Play(_soundClaps);
          PRESET_TIMER(RESTART_AD_TIME);
          _lightTimer = 0;

          resetMountainsPos();

        }
        break;


      case ST_DISPLAYING_BETWEEN_PHASES_AD:
        if (HAS_TIMER_EXPIRED()) {
          _state = ST_PLAYING;
          // start song
          if (_cfg.mute_all == 0 && _cfg.mute_song == 0) {
            _soundSong1->rewind();
            _soundSong1->play();
          }

        }
        break;


      }


    default:
      break;
  }

}

void CaminandesMain::resetMountainsPos (void)
{
  if (_mountainTranslations != 0.0) {
    _mountainsMiddleRangeNode_1->translateLeft(-_mountainTranslations);
    _mountainsMiddleRangeNode_2->translateLeft(-_mountainTranslations);
    _mountainsMiddleRangeNode_3->translateLeft(-_mountainTranslations);
  }
  _mountainTranslations = 0;
}

void CaminandesMain::render(float elapsedTime)
{
  // Clear the color and depth buffers
  clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

  // Visit all the nodes in the scene for drawing
  _scene->visit(this, &CaminandesMain::drawScene);
  _scene->visit(this, &CaminandesMain::drawSceneTransparent);

//  Rectangle dst(0, 0, 1280, 720);
  Rectangle src(0, 0, 1024, 1024);

  if(_state == ST_DISPLAYING_RESTART_AD) {
    drawSprite(_ad1_sprite, NULL, &src);
    return;
  }

  if(_state == ST_DISPLAYING_BETWEEN_PHASES_AD) {
    if (_currentGamePhase == 1)
      drawSprite(_ad1_sprite, NULL, &src);
    else
      drawSprite(_ad2_sprite, NULL, &src);
    return;
  }

  if (_state == ST_PLAYING)
  {
    Rectangle src = Rectangle(0, 0, 128, 128);
    Rectangle dst1 = Rectangle(5, 5, 75, 75);
    _setupIcon->start();
    _setupIcon->draw(dst1, src, Vector4::fromColor(0xFFFFFFFF));
    _setupIcon->finish();

  }

  _activeMvSprite->BackwardsProperty = _backwards;
  SpriteBatch * to_draw = _activeMvSprite->GetNextSpriteToDraw(elapsedTime);

  if (to_draw == NULL) { // we finished performing a run_once action
    if (_isFallingAction) {
      // TODO: create callback for actions and manage it OOP
      // for now switch back to walk1
      _activeMvSprite = _spritesWalk1;
      to_draw = _activeMvSprite->GetFirstSpriteToDraw(elapsedTime, false);
      _isFallingAction = false;
     _currentFenceAhead = GetCurrentFence();
      _spritesWalk1->Pause();

    }
    if (_isJumpingAction) {
      // TODO: create callback for actions and manage it OOP
      // for now switch back to walk1
      _activeMvSprite = _spritesWalk1;
      to_draw = _activeMvSprite->GetFirstSpriteToDraw(elapsedTime, false);
      _isJumpingAction = false;
      _currentFenceAhead = GetCurrentFence();
      if (_goodJump) {
        int level_idx = _currentGamePhase;
        int fence_idx = _currentFenceAhead;
        _lightTimer = 1;
        LOG("level_idx=%d, fence_idx=%d", level_idx, fence_idx);
        _score += fence_points_array[level_idx][fence_idx - 1];
      }
      _goodJump = false;

    }
    if (_isSnappingAction) {
      // TODO: create callback for actions and manage it OOP
      // for now switch back to walk1
      _activeMvSprite = _spritesWalk1;
      to_draw = _activeMvSprite->GetFirstSpriteToDraw(elapsedTime, false);
      _isSnappingAction = false;
      _liveBonusCounter++;
      if (_liveBonusCounter >= LIVE_BONUS) {
        if (_lives < NUM_LIVES) {
          _lives++;
          _liveBonusCounter = 0;
        }
      }

    }
  }
  Rectangle screen_rect = Rectangle(0, 0, getWidth()/2 * 1.125, getHeight());
  src.set(0,0,512,512);
  drawSprite(to_draw, &screen_rect, &src);

  // Draw FPS
  if (_font) {
    Vector4 color = Vector4(136.0/256.0, 95.0/256.0, 50.0/256.0, 1);
    _font->start();
    char fps[32];
    sprintf(fps, "Score: %d Level %d", _score, _currentGamePhase + 1);
    _font->drawText(fps, getWidth()/2, 5, color, 60);

#ifdef SHOW_FPS
    if (_fpsFlag) {
      sprintf(fps, "%d", getFrameRate());
      _font->drawText(fps, FPS_X, FPS_Y, color, 40);
    }
#endif
    _font->finish();
  }

  drawLives();


  if (_state == ST_FINISHED) {
    _finishForm->draw();
    return;
  }

  if (_state == ST_GAME_OVER){
    _gameOverForm->draw();
    return;
  }

  if (_mainFormVisible && _mainForm != NULL)
    _mainForm->draw();

}

bool CaminandesMain::drawSceneTransparent(Node* node)
{
  // If the node visited contains a model, draw it
  Model* model = node->getModel();

  if (model) {
    std::string id = node->getId();
    if (id.find(NODE_MIDDLE_RANGE) != std::string::npos)
      model->draw();
    if (id.find(NODE_BUSH) != std::string::npos) {
      if (isBushVisible(node))
        model->draw();
    }
  }
  return true;

}

bool CaminandesMain::drawScene(Node* node)
{

  // If the node visited contains a model, draw it
  Model* model = node->getModel();

  if (model) {
    std::string id = node->getId();
    //LOG("nodel id=%s", id.c_str());

    if (id.compare(0, sizeof(NODE_SPRITE_MAIN), NODE_SPRITE_MAIN) == 0)
      return true;

    if (id.find(NODE_MIDDLE_RANGE) != std::string::npos)
      return true;
    if (id.find(NODE_BUSH) != std::string::npos)
      return true;


    if (_lightOnFlag == false) {
      if (id.find(LIGHT_ON_NAME) != std::string::npos) {
        return true;
      }
    }


    model->draw();
  }


  return true;
}

void CaminandesMain::drawSprite(SpriteBatch* sprite, Rectangle *dst, Rectangle *src)
{
  Rectangle s_rect = Rectangle(0, 0, getWidth(), getHeight());
  sprite->start();
  if (dst == NULL)
    sprite->draw(s_rect, *src, Vector4::fromColor(0xFFFFFFFF));
  else
    sprite->draw(*dst, *src, Vector4::fromColor(0xFFFFFFFF));

  sprite->finish();
}


#ifdef USE_GP_COLLISION
void CaminandesMain::animationEvent(AnimationClip* clip, AnimationClip::Listener::EventType type)
{
  LOG("Animation Event %d", 1);
}

void CaminandesMain::collisionEvent(PhysicsCollisionObject::CollisionListener::EventType type,
    const PhysicsCollisionObject::CollisionPair& collisionPair,
    const Vector3& contactPointA,
    const Vector3& contactPointB)
{
  // objectA -> basketball, only care about collisions between the physics character and the basketball.
  //    if (type == PhysicsCollisionObject::CollisionListener::COLLIDING && collisionPair.objectB == _character)
  //        _hasBall = true;
  if (type == COLLIDING) {
#if 0
    LOG("Collide %d", 1);
    if (collisionPair.objectA == _cBoxCharacterNode->getCollisionObject()) {
      LOG("object A is Character %d",1);
    }
    if (collisionPair.objectB == _cBoxCharacterNode->getCollisionObject()) {
      LOG("object B is Character %d",1);
    }
    if (collisionPair.objectA == _cBoxFenceNode->getCollisionObject()) {
      LOG("object A is Fence %d",1);
    }
    if (collisionPair.objectB == _cBoxFenceNode->getCollisionObject()) {
      LOG("object B is Fence %d",1);
    }
#endif
    if (collisionPair.objectB == _cBoxFenceNode->getCollisionObject()) {
      if (_goodJump == false)
        _fenceCollision = true;
    }

  }
}
#endif // #ifdef USE_GP_COLLISION

int CaminandesMain::detectColligion(fence_colligion_t *coll_type, float *fence_pos)
{
#ifdef IGNORE_COLLIGION
  return -1;
#endif

  int i;
  Vector3 pos = _cBoxCharacterNode->getTranslationWorld();
  float   char_x = pos.x;
  for (i=0; fence_pos_array[i] != 0; i++) {
    if (char_x < fence_pos_array[i]) break;
  }

  if (i>0) { // need to check if touches back fence
    float dis = char_x - fence_pos_array[i-1];
    if (dis <= FENCE_BACK_THRESHOUD) {
      //LOG("Colligion back fence %d", i-1);
      *coll_type = FENCE_BACK;
      *fence_pos = fence_pos_array[i-1];
      return i-1;
    }
  }

  if (fence_pos_array[i] != 0) {
    float dis = fence_pos_array[i] - char_x;
    if (dis <= FENCE_AHEAD_THRESHOUD) {
      //LOG("Colligion ahead fence %d", i);
      *coll_type = FENCE_AHEAD;
      *fence_pos = fence_pos_array[i];
      return i;
    }
  }
  else {
    LOG("char after last fence %d", i);
  }
  *fence_pos = 0;
  *coll_type = FENCE_NONE;
  return -1; // no colligion in any fence
}

int CaminandesMain::detectBush(void)
{
  int i;
  Vector3 pos = _cBoxCharacterNode->getTranslationWorld();
  float   char_x = pos.x + BUSH_CHARACTER_OFFSET;//0.24 + 0.18;
  float   delta;
  for (i=0; i < NUM_BUSHES; i++) {
    if (_bushArray[i].visible) {
      delta = char_x -_bushArray[i].pos;
      if (delta < 0) { // char is before bush
        delta *= -1;
        if (delta < BUSH_THRESHOLD_BEFORE) {
          LOG("bush in range (charac is before) %f", delta);
          return i;
        }
      }
      else { // chara is after bush
        if (delta < BUSH_THRESHOLD_AFTER) {
          LOG("bush in range (charac is after) %f", delta);
          return i;
        }

      }

    }
  }
  return -1;
}

bool CaminandesMain::isBushVisible(Node * bush_node)
{
  int i;
  const char * id = bush_node->getId();

  for (i=0; i < NUM_BUSHES; i++) {
    if (strcmp(id, bush_front_ids[i]) == 0) {
      return _bushArray[i].visible;
    }
    if (strcmp(id, bush_back_ids[i]) == 0) {
      return _bushArray[i].visible;
    }
  }
  return false;
}


float CaminandesMain::GetFenceAhead(void)
{
  int i;
  Vector3 pos = _cBoxCharacterNode->getTranslationWorld();
  float   char_x = pos.x;
  for (i=0; fence_pos_array[i] != 0; i++) {
    if (char_x < fence_pos_array[i]) break;
  }

  if (fence_pos_array[i] != 0) {
    return fence_pos_array[i];
  }
  return 0;
}

int CaminandesMain::GetCurrentFence(void)
{
  int i;
  Vector3 pos = _cBoxCharacterNode->getTranslationWorld();
  float   char_x = pos.x;
  for (i=0; fence_pos_array[i] != 0; i++) {
    if (char_x < fence_pos_array[i]) break;
  }

  if (fence_pos_array[i] != 0) {
    return i;
  }
  return NUM_FENCES;
}


void CaminandesMain::processLight(int lapseInMilli)
{
  if (_lightTimer > 0) {
    _lightTimer -= lapseInMilli;
  }

  if (_lightTimer <= 0) {
    _lightTimer =     _currentFlashPattern[_periodIdx];
    _periodIdx++;
    if (_currentFlashPattern[_periodIdx] == 0)
      _periodIdx = 0;

    if(_periodIdx & 1)
      _lightOnFlag = true;
    else
      _lightOnFlag = false;

    if (_currentGamePhase >= NUM_PHASES) {
      return;
    }
    if (_currentFenceAhead >= NUM_FENCES) {
      return;
    }
    _currentFlashPattern = flashing_control_array[_currentGamePhase][_currentFenceAhead];

  }
}

void CaminandesMain::CloneBush(void)
{
  int i;
  _bushArray[0].bushNodeF = _scene->findNode(NODE_BUSH_FRONT);
  _bushArray[0].bushNodeB = _scene->findNode(NODE_BUSH_BACK);
  _bushArray[0].bushNodeF->setId(bush_front_ids[0]);
  _bushArray[0].bushNodeB->setId(bush_back_ids[0]);

  //hack tranelate z
  //_bushArray[0].bushNodeB->translateZ(-0.2);

  for (i=1; i < NUM_BUSHES; i++) {
    _bushArray[i].bushNodeF = _bushArray[0].bushNodeF->clone();
    _bushArray[i].bushNodeB = _bushArray[0].bushNodeB->clone();
    _bushArray[i].visible = true;
    _bushArray[i].pos = bush_location[_currentGamePhase][i];
    _bushArray[i].bushNodeF->setId(bush_front_ids[i]);
    _bushArray[i].bushNodeB->setId(bush_back_ids[i]);
    _scene->addNode(_bushArray[i].bushNodeF);
    _scene->addNode(_bushArray[i].bushNodeB);
  }
  _lastBushPosition = 0;
}

//void CaminandesMain::updateBushes(bool first_time)
//{
//  int i;
//  Vector3 f_v = _bushArray[0].bushNodeF->getTranslationWorld();
//  Vector3 b_v = _bushArray[0].bushNodeB->getTranslationWorld();
//  float f_y = f_v.y;
//  float f_z = f_v.z;
//  float b_y = b_v.y;
//  float b_z = b_v.z;
//
//  for (i=0; i < NUM_BUSHES; i++) {
//    _bushArray[i].visible = true;
//    _bushArray[i].pos = bush_location[_currentGamePhase][i];
//    Vector3 f_v = _bushArray[i].bushNodeF->getTranslationWorld();
//    LOG("before x=%f, y=%f, z=%f", f_v.x, f_v.y, f_v.z);
//#if 0
//    _bushArray[i].bushNodeF->setIdentity();
//    _bushArray[i].bushNodeB->setIdentity();
//
//    _bushArray[i].bushNodeF->translate(_bushArray[i].pos, f_y, f_z);
//    _bushArray[i].bushNodeB->translate(_bushArray[i].pos, b_y, b_z);
//#else
//    if (first_time ==  true) {
//      _bushArray[i].bushNodeF->translateLeft(-_bushArray[i].pos);
//      _bushArray[i].bushNodeB->translateLeft(-_bushArray[i].pos);
//    }
//    else {
//
//    }
//#endif
//
//    Vector3 f_va = _bushArray[i].bushNodeF->getTranslationWorld();
//    LOG("After x=%f, y=%f, z=%f", f_va.x, f_va.y, f_va.z);
//  }
//
//}

void CaminandesMain::updateBushes(bool first_time)
{
  int i;

  if ( (_currentGamePhase == _lastBushPosition) && first_time == false ) { // no need to move
  // just make then visible
    LOG("No need to move bushes %d", 1);
    for (i=0; i < NUM_BUSHES; i++) {
      _bushArray[i].visible = true;
    }
    return;
  }

  for (i=0; i < NUM_BUSHES; i++) {
    _bushArray[i].visible = true;

    if (first_time == false) { //need to translate back
      _bushArray[i].bushNodeF->translateLeft(bush_location[_lastBushPosition][i]);
      _bushArray[i].bushNodeB->translateLeft(bush_location[_lastBushPosition][i]);
    }
    _bushArray[i].pos = bush_location[_currentGamePhase][i];
    _bushArray[i].bushNodeF->translateLeft(-_bushArray[i].pos);
    _bushArray[i].bushNodeB->translateLeft(-_bushArray[i].pos);
  }
  _lastBushPosition = _currentGamePhase;

}

void CaminandesMain::cloneFences(void)
{
  Node * node, * nnode;
  int i;
  const char * id;
  Node * nodeArray[96];

  //----------- create a complete pole ---------
  Node *iso_main, * iso_2, *iso_3, *iso_4;
  Node *iso_empty_2;

  iso_main = _scene->findNode("isolator_main");
  iso_empty_2 = _scene->findNode("isolator_main_empty_2");
  iso_2 = iso_main->clone();
  iso_3 = iso_main->clone();
  iso_4 = iso_main->clone();
  Vector3 iso_main_pos = iso_main->getTranslationWorld();
  Vector3 iso_empty_2_pos = iso_empty_2->getTranslationWorld();
  float iso_delta = iso_main_pos.y - iso_empty_2_pos.y;
  iso_2->translateY(-iso_delta);
  iso_3->translateY(-iso_delta * 2);
  iso_4->translateY(-iso_delta * 3);
  _scene->addNode(iso_2);
  _scene->addNode(iso_3);
  _scene->addNode(iso_4);

  // Wires
  Node * fence_wire_main = _scene->findNode("fence_wire_main");
  Node * wire_2 = fence_wire_main->clone();
  Node * wire_3 = fence_wire_main->clone();
  Node * wire_4 = fence_wire_main->clone();
  wire_2->translateY(-iso_delta);
  wire_3->translateY(-iso_delta * 2);
  wire_4->translateY(-iso_delta * 3);
  _scene->addNode(wire_2);
  _scene->addNode(wire_3);
  _scene->addNode(wire_4);


  Node * complete_fence = _scene->addNode("complete_fence");
  Node * pole_main = _scene->findNode("fence_still_main");
  Node * complete_pole = _scene->addNode("complete_pole");
  //    complete_pole->setTranslation(0, 0, 0);
  complete_pole->addChild(pole_main->clone());
  complete_pole->addChild(iso_main->clone());
  complete_pole->addChild(iso_2->clone());
  complete_pole->addChild(iso_3->clone());
  complete_pole->addChild(iso_4->clone());
  complete_pole->addChild(fence_wire_main->clone());
  complete_pole->addChild(wire_2->clone());
  complete_pole->addChild(wire_3->clone());
  complete_pole->addChild(wire_4->clone());

  Vector3 pole_main_pos = pole_main->getTranslationWorld();
  //    complete_pole->setTranslation(pole_main_pos);

  Node * pole_empty_1 = _scene->findNode("fence_pole_empty_1");
  Vector3 pole_empty_1_pos = pole_empty_1->getTranslationWorld();
  Node * pole_empty_2 = _scene->findNode("fence_pole_empty_2");
  Vector3 pole_empty_2_pos = pole_empty_2->getTranslationWorld();
  Node * pole_empty_3 = _scene->findNode("fence_pole_empty_3");
  Vector3 pole_empty_3_pos = pole_empty_3->getTranslationWorld();
  Node * pole_empty_4 = _scene->findNode("fence_pole_empty_4");
  Vector3 pole_empty_4_pos = pole_empty_4->getTranslationWorld();
  Node * pole_empty_5 = _scene->findNode("fence_pole_empty_diag");
  Vector3 pole_empty_5_pos = pole_empty_5->getTranslationWorld();
  float pole_z_delta_1 = pole_main_pos.z - pole_empty_1_pos.z;
  float pole_z_delta_2 = pole_main_pos.z - pole_empty_2_pos.z;
  float pole_z_delta_3 = pole_main_pos.z - pole_empty_3_pos.z;
  float pole_z_delta_4 = pole_main_pos.z - pole_empty_4_pos.z;
  float pole_z_delta_5 = pole_main_pos.z - pole_empty_5_pos.z;
  float pole_y_delta_5 = pole_main_pos.y - pole_empty_5_pos.y;
  LOG("delta = %f", pole_z_delta_1);
  LOG("delta = %f", pole_z_delta_2);
  LOG("delta = %f", pole_z_delta_3);
  LOG("delta = %f", pole_z_delta_4);
  LOG("delta = %f", pole_z_delta_5);

  Node * pole_1 = complete_pole->clone();
  pole_1->translateZ(-pole_z_delta_1);
  Node * pole_2 = complete_pole->clone();
  pole_2->translateZ(-pole_z_delta_2);
  Node * pole_3 = complete_pole->clone();
  pole_3->translateZ(-pole_z_delta_3);
  Node * pole_4 = complete_pole->clone();
  pole_4->translateZ(-pole_z_delta_4);

  Node * pole_5 = _scene->addNode("pole_5");
  pole_5->addChild(pole_main->clone());
  pole_5->addChild(iso_main->clone());
  Node * p5_2 =iso_main->clone();
  Node * p5_3 =iso_main->clone();
  Node * p5_4 =iso_main->clone();
  _scene->addNode(p5_2);
  _scene->addNode(p5_3);
  _scene->addNode(p5_4);
  p5_2->translateY(-iso_delta);
  p5_3->translateY(-iso_delta * 2);
  p5_4->translateY(-iso_delta * 3);
  pole_5->addChild(p5_2);
  pole_5->addChild(p5_3);
  pole_5->addChild(p5_4);



  //pole_5->rotateX((14.46 * 3.1415) / 180);
  pole_5->translate(0, -pole_y_delta_5, -pole_z_delta_5);


  // complete complete_fence node

  complete_fence->addChild(pole_main);
  complete_fence->addChild(pole_1);
  complete_fence->addChild(pole_2);
  complete_fence->addChild(pole_3);
  complete_fence->addChild(pole_4);
  complete_fence->addChild(pole_5);
  complete_fence->addChild(iso_2->clone());
  complete_fence->addChild(iso_3->clone());
  complete_fence->addChild(iso_4->clone());
  complete_fence->addChild(_scene->findNode("isolator_main")->clone());
  complete_fence->addChild(_scene->findNode("light_off")->clone());
  complete_fence->addChild(_scene->findNode("light_on")->clone());
  complete_fence->addChild(fence_wire_main->clone());

  complete_fence->addChild(_scene->findNode("fence_wire_021")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_022")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_023")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_024")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_025")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_026")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_027")->clone());
  complete_fence->addChild(_scene->findNode("fence_wire_028")->clone());

  Node * w5_1 = fence_wire_main->clone();
  Node * w5_2 = fence_wire_main->clone();
  Node * w5_3 = fence_wire_main->clone();
  Node * w5_4 = fence_wire_main->clone();
  w5_2->translateY(-iso_delta);
  w5_3->translateY(-iso_delta * 2);
  w5_4->translateY(-iso_delta * 3);
  w5_1->translateZ(-pole_z_delta_5);
  w5_2->translateZ(-pole_z_delta_5);
  w5_3->translateZ(-pole_z_delta_5);
  w5_4->translateZ(-pole_z_delta_5);
  complete_fence->addChild(w5_1);
  complete_fence->addChild(w5_2);
  complete_fence->addChild(w5_3);
  complete_fence->addChild(w5_4);

  _fenceNodes[0] = complete_fence;
  for (i=1; i < NUM_FENCES; i++) {
    nnode = complete_fence->clone();
    _fenceNodes[i] = nnode;
    nnode->translateLeft(fence_pos_array[0] - fence_pos_array[i]);
    _scene->addNode(nnode);
  }

  // clone terrain
  Node * terrain_main = _scene->findNode("terrain_1");
  Node * t_empty_1 = _scene->findNode("terrain_empty_1");
  Node * t1 = terrain_main->clone();
  Node * t2 = terrain_main->clone();
  Vector3 t_main_pos = terrain_main->getTranslationWorld();
  Vector3 t_empty_1_pos = t_empty_1->getTranslationWorld();
  float t_x_delta = t_empty_1_pos.x - t_main_pos.x;
  t1->translateX(t_x_delta);
  t2->translateX(t_x_delta * 2);
  _scene->addNode(t1);
  _scene->addNode(t2);

}

void CaminandesMain::keyEvent(Keyboard::KeyEvent evt, int key)
{
  Vector3 pos_char = _cBoxCharacterNode->getTranslationWorld();

  switch (evt)
  {
    case Keyboard::KEY_PRESS:
      //LOG("Key press %d", key);
      switch(key) {
        case Keyboard::KEY_UP_ARROW:
          if ((_isJumpingAction == false) && (pos_char.x < LIMIT_WALK_RIGHT)) {
            //Vector3 pos_char = _cBoxCharacterNode->getTranslationWorld();
            float pos_fence = GetFenceAhead();
            float dis;
            if (pos_fence != 0) {
              dis = pos_fence - pos_char.x;
              //LOG("DIS = %f", dis);
              if (dis >= GOOD_JUMP_THRESHOUD_LOW && dis <= GOOD_JUMP_THRESHOUD_HIGH)
                _goodJump = true;
              else
                _goodJump = false;

              //_jumpFlag = true;
            }
            _jumpFlag = true;
          }
          break;

        case Keyboard::KEY_DOWN_ARROW:
          if (_isSnappingAction == false) {
            _bushEaten = detectBush();
            _snapFlag =  true;
          }
          break;

        case Keyboard::KEY_RIGHT_ARROW:
          if ((_activeMove != MOVE_RIGHT) && (pos_char.x < LIMIT_WALK_RIGHT)) {
            _activeMove = MOVE_RIGHT;
            _resumeFlag = true;
            _backwards = false;
          }
          break;
        case Keyboard::KEY_LEFT_ARROW:
          if ((_activeMove != MOVE_LEFT)  && (pos_char.x > LIMIT_WALK_LEFT)) {
            _activeMove = MOVE_LEFT;
            _resumeFlag = true;
            _backwards = true;
          }
          break;

        case Keyboard::KEY_SHIFT:
          _turbo = true;

        default:
          break;
      }


      if (key == Keyboard::KEY_SPACE)
      {
        Vector3 pos = _cBoxCharacterNode->getTranslationWorld();
        LOG("CBox pos: x=%f, y=%f, z=%f", pos.x, pos.y, pos.z);
        _mainFormVisible = !_mainFormVisible;
        if (_mainFormVisible && _mainForm != NULL) {
          _mainForm->setEnabled(true);
          _mainForm->setVisible(true);
          _mainForm->setFocus();
          _soundSong1->pause();
        }
        else {
          _mainForm->setEnabled(false);
          _mainForm->setVisible(false);
        }
      }


      if (key == Keyboard::KEY_ESCAPE)
      {
        LOG("Escape %d",1);
        exit();
      }

      break;


    case Keyboard::KEY_RELEASE:

      if (key == Keyboard::KEY_SHIFT) {
        _turbo = false;
      }
      else {

        //LOG("Key release %d", key);
        _activeMove = MOVE_NONE;
        _pauseFlag = true;
        _resumeFlag = false;
      }
      break;

    case Keyboard::KEY_CHAR:
      if (key == Keyboard::KEY_BACKSPACE)
      {
        LOG("Backspace %d",1);
      }
      else
      {
        LOG("char key \"%c\"", key);
      }
      break;


  };
}

#define TOUCH_THRESHOLD_X  80
#define TOUCH_THRESHOLD_Y  60
#define SETUP_DIALOG_X      80
#define SETUP_DIALOG_Y      80

void CaminandesMain::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
  switch (evt)
  {
    case Touch::TOUCH_PRESS:
      LOG("Touch x=%d, y=%d", x,y);
      _touchX = x;
      _touchY = y;
      _touching = true;

      if (x < SETUP_DIALOG_X && y < SETUP_DIALOG_Y) {
        _mainFormVisible = true;
        _mainForm->setEnabled(true);
        _mainForm->setVisible(true);
        _mainForm->setFocus();
        _soundSong1->pause();
      }
      break;
    case Touch::TOUCH_RELEASE:
      _touching = false;
      keyEvent(Keyboard::KEY_RELEASE, Keyboard::KEY_UP_ARROW);
      keyEvent(Keyboard::KEY_RELEASE, Keyboard::KEY_DOWN_ARROW);
      keyEvent(Keyboard::KEY_RELEASE, Keyboard::KEY_RIGHT_ARROW);
      keyEvent(Keyboard::KEY_RELEASE, Keyboard::KEY_LEFT_ARROW);
      _spritesWalk1->Pause();
      break;
    case Touch::TOUCH_MOVE:
      if ((y - _touchY) > TOUCH_THRESHOLD_Y) {
        keyEvent(Keyboard::KEY_PRESS, Keyboard::KEY_DOWN_ARROW);
      }


      if ((_touchY - y) > TOUCH_THRESHOLD_Y) {
        keyEvent(Keyboard::KEY_PRESS, Keyboard::KEY_UP_ARROW);
      }

      //------------------ right/left -----------------

      if ((x - _touchX) > TOUCH_THRESHOLD_X * 2) _turbo = true;
      else _turbo = false;

      if ((x - _touchX) > TOUCH_THRESHOLD_X) {
        keyEvent(Keyboard::KEY_PRESS, Keyboard::KEY_RIGHT_ARROW);
        break;
      }

      if ( ((x - _touchX) > 0) && ((x - _touchX) < (TOUCH_THRESHOLD_X / 2))) {
        keyEvent(Keyboard::KEY_RELEASE, Keyboard::KEY_RIGHT_ARROW);
        break;
      }

      //--------

      if ((_touchX - x) > TOUCH_THRESHOLD_X * 2) _turbo = true;
      else _turbo = false;

      if ((_touchX - x) > TOUCH_THRESHOLD_X) {
        keyEvent(Keyboard::KEY_PRESS, Keyboard::KEY_LEFT_ARROW);
      }

      if ( ((_touchX - x) > 0) && ((_touchX - x) < (TOUCH_THRESHOLD_X / 2))) {
        keyEvent(Keyboard::KEY_RELEASE, Keyboard::KEY_LEFT_ARROW);
      }
      break;
  };

}

void CaminandesMain::initForm (void)
{

  _mainForm = Form::create(MAIN_FORM);
  if(_mainForm) {
    int w = _mainForm->getWidth();
    int s_w = getWidth();
    int h = _mainForm->getHeight();
    int s_h = getHeight();
    if ((s_w > w) && (s_h > h))
      _mainForm->setPosition((s_w - w) / 2, (s_h - h) / 2);

    Button* supportButton = static_cast<Button*>(_mainForm->getControl("supportButton"));
    Button* restartButton = static_cast<Button*>(_mainForm->getControl("restartButton"));
    Button* resumeButton = static_cast<Button*>(_mainForm->getControl("resumeButton"));
    supportButton->addListener(this, Control::Listener::PRESS);// | Control::Listener::RELEASE);
    restartButton->addListener(this, Control::Listener::PRESS);// | Control::Listener::RELEASE);
    resumeButton->addListener(this, Control::Listener::PRESS);// | Control::Listener::RELEASE);

    CheckBox* muteSongCheckbox = static_cast<CheckBox*>(_mainForm->getControl("muteSongCheckbox"));
    CheckBox* muteAllCheckbox = static_cast<CheckBox*>(_mainForm->getControl("muteAllCheckbox"));

    muteSongCheckbox->setChecked(_cfg.mute_song != 0);
    muteAllCheckbox->setChecked(_cfg.mute_all != 0);

    muteSongCheckbox->addListener(this, Control::Listener::PRESS);
    muteAllCheckbox->addListener(this, Control::Listener::PRESS);

    _mainForm->setEnabled(false);
    _mainForm->setVisible(false);
  }
  //---------------- finish form --------------------
  _finishForm = Form::create(FINISH_FORM);

  if(_finishForm) {
    int w = _finishForm->getWidth();
    int s_w = getWidth();
    int h = _finishForm->getHeight();
    int s_h = getHeight();
    _finishForm->setPosition((s_w - w) / 2, (s_h - h) / 2);
    _finishForm->setEnabled(false);
    _finishForm->setVisible(false);

    Button* restartButton = static_cast<Button*>(_finishForm->getControl("restartButton"));
    restartButton->addListener(this, Control::Listener::PRESS);// | Control::Listener::RELEASE);
  }

  //---------------- Game Over form --------------------
  _gameOverForm = Form::create(GAME_OVER_FORM);

  if(_gameOverForm) {
    int w = _gameOverForm->getWidth();
    int s_w = getWidth();
    int h = _gameOverForm->getHeight();
    int s_h = getHeight();
    _gameOverForm->setPosition((s_w - w) / 2, (s_h - h) / 2);
    _gameOverForm->setEnabled(false);
    _gameOverForm->setVisible(false);

    Button* restartButton = static_cast<Button*>(_gameOverForm->getControl("restartButton"));
    restartButton->addListener(this, Control::Listener::PRESS);// | Control::Listener::RELEASE);
  }

}

void CaminandesMain::controlEvent(Control* control, EventType evt)
{
  switch(evt)
  {
    case Control::Listener::PRESS:
      if (strcmp("resumeButton", control->getId()) == 0)
      {
        // Do something.
        LOG("Resume button %d", evt);
        _mainFormVisible = false;
        _mainForm->setEnabled(false);
        _mainForm->setVisible(false);
        updateConfig (_cfg.ad_time);
        if (_cfg.mute_song == 0)
          Play(_soundSong1);

        break;

      }
      else if (strcmp("supportButton", control->getId()) == 0)
      {
        // Do something.
        LOG("Support button %d", evt);
        openURL(SUPPORT_URL);
        break;
      }
      else if (strcmp("restartButton", control->getId()) == 0)
      {
        // Do something.
        LOG("Restart button %d", evt);
        _restartFlag = true;
        break;
      }
      else if (strcmp("muteSongCheckbox", control->getId()) == 0)
      {
        // Do something.
        _cfg.mute_song = ((CheckBox*) control)->isChecked() == 0;
        LOG("Mute Song %d", _cfg.mute_song);
        break;
      }
      else if (strcmp("muteAllCheckbox", control->getId()) == 0)
      {
        // Do something.
        _cfg.mute_all = ((CheckBox*) control)->isChecked() == 0;
        LOG("Mute All %d", _cfg.mute_all);
        break;
      }
      break;

      //        case Control::Listener::RELEASE:
      //            if (strcmp("myButton", control->getId()) == 0)
      //            {
      //                // Do something else.
      //            }
      //            break;

    default: break;
  }
}

void CaminandesMain::gestureSwipeEvent(int x, int y, int direction)
{
  LOG("Swipe x=%d, y=%d, dir=%d", x, y, direction);

}

void CaminandesMain::gestureTapEvent(int x, int y)
{
  LOG("Tap x=%d, y=%d", x, y);
}

void CaminandesMain::Play(AudioSource * s)
{
  if (_cfg.mute_all == 0)
    s->play();
}

#define     LIVES_POSITION_X    150
void CaminandesMain::drawLives (void)
{
  int i;

  if (_lives<=0) return;//
//  Rectangle src = Rectangle(230, 300, 550, 500);
  Rectangle src = Rectangle(150, 210, 420, 500);
  Rectangle dst1 = Rectangle(50, 5, 110, 115);
  _spritesWalk1->_sprites[0]->start();
  for (i = 0; i<_lives; i++) {
    dst1.setPosition(LIVES_POSITION_X + (i * 60), 5);
    _spritesWalk1->_sprites[0]->draw(dst1, src, Vector4::fromColor(0xFFFFFFFF));
  }
  _spritesWalk1->_sprites[0]->finish();
}

void CaminandesMain::GetConfig (void)
{
  int err = 0;
  FILE * f = FOPEN(CONFIG_FILENAME, "rb");

  if (f != NULL) {
    int len;
    if ((len = fread((void*) &_cfg, 1, sizeof(_cfg), f)) == sizeof(_cfg)) {
      LOG("cfg read OK %d",1);
      return;
    }
    else {
      LOG("cfg not read %d",len);
      fclose(f);
    }
  }
  LOG("cfg file not found %d", 1);
  _cfg.mute_all = 0;
  _cfg.mute_song = 0;
  _cfg.ad_time = time (NULL) - TIME_TO_FETCH;
  _cfg.num_plays = 0;
  _cfg.max_level = 0;
  _cfg.campain_id = 0;
  _cfg.unique_val = time(NULL);

  updateConfig (time (NULL)  - TIME_TO_FETCH);
}

void CaminandesMain::updateConfig (time_t ad_time)
{
  _cfg.ad_time = ad_time;

  FILE * f = FOPEN(CONFIG_FILENAME, "wb");

  if (f != NULL) {
    int len;
    if ((len = fwrite((void*) &_cfg, 1, sizeof(_cfg), f)) == sizeof(_cfg)) {
      LOG("cfg create was OK %d",1);
      fclose(f);
      return;
    }
    else {
      LOG("cfg write error %d",len);
      fclose(f);
    }
  }
  LOG("cfg creation error %d",1);

}

#if defined(__QNX__) || defined(__ANDROID__)

extern "C" {
  static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
  {
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
  }
}

#include <sys/stat.h>
bool file_exists(char * file)
{
  struct stat  buf;
  return (stat( file, &buf )) == 0;

}

bool CaminandesMain::fetch(CURL *curl_handle, char * url, char * local_header, char * local_doc)
{
  FILE *headerfile;
  FILE *bodyfile;
  CURLcode r;


  long http_code = 0;
  /* open the files */
  headerfile = FOPEN(local_header,"wb");
  if (headerfile == NULL) {
    LOG("Cound not open local header \"%s\"", local_header);
    curl_easy_cleanup(curl_handle);
    return false;
  }
  bodyfile = FOPEN(local_doc,"wb");
  if (bodyfile == NULL) {
    LOG("Cound not open local doc \"%s\"", local_doc);
    fclose(headerfile);
    curl_easy_cleanup(curl_handle);
    return false;
  }

  /* set URL to get */
  r = curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  //LOG("CURLOPT_URL r = %d, url: \"%s\"", r, url);


  /* we want the headers be written to this file handle */
  r = curl_easy_setopt(curl_handle,   CURLOPT_WRITEHEADER, headerfile);
  //LOG("CURLOPT_WRITEHEADER r = %d", r);

  /* we want the body be written to this file handle instead of stdout */
  curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, bodyfile);
  //LOG("CURLOPT_WRITEDATA r = %d", r);

  /* get it! */
  r = curl_easy_perform(curl_handle);
  //LOG("curl_easy_perform r = %d", r);

  /* close the header file */
  fclose(headerfile);

  /* close the body file */
  fclose(bodyfile);

  curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
  LOG("\"%s\" http_code = %d", local_doc, http_code);
  if (http_code != 200 ||  r == CURLE_ABORTED_BY_CALLBACK) {
    curl_easy_cleanup(curl_handle);
    return false;
  }
  return true;
}
#else
bool file_exists(char * file)
{
  return false;
}

#endif

bool CaminandesMain::fetchAd(void)
{
  // params: "?ver=1.0.0&np=5&ml=1005&ad=1"
#if defined(__QNX__) || defined(__ANDROID__)

  CURL *curl_handle;
  CURLcode r;


  char get_params_base[256];
  char get_params_final[256];

  sprintf(get_params_base,"?ver=%s&uid=%s&loc=%s&lan=%s&np=%d&ml=%d&uval=%d&ad=", VERSION_TEXT, _device_id, _device_location, _device_language, _cfg.num_plays, _cfg.max_level, _cfg.unique_val );
  //const char *res_path = FileSystem::getResourcePath ();


  r = curl_global_init(CURL_GLOBAL_ALL);
  //LOG("curl_global_init ret = %d", r);

  /* init the curl session */
  curl_handle = curl_easy_init();
  //LOG("curl_easy_init curl_handle = 0x%x", curl_handle);

  /* no progress meter please */
  r = curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
  //LOG("CURLOPT_NOPROGRESS = %d", r);

  /* send all data to this function  */
  r = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, ::write_data);
  //LOG("CURLOPT_WRITEFUNCTION ret = %d", r);

  r = curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
  //LOG("CURLOPT_USERAGENT ret = %d", r);

  // CRC
  /* set URL to get */
  //	curl_easy_setopt(curl_handle, CURLOPT_URL, URL_AD_CRC_1);

  sprintf(get_params_final, "%s%s%d", URL_AD_CRC, get_params_base, 1);
  if (!(fetch(curl_handle, get_params_final, LOCAL_HEADER, LOCAL_FILE_CRC_1))) {
    return false;
  }

  //----------------------------

  reply_info_t info1;
  FILE * f = FOPEN(LOCAL_FILE_CRC_1,"rb");
  if (f == 0) {
    LOG("fail to open info file 1 %d", 1);
    return false;
  }
  if (fread(&info1, 1, sizeof(info1), f) != sizeof(info1)) {
    LOG("fail to read info file 1 %d", 1);
    fclose(f);
    return false;
  }
  fclose(f);
  LOG("crc=0x%x, version = 0x%x, campain_id = %d, disable = %d, Magic = 0x%x\n\n",
      info1.crc, info1.latest_version, info1.campain_id, info1.disable_game, info1.magic_number);

  if (_cfg.campain_id == info1.campain_id) {
    LOG("We already running campain %d", _cfg.campain_id);
    return false;
  }
  //-------------------------------
  sprintf(get_params_final, "%s%s%d", URL_AD, get_params_base, 1);
  if (!(fetch(curl_handle, get_params_final, LOCAL_HEADER, LOCAL_FILE_AD_TMP_1))) {
    return false;
  }


  sprintf(get_params_final, "%s%s%d", URL_AD, get_params_base, 2);
  if (!(fetch(curl_handle, get_params_final, LOCAL_HEADER, LOCAL_FILE_AD_TMP_2))) {
    return false;
  }

  sprintf(get_params_final, "%s%s%d", URL_AD_CRC, get_params_base, 2);
  if (!(fetch(curl_handle, get_params_final, LOCAL_HEADER, LOCAL_FILE_CRC_2))) {
    return false;
  }

  curl_easy_cleanup(curl_handle);

  // crc
  uint32_t crc1, crc2;
  f = FOPEN(LOCAL_FILE_AD_TMP_1,"rb");
  if (f) crc1 = crc32_file(f);
  else {
    LOG("fail to open file to calc CRC %d", 1);
    return false;
  }
  LOG("CRC1 = 0x%x", crc1);

  f = FOPEN(LOCAL_FILE_AD_TMP_2,"rb");
  if (f) crc2 = crc32_file(f);
  else {
    LOG("fail to open file to calc CRC %d", 1);
    return false;
  }
  LOG("CRC2 = 0x%x", crc2);

  //-----------------------------

  reply_info_t info2;
  f = FOPEN(LOCAL_FILE_CRC_2,"rb");
  if (f == 0) {
    LOG("fail to open info file 2 %d", 1);
    return false;
  }
  if (fread(&info2, 1, sizeof(info2), f) != sizeof(info2)) {
    LOG("fail to read info file 2 %d", 1);
    fclose(f);
    return false;
  }
  fclose(f);
  LOG("crc=0x%x, version = 0x%x, campain_id = %d, disable = %d, Magic = 0x%x\n\n",
      info2.crc, info2.latest_version, info2.campain_id, info2.disable_game, info2.magic_number);

  // validate CRC's
  if (info1.crc != crc1) {
    LOG("Mismatch CRC1 file=0x%x, info=0x%x", info1.crc, crc1);
    return false;
  }
  if (info2.crc != crc2) {
    LOG("Mismatch CRC2 file=0x%x, info=0x%x", info2.crc, crc2);
    return false;
  }

  // ------------- refresh Ad 1 -------------
  int ret;
  if (FILE_EXISTS(ACTIVE_FILE_AD_1) ) {
    LOG("Ad1 file %s exists... lets delete it", ACTIVE_FILE_AD_1);
    ret = REMOVE (ACTIVE_FILE_AD_1);
    if (ret) {
      LOG("could not remove %s", ACTIVE_FILE_AD_1);
      return false;
    }

  }
  // rename
  ret = RENAME( LOCAL_FILE_AD_TMP_1, ACTIVE_FILE_AD_1 );
  LOG("rename \"%s\" to \"%s\"", LOCAL_FILE_AD_TMP_1, ACTIVE_FILE_AD_1);
  if (ret) {
    LOG("could not rename to %s code %d", ACTIVE_FILE_AD_1, errno);
    return false;
  }
  LOG("Ad1 File %s refreshed OK", ACTIVE_FILE_AD_1);

  // ------------- refresh Ad 2 -------------
  if (FILE_EXISTS(ACTIVE_FILE_AD_2) ) {
    LOG("Ad2 file %s exists... lets delete it", ACTIVE_FILE_AD_2);
    ret = REMOVE (ACTIVE_FILE_AD_2);
    if (ret) {
      LOG("could not remove %s", ACTIVE_FILE_AD_2);
      return false;
    }

  }
  // --------- rename Ad 2 ------------
  LOG("before LOCAL_FILE_AD_TMP_2    exist = %d ", FILE_EXISTS(LOCAL_FILE_AD_TMP_2));
  LOG("before ACTIVE_FILE_AD_2 exist = %d ", FILE_EXISTS(ACTIVE_FILE_AD_2));
  ret = RENAME( LOCAL_FILE_AD_TMP_2, ACTIVE_FILE_AD_2 );
  LOG("rename \"%s\" to \"%s\"", LOCAL_FILE_AD_TMP_2, ACTIVE_FILE_AD_2);
  LOG("after LOCAL_FILE_AD_TMP_2    exist = %d ", FILE_EXISTS(LOCAL_FILE_AD_TMP_2));
  LOG("after ACTIVE_FILE_AD_2 exist = %d ", FILE_EXISTS(ACTIVE_FILE_AD_2));
  if (ret) {
    LOG("could not rename to %s code %d", ACTIVE_FILE_AD_2, errno);
    return false;
  }
  LOG("Ad2 File %s refreshed OK", ACTIVE_FILE_AD_2);

  // ---------------update ad sprites ----------------
  //  LOG("chmod res = %d", chmod( ACTIVE_FILE_AD_1, 0b000111111111) );
  //  LOG("chmod res = %d", chmod( ACTIVE_FILE_AD_2, 0b000111111111) );
  char filename_ad1[128];
  char filename_ad2[128];
#ifdef __ANDROID__
  ANativeActivity* nativeActivity = __state->activity;
  const char* internalPath = nativeActivity->internalDataPath;
  char const * res_path = FileSystem::getResourcePath  (   ) ;

  sprintf(filename_ad1, "%s/%s", internalPath, ACTIVE_FILE_AD_1);
  sprintf(filename_ad2, "%s/%s", internalPath, ACTIVE_FILE_AD_2);

#else
  strcpy(filename_ad1, ACTIVE_FILE_AD_1);
  strcpy(filename_ad2, ACTIVE_FILE_AD_2);
#endif


  if (FILE_EXISTS(ACTIVE_FILE_AD_1)  == false) {
    LOG("fail to create sprite as \"%s\" was not found", ACTIVE_FILE_AD_1);
    return false;
  }

  if (_ad1_sprite) {
    LOG("delete sprite for AD1 %d",1);
    delete(_ad1_sprite);
  }
#ifdef __ANDROID__
  FILE * fd1 = FOPEN(ACTIVE_FILE_AD_1, "rb");
  if (!fd1) {
    return false;
  }
  _ad1_sprite = SpriteBatch::createOnFS(fd1);
#else
  _ad1_sprite = SpriteBatch::create(filename_ad1, NULL, 0);
#endif

  if (FILE_EXISTS(ACTIVE_FILE_AD_2)  == false) {
    LOG("fail to create sprite as \"%s\" was not found", ACTIVE_FILE_AD_2);
    return false;
  }
  if (_ad2_sprite) {
    LOG("delete sprite for AD2 %d",1);
    delete(_ad2_sprite);
  }
#ifdef __ANDROID__
  FILE * fd2 = FOPEN(ACTIVE_FILE_AD_2, "rb");
  if (!fd2) {
    return false;
  }
  _ad2_sprite = SpriteBatch::createOnFS(fd2);
#else
  _ad2_sprite = SpriteBatch::create(filename_ad2, NULL, 0);
#endif


  // all done
//  _ad_files_ok = true;
  LOG("Ad was refreshed OK %d",1);

  _cfg.campain_id = info1.campain_id;

#endif
  return true;
}


#ifdef __QNX__
bool CaminandesMain::GetDeviceInfo(void)
{
#ifndef PREMIUM
  deviceinfo_identifying_details_t *details;

  if ( deviceinfo_get_identifying_details(&details) != BPS_SUCCESS ) {
    LOG("deviceinfo_get_details fail %d", 1);
    return false;
  }

  const char* pin_src = deviceinfo_identifying_details_get_pin_string(details);
  if (!pin_src) {
    LOG("deviceinfo_identifying_details_get_pin_string fail %d", 1);
    return false;
  }
  _device_id[sizeof(_device_id) - 1] = 0;
  strncpy(_device_id, pin_src, sizeof(_device_id)-1);
  LOG("PIN = %s", _device_id);

  char *language;
  char *country;
  if (locale_get(&language, &country) != BPS_SUCCESS) {
    LOG("locale_get fail %d", 1);
    return false;
  }
  _device_location[sizeof(_device_location) - 1] = 0;
  strncpy(_device_location, country, sizeof(_device_location) - 1);
  _device_language[sizeof(_device_language) - 1] = 0;
  strncpy(_device_language, language, sizeof(_device_language) - 1);
  LOG("Country: \"%s\", language: \"%s\"", _device_location, _device_language);

#else
#endif
  return true;
}
#else
bool CaminandesMain::GetDeviceInfo(void)
{
  strcpy(_device_id, "12345678");
  strcpy(_device_location, "CAN");
  strcpy(_device_language, "EN");
  return true;
}

#endif
#include <inttypes.h>
void CaminandesMain::refreshAds(bool from_init)
{
#ifndef PREMIUM
  // check if we need to fetch ads
  int refresh = 0;
  time_t now = time(NULL);
  int64_t delta = now - _cfg.ad_time;

  if (delta < 0) {
    LOG("user rolled back clock %d", 1);
    refresh = 1;
  }
  //delta in seconds ... 1 day has 60 * 60 * 24 = 86400 seconds
  LOG("time since last fetch = %lld sec", delta);
  if (delta >= TIME_TO_FETCH) {
    LOG("Need to fetch as delta >  TIME_TO_FETCH %lld", delta);
    refresh = 1;
  }
  else {
    LOG("NO need to fetch as delta <  TIME_TO_FETCH %lld", delta);
  }

  if (refresh) {
    int res;
    if (fetchAd()) {
      LOG("fetch OK... updating config %d", 1);
      updateConfig(now);
    }
  }
  else
    LOG("No need to fetch delta = %lld", delta);
#endif
}

void CaminandesMain::openURL(char * url)
{
#ifdef __QNX__
  navigator_invoke_invocation_t *invoke = NULL;
  navigator_invoke_invocation_create(&invoke);

  navigator_invoke_invocation_set_target(invoke, "sys.browser");
  navigator_invoke_invocation_set_action(invoke, "bb.action.OPEN");
  navigator_invoke_invocation_set_uri(invoke, url);

  navigator_invoke_invocation_send(invoke);
  navigator_invoke_invocation_destroy(invoke);
#else //elif defined(__ANDROID__)
  launchURL ( (const char *)  url );
#endif

}

//-----------------------------------------------------------------
Vector3 CaminandesMain::getLightDirection0() const
{
  return _directionalLight->getNode()->getForwardVectorView();
}

Vector3 CaminandesMain::getLightColor0() const
{
  return _directionalLight->getColor();
}

fsAutoBindingResolver::fsAutoBindingResolver(CaminandesMain* caminandes_main) {
  _caminandesMain = caminandes_main;
}

bool fsAutoBindingResolver::resolveAutoBinding(const char* autoBinding, Node* node, MaterialParameter* parameter)
{
  if (strcmp(autoBinding, "LIGHT_DIRECTION_0") == 0)
  {
    parameter->bindValue(this->_caminandesMain, &CaminandesMain::getLightDirection0);
    return true;
  }
  else if (strcmp(autoBinding, "LIGHT_COLOR_0") == 0)
  {
    parameter->bindValue(this->_caminandesMain, &CaminandesMain::getLightColor0);
    return true;
  }

  return false;
}

