#pragma once
#include "object.hpp"
#include "link.hpp"
#include "gift.hpp"
#include "json.hpp"

using json = nlohmann::json;

class Prayer : public Object {
    private:
        std::string text_;
        Link* link_;
    public:
        Prayer() : Object() {}

        Prayer(const std::string& text) 
        : Object()
        , text_(text)
        , link_(nullptr) {}

        Prayer(json& data) 
        : Object(data)
        , text_(data["text"])
        , link_(nullptr) {}

        Prayer(const std::string& text, Link* link)
        : Object()
        , text_(text)
        , link_(link) {
            link_->LinkObject(this);
        }

        Prayer(const Prayer& other) : Object(other) {
            text_ = other.text_;
            link_ = other.link_;
        }

        Prayer(Prayer&& other) : Object(other) {
            text_ = other.text_;
            link_ = other.link_;
            
            if (other.link_)
                other.link_ = nullptr;
        }

        Prayer& operator=(Prayer&& other) {
            Object::operator=(std::move(other));
            if (link_)
                delete link_;
            
            link_ = other.link_;
            text_ = other.text_;

            return *this;
        }

        Prayer& operator=(const Prayer& other) {
            return *this = Prayer(other);
        }

        ~Prayer() {
            link_->UnlinkObject(*this);
        }

        Link* GetLink() const {
            return link_;
        }

        void SetLink(Link* link) {
            link_ = link;
        }

        const std::string& GetText() const {
            return text_;
        }

        void Show() {
            std::cout << "-- Prayer --" << std::endl;
            std::cout << "Linked: " << link_->IsLinked() << std::endl;
            std::cout << "Prayer id: " << Uuid() << std::endl;
        }

        virtual std::string ToJson() const {
            json data;
            
            data["text"] = text_;
            data["id"] = Uuid();
            data["link_id"] = link_->Uuid();

            std::string json_data = data.dump();

            return json_data;   
        }

        virtual ObjectType GetType() const {
            return ObjectType::Prayer;
        }
};
