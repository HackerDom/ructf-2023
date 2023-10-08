#pragma once
#include <uuid/uuid.h>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;
static constexpr size_t kUUIDBufferSize = 512;

enum class ObjectType {
    Object,
    Link,
    Gift,
    Prayer,
};

class Object {
    private:
        uuid_t id_;
    public:
        Object() {
            uuid_generate(id_);
        }

        Object(json& data) {
            std::string uuid;
            if (data.contains("id")) {
                uuid = data["id"];
                uuid_parse(uuid.c_str(), id_);
            }
        };

        ~Object() {
            uuid_clear(id_);
        }

        Object(const Object& other) {
            uuid_copy(id_, other.id_);
        }

        Object(Object&& other) {
            uuid_copy(id_, other.id_);
            uuid_clear(other.id_);
        }

        Object& operator=(Object&& other) {
            uuid_copy(id_, other.id_);
            uuid_clear(other.id_);
            return *this;
        }

        Object& operator=(const Object& other) {
            return *this = Object(other);
        }

        const uuid_t& GetId() const {
            return id_;
        }

        std::string Uuid() const {
            char buf[kUUIDBufferSize];
            uuid_unparse(id_, buf);
            std::string uuid(buf);
            return uuid;
        }

        virtual std::string ToJson() const {
            json data;
            data["id"] = Uuid();

            std::string json_data = data.dump();
            return json_data;
        }

        virtual ObjectType GetType() const {
            return ObjectType::Object;
        }
};
