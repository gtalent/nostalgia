/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "utility.hpp"

namespace ox {

template<typename T>
struct DefaultDelete {
	constexpr void operator()(T *p) noexcept {
		delete p;
	}
};

template<typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {

	private:
		T *m_t;

	public:
		explicit constexpr UniquePtr(T *t = nullptr) noexcept: m_t(t) {
		}

		constexpr UniquePtr(const UniquePtr&) = delete;

		template<typename U>
		constexpr UniquePtr(UniquePtr<U> &&other) noexcept {
			m_t = other.release();
		}

		~UniquePtr() {
			DefaultDelete<T>()(m_t);
		}

		constexpr T *release() noexcept {
			auto t = m_t;
			m_t = nullptr;
			return t;
		}

		[[nodiscard]]
		constexpr T *get() const noexcept {
			return m_t;
		}

		template<typename U>
		constexpr void reset(UniquePtr<U> &&other = UniquePtr()) {
			auto t = m_t;
			m_t = other.m_t;
			other.m_t = nullptr;
			DefaultDelete<T>()(t);
		}

		template<typename U>
		constexpr UniquePtr &operator=(UniquePtr<U> &&other) {
			reset(move(other));
			return *this;
		}

		constexpr T *operator->() const noexcept {
			return m_t;
		}

		constexpr T &operator*() const noexcept {
			return *m_t;
		}

		constexpr operator bool() const noexcept {
			return m_t;
		}

};

template<typename T>
constexpr bool operator==(const UniquePtr<T> &p1, const UniquePtr<T> &p2) noexcept {
	return p1.get() == p2.get();
}

template<typename T>
constexpr bool operator==(const UniquePtr<T> &p1, std::nullptr_t) noexcept {
	return p1.get();
}

template<typename T>
constexpr bool operator==(std::nullptr_t, const UniquePtr<T> &p2) noexcept {
	return p2.get();
}


template<typename T>
constexpr bool operator!=(const UniquePtr<T> &p1, const UniquePtr<T> &p2) noexcept {
	return p1.get() != p2.get();
}

template<typename T>
constexpr bool operator!=(const UniquePtr<T> &p1, std::nullptr_t) noexcept {
	return !p1.get();
}

template<typename T>
constexpr bool operator!=(std::nullptr_t, const UniquePtr<T> &p2) noexcept {
	return !p2.get();
}


template<typename T, typename ...Args>
[[nodiscard]]
constexpr auto make_unique(Args&&... args) {
	return UniquePtr(new T(args...));
}

}
