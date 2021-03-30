#pragma once

#include <com/fatal_error.hpp>

#define CLZ __builtin_clz
#define CTZ __builtin_ctz

namespace miu::mkt {

template<typename H, typename T>
class ring {
  public:
    static auto resolve_size(uint32_t capacity) {
        if (!capacity || CLZ(capacity) + CTZ(capacity) != 31) {
            FATAL_ERROR("ring capacity should be pow of 2", capacity);
        }
        return sizeof(H) + sizeof(T) * capacity;
    }

    auto capacity() const { return _head.mask + 1; }
    auto index() const { return _head.index; }

    auto at(uint32_t i) { return (T*)(&_head + 1) + (i & _head.mask); }
    auto at(uint32_t i) const { return const_cast<ring*>(this)->at(i); }

  protected:
    static ring* make(void* buf, uint32_t len, uint32_t capacity) {
        if (resolve_size(capacity) > len) {
            FATAL_ERROR("lack of space to make ring", len, "<", capacity);
        }

        auto ptr        = (ring*)buf;
        ptr->_head.mask = capacity - 1;
        return ptr;
    }

    auto head() { return &_head; }
    auto head() const { return &_head; }

  private:
    H _head;
};

}    // namespace miu::mkt
