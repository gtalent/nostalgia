/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>
#include <ox/std/types.hpp>
#include <ox/std/utility.hpp>

#include "types.hpp"

namespace ox {

namespace detail {

template<std::size_t size>
class MemberList {

	private:
		std::size_t m_i = 0;

	public:
		void *vars[size];

		template<typename T>
		constexpr Error field(const char*, T *v) noexcept {
			vars[m_i++] = static_cast<void*>(v);
			return OxError(0);
		}

		template<typename T>
		constexpr Error field(const char*, T *v, int) noexcept {
			vars[m_i++] = static_cast<void*>(v);
			return OxError(0);
		}

		template<typename U>
		constexpr Error field(const char*, UnionView<U> u) noexcept {
			vars[m_i++] = static_cast<void*>(u.get());
			return OxError(0);
		}

		constexpr Error field(const char *name, SerStr s) noexcept {
			return field(name, s.target());
		}

		template<typename T = void>
		constexpr void setTypeInfo(const char* = T::TypeName, int = T::Fields) noexcept {
		}

		[[nodiscard]]
		static constexpr auto opType() noexcept {
			return "GetMembers";
		}

};

template<std::size_t size>
class Copier {

	private:
		std::size_t m_i = 0;
		MemberList<size> *m_dst = nullptr;

	public:
		constexpr Copier(MemberList<size> *dst) noexcept: m_dst(dst) {
		}

		template<typename FT>
		constexpr Error field(const char *name, const FT *v) noexcept {
			if constexpr(isVector_v<FT>) {
				return field(name, v->data(), v->size());
			} else {
				auto &src = *v;
				auto &dst = *cbit_cast<FT*>(m_dst->vars[m_i]);
				dst = src;
				++m_i;
				return OxError(0);
			}
		}

		template<typename FT>
		constexpr Error field(const char*, const FT *list, int elements) {
			for (auto i = 0l; i < elements; ++i) {
				auto &src = list[i];
				auto &dst = cbit_cast<FT*>(m_dst->vars[m_i])[i];
				dst = src;
			}
			++m_i;
			return OxError(0);
		}

		template<typename U>
		constexpr Error field(const char*, UnionView<U> u) {
			auto &dst = *cbit_cast<U*>(m_dst->vars[m_i]);
			auto &src = *u.get();
			dst = src;
			++m_i;
			return OxError(0);
		}

		constexpr Error field(const char *name, SerStr s) {
			return field(name, s.target());
		}

		template<typename T = void>
		constexpr void setTypeInfo(const char* = T::TypeName, int = T::Fields) noexcept {
		}

		[[nodiscard]]
		static constexpr auto opType() noexcept {
			return "Copy";
		}

};

template<std::size_t size>
class Mover {

	private:
		std::size_t m_i = 0;
		MemberList<size> *m_dst = nullptr;

	public:
		constexpr Mover(MemberList<size> *dst) noexcept: m_dst(dst) {
		}

		template<typename FT>
		constexpr Error field(const char *name, FT *v) noexcept {
			if constexpr(isVector_v<FT>) {
				return field(name, v->data(), v->size());
			} else {
				auto &src = *v;
				auto &dst = *cbit_cast<FT*>(m_dst->vars[m_i]);
				dst = move(src);
				src = FT{};
				++m_i;
				return OxError(0);
			}
		}

		template<typename FT>
		constexpr Error field(const char*, FT *list, int elements) noexcept {
			for (auto i = 0l; i < elements; ++i) {
				auto &src = list[i];
				auto &dst = cbit_cast<FT*>(m_dst->vars[m_i])[i];
				dst = move(src);
				src = FT{};
			}
			++m_i;
			return OxError(0);
		}

		template<typename U>
		constexpr Error field(const char*, UnionView<U> u) noexcept {
			auto &dst = *cbit_cast<U*>(m_dst->vars[m_i]);
			auto &src = *u.get();
			dst = move(src);
			++m_i;
			return OxError(0);
		}

		constexpr Error field(const char *name, SerStr s) noexcept {
			return field(name, s.target());
		}

		template<typename T = void>
		constexpr void setTypeInfo(const char* = T::TypeName, int = T::Fields) noexcept {
		}

		[[nodiscard]]
		static constexpr auto opType() noexcept {
			return "Copy";
		}

};

template<std::size_t size>
class Equals {

	private:
		std::size_t m_i = 0;
		MemberList<size> *m_other = nullptr;

	public:
		bool value = false;

		constexpr Equals(MemberList<size> *other) noexcept: m_other(other) {
		}

		template<typename FT>
		constexpr Error field(const char*, const FT *v) noexcept {
			const auto &src = *v;
			const auto &dst = *cbit_cast<FT*>(m_other->vars[m_i]);
			++m_i;
			if (dst == src) {
				return OxError(0);
			} else {
				this->value = false;
				return OxError(1);
			}
		}

		template<typename FT>
		constexpr Error field(const char*, const FT *list, int elements) noexcept {
			for (auto i = 0l; i < elements; ++i) {
				const auto &src = list[i];
				const auto &dst = cbit_cast<FT*>(m_other->vars[m_i])[i];
				if (!(dst == src)) {
					this->value = false;
					return OxError(1);
				}
			}
			++m_i;
			return OxError(0);
		}

		template<typename U>
		constexpr Error field(const char*, UnionView<U> u) noexcept {
			const auto &dst = *cbit_cast<U*>(m_other->vars[m_i]);
			const auto &src = *u.get();
			++m_i;
			if (dst == src) {
				return OxError(0);
			} else {
				this->value = false;
				return OxError(1);
			}
		}

		constexpr Error field(const char*, SerStr s) noexcept {
			const auto a = s.c_str();
			const auto b = *cbit_cast<const char**>(m_other->vars[m_i]);
			++m_i;
			if (a && b && ox_strcmp(a, b) == 0) {
				return OxError(0);
			} else {
				this->value = false;
				return OxError(1);
			}
		}

		template<typename T = void>
		constexpr void setTypeInfo(const char* = T::TypeName, int = T::Fields) noexcept {
		}

		[[nodiscard]]
		static constexpr auto opType() noexcept {
			return "Copy";
		}

};

}

template<typename T>
constexpr void moveModel(T *dst, T *src) noexcept {
	constexpr auto size = T::Fields;
	detail::MemberList<size> dstFields;
	detail::Mover<size> mover(&dstFields);
	oxIgnoreError(model(&dstFields, dst));
	oxIgnoreError(model(&mover, src));
}

template<typename T>
constexpr void copyModel(T *dst, const T *src) noexcept {
	constexpr auto size = T::Fields;
	detail::MemberList<size> dstFields;
	detail::Copier<size> copier(&dstFields);
	oxIgnoreError(model(&dstFields, dst));
	oxIgnoreError(model(&copier, src));
}

template<typename T>
[[nodiscard]]
constexpr bool equalsModel(T *a, T *b) noexcept {
	constexpr auto size = T::Fields;
	detail::MemberList<size> aFields;
	detail::Equals<size> equals(&aFields);
	oxIgnoreError(model(&aFields, a));
	oxIgnoreError(model(&equals, b));
	return equals.value;
}

}
