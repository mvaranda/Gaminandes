#ifndef __CRC_H__
#define __CRC_H__

#include <stdio.h>
#include <stdint.h>

#define MAGIC_NUM   0xaa55aa55

uint32_t crc32(uint32_t crc, const void *buf, size_t size);
uint32_t crc32_file(FILE * f);

typedef struct reply_info_st {
  uint32_t    crc;
  uint32_t    latest_version;
  uint32_t    campain_id;
  uint32_t    disable_game;
  uint32_t    magic_number;
} reply_info_t;

//----------------------------------------------------------------
#endif
