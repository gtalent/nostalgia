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
#include <ox/std/trace.hpp>
#include <ox/std/types.hpp>
#include <ox/std/vector.hpp>

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

		template<typename T>
		ox::Error op(const char *name, T *val);

		void setTypeInfo(const char *name, int fields);

		[[nodiscard]] mc::Type *definition() noexcept {
			return m_type;
		}

      static constexpr OpType opType() {
			return OpType::WriteDefinition;
      }

	private:
		mc::Type *type(int8_t *val, bool *alreadyExisted);
		mc::Type *type(int16_t *val, bool *alreadyExisted);
		mc::Type *type(int32_t *val, bool *alreadyExisted);
		mc::Type *type(int64_t *val, bool *alreadyExisted);

		mc::Type *type(uint8_t *val, bool *alreadyExisted);
		mc::Type *type(uint16_t *val, bool *alreadyExisted);
		mc::Type *type(uint32_t *val, bool *alreadyExisted);
		mc::Type *type(uint64_t *val, bool *alreadyExisted);

		mc::Type *type(bool *val, bool *alreadyExisted);

		mc::Type *type(McStr val, bool *alreadyExisted);

		template<typename T>
		mc::Type *type(T *val, bool *alreadyExisted);

		mc::Type *getType(mc::TypeName tn, mc::PrimitiveType t, int b, bool *alreadyExisted);
};

// array handler
template<typename T>
ox::Error MetalClawDefWriter::op(const char *name, T *val, std::size_t) {
	if (m_type) {
		constexpr typename RemoveIndirection<decltype(val)>::type *p = nullptr;
		bool alreadyExisted = false;
		const auto t = type(p, &alreadyExisted);
		m_type->fieldList.push_back(mc::Field{t, name, indirectionLevels(val), alreadyExisted ? t->typeName : "", !alreadyExisted});
		return OxError(0);
	}
	return OxError(1);
}

template<typename T>
ox::Error MetalClawDefWriter::op(const char *name, ox::Vector<T> *val) {
	return op(name, val->data(), val->size());
}

template<typename T>
ox::Error MetalClawDefWriter::op(const char *name, T *val) {
	if (m_type) {
		bool alreadyExisted = false;
		const auto t = type(val, &alreadyExisted);
		m_type->fieldList.push_back(mc::Field{t, name, 0, alreadyExisted ? t->typeName : "", !alreadyExisted});
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

template<typename T>
[[nodiscard]] ValErr<mc::Type*> writeMCDef(T *val) {
	MetalClawDefWriter writer;
	ox::Error err = ioOp(&writer, val);
	return {writer.definition(), err};
}

}
