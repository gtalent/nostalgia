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
#elif OX_USE_STDLIB
#include <alloca.h>
#define ox_alloca(size) alloca(size)
#else
#define ox_alloca(size) __builtin_alloca(size)
#endif


void *operator new(std::size_t, void*) noexcept;


/**
 * @return an ox::MallocaPtr of the given type pointing to the requested size memory allocation
 */
#if defined(OX_USE_STDLIB)
#define ox_malloca(size, Type, ...) ox::MallocaPtr<Type>(size > MallocaStackLimit, new (size > MallocaStackLimit ? new uint8_t[size] : ox_alloca(size)) Type(__VA_ARGS__))
#else
#define ox_malloca(size, Type, ...) ox::MallocaPtr<Type>(false, new (ox_alloca(size)) Type(__VA_ARGS__))
#endif

namespace ox {

constexpr auto MallocaStackLimit = ox::defines::UseStdLib ? 1024 : 0;

/**
 * MallocaPtr will automatically cleanup the pointed to address upon
 * destruction if the size of the allocation is greater than MallocaStackLimit.
 */
template<typename T>
class MallocaPtr {

	private:
		bool m_onHeap = false;
		T *m_val = nullptr;

	public:
		inline MallocaPtr() noexcept = default;

		inline MallocaPtr(MallocaPtr &other) = delete;

		inline MallocaPtr(const MallocaPtr &other) = delete;

		inline MallocaPtr(MallocaPtr &&other) noexcept {
			m_onHeap = other.m_onHeap;
			m_val = other.m_val;
			other.m_onHeap = false;
			other.m_val = nullptr;
		}

		inline MallocaPtr(bool onHeap, T *val) noexcept {
			m_onHeap = onHeap;
			m_val = val;
		}

		inline ~MallocaPtr() noexcept {
			if (m_onHeap && m_val) {
				delete[] reinterpret_cast<uint8_t*>(m_val);
			}
		}

		inline const T *get() const noexcept {
			return reinterpret_cast<T*>(m_val);
		}

		inline T *get() noexcept {
			return reinterpret_cast<T*>(m_val);
		}

		inline const T &operator=(MallocaPtr &other) = delete;

		inline const T &operator=(const MallocaPtr &other) = delete;

		inline const T &operator=(MallocaPtr &&other) noexcept {
			if (m_onHeap && m_val) {
				delete[] reinterpret_cast<uint8_t*>(m_val);
			}
			m_onHeap = other.m_onHeap;
			m_val = other.m_val;
			other.m_onHeap = false;
			other.m_val = nullptr;
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
			return m_val == other.m_val && m_onHeap == other.m_onHeap;
		}

		inline bool operator!=(const MallocaPtr<T> &other) const noexcept {
			return m_val != other.m_val || m_onHeap != other.m_onHeap;
		}

};

}
