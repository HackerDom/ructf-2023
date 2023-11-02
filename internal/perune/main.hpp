#include <iostream>
#include <cstdint>
#include <string>
#include <uuid/uuid.h>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <map>

#include "object.hpp"
#include "gift.hpp"
#include "prayer.hpp"
#include "link.hpp"

static constexpr auto kDbName = "./database/";
static constexpr auto kMenu = "1. Make gift\n2. View gift\n3. Make prayer\n4. Call Perune\n5. Exit";
static constexpr size_t kMaxCacheSize = 0x100;
static constexpr size_t kMaxPasswordSize = 0x100;
static constexpr auto kMaxPrayerTextSize = kMaxPasswordSize;

std::unordered_map<std::string, Object*> gCache;

// menu handlers
size_t MakeGift(void);
size_t ViewGift(void);
size_t MakePrayer(void);
size_t CallPerune(void);

// json utils
json GetJsonFromFile(std::string);
json GetObjectJson(std::string);
bool CheckJson(json);
void CreateObjects(json, json, json);

// cache utils
template <typename T>
T* GetObjectFromCache(std::string& ObjectId);

template <typename T>
void DeleteFromCache(T* pObj);

void AddObjectToCache(Object* pObj);
void FlushCacheToDb();

// db utils
template <typename T>
T* GetObjectFromDb(std::string ObjectId);

void AddObjectToDb(Object* pObj);

// other utils
Gift* GetGift(void);
void trim(std::string& buf);