#pragma once

#include <string>
#include <list>
#include <StlBlockAllocator.h>
#include <functional>
#include <map>
#include <set>

namespace common {

typedef std::basic_string<char, std::char_traits<char>, common::StlBlockAllocator<char>> block_string;

template <typename T>
struct hash;

template <>
struct hash<block_string> : public std::__hash_base<size_t, block_string> {
    size_t operator()( const block_string& __s ) const noexcept {
        return std::_Hash_impl::hash( __s.data(), __s.length() );
    }
};

template <typename _Ty, typename _Ax = common::StlBlockAllocator<_Ty>>
class block_list : public std::list<_Ty, _Ax> {
    using std::list<_Ty, _Ax>::list;
};

template <typename _Key,
          typename _Tp,
          typename _Compare = std::less<_Key>,
          typename _Alloc = common::StlBlockAllocator<std::pair<const _Key, _Tp>>>
class block_map : public std::map<_Key, _Tp, _Compare, _Alloc> {
    using std::map<_Key, _Tp, _Compare, _Alloc>::map;
};

template <typename _Key, typename _Compare = std::less<_Key>, typename _Alloc = common::StlBlockAllocator<_Key>>
class block_set : public std::set<_Key, _Compare, _Alloc> {
    using std::set<_Key, _Compare, _Alloc>::set;
};

}  // end of namespace common
