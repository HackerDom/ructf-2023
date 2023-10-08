#include "main.hpp"

int main() {
    while (true) {
        int option = 0;
        std::cout << "---- Menu ----" << std::endl;
        std::cout << kMenu << std::endl;
        std::cout << "> ";
        std::cin >> option;

        switch (option) {
            case 1:
                MakeGift();
                break;
            case 2:
                ViewGift();
                break;
            case 3:
                MakePrayer();
                break;
            case 4:
                CallPerune();
                break;
            case 5:
                return 0;
            default:
                return 0;
        }
    }

    return 0;
};

size_t MakeGift(void) {
    std::cout << "-- Choose gift --" << std::endl;
    size_t Idx = 0;

    std::cout << GiftType::Egg << ". Egg" << std::endl;
    std::cout << GiftType::Milk << ". Milk" << std::endl;
    std::cout << GiftType::Meat << ". Meat" << std::endl;
    std::cout << GiftType::Porridge << ". Porridge" << std::endl;
    std::cout << GiftType::Bread << ". Bread" << std::endl;
    std::cout << GiftType::Orange << ". Orange" << std::endl;
    std::cout << "> ";
    std::cin >> Idx;

    GiftType type;

    switch(Idx) {
        case GiftType::Egg:
            type = GiftType::Egg;
            break;
        case GiftType::Milk:
            type = GiftType::Milk;
            break;
        case GiftType::Meat:
            type = GiftType::Meat;
            break;
        case GiftType::Porridge:
            type = GiftType::Porridge;
            break;
        case GiftType::Bread:
            type = GiftType::Bread;
            break;
        case GiftType::Orange:
            type = GiftType::Orange;
            break;
        default:
            std::cout << "{-} Invalid index!" << std::endl;
            return 1;
    }

    size_t GiftCount = 0;
    std::cout << "{?} Enter gift count: ";
    std::cin >> GiftCount;

    std::string GiftPassword;
    std::cout << "{?} Enter password: ";
    std::cin >> GiftPassword;

    if (GiftPassword.size() > kMaxPasswordSize) {
        std::cout << "{-} Invalid password size!" << std::endl;
        std::exit(0);
    }

    Gift* pGift = new Gift(type, GiftCount, GiftPassword);    
    AddObjectToCache(pGift);

    std::cout << "{!} Your gift id: " << pGift->Uuid() << std::endl;

    return 0;
};

size_t ViewGift(void) {
    Gift* gift = GetGift();

    if (gift == nullptr) {
        return 1;
    }

    std::cout << "--- Gift ---" << std::endl;
    std::cout << "Type: " << gift->GetGiftType() << std::endl;
    std::cout << "Count: " << gift->GetCount() << std::endl;
    std::cout << "Password: " << gift->GetPassword() << std::endl;
    
    if (gift->IsLinked()) {
        Link* link = gift->GetLink();
        Prayer* prayer = link->GetObjectByType<Prayer, ObjectType::Prayer>();

        if (prayer == nullptr) {
            std::cout << "{-} For this gift prayer isn't exists!" << std::endl;
            return 1;
        }

        prayer->Show();
    }

    return 0;
};

size_t MakePrayer(void) {
    Gift* pGift = GetGift();
    
    if (pGift == nullptr) {
        return 1;
    }

    if (pGift->GetLink()->IsLinked()) {
        std::cout << "{-} This gift already linked with prayer!" << std::endl;
        return 2;
    }

    std::string PrayerText;
    std::cout << "{?} Enter prayer text: ";
    std::cin >> PrayerText;

    if (PrayerText.size() > kMaxPrayerTextSize) {
        std::cout << "{-} Invalid prayer text size!" << std::endl;
        std::exit(0);
    }

    Prayer* pPrayer = new Prayer(PrayerText, pGift->GetLink());
    AddObjectToCache(pGift->GetLink());

    std::cout << "{!} Your prayer id: " << pPrayer->Uuid() << std::endl;
    
    AddObjectToCache(pPrayer);

    return 0;
};

size_t CallPerune(void) {
    std::cout << "{?} Enter prayer id: ";
    std::string PrayerId;
    std::cin >> PrayerId;

    Prayer* pPrayer = GetObjectFromCache<Prayer>(PrayerId);
    
    if (pPrayer == nullptr) {
        pPrayer = GetObjectFromDb<Prayer>(PrayerId);

        if (pPrayer == nullptr) {
            std::cout << "{-} Invalid prayer ID!" << std::endl;
            return 1;
        }
    }

    if (pPrayer->GetType() != ObjectType::Prayer) {
        std::cout << "{-} Invalid prayer ID!" << std::endl;
        return 1;
    }

    Gift* pGift = pPrayer->GetLink()->GetObjectByType<Gift, ObjectType::Gift>();

    if (pGift == nullptr) {
        std::cout << "{-} Prayer has invalid gift!" << std::endl;
        return 1;
    }

    int NeededScore = 1337;
    int UserScore = pGift->Score();

    if (UserScore >= NeededScore) {
        std::cout << "{+} Your prayer {" << pPrayer->GetText();
        std::cout << "} is completed!" << std::endl;

        pGift->ReduceScore(NeededScore);
        FlushCacheToDb();
    } else {
        std::cout << "{-} I think you're a lizard! Get out here!" << std::endl;

        Prayer* pPrayer = pGift->GetLink()->GetObjectByType<Prayer, ObjectType::Prayer>();
        DeleteFromCache(pPrayer);
        delete pPrayer;

        DeleteFromCache(pGift);
        delete pGift;
    }

    return 0;
};

