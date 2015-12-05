#ifndef __SUBWAY_COMMON_H__
#define __SUBWAY_COMMON_H__

#include <iostream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

//메뉴
enum { ERROR, FIND_PATH, EXIT };

//검색모드 설정시 쓰임
enum { NONE, MINIMUM_TIME, MINIMUM_TRANSIT, MINIMUM_DISTANCE };

#endif