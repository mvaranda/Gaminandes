//
//  tables.h
//  caminandes
//
//  Created by Marcelo Varanda on 2014-03-14.
//
//

#ifndef caminandes_tables_h
#define caminandes_tables_h

#define     NUM_FENCES          7
#define     NUM_PHASES          3
#define     NUM_BUSHES          24


/****************************************
 *
 *           Flashing Patterns
 *
 ****************************************/

//------------------- phase 1 ----------------------

const int   flashing_pattern_phese_0_fence_0[] = {
    50,
    10000,
    0,
};

const int   flashing_pattern_phese_0_fence_1[] = {
    1000,
    2000,
    0,
};
const int   flashing_pattern_phese_0_fence_2[] = {
    1000,
    2000,
    0,
};
const int   flashing_pattern_phese_0_fence_3[] = {
    2000,
    1000,
    0,
};
const int   flashing_pattern_phese_0_fence_4[] = {
    2000,
    1000,
    0,
};
const int   flashing_pattern_phese_0_fence_5[] = {
    1000,
    1000,
    0,
};
const int   flashing_pattern_phese_0_fence_6[] = {
    500,
    500,
    500,
    1000,
    0,
};

//------------------- phase 2 ----------------------

const int   flashing_pattern_phese_1_fence_0[] = {
    250,
    250,
    250,
    1000,
    0,
};

const int   flashing_pattern_phese_1_fence_1[] = {
    250,
    250,
    250,
    250,
    250,
    1000,
    0,
};
const int   flashing_pattern_phese_1_fence_2[] = {
    1000,
    200,
    500,
    1000,
    0,
};
const int   flashing_pattern_phese_1_fence_3[] = {
    750,
    750,
    0,
};
const int   flashing_pattern_phese_1_fence_4[] = {
    750,
    750,
    0,
};
const int   flashing_pattern_phese_1_fence_5[] = {
  1000,
  200,
  500,
  1000,
    0,
};
const int   flashing_pattern_phese_1_fence_6[] = {
    100,
    100,
    100,
    100,
    100,
    100,
    100,
    100,
    100,
    1000,
    0
};

//------------------- phase 3 ----------------------

const int   flashing_pattern_phese_2_fence_0[] = {
    250,
    250,
  250,
  250,
  250,
  250,
  250,
  700,
    0,
};

const int   flashing_pattern_phese_2_fence_1[] = {
    250,
    250,
  250,
  250,
  800,
  250,
  250,
  250,
  250,
  800,
    0,
};
const int   flashing_pattern_phese_2_fence_2[] = {
    500,
    500,
    0,
};
const int   flashing_pattern_phese_2_fence_3[] = {
    200,
    500,
    0,
};
const int   flashing_pattern_phese_2_fence_4[] = {
  200,
  200,
  200,
  200,
  200,
  500,
    0,
};
const int   flashing_pattern_phese_2_fence_5[] = {
  200,
  200,
  200,
  200,
  200,
  200,
  200,
  500,
    0,
};
const int   flashing_pattern_phese_2_fence_6[] = {
  200,
  200,
  200,
  200,
  200,
  200,
  200,
  200,
  200,
  500,
    0,
};
const int fence_points_array [NUM_PHASES][NUM_FENCES] = {
    { 1, 2, 3, 4, 5, 6, 7 },
    { 8, 9, 10, 11, 12, 13, 14} ,
    { 15, 16, 17, 18, 19, 20, 21}
};


//-------------------- flashing_control_array ----------------------
const int * flashing_control_array [NUM_PHASES][NUM_FENCES] = {
    {
        flashing_pattern_phese_0_fence_0,
        flashing_pattern_phese_0_fence_1,
        flashing_pattern_phese_0_fence_2,
        flashing_pattern_phese_0_fence_3,
        flashing_pattern_phese_0_fence_4,
        flashing_pattern_phese_0_fence_5,
        flashing_pattern_phese_0_fence_6,
    },

    {
        flashing_pattern_phese_1_fence_0,
        flashing_pattern_phese_1_fence_1,
        flashing_pattern_phese_1_fence_2,
        flashing_pattern_phese_1_fence_3,
        flashing_pattern_phese_1_fence_4,
        flashing_pattern_phese_1_fence_5,
        flashing_pattern_phese_1_fence_6,
    },

    {
        flashing_pattern_phese_2_fence_0,
        flashing_pattern_phese_2_fence_1,
        flashing_pattern_phese_2_fence_2,
        flashing_pattern_phese_2_fence_3,
        flashing_pattern_phese_2_fence_4,
        flashing_pattern_phese_2_fence_5,
        flashing_pattern_phese_2_fence_6,
    },
};

