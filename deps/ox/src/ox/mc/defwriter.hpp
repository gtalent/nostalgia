/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/byteswap.hpp>
#include <ox/std/hashmap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/types.hpp>
#include <ox/std/vector.hpp>
#include <ox/trace/trace.hpp>

#include "deftypes.hpp"
#include "err.hpp"
#include "optype.hpp"
#include "types.hpp"

namespace ox {

template<typename T>
static constexpr int indirectionLevels(T) {
	return 0;
}

template<typename T>
static constexpr int indirectionLevels(T *t) {
	return 1 + indirectionLevels(*t);
}

static_assert([] {
	int i = 0;
	return indirectionLevels(i) == 0;
}(), "indirectionLevels broken: indirectionLevels(int)");

static_assert([] {
	int i = 0;
	return indirectionLevels(&i) == 1;
}(), "indirectionLevels broken: indirectionLevels(int*)");

static_assert([] {
	int i[2] = {};
	return indirectionLevels(i) == 1;
}(), "indirectionLevels broken: indirectionLevels(int[])");

static_assert([] {
	int i[2][2] = {{}};
	return indirectionLevels(i) == 2;
}(), "indirectionLevels broken: indirectionLevels(int[][])");

class MetalClawDefWriter {

	private:
		mc::TypeStore *m_typeStoreOwnerRef = nullptr;
		mc::TypeStore *m_typeStore = nullptr;
		mc::Type *m_type = nullptr;
		// indicates whether or not m_type already existed in the TypeStore
		bool m_typeAlreayExisted = false;

	public:
		explicit MetalClawDefWriter(mc::TypeStore *typeStore = nullptr);

		~MetalClawDefWriter();

		template<typename T>
		ox::Error op(const char *name, T *val, std::size_t valLen);

		template<typename T>
		ox::Error op(const char *name, ox::Vector<T> *val);

		template<std::size_t L>
		ox::Error op(const char *name, ox::BString<L> *val);

		template<typename T>
		constexpr ox::Error op(const char *name, T *val);

		constexpr void setTypeInfo(const char *name, int fields);

      constexpr OpType opType() {
			return OpType::WriteDefinition;
      }

	private:
		constexpr mc::Type *type(int8_t *val, bool *alreadyExisted);
		constexpr mc::Type *type(int16_t *val, bool *alreadyExisted);
		constexpr mc::Type *type(int32_t *val, bool *alreadyExisted);
		constexpr mc::Type *type(int64_t *val, bool *alreadyExisted);

		constexpr mc::Type *type(uint8_t *val, bool *alreadyExisted);
		constexpr mc::Type *type(uint16_t *val, bool *alreadyExisted);
		constexpr mc::Type *type(uint32_t *val, bool *alreadyExisted);
		constexpr mc::Type *type(uint64_t *val, bool *alreadyExisted);

		constexpr mc::Type *type(bool *val, bool *alreadyExisted);

		constexpr mc::Type *type(const char *val, bool *alreadyExisted);

		mc::Type *type(McStr val, bool *alreadyExisted);

		template<std::size_t L>
		constexpr mc::Type *type(ox::BString<L> *val, bool *alreadyExisted);

		template<typename T>
		mc::Type *type(T *val, bool *alreadyExisted);

		constexpr mc::Type *getPrimitive(mc::String tn, mc::PrimitiveType t, int b, bool *alreadyExisted);
};

// array handler
template<typename T>
ox::Error MetalClawDefWriter::op(const char *name, T *val, std::size_t) {
	if (m_type) {
		constexpr typename RemoveIndirection<decltype(val)>::type *p = nullptr;
		bool alreadyExisted = false;
		const auto t = type(p, &alreadyExisted);
		m_type->fieldList.push_back(mc::Field{t, name, subscriptLevels(val), !alreadyExisted});
		return OxError(0);
	}
	return OxError(1);
}

template<typename T>
ox::Error MetalClawDefWriter::op(const char *name, ox::Vector<T> *val) {
	return op(name, val->data(), val->size());
}

template<std::size_t L>
ox::Error MetalClawDefWriter::op(const char *name, ox::BString<L> *val) {
	return op(name, val->c_str());
}

template<typename T>
constexpr ox::Error MetalClawDefWriter::op(const char *name, T *val) {
	if (m_type) {
		bool alreadyExisted = false;
		const auto t = type(val, &alreadyExisted);
		m_type->fieldList.push_back(mc::Field{t, name, 0, !alreadyExisted});
		return OxError(0);
	}
	return OxError(1);
}

template<typename T>
mc::Type *MetalClawDefWriter::type(T *val, bool *alreadyExisted) {
	MetalClawDefWriter dw(m_typeStore);
	oxLogError(ioOp(&dw, val));
	*alreadyExisted = dw.m_typeAlreayExisted;
	return dw.m_type;
}

template<std::size_t L>
constexpr mc::Type *MetalClawDefWriter::type(ox::BString<L>*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:string";
	constexpr auto PT = mc::PrimitiveType::String;
	return getPrimitive(TypeName, PT, 0, alreadyExisted);
}

}
