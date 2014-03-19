#ifndef BOOST_ATOMIC_DETAIL_GCC_ATOMIC_HPP
#define BOOST_ATOMIC_DETAIL_GCC_ATOMIC_HPP

//  Copyright (c) 2013 Andrey Semashev
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <string.h>
#include <cstddef>
#include <boost/cstdint.hpp>
#include <boost/atomic/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {
namespace atomics {
namespace detail {

#if (defined(__i386__) && defined(__SSE2__)) || defined(__x86_64__)
#define BOOST_ATOMIC_X86_PAUSE() __asm__ __volatile__ ("pause\n")
#endif

#if defined(__i386__) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
#define BOOST_ATOMIC_X86_HAS_CMPXCHG8B 1
#endif

#if defined(__x86_64__) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16)
#define BOOST_ATOMIC_X86_HAS_CMPXCHG16B 1
#endif

#if defined(BOOST_ATOMIC_X86_HAS_CMPXCHG16B) && defined(__clang__)
// Worraround for bug: http://llvm.org/bugs/show_bug.cgi?id=19149
// Clang 3.4 does not implement 128-bit __atomic* intrinsics even though it defines __GCC_HAVE_SYNC_COMPARE_AND_SWAP_16
#define BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS
#endif

BOOST_FORCEINLINE BOOST_CONSTEXPR int convert_memory_order_to_gcc(memory_order order) BOOST_NOEXCEPT
{
    return (order == memory_order_relaxed ? __ATOMIC_RELAXED : (order == memory_order_consume ? __ATOMIC_CONSUME :
        (order == memory_order_acquire ? __ATOMIC_ACQUIRE : (order == memory_order_release ? __ATOMIC_RELEASE :
        (order == memory_order_acq_rel ? __ATOMIC_ACQ_REL : __ATOMIC_SEQ_CST)))));
}

} // namespace detail
} // namespace atomics

#if __GCC_ATOMIC_BOOL_LOCK_FREE == 2

class atomic_flag
{
private:
    bool v_;

public:
    BOOST_CONSTEXPR atomic_flag(void) BOOST_NOEXCEPT : v_(false) {}

