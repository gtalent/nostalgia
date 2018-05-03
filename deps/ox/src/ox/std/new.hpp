/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/__buildinfo/defines.hpp>

#include "types.hpp"

#if defined(_MSC_VER)
#include <malloc.h>
#define ox_alloca(size) _alloca(size)
#elif !defined(OX_USE_STDLIB)
#define ox_alloca(size) __builtin_alloca(size)
#else
#include <cstdlib>
#include <alloca.h>
#define ox_alloca(size) alloca(size)
#endif


void *operator new(std::size_t, void*) noexcept;


namespace ox {

constexpr auto MallocaStackLimit = 1024;

}

#if defined(OX_USE_STDLIB)
#define ox_malloca(size, Type, ...) ox::MallocaPtr<Type>(size, new (size > MallocaStackLimit ? new uint8_t[size] : ox_alloca(size)) Type(__VA_ARGS__))
#else                          
#define ox_malloca(size, Type, ...) ox::MallocaPtr<Type>(size, ox_alloca(size))
#endif

inline constexpr void ox_freea(std::size_t size, void *ptr) {
	if constexpr(ox::defines::UseStdLib) {
		if (size > ox::MallocaStackLimit) {
			delete[] reinterpret_cast<uint8_t*>(ptr);
		}
	}
}


namespace ox {

template<typename T>
class MallocaPtr {

	private:
		std::size_t m_size = 0;
		T *m_val = nullptr;

	public:
		inline MallocaPtr() noexcept = default;

		inline MallocaPtr(MallocaPtr &&other) noexcept {
			m_size = other.m_size;
			m_val = other.m_val;
			other.m_size = 0;
		}

		inline MallocaPtr(std::size_t size, T *val) noexcept {
			m_size = size;
			m_val = val;
		}

		inline ~MallocaPtr() noexcept {
			if constexpr(ox::defines::UseStdLib) {
				if (m_size > ox::MallocaStackLimit) {
					delete[] reinterpret_cast<uint8_t*>(m_val);
				}
			}
		}

		inline const T *operator->() const noexcept {
			return reinterpret_cast<T*>(m_val);
		}

		inline T *operator->() noexcept {
			return reinterpret_cast<T*>(m_val);
		}

		inline operator const T*() const noexcept {
			return reinterpret_cast<T*>(m_val);
		}

		inline operator T*() noexcept {
			return reinterpret_cast<T*>(m_val);
		}

		inline const T &operator*() const noexcept {
			return *reinterpret_cast<T*>(m_val);
		}

		inline T &operator*() noexcept {
			return *reinterpret_cast<T*>(m_val);
		}

		inline bool operator==(const MallocaPtr<T> &other) const noexcept {
			return m_val == other.m_val && m_size == other.m_size;
		}

		inline bool operator!=(const MallocaPtr<T> &other) const noexcept {
			return m_val != other.m_val || m_size != other.m_size;
		}

};

extern template class MallocaPtr<int>;

}
