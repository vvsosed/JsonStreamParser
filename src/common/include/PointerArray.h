#pragma once

#include <stdint.h>

#include <Array.h>

namespace common {
template <class T, uint32_t N>
class PointerArray : public Array<T*, N> {};
};  // namespace common
