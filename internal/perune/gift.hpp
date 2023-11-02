#pragma once

#include "object.hpp"
#include "link.hpp"
#include "json.hpp"

using json = nlohmann::json;

enum GiftType {
    Egg,
    Milk,
    Meat,
    Porridge,
    Bread,
    Orange,
};

class Gift : public Object {
    private:
        GiftType type_;
        size_t count_;
        std::string password_;
        Link* link_;

        void DestroyLink() {
            if (link_ != nullptr) {
                if (link_->IsObjectInLink(*this))
                    link_->UnlinkObject(*this);
                    
                if (!link_->HasLinked()) {
                    delete link_;
                    link_ = nullptr;
                }
            }
        }

    public:
        Gift() : Object() {}

        Gift(GiftType type, size_t count, std::string password) 
        : Object()
        , type_(type)
        , count_(count)
        , password_(password)
        , link_(new Link(this)) {}
        
        Gift(GiftType type, size_t count, std::string password, Link* link) 
        : Object()
        , type_(type)
        , count_(count)
        , password_(password)
        , link_(link) {}

        Gift(json& data) 
        :  Object(data)
        , type_(data["type"])
        , count_(data["count"])
        , password_(data["password"])
        , link_(nullptr) {}
        
        Gift(const Gift& other) : Object(other) {
            type_ = other.type_;
            count_ = other.count_;
            password_ = other.password_;

            link_ = new Link(this, other.link_->Get().second);              
        }

        Gift(Gift&& other) : Object(other) {
            type_ = other.type_;
            count_ = other.count_;
            password_ = other.password_;
            link_ = other.link_;

            other.link_ = nullptr;
        }

        Gift& operator=(Gift&& other) {
            DestroyLink();
            Object::operator=(std::move(other));
            
            other.link_ = nullptr;
            return *this;
        }

        Gift& operator=(const Gift& other) {
            return *this = Gift(other);
        }

        ~Gift() {
            DestroyLink();
        }

        bool IsLinked() const {
            return link_->IsLinked();
        }

        Link* GetLink() const {
            return link_;
        }

        void SetLink(Link* link) {
            if (link_ != nullptr) {
                DestroyLink();
            }
            link_ = link;
        }

        const std::string& GetPassword() const {
            return password_;
        }

        const size_t& GetCount() const {
            return count_;
        }

        const GiftType& GetGiftType() const {
            return type_;
        }

        double Coef() const {
            double coef = 0.0f;

            switch(type_) {
                case GiftType::Egg:
                    coef = 0.1f;
                    break;
                case GiftType::Milk:
                    coef = 0.2f;
                    break;
                case GiftType::Meat:
                    coef = 1.337f;
                    break;
                case GiftType::Porridge:
                    coef = 0.55f;
                    break;
                case GiftType::Bread:
                    coef = 0.77f;
                    break;
                case GiftType::Orange:
                    coef = 1.5f;
                    break;
                default:
                    coef = 0.0f;
                    break;
            }
            return coef;
        }

        int Score() const {
            double count = static_cast<double>(count_);
            double coef = Coef();
            return static_cast<int>(count * coef);
        }

        void ReduceScore(int score) {
            int cur_score = Score();
            cur_score -= score;

            if (cur_score <= 0) {
                count_ = 0;
            } else {
                count_ = static_cast<int>(static_cast<double>(cur_score) / Coef());
            }
        }

        virtual std::string ToJson() const {
            json data;
            
            data["count"] = count_;
            data["password"]  = password_;
            data["id"] = Uuid();
            data["type"] = type_;
            data["link_id"] = link_->Uuid();
            
            std::string json_data = data.dump();

            return json_data;   
        }

        virtual ObjectType GetType() const {
            return ObjectType::Gift;
        }
};
