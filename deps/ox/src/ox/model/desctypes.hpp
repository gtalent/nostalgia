/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/bit.hpp>
#include <ox/std/error.hpp>
#include <ox/std/hashmap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

#include "types.hpp"

namespace ox {

using FieldName = String;
using TypeName = String;

enum class PrimitiveType: uint8_t {
	UnsignedInteger = 0,
	SignedInteger = 1,
	Bool = 2,
	// Float = 3, reserved, but not implemented
	String = 4,
	Struct = 5,
	Union = 6,
};

struct DescriptorField {
	// order of fields matters

	static constexpr auto TypeVersion = 1;

	// only serialize type name if type has already been serialized
	struct DescriptorType *type = nullptr;
	String fieldName;
	int subscriptLevels = 0;

	// do not serialize the following
	TypeName typeName; // gives reference to type for lookup if type is null
	bool ownsType = false;

	constexpr DescriptorField() noexcept = default;

	/**
	 * Allow for explicit copying.
	 */
	constexpr explicit DescriptorField(const DescriptorField &other) noexcept {
		type = other.type;
		fieldName = other.fieldName;
		subscriptLevels = other.subscriptLevels;
		typeName = other.typeName;
		ownsType = false; // is copy, only owns type if move
	}

	constexpr DescriptorField(DescriptorType *type, const String &fieldName, int subscriptLevels, const TypeName &typeName, bool ownsType) noexcept {
		this->type = type;
		this->fieldName = fieldName;
		this->subscriptLevels = subscriptLevels;
		this->typeName = typeName;
		this->ownsType = ownsType;
	}

	constexpr DescriptorField(DescriptorField &&other) noexcept {
		type = other.type;
		fieldName = other.fieldName;
		subscriptLevels = other.subscriptLevels;
		typeName = other.typeName;
		ownsType = other.ownsType;

		other.type = {};
		other.fieldName = "";
		other.subscriptLevels = {};
		other.typeName = "";
		other.ownsType = {};
	}

	~DescriptorField();

	const DescriptorField &operator=(DescriptorField &&other) noexcept {
		type = other.type;
		fieldName = other.fieldName;
		subscriptLevels = other.subscriptLevels;
		typeName = other.typeName;
		ownsType = other.ownsType;

		other.type = {};
		other.fieldName = "";
		other.subscriptLevels = {};
		other.typeName = "";
		other.ownsType = {};

		return *this;
	}

};

using FieldList = Vector<DescriptorField>;

struct DescriptorType {
	static constexpr auto TypeVersion = 1;
	TypeName typeName;
	PrimitiveType primitiveType;
	// fieldList only applies to structs
	FieldList fieldList;
	// - number of bytes for integer and float types
	// - number of fields for structs and lists
	int64_t length = 0;
	bool preloadable = false;

	DescriptorType() = default;

	DescriptorType(const TypeName &tn, PrimitiveType t, int b): typeName(tn), primitiveType(t), length(b) {
	}

	DescriptorType(const TypeName &tn, PrimitiveType t, FieldList fl): typeName(tn), primitiveType(t), fieldList(fl) {
	}
};


template<typename T>
constexpr Error model(T *io, DescriptorType *type) noexcept {
	io->template setTypeInfo<T>("net.drinkingtea.ox.DescriptorType", 5);
	oxReturnError(io->field("typeName", &type->typeName));
	oxReturnError(io->field("primitiveType", bit_cast<uint8_t*>(&type->primitiveType)));
	oxReturnError(io->field("fieldList", &type->fieldList));
	oxReturnError(io->field("length", &type->length));
	oxReturnError(io->field("preloadable", &type->preloadable));
	return OxError(0);
}

template<typename T>
Error modelWrite(T *io, DescriptorField *field) noexcept {
	io->setTypeInfo("ox::DescriptorField", 4);
	if (field->ownsType) {
		BString<2> empty = "";
		oxReturnError(io->field("typeName", SerStr(&empty)));
		oxReturnError(io->field("type", field->type));
	} else {
		oxReturnError(io->field("typeName", &field->type->typeName));
		oxReturnError(io->field("type", static_cast<decltype(field->type)>(nullptr)));
	}
	oxReturnError(io->field("fieldName", &field->fieldName));
	// defaultValue is unused now, but leave placeholder for backwards compatibility
	int DefaultValue = 0;
	oxReturnError(io->field("defaultValue", &DefaultValue));
	return OxError(0);
}

template<typename T>
Error modelRead(T *io, DescriptorField *field) noexcept {
	auto &typeStore = io->typeStore();
	io->setTypeInfo("ox::DescriptorField", 4);
	oxReturnError(io->field("typeName", &field->typeName));
	if (field->typeName == "") {
		field->ownsType = true;
		if (field->type == nullptr) {
			field->type = new DescriptorType;
		}
		oxReturnError(io->field("type", field->type));
		typeStore[field->type->typeName] = field->type;
	} else {
		// should be empty, so discard
		DescriptorType t;
		oxReturnError(io->field("type", &t));
		field->type = typeStore[field->typeName];
	}
	oxReturnError(io->field("fieldName", &field->fieldName));
	// defaultValue is unused now, but placeholder for backwards compatibility
	oxReturnError(io->field("defaultValue", nullptr));
	return OxError(0);
}

using TypeStore = HashMap<String, DescriptorType*>;

}