    bool test_and_set(memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_test_and_set(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    void clear(memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_clear(const_cast<bool*>(&v_), atomics::detail::convert_memory_order_to_gcc(order));
    }

    BOOST_DELETED_FUNCTION(atomic_flag(atomic_flag const&))
    BOOST_DELETED_FUNCTION(atomic_flag& operator= (atomic_flag const&))
};

#define BOOST_ATOMIC_FLAG_LOCK_FREE 2

#endif // __GCC_ATOMIC_BOOL_LOCK_FREE == 2

} // namespace boost

#include <boost/atomic/detail/base.hpp>

#if !defined(BOOST_ATOMIC_FORCE_FALLBACK)

#if __GCC_ATOMIC_CHAR_LOCK_FREE == 2
#define BOOST_ATOMIC_CHAR_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_CHAR16_T_LOCK_FREE == 2
#define BOOST_ATOMIC_CHAR16_T_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_CHAR32_T_LOCK_FREE == 2
#define BOOST_ATOMIC_CHAR32_T_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_WCHAR_T_LOCK_FREE == 2
#define BOOST_ATOMIC_WCHAR_T_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_SHORT_LOCK_FREE == 2
#define BOOST_ATOMIC_SHORT_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_INT_LOCK_FREE == 2
#define BOOST_ATOMIC_INT_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_LONG_LOCK_FREE == 2
#define BOOST_ATOMIC_LONG_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_LLONG_LOCK_FREE == 2
#define BOOST_ATOMIC_LLONG_LOCK_FREE 2
#endif
#if defined(BOOST_ATOMIC_X86_HAS_CMPXCHG16B) && (defined(BOOST_HAS_INT128) || !defined(BOOST_NO_ALIGNMENT))
#define BOOST_ATOMIC_INT128_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_POINTER_LOCK_FREE == 2
#define BOOST_ATOMIC_POINTER_LOCK_FREE 2
#endif
#if __GCC_ATOMIC_BOOL_LOCK_FREE == 2
#define BOOST_ATOMIC_BOOL_LOCK_FREE 2
#endif

namespace boost {

#define BOOST_ATOMIC_THREAD_FENCE 2
BOOST_FORCEINLINE void atomic_thread_fence(memory_order order)
{
    __atomic_thread_fence(atomics::detail::convert_memory_order_to_gcc(order));
}

#define BOOST_ATOMIC_SIGNAL_FENCE 2
BOOST_FORCEINLINE void atomic_signal_fence(memory_order order)
{
    __atomic_signal_fence(atomics::detail::convert_memory_order_to_gcc(order));
}

namespace atomics {
namespace detail {

#if defined(BOOST_ATOMIC_CHAR_LOCK_FREE) && BOOST_ATOMIC_CHAR_LOCK_FREE > 0

template<typename T, bool Sign>
class base_atomic<T, int, 1, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef T difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    value_type fetch_and(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_and(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_or(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_or(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_xor(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_xor(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_INTEGRAL_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

template<typename T, bool Sign>
class base_atomic<T, void, 1, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef uint8_t storage_type;

protected:
    typedef value_type const& value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type const& v) BOOST_NOEXCEPT :
        v_(reinterpret_cast<storage_type const&>(v))
    {
    }

    void store(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store(&v_, (storage_type*)&v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        value_type v;
        __atomic_load(&v_, (storage_type*)&v, atomics::detail::convert_memory_order_to_gcc(order));
        return v;
    }

    value_type exchange(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        value_type r;
        __atomic_exchange(&v_, (storage_type*)&v, (storage_type*)&r, atomics::detail::convert_memory_order_to_gcc(order));
        return r;
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange(&v_, (storage_type*)&expected, (storage_type*)&desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type & expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange(&v_, (storage_type*)&expected, (storage_type*)&desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_BASE_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    storage_type v_;
};

#endif // defined(BOOST_ATOMIC_CHAR_LOCK_FREE) && BOOST_ATOMIC_CHAR_LOCK_FREE > 0

#if defined(BOOST_ATOMIC_SHORT_LOCK_FREE) && BOOST_ATOMIC_SHORT_LOCK_FREE > 0

template<typename T, bool Sign>
class base_atomic<T, int, 2, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef T difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    value_type fetch_and(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_and(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_or(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_or(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_xor(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_xor(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_INTEGRAL_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

template<typename T, bool Sign>
class base_atomic<T, void, 2, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef uint16_t storage_type;

protected:
    typedef value_type const& value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type const& v) BOOST_NOEXCEPT :
        v_(reinterpret_cast<storage_type const&>(v))
    {
    }

    void store(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store(&v_, (storage_type*)&v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        value_type v;
        __atomic_load(&v_, (storage_type*)&v, atomics::detail::convert_memory_order_to_gcc(order));
        return v;
    }

    value_type exchange(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        value_type r;
        __atomic_exchange(&v_, (storage_type*)&v, (storage_type*)&r, atomics::detail::convert_memory_order_to_gcc(order));
        return r;
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange(&v_, (storage_type*)&expected, (storage_type*)&desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type & expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange(&v_, (storage_type*)&expected, (storage_type*)&desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_BASE_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    storage_type v_;
};

#endif // defined(BOOST_ATOMIC_SHORT_LOCK_FREE) && BOOST_ATOMIC_SHORT_LOCK_FREE > 0

#if defined(BOOST_ATOMIC_INT_LOCK_FREE) && BOOST_ATOMIC_INT_LOCK_FREE > 0

template<typename T, bool Sign>
class base_atomic<T, int, 4, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef T difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    value_type fetch_and(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_and(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_or(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_or(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_xor(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_xor(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_INTEGRAL_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

template<typename T, bool Sign>
class base_atomic<T, void, 4, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef uint32_t storage_type;

protected:
    typedef value_type const& value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    explicit base_atomic(value_type const& v) BOOST_NOEXCEPT : v_(0)
    {
        memcpy(&v_, &v, sizeof(value_type));
    }

    void store(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        storage_type tmp = 0;
        memcpy(&tmp, &v, sizeof(value_type));
        __atomic_store_n(&v_, tmp, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        storage_type tmp = __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
        value_type v;
        memcpy(&v, &tmp, sizeof(value_type));
        return v;
    }

    value_type exchange(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        storage_type tmp = 0;
        memcpy(&tmp, &v, sizeof(value_type));
        tmp = __atomic_exchange_n(&v_, tmp, atomics::detail::convert_memory_order_to_gcc(order));
        value_type res;
        memcpy(&res, &tmp, sizeof(value_type));
        return res;
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        storage_type expected_s = 0, desired_s = 0;
        memcpy(&expected_s, &expected, sizeof(value_type));
        memcpy(&desired_s, &desired, sizeof(value_type));
        const bool success = __atomic_compare_exchange_n(&v_, &expected_s, desired_s, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
        memcpy(&expected, &expected_s, sizeof(value_type));
        return success;
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        storage_type expected_s = 0, desired_s = 0;
        memcpy(&expected_s, &expected, sizeof(value_type));
        memcpy(&desired_s, &desired, sizeof(value_type));
        const bool success = __atomic_compare_exchange_n(&v_, &expected_s, desired_s, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
        memcpy(&expected, &expected_s, sizeof(value_type));
        return success;
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_BASE_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    storage_type v_;
};

#endif // defined(BOOST_ATOMIC_INT_LOCK_FREE) && BOOST_ATOMIC_INT_LOCK_FREE > 0

#if defined(BOOST_ATOMIC_LLONG_LOCK_FREE) && BOOST_ATOMIC_LLONG_LOCK_FREE > 0

template<typename T, bool Sign>
class base_atomic<T, int, 8, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef T difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    value_type fetch_and(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_and(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_or(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_or(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_xor(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_xor(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_INTEGRAL_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

template<typename T, bool Sign>
class base_atomic<T, void, 8, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef uint64_t storage_type;

protected:
    typedef value_type const& value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    explicit base_atomic(value_type const& v) BOOST_NOEXCEPT : v_(0)
    {
        memcpy(&v_, &v, sizeof(value_type));
    }

    void store(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        storage_type tmp = 0;
        memcpy(&tmp, &v, sizeof(value_type));
        __atomic_store_n(&v_, tmp, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        storage_type tmp = __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
        value_type v;
        memcpy(&v, &tmp, sizeof(value_type));
        return v;
    }

    value_type exchange(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        storage_type tmp = 0;
        memcpy(&tmp, &v, sizeof(value_type));
        tmp = __atomic_exchange_n(&v_, tmp, atomics::detail::convert_memory_order_to_gcc(order));
        value_type res;
        memcpy(&res, &tmp, sizeof(value_type));
        return res;
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        storage_type expected_s = 0, desired_s = 0;
        memcpy(&expected_s, &expected, sizeof(value_type));
        memcpy(&desired_s, &desired, sizeof(value_type));
        const bool success = __atomic_compare_exchange_n(&v_, &expected_s, desired_s, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
        memcpy(&expected, &expected_s, sizeof(value_type));
        return success;
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        storage_type expected_s = 0, desired_s = 0;
        memcpy(&expected_s, &expected, sizeof(value_type));
        memcpy(&desired_s, &desired, sizeof(value_type));
        const bool success = __atomic_compare_exchange_n(&v_, &expected_s, desired_s, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
        memcpy(&expected, &expected_s, sizeof(value_type));
        return success;
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_BASE_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    storage_type v_;
};

#endif // defined(BOOST_ATOMIC_LLONG_LOCK_FREE) && BOOST_ATOMIC_LLONG_LOCK_FREE > 0

#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0 && !defined(BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS)

template<typename T, bool Sign>
class base_atomic<T, int, 16, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef T difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    value_type fetch_and(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_and(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_or(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_or(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_xor(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_xor(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_INTEGRAL_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

#if defined(BOOST_HAS_INT128)

typedef boost::uint128_type storage128_type;

#else // defined(BOOST_HAS_INT128)

struct BOOST_ALIGNMENT(16) storage128_type
{
    uint64_t data[2];
};

inline bool operator== (storage128_type const& left, storage128_type const& right)
{
    return left.data[0] == right.data[0] && left.data[1] == right.data[1];
}
inline bool operator!= (storage128_type const& left, storage128_type const& right)
{
    return !(left == right);
}

#endif // defined(BOOST_HAS_INT128)

template<typename T, bool Sign>
class base_atomic<T, void, 16, Sign>
{
private:
    typedef base_atomic this_type;
    typedef T value_type;
    typedef storage128_type storage_type;

protected:
    typedef value_type const& value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    explicit base_atomic(value_type const& v) BOOST_NOEXCEPT
    {
        memset(&v_, 0, sizeof(v_));
        memcpy(&v_, &v, sizeof(value_type));
    }

    void store(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        storage_type tmp;
        memset(&tmp, 0, sizeof(tmp));
        memcpy(&tmp, &v, sizeof(value_type));
        __atomic_store_n(&v_, tmp, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        storage_type tmp = __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
        value_type v;
        memcpy(&v, &tmp, sizeof(value_type));
        return v;
    }

    value_type exchange(value_type const& v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        storage_type tmp;
        memset(&tmp, 0, sizeof(tmp));
        memcpy(&tmp, &v, sizeof(value_type));
        tmp = __atomic_exchange_n(&v_, tmp, atomics::detail::convert_memory_order_to_gcc(order));
        value_type res;
        memcpy(&res, &tmp, sizeof(value_type));
        return res;
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        storage_type expected_s, desired_s;
        memset(&expected_s, 0, sizeof(expected_s));
        memset(&desired_s, 0, sizeof(desired_s));
        memcpy(&expected_s, &expected, sizeof(value_type));
        memcpy(&desired_s, &desired, sizeof(value_type));
        const bool success = __atomic_compare_exchange_n(&v_, &expected_s, desired_s, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
        memcpy(&expected, &expected_s, sizeof(value_type));
        return success;
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type const& desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        storage_type expected_s, desired_s;
        memset(&expected_s, 0, sizeof(expected_s));
        memset(&desired_s, 0, sizeof(desired_s));
        memcpy(&expected_s, &expected, sizeof(value_type));
        memcpy(&desired_s, &desired, sizeof(value_type));
        const bool success = __atomic_compare_exchange_n(&v_, &expected_s, desired_s, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
        memcpy(&expected, &expected_s, sizeof(value_type));
        return success;
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_BASE_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    storage_type v_;
};

#endif // defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0 && !defined(BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS)


/* pointers */

#if defined(BOOST_ATOMIC_POINTER_LOCK_FREE) && BOOST_ATOMIC_POINTER_LOCK_FREE > 0

template<typename T, bool Sign>
class base_atomic<T*, void*, sizeof(void*), Sign>
{
private:
    typedef base_atomic this_type;
    typedef T* value_type;
    typedef std::ptrdiff_t difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v * sizeof(T), atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v * sizeof(T), atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_POINTER_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

template<bool Sign>
class base_atomic<void*, void*, sizeof(void*), Sign>
{
private:
    typedef base_atomic this_type;
    typedef void* value_type;
    typedef std::ptrdiff_t difference_type;

protected:
    typedef value_type value_arg_type;

public:
    BOOST_DEFAULTED_FUNCTION(base_atomic(void), {})
    BOOST_CONSTEXPR explicit base_atomic(value_type v) BOOST_NOEXCEPT : v_(v) {}

    void store(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        __atomic_store_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type load(memory_order order = memory_order_seq_cst) const volatile BOOST_NOEXCEPT
    {
        return __atomic_load_n(&v_, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_add(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_add(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type fetch_sub(difference_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_fetch_sub(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    value_type exchange(value_type v, memory_order order = memory_order_seq_cst) volatile BOOST_NOEXCEPT
    {
        return __atomic_exchange_n(&v_, v, atomics::detail::convert_memory_order_to_gcc(order));
    }

    bool compare_exchange_strong(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, false,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool compare_exchange_weak(
        value_type& expected,
        value_type desired,
        memory_order success_order,
        memory_order failure_order) volatile BOOST_NOEXCEPT
    {
        return __atomic_compare_exchange_n(&v_, &expected, desired, true,
            atomics::detail::convert_memory_order_to_gcc(success_order),
            atomics::detail::convert_memory_order_to_gcc(failure_order));
    }

    bool is_lock_free(void) const volatile BOOST_NOEXCEPT
    {
        return __atomic_is_lock_free(sizeof(v_), &v_);
    }

    BOOST_ATOMIC_DECLARE_VOID_POINTER_OPERATORS

    BOOST_DELETED_FUNCTION(base_atomic(base_atomic const&))
    BOOST_DELETED_FUNCTION(base_atomic& operator=(base_atomic const&))

private:
    value_type v_;
};

#endif // defined(BOOST_ATOMIC_POINTER_LOCK_FREE) && BOOST_ATOMIC_POINTER_LOCK_FREE > 0

#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0 && defined(BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS)

inline void platform_fence_before(memory_order order)
{
    switch(order)
    {
    case memory_order_relaxed:
    case memory_order_acquire:
    case memory_order_consume:
        break;
    case memory_order_release:
    case memory_order_acq_rel:
        __asm__ __volatile__ ("" ::: "memory");
        /* release */
        break;
    case memory_order_seq_cst:
        __asm__ __volatile__ ("" ::: "memory");
        /* seq */
        break;
    default:;
    }
}

inline void platform_fence_after(memory_order order)
{
    switch(order)
    {
    case memory_order_relaxed:
    case memory_order_release:
        break;
    case memory_order_acquire:
    case memory_order_acq_rel:
        __asm__ __volatile__ ("" ::: "memory");
        /* acquire */
        break;
    case memory_order_consume:
        /* consume */
        break;
    case memory_order_seq_cst:
        __asm__ __volatile__ ("" ::: "memory");
        /* seq */
        break;
    default:;
    }
}

inline void platform_fence_after_load(memory_order order)
{
    switch(order)
    {
    case memory_order_relaxed:
    case memory_order_release:
        break;
    case memory_order_acquire:
    case memory_order_acq_rel:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    case memory_order_consume:
        break;
    case memory_order_seq_cst:
        __asm__ __volatile__ ("" ::: "memory");
        break;
    default:;
    }
}

inline void platform_fence_before_store(memory_order order)
{
    switch(order)
    {
    case memory_order_relaxed:
    case memory_order_acquire:
    case memory_order_consume:
        break;
    case memory_order_release:
    case memory_order_acq_rel:
        __asm__ __volatile__ ("" ::: "memory");
        /* release */
        break;
    case memory_order_seq_cst:
        __asm__ __volatile__ ("" ::: "memory");
        /* seq */
        break;
    default:;
    }
}

inline void platform_fence_after_store(memory_order order)
{
    switch(order)
    {
    case memory_order_relaxed:
    case memory_order_release:
        break;
    case memory_order_acquire:
    case memory_order_acq_rel:
        __asm__ __volatile__ ("" ::: "memory");
        /* acquire */
        break;
    case memory_order_consume:
        /* consume */
        break;
    case memory_order_seq_cst:
        __asm__ __volatile__ ("" ::: "memory");
        /* seq */
        break;
    default:;
    }
}

template<typename T>
inline bool platform_cmpxchg128_strong(T& expected, T desired, volatile T* ptr) BOOST_NOEXCEPT
{
    T old_expected = expected;
    expected = __sync_val_compare_and_swap(ptr, old_expected, desired);
    return expected == old_expected;
}

template<typename T>
inline void platform_store128(T value, volatile T* ptr) BOOST_NOEXCEPT
{
    uint64_t const* p_value = (uint64_t const*)&value;
    __asm__ __volatile__
    (
        "movq 0(%[dest]), %%rax\n\t"
        "movq 8(%[dest]), %%rdx\n\t"
        ".align 16\n\t"
        "1: lock; cmpxchg16b 0(%[dest])\n\t"
        "jne 1b"
        :
        : "b" (p_value[0]), "c" (p_value[1]), [dest] "r" (ptr)
        : "memory", "cc", "rax", "rdx"
    );
}

template<typename T>
inline T platform_load128(const volatile T* ptr) BOOST_NOEXCEPT
{
    T value = T();
    return __sync_val_compare_and_swap(ptr, value, value);
}

#endif // defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0 && defined(BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS)

} // namespace detail
} // namespace atomics
} // namespace boost

#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0 && defined(BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS)
#undef BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS
#include <boost/atomic/detail/cas128strong.hpp>
#endif // defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0 && defined(BOOST_ATOMIC_X86_NO_GCC_128_BIT_ATOMIC_INTRINSICS)

#endif // !defined(BOOST_ATOMIC_FORCE_FALLBACK)

#endif // BOOST_ATOMIC_DETAIL_GCC_ATOMIC_HPP
