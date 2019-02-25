/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/hashmap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

namespace ox::mc {

using String = BString<100>;

enum class PrimitiveType: uint8_t {
	UnsignedInteger = 0,
	SignedInteger = 1,
	Bool = 2,
	Float = 3,
	String = 4,
	Struct = 5,
};

using FieldName = String;

struct Field {
	// order of fields matters

	// only serialize type name if type has already been serialized
	const struct Type *type = nullptr;
	FieldName fieldName;
	int subscriptLevels = 0;

	// do not serialize the following
	String typeName; // gives reference to type for lookup if type is null
	bool serializeType = false;
};

using FieldList = Vector<Field>;
using TypeName = String;

struct Type {
	TypeName typeName;
	PrimitiveType primitiveType;
	// fieldList only applies to structs
	Vector<Field> fieldList;
	// - number of bytes for integer and float types
	// - number of fields for structs and lists
	int64_t length = 0;

	Type() = default;

	Type(String tn, PrimitiveType t, int b): typeName(tn), primitiveType(t), length(b) {
	}

	Type(String tn, PrimitiveType t, FieldList fl): typeName(tn), primitiveType(t), fieldList(fl) {
	}
};


template<typename T>
int ioOp(T *io, Type *type) {
	int32_t err = 0;
	io->setTypeInfo("ox::mc::Type", 4);
	err |= io->op("typeName", &type->typeName);
	err |= io->op("primitiveType", &type->primitiveType);
	err |= io->op("fieldList", &type->fieldList);
	err |= io->op("length", &type->length);
	return err;
}

template<typename T>
int ioOpWrite(T *io, Field *field) {
	int32_t err = 0;
	io->setTypeInfo("ox::mc::Field", 4);
	if (field->serializeType) {
		err |= io->op("typeName", "");
		err |= io->op("type", field->type);
	} else {
		err |= io->op("typeName", &field->type->typeName);
		err |= io->op("type", static_cast<decltype(field->type)>(nullptr));
	}
	err |= io->op("fieldName", &field->fieldName);
	// defaultValue is unused now, but placeholder for backwards compatibility
	err |= io->op("defaultValue", nullptr);
	return err;
}

template<typename T>
int ioOpRead(T *io, Field *field) {
	int32_t err = 0;
	io->setTypeInfo("ox::mc::Field", 4);
	err |= io->op("typeName", &field->typeName);
	if (field->typeName == "") {
		field->serializeType = true;
		if (field->type == nullptr) {
			field->type = new Type;
		}
		err |= io->op("type", field->type);
	} else {
		// should be empty, so discard
		Type t;
		err |= io->op("type", &t);
	}
	err |= io->op("fieldName", &field->fieldName);
	// defaultValue is unused now, but placeholder for backwards compatibility
	err |= io->op("defaultValue", nullptr);
	return err;
}

using TypeStore = ox::HashMap<mc::String, mc::Type>;

}
