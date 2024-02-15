#ifndef SHARED_PTR
#define SHARED_PTR

template <typename T>
class SharedPtr
{
private:
    T *_p;
    long *cnt;

public:
    SharedPtr() {
        _p = nullptr;
        cnt = nullptr;
    } 
    explicit SharedPtr(T *pointer) {
        _p = pointer;
        if (_p != nullptr)
            cnt = new long(1);
        else
            cnt = nullptr;
    }
    SharedPtr(const SharedPtr<T> &other) {
        _p = other._p;
        cnt = other.cnt;
        if (cnt != nullptr)
            (*cnt)++;
    }
    ~SharedPtr() {
        if (_p != nullptr && cnt != nullptr)
        {
            (*cnt)--;
            if ((*cnt) == 0)
            {
                delete cnt;
                delete _p;
            }
        }
        _p = nullptr;
        cnt = nullptr;
    }

    SharedPtr &operator=(const SharedPtr<T> &other) {
        if (&other == this) // other == *this :  WA
            return *this;
        if (_p != nullptr && cnt != nullptr)
        {
            (*cnt)--;
            if ((*cnt) == 0)
            {
                delete cnt;
                delete _p;
            }
        }
        _p = other._p;
        cnt = other.cnt;
        if (cnt != nullptr)
            (*cnt)++;
        return *this;
    }
    //(a=1)=2

    operator bool() const {
        if (_p == nullptr)
            return false;
        else
            return true;
    }
    T &operator*() const {
        return *_p;
    }
    T *operator->() const {
        return this->_p;
    }

    T *get() const noexcept {
        return this->_p;
    }
    void reset() {
        if (_p != nullptr && cnt != nullptr)
        {
            (*cnt)--;
            if ((*cnt) == 0) {
                delete cnt;
                delete _p;
            }
        }
        _p = nullptr;
        cnt = nullptr;
    }
    void reset(T *new_pointer) {
        if (_p == new_pointer)
            return;
        if (_p != nullptr && cnt != nullptr)
        {
            (*cnt)--;
            if ((*cnt) == 0) {
                delete cnt;
                delete _p;
            }
        }
        _p = new T(*new_pointer);
        delete new_pointer;
        if (_p != nullptr)
            cnt = new long(1);
        else
            cnt = nullptr;
    }
    long use_count() const {
        if (_p == nullptr)
            return 0;
        if (cnt == nullptr)
            return 0;
        return *cnt;
    }
};

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args &&...args) {
    return SharedPtr<T>(new T(std::forward(args...)));
}

#endif // SHARED_PTR