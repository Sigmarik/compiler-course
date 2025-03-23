#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#define ADD_OPERATOR_EQUAL(type) \
    friend bool operator==(const type& left, const type& right) = default;

template <class T>
struct Ptr {
    Ptr(const Ptr<T>& other) { data = new T(*other.data); }
    Ptr(Ptr<T>&& other) {
        data = other.data;
        other.data = nullptr;
    }

    template <class... Args>
    Ptr(Args&&... args) {
        data = new T(std::forward<Args>(args)...);
    }

    Ptr(T&& other) { data = new T(other); }

    Ptr<T>& operator=(const Ptr<T>& other) {
        data = new T(*other.data);
        return *this;
    }

    Ptr<T>& operator=(Ptr<T>&& other) {
        data = other.data;
        return *this;
    }

    Ptr<T>& operator=(const T& other) {
        data = new T(other);
        return *this;
    }

    Ptr<T>& operator=(T&& other) {
        data = new T(std::move(other));
        return *this;
    }

    ~Ptr() {
        if (data) delete data;
        data = nullptr;
    }

    T& operator*() { return *data; }
    const T& operator*() const { return *data; }

    T* operator->() { return data; }
    const T* operator->() const { return data; }

    T&& move() {
        T* data_copy = data;
        data = nullptr;
        return std::move(*data_copy);
    }

   private:
    T* data = nullptr;
};

template <class T>
struct ChillVector {
    ChillVector() = default;
    ChillVector(ChillVector&&) = default;
    ChillVector<T>& operator=(ChillVector&&) = default;

    void push_back(T&& elem) { data.push_back(new T(std::move(elem))); }

    ~ChillVector() {
        for (T* elem : data) {
            delete elem;
        }
    }

   private:
    std::vector<T*> data{};
};

struct Assignment;
struct Branch;
struct Print;

using Action = std::variant<Assignment, Branch, Print>;

struct Sequence {
    ~Sequence();

    std::vector<Action*> actions{};
};

using ExprValueType = int;

struct Variable {
    std::string name{};
    ExprValueType* value{};
};

struct Add;
struct Subtract;
struct Equal;
struct Constant;
struct Variable;

using Expression = std::variant<Add, Subtract, Equal, Constant, Variable>;

struct Add {
    Expression* left{};
    Expression* right{};
};

struct Subtract {
    Expression* left{};
    Expression* right{};
};

struct Equal {
    Expression* left{};
    Expression* right{};
};

struct Constant {
    ExprValueType value;
};

struct Assignment {
    Variable* var{};
    Expression* expr{};
};

struct Branch {
    Expression* condition{};

    Sequence* true_branch{};
    Sequence* false_branch{};
};

struct Print {
    Expression* expr{};
};

using NameMap = std::unordered_map<std::string, std::unique_ptr<ExprValueType>>;

extern Sequence* root_sequence;
