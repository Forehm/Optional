#pragma once
#include <stdexcept>
#include <utility>

class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;
    Optional(const T& value);
    Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other);

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs);
    Optional& operator=(const Optional& rhs);
    Optional& operator=(Optional&& rhs);

    ~Optional();

    bool HasValue() const;

    // ќператоры * и -> не должны делать никаких проверок на пустоту Optional.
    // Ёти проверки остаютс€ на совести программиста
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;

    // ћетод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value();
    const T& Value() const;

    void Reset();

private:
    // alignas нужен дл€ правильного выравнивани€ блока пам€ти
    alignas(T) char data_[sizeof(T)];
    T* ptr_ = nullptr;
    bool is_initialized_ = false;
};

template <typename T>
Optional<T>::Optional(const T& value)
    : is_initialized_(true) {
    ptr_ = new(&data_[0]) T(value);
}

template <typename T>
Optional<T>::Optional(T&& value)
    : is_initialized_(true) {
    ptr_ = new(&data_[0]) T(std::move(value));
}

template <typename T>
Optional<T>::Optional(const Optional& other)
    : is_initialized_(other.is_initialized_) {
    if (is_initialized_) {
        ptr_ = new(&data_[0]) T(other.Value());
    }
}

template <typename T>
Optional<T>::Optional(Optional&& other)
    : is_initialized_(std::move(other.is_initialized_)) {
    if (is_initialized_) {
        ptr_ = new(&data_[0]) T(std::move(other.Value()));
    }
}

template <typename T>
Optional<T>::~Optional() {
    Reset();
}

template <typename T>
Optional<T>& Optional<T>::operator=(const T& value) {
    if (!is_initialized_) {
        ptr_ = new(&data_[0]) T(value);
        is_initialized_ = true;
    }
    else {
        *ptr_ = value;
    }
    return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(T&& rhs) {
    if (!is_initialized_) {
        ptr_ = new(&data_[0]) T(std::move(rhs));
        is_initialized_ = true;
    }
    else {
        *ptr_ = std::move(rhs);
    }
    return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(const Optional& rhs) {
    if (!is_initialized_) {
        if (rhs.is_initialized_) {
            ptr_ = new(&data_[0]) T(rhs.Value());
            is_initialized_ = rhs.is_initialized_;
        }
    }
    else {
        if (rhs.is_initialized_) {
            *ptr_ = rhs.Value();
        }
        else {
            Reset();
        }
    }
    return *this;
}

template <typename T>
Optional<T>& Optional<T>::operator=(Optional&& rhs) {
    if (!is_initialized_) {
        if (rhs.is_initialized_) {
            ptr_ = new(&data_[0]) T(std::move(rhs.Value()));
            is_initialized_ = std::move(rhs.is_initialized_);
        }
    }
    else {
        if (rhs.is_initialized_) {
            *ptr_ = std::move(rhs.Value());
        }
        else {
            Reset();
        }
    }
    return *this;
}

template <typename T>
bool Optional<T>::HasValue() const {
    return is_initialized_;
}

template <typename T>
T& Optional<T>::operator*() {
    return *ptr_;
}

template <typename T>
const T& Optional<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* Optional<T>::operator->() {
    return ptr_;
}

template <typename T>
const T* Optional<T>::operator->() const {
    return ptr_;
}

template <typename T>
T& Optional<T>::Value() {
    if (!is_initialized_) {
        throw BadOptionalAccess();
    }
    return *ptr_;
}

template <typename T>
const T& Optional<T>::Value() const {
    if (!is_initialized_) {
        throw BadOptionalAccess();
    }
    return *ptr_;
}

template <typename T>
void Optional<T>::Reset() {
    if (is_initialized_) {
        ptr_->~T();
        ptr_ = nullptr;
    }
    is_initialized_ = false;
}
