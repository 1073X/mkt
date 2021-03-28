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
            FATAL_ERROR("line capacity should be pow of 2", capacity);
        }
        return sizeof(H) + sizeof(T) * capacity;
    }

    auto capacity() const { return _head.mask + 1; }
    auto index() const { return _head.index; }

    auto at(uint32_t i) { return (T*)(&_head + 1) + (i & _head.mask); }

    template<typename CB>
    auto next(CB const& cb) {
        cb(at(index()));
        _head.index++;
    }

  protected:
    static ring* make(void* buf, uint32_t len, uint32_t capacity) {
        if (resolve_size(capacity) > len) {
            FATAL_ERROR("lack of space", len, "<", capacity);
        }

        auto ptr         = new (buf) ring {};
        ptr->_head.mask  = capacity - 1;
        ptr->_head.index = 0;

        return ptr;
    }

    auto head() { return &_head; }
    auto head() const { return &_head; }

  private:
    H _head;
};

}    // namespace miu::mkt
