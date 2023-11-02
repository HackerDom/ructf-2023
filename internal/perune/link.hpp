#pragma once
#include "object.hpp"
#include "json.hpp"

using json = nlohmann::json;
class Link : public Object {
    private:
        std::pair<Object*, Object*> link_;
    public:
        Link() : Object() {}

        Link(json& data) : Object(data) {}

        Link(Object* object1, Object* object2) : Object() {
            link_ = std::make_pair(object1, object2);
        }

        Link(Object* object) : Object() {
            if (object->GetType() == ObjectType::Gift) {
                link_.first = object;
                link_.second = nullptr;
            }

            if (object->GetType() == ObjectType::Prayer) {
                link_.second = object;
                link_.first = nullptr;
            }
        }

        Link(const Link& other) : Object(other) {
            link_ = std::make_pair(other.link_.first, other.link_.second);
        }

        Link(Link&& other) : Object(other) {
            link_ = std::make_pair(other.link_.first, other.link_.second);
            other.Unlink();
        }

        Link& operator=(Link&& other) {
            if (this != &other) {
                Object::operator=(std::move(other));
                link_ = other.link_;
                other.Unlink();
            }
            return *this;
        }

        Link& operator=(const Link& other) {
            return *this = Link(other);
        }

        ~Link() {
            Unlink();
        }

        void Unlink() {
            link_.first = nullptr;
            link_.second = nullptr;
        }

        bool UnlinkObject(const Object& object) {
            if (link_.first != nullptr) {
                if (!uuid_compare(link_.first->GetId(), object.GetId())) {
                    link_.first = nullptr;
                    return true;
                }
            }

            if (link_.second != nullptr) {
                if (!uuid_compare(link_.second->GetId(), object.GetId())) {
                    link_.second = nullptr;
                    return true;
                }
            }
            
            return false;
        }

        bool IsObjectInLink(const Object& object) const {
            if (link_.first != nullptr) {
                if (!uuid_compare(link_.first->GetId(), object.GetId())) {
                    return true;
                } else {
                    return false;
                }
            }

            if (link_.second != nullptr) {
                if (!uuid_compare(link_.second->GetId(), object.GetId())) {
                    return true;
                } else {
                    return false;
                }
            }
            
            return false;
        }

        bool IsLinked() const {
            if (link_.first != nullptr && link_.second != nullptr)
                return true;
            return false;
        }

        bool HasLinked() const {
            if (link_.first != nullptr || link_.second != nullptr)
                return true;
            return false;
        }

        bool LinkObject(Object* object) {
            if (object->GetType() == ObjectType::Gift) {
                link_.first = object;
                return true;
            }

            if (object->GetType() == ObjectType::Prayer) {
                link_.second = object;
                return true;
            }

            return false;
        }

        const std::pair<Object*, Object*>& Get() const {
            return link_;
        }

        template <typename T, ObjectType U>
        T* GetObjectByType() {
            if (U == ObjectType::Gift) {
                return static_cast<T*>(link_.first);
            } else if (U == ObjectType::Prayer) {
                return static_cast<T*>(link_.second);
            }

            return nullptr;
        }
        
        virtual std::string ToJson() const {
            json data;
            
            data["id"] = Uuid();

            if (link_.first != nullptr)
                data["gift_id"] = link_.first->Uuid();
            
            if (link_.second != nullptr)
                data["prayer_id"] = link_.second->Uuid();
            
            std::string json_data = data.dump();

            return json_data;   
        }
        
        virtual ObjectType GetType() const {
            return ObjectType::Link;
        }
};
