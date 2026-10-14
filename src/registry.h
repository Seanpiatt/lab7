#ifndef REGISTRY_H
#define REGISTRY_H
#include <vector>
#include <cassert>
#include <cstddef>
#include <utility>
#include <type_traits>

template <typename T>
class Registry {
    public:

        size_t store(const T& entity);

        size_t store(T&& entity);

        void remove(std::size_t id);

        T& operator[](std::size_t id);

        const T& operator[](std::size_t id) const;
            
        bool valid(std::size_t id) const;

        size_t size() const;

        void reserve(std::size_t capacity);

        std::vector<bool> mValid;
        std::vector<size_t> mFreeList;
        std::vector<T> mData;
};

template <typename T>
size_t Registry<T>::store(const T& entity) {
    if (!mFreeList.empty()) {
        std::size_t i = mFreeList.back();
        mFreeList.pop_back();
        mData[i] = entity;
        mValid[i] = true;
        return i;
    }
    mData.push_back(entity);
    mValid.push_back(true);
    return mData.size() - 1;
}

template <typename T>
size_t Registry<T>::store(T&& entity) {
    if (!mFreeList.empty()) {
        std::size_t i = mFreeList.back();
        mFreeList.pop_back();
        mData[i] = std::move(entity);
        mValid[i] = true;
        return i;
    }
    mData.push_back(std::move(entity));
    mValid.push_back(true);
    return mData.size() - 1;
}

template <typename T>
void Registry<T>::remove(std::size_t id) {
    assert(valid(id) && "removing");
    mData[id] = T{};
    mValid[id] = false;
    mFreeList.push_back(id);
}

template <typename T>
T& Registry<T>::operator[](std::size_t id) {
    return mData[id]; 
}

template <typename T>
const T& Registry<T>::operator[](std::size_t id) const {
    return mData[id]; 
}

template <typename T>
bool Registry<T>::valid(std::size_t id) const {
    return id < mValid.size() && mValid[id];
}

template <typename T>
size_t Registry<T>::size() const {
    return mData.size() - mFreeList.size(); 
}
template <typename T>
void Registry<T>::reserve(std::size_t capacity) {
    mData.reserve(capacity);
    mValid.reserve(capacity);
}
#endif  // REGISTRY_H