Gift* GetGift() {
    std::cout << "{?} Enter gift ID: ";
    std::string GiftId;
    std::cin >> GiftId;
    
    Gift* pGift = GetObjectFromCache<Gift>(GiftId);

    if (pGift == nullptr) {
        pGift = GetObjectFromDb<Gift>(GiftId);

        if (pGift == nullptr) {
            return nullptr;
        }
    }

    if (pGift->GetType() != ObjectType::Gift) {
        std::cout << "{-} Invalid gift ID!" << std::endl;
        return nullptr;
    }

    std::cout << "{?} Enter password: ";
    std::string password;
    std::cin >> password;

    if (password.size() > kMaxPasswordSize) {
        std::cout << "{-} Invalid password size!" << std::endl;
        std::exit(0);
    }

    if (pGift->GetPassword() != password) {
        std::cout << "{-} Incorrect password!" << std::endl;
        return nullptr;
    }

    return pGift;
};

template <typename T>
T* GetObjectFromDb(std::string ObjectId) {
    auto ObjectJson = GetObjectJson(ObjectId);

    if (ObjectJson == nullptr) {
        return nullptr;
    }

    if (!ObjectJson.contains("link_id")) {
        std::cout << "{-} Invalid object type!" << std::endl;
        return nullptr;
    }

    auto LinkJson = GetObjectJson(std::string(ObjectJson["link_id"]));
    
    if (LinkJson == nullptr) {
        return nullptr;
    }

    auto PrayerJson = GetObjectJson(std::string(LinkJson["prayer_id"]));
    
    if (PrayerJson == nullptr) {
        return nullptr;
    }

    auto GiftJson = GetObjectJson(std::string(LinkJson["gift_id"]));

    if (GiftJson == nullptr) {
        return nullptr;
    }

    if ((LinkJson["gift_id"] != GiftJson["id"]) ||
        (LinkJson["prayer_id"] != PrayerJson["id"])) {
            std::cout << "{-} Invalid link!" << std::endl;
            return nullptr;
    }
    
    if ((GiftJson["link_id"] != LinkJson["id"]) ||
        (PrayerJson["link_id"] != LinkJson["id"])) {
            std::cout << "{-} Invalid link!" << std::endl;
            return nullptr;
    }

    if (ObjectJson["id"] != GiftJson["id"] &&
        ObjectJson["id"] != PrayerJson["id"]) {
            std::cout << "{-} Corrupted items detected!" << std::endl;
            return nullptr;
    }

    CreateObjects(GiftJson, PrayerJson, LinkJson);

    return GetObjectFromCache<T>(ObjectId);
};

void CreateObjects(json GiftJson, json PrayerJson, json LinkJson) {
    Gift* gift = new Gift(GiftJson);
    Prayer* prayer = new Prayer(PrayerJson);
    Link* link = new Link(LinkJson);

    link->LinkObject(gift);
    link->LinkObject(prayer);

    gift->SetLink(link);
    prayer->SetLink(link);

    AddObjectToCache(gift);
    AddObjectToCache(prayer);
    AddObjectToCache(link);
};

bool CheckJson(json Data) {
    if (Data == nullptr) {
        std::cout << "{-} No such object!" << std::endl;
        return false;
    }

    if (!Data.contains("id")) {
        std::cout << "{-} Invalid object json format!" << std::endl;
        return false;
    }

    return true;
};

json GetObjectJson(std::string Id) {
    trim(Id);

    auto ObjectJson = GetJsonFromFile(kDbName + Id);

    if (!CheckJson(ObjectJson)) {
        return nullptr;
    }

    if (ObjectJson["id"] != Id) {
        return nullptr;
    }

    return ObjectJson;
};

json GetJsonFromFile(std::string Name) {
    std::ifstream file(Name);

    if (!file.is_open()) {
        return nullptr;
    }

    json data = json::parse(file);
    file.close();

    return data;
};

void AddObjectToDb(Object* pObj) {    
    std::string key = pObj->Uuid();
    std::string value = pObj->ToJson();

    std::ofstream file;

    file.open(kDbName + key);
    
    if (!file.is_open())
        return;

    file << value;
    file.close();
};

template <typename T>
T* GetObjectFromCache(std::string& ObjectId) {
    if (gCache.count(ObjectId)) {
        return static_cast<T*>(gCache[ObjectId]);
    } else {
        return nullptr;
    }
};

void FlushCacheToDb() {
    for(auto [key, value] : gCache)
        AddObjectToDb(value);
};

void trim(std::string& buf) {
    std::replace(buf.begin(), buf.end(), '.', '\x00');
    std::replace(buf.begin(), buf.end(), '/', '\x00');
};

void AddObjectToCache(Object* pObj) {
    if ((gCache.size() + 1) > kMaxCacheSize) {
        std::cout << "{-} Too many requests! Try again later!" << std::endl;
        std::exit(0);
    }
    gCache.emplace(pObj->Uuid(), pObj);
};

template <typename T>
void DeleteFromCache(T* pObj) {
    gCache.erase(pObj->Uuid());
};