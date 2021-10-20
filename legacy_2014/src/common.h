#ifndef __COMMON_H__
#define __COMMON_H__
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
#include <time.h>

//#define LOG_ENABLED


#ifdef LOG_ENABLED
#define LOG(fmt, ...) do {  \
    Logger::log(Logger::LEVEL_INFO, "line %d: ", __LINE__); \
    Logger::log(Logger::LEVEL_INFO, fmt, __VA_ARGS__); \
    Logger::log(Logger::LEVEL_INFO, "\n"); \
} while (0)
#else
#define LOG(fmt, ...) do { /* dummy */ } while(0)
#endif

typedef struct config_st {
  int mute_all;
  int mute_song;
  time_t ad_time;

  uint32_t campain_id;
  int num_plays;
  int max_level;

  time_t unique_val;
} config_t;


//----------------------------------------------------
#endif //#define __COMMON_H__
