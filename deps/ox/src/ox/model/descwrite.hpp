/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/byteswap.hpp>
#include <ox/std/bstring.hpp>
#include <ox/std/string.hpp>
#include <ox/std/trace.hpp>
#include <ox/std/types.hpp>
#include <ox/std/vector.hpp>

#include "desctypes.hpp"
#include "optype.hpp"
#include "types.hpp"

namespace ox {

namespace detail {

template<bool>
struct BoolWrapper {
};

template<typename T, typename = BoolWrapper<true>>
struct preloadable: false_type {};

template<typename T>
struct preloadable<T, BoolWrapper<T::Preloadable>> {
	static constexpr bool value = T::Preloadable;
};

}

template<typename T>
static constexpr int indirectionLevels(T) {
	return 0;
}

template<typename T>
static constexpr int indirectionLevels(T *t) {
	return 1 + indirectionLevels(*t);
}

class TypeDescWriter {

	private:
		struct NameCatcher {

			TypeName name;

			template<typename T = std::nullptr_t>
			constexpr void setTypeInfo(const char *n = T::TypeName, int = T::Fields) noexcept {
				this->name = n;
			}

			template<typename T>
			constexpr Error field(const char*, T*, std::size_t) noexcept {
				return OxError(0);
			}

			template<typename T>
			constexpr Error field(const char*, T) noexcept {
				return OxError(0);
			}

			static constexpr auto opType() {
				return OpType::WriteDefinition;
			}

		};

		TypeStore *m_typeStoreOwnerRef = nullptr;
		TypeStore *m_typeStore = nullptr;
		DescriptorType *m_type = nullptr;

	public:
		explicit TypeDescWriter(TypeStore *typeStore = nullptr);

		~TypeDescWriter();

		template<typename T>
		Error field(const char *name, T *val, std::size_t valLen);

		template<typename T>
		Error field(const char *name, T val);

		template<typename T>
		Error field(const char *name, T *val);

		template<typename T = std::nullptr_t>
		void setTypeInfo(const char *name = T::TypeName, int fields = T::Fields);

		[[nodiscard]] DescriptorType *definition() noexcept {
			return m_type;
		}

		static constexpr auto opType() {
			return OpType::WriteDefinition;
		}

	private:
		DescriptorType *type(int8_t *val, bool *alreadyExisted);
		DescriptorType *type(int16_t *val, bool *alreadyExisted);
		DescriptorType *type(int32_t *val, bool *alreadyExisted);
		DescriptorType *type(int64_t *val, bool *alreadyExisted);

		DescriptorType *type(uint8_t *val, bool *alreadyExisted);
		DescriptorType *type(uint16_t *val, bool *alreadyExisted);
		DescriptorType *type(uint32_t *val, bool *alreadyExisted);
		DescriptorType *type(uint64_t *val, bool *alreadyExisted);

		DescriptorType *type(bool *val, bool *alreadyExisted);

		DescriptorType *type(char *val, bool *alreadyExisted);

		DescriptorType *type(SerStr val, bool *alreadyExisted);

		DescriptorType *type(String *val, bool *alreadyExisted);

		template<std::size_t sz>
		DescriptorType *type(BString<sz> *val, bool *alreadyExisted);

		template<typename T>
		DescriptorType *type(T *val, bool *alreadyExisted);

		template<typename T>
		DescriptorType *type(Vector<T> *val, bool *alreadyExisted);

		template<typename T>
		DescriptorType *type(HashMap<String, T> *val, bool *alreadyExisted);

		template<typename U>
		DescriptorType *type(UnionView<U> val, bool *alreadyExisted);

		DescriptorType *getType(TypeName tn, PrimitiveType t, int b, bool *alreadyExisted);
};

// array handler
template<typename T>
Error TypeDescWriter::field(const char *name, T *val, std::size_t) {
	if (m_type) {
		constexpr typename ox::remove_pointer<decltype(val)>::type *p = nullptr;
		bool alreadyExisted = false;
		const auto t = type(p, &alreadyExisted);
		oxAssert(t != nullptr, "field(const char *name, T *val, std::size_t): Type not found or generated");
		if (t == nullptr) {
			type(p, &alreadyExisted);
		}
		m_type->fieldList.emplace_back(t, name, indirectionLevels(val), alreadyExisted ? t->typeName : "", !alreadyExisted);
		return OxError(0);
	}
	return OxError(1);
}

template<typename T>
Error TypeDescWriter::field(const char *name, T val) {
	if (m_type) {
		bool alreadyExisted = false;
		const auto t = type(val, &alreadyExisted);
		oxAssert(t != nullptr, "field(const char *name, T val): Type not found or generated");
		m_type->fieldList.emplace_back(t, name, 0, alreadyExisted ? t->typeName : "", !alreadyExisted);
		return OxError(0);
	}
	return OxError(1);
}

template<typename T>
Error TypeDescWriter::field(const char *name, T *val) {
	if (m_type) {
		bool alreadyExisted = false;
		const auto t = type(val, &alreadyExisted);
		oxAssert(t != nullptr, "field(const char *name, T *val): Type not found or generated");
		m_type->fieldList.emplace_back(t, name, 0, alreadyExisted ? t->typeName : "", !alreadyExisted);
		return OxError(0);
	}
	return OxError(1);
}

template<std::size_t sz>
DescriptorType *TypeDescWriter::type(BString<sz> *val, bool *alreadyExisted) {
	return type(SerStr(val), alreadyExisted);
}

template<typename T>
DescriptorType *TypeDescWriter::type(T *val, bool *alreadyExisted) {
	NameCatcher nc;
	oxLogError(model(&nc, val));
	if (m_typeStore->contains(nc.name)) {
		*alreadyExisted = true;
		return m_typeStore->operator[](nc.name);
	} else {
		TypeDescWriter dw(m_typeStore);
		oxLogError(model(&dw, val));
		*alreadyExisted = false;
		return dw.m_type;
	}
}

template<typename T>
DescriptorType *TypeDescWriter::type(Vector<T> *val, bool *alreadyExisted) {
	return type(val->data(), alreadyExisted);
}

template<typename T>
DescriptorType *TypeDescWriter::type(HashMap<String, T>*, bool *alreadyExisted) {
	return type(static_cast<T*>(nullptr), alreadyExisted);
}

template<typename U>
DescriptorType *TypeDescWriter::type(UnionView<U> val, bool *alreadyExisted) {
	return type(val.get(), alreadyExisted);
}

template<typename T>
void TypeDescWriter::setTypeInfo(const char *name, int) {
	auto &t = m_typeStore->operator[](name);
	if (!t) {
		t = new DescriptorType;
	}
	m_type = t;
	m_type->typeName = name;
	if (is_union_v<T>) {
		m_type->primitiveType = PrimitiveType::Union;
	} else {
		m_type->primitiveType = PrimitiveType::Struct;
	}
	m_type->preloadable = detail::preloadable<T>::value;
}

template<typename T>
Result<DescriptorType*> buildTypeDef(T *val) {
	TypeDescWriter writer;
	Error err = model(&writer, val);
	return {writer.definition(), err};
}

template<typename T>
Error writeTypeDef(uint8_t *buff, std::size_t buffLen, T *val, std::size_t *sizeOut = nullptr) {
	auto def = buildTypeDef(val);
	auto err = def.error;
	if (!err) {
		oxReturnError(writeType(buff, buffLen, def.value, sizeOut));
	}
	delete def.value;
	return err;
}

}