/****************************************
 *
 *           Bushes Locations
 *
 ****************************************/

const char * bush_front_ids[NUM_BUSHES] = {
    "bush_front_0",
    "bush_front_1",
    "bush_front_2",
    "bush_front_3",
    "bush_front_4",
    "bush_front_5",
    "bush_front_6",
    "bush_front_7",
    "bush_front_8",
    "bush_front_9",
    "bush_front_10",
    "bush_front_11",
    "bush_front_12",
    "bush_front_13",
    "bush_front_14",
    "bush_front_15",
    "bush_front_16",
    "bush_front_17",
    "bush_front_18",
    "bush_front_19",
    "bush_front_20",
    "bush_front_21",
    "bush_front_22",
    "bush_front_23",
};

const char * bush_back_ids[NUM_BUSHES] = {
    "bush_back_0",
    "bush_back_1",
    "bush_back_2",
    "bush_back_3",
    "bush_back_4",
    "bush_back_5",
    "bush_back_6",
    "bush_back_7",
    "bush_back_8",
    "bush_back_9",
    "bush_back_10",
    "bush_back_11",
    "bush_back_12",
    "bush_back_13",
    "bush_back_14",
    "bush_back_15",
    "bush_back_16",
    "bush_back_17",
    "bush_back_18",
    "bush_back_19",
    "bush_back_20",
    "bush_back_21",
    "bush_back_22",
    "bush_back_23",
};

const float bush_location[NUM_PHASES][NUM_BUSHES] = {
    {
        1,     // cbox_fence_0
        1.5,     // cbox_fence_1
        2.5,     // cbox_fence_2
        7.5,     // cbox_fence_3
        8.2,     // cbox_fence_4
        9,     // cbox_fence_5
        11,     // cbox_fence_6
        14,     // cbox_fence_7
        18,     // cbox_fence_8
        18.7,     // cbox_fence_9
        22,     // cbox_fence_10
        25,     // cbox_fence_11
        26,     // cbox_fence_12
        30,     // cbox_fence_13
        31,     // cbox_fence_14
        32,     // cbox_fence_15
        35,     // cbox_fence_6
        37,     // cbox_fence_7
        40,     // cbox_fence_8
        41,     // cbox_fence_9
        41.6,     // cbox_fence_10
        43,     // cbox_fence_11
        49,     // cbox_fence_12
        51.5,     // cbox_fence_13
    },
    {
      1,     // cbox_fence_0
      1.5,     // cbox_fence_1
      2.5,     // cbox_fence_2
      7.5,     // cbox_fence_3
      8.2,     // cbox_fence_4
      9,     // cbox_fence_5
      11,     // cbox_fence_6
      14,     // cbox_fence_7
      18,     // cbox_fence_8
      18.7,     // cbox_fence_9
      22,     // cbox_fence_10
      25,     // cbox_fence_11
      26,     // cbox_fence_12
      30,     // cbox_fence_13
      31,     // cbox_fence_14
      32,     // cbox_fence_15
      35,     // cbox_fence_6
      37,     // cbox_fence_7
      40,     // cbox_fence_8
      41,     // cbox_fence_9
      41.6,     // cbox_fence_10
      43,     // cbox_fence_11
      49,     // cbox_fence_12
      51.5,     // cbox_fence_13
    },
    {
      -1,     // cbox_fence_0
      4,     // cbox_fence_1
      5,     // cbox_fence_2
      7.1,     // cbox_fence_3
      10,     // cbox_fence_4
      14.8,     // cbox_fence_5
      17.1,     // cbox_fence_6
      19.1,     // cbox_fence_7
      22,     // cbox_fence_8
      24.3,     // cbox_fence_9
      26.2,     // cbox_fence_10
      27,     // cbox_fence_11
      28.5,     // cbox_fence_12
      29.4,     // cbox_fence_13
      32.4,     // cbox_fence_14
      34.3,     // cbox_fence_15
      35.3,     // cbox_fence_6
      37,     // cbox_fence_7
      37.6,     // cbox_fence_8
      39.6,     // cbox_fence_9
      40.4,     // cbox_fence_10
      42,     // cbox_fence_11
      44,     // cbox_fence_12
      51.5,     // cbox_fence_13
    },
};


#endif
