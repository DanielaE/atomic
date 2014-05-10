/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2009 Helge Bahmann
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/platform.hpp
 *
 * This header defines macros for the target platform detection
 */

#ifndef BOOST_ATOMIC_DETAIL_PLATFORM_HPP_INCLUDED_
#define BOOST_ATOMIC_DETAIL_PLATFORM_HPP_INCLUDED_

#include <boost/atomic/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_ATOMIC_FORCE_FALLBACK)

#define BOOST_ATOMIC_DETAIL_PLATFORM emulated
#define BOOST_ATOMIC_EMULATED

// Intel compiler does not support __atomic* intrinsics properly, although defines them (tested with 13.0.1 and 13.1.1 on Linux)
#elif (defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 407) && !defined(BOOST_INTEL_CXX_VERSION))\
    || (defined(BOOST_CLANG) && ((__clang_major__ * 100 + __clang_minor__) >= 302))

#define BOOST_ATOMIC_DETAIL_PLATFORM gcc_atomic

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

#define BOOST_ATOMIC_DETAIL_PLATFORM gcc_x86

#elif defined(__GNUC__) && (defined(__POWERPC__) || defined(__PPC__))

#define BOOST_ATOMIC_DETAIL_PLATFORM gcc_ppc

// This list of ARM architecture versions comes from Apple's arm/arch.h header.
// I don't know how complete it is.
#elif defined(__GNUC__) &&\
    (\
        defined(__ARM_ARCH_6__)  || defined(__ARM_ARCH_6J__) ||\
        defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) ||\
        defined(__ARM_ARCH_6ZK__) ||\
        defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) ||\
        defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) ||\
        defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7S__)\
    )

#define BOOST_ATOMIC_DETAIL_PLATFORM gcc_arm

#elif defined(__GNUC__) && defined(__sparc_v9__)

#define BOOST_ATOMIC_DETAIL_PLATFORM gcc_sparc

#elif defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 401)

#define BOOST_ATOMIC_DETAIL_PLATFORM gcc_sync

#elif defined(__linux__) && defined(__arm__)

#define BOOST_ATOMIC_DETAIL_PLATFORM linux_arm

#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

#define BOOST_ATOMIC_DETAIL_PLATFORM msvc_x86

#elif defined(_MSC_VER) && _MSC_VER >= 1700 && defined(_M_ARM)

#define BOOST_ATOMIC_DETAIL_PLATFORM msvc_arm

#elif defined(BOOST_WINDOWS) || defined(_WIN32_CE)

#define BOOST_ATOMIC_DETAIL_PLATFORM windows

#else

#define BOOST_ATOMIC_DETAIL_PLATFORM emulated
#define BOOST_ATOMIC_EMULATED

#endif

#define BOOST_ATOMIC_DETAIL_HEADER(prefix) <BOOST_JOIN(prefix, BOOST_ATOMIC_DETAIL_PLATFORM).hpp>

#endif // BOOST_ATOMIC_DETAIL_PLATFORM_HPP_INCLUDED_
