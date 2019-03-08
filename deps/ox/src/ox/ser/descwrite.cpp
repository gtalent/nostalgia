/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/typeinfo.hpp>

#include "descwrite.hpp"

namespace ox {

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


TypeDescWriter::TypeDescWriter(TypeStore *typeStore) {
	if (!typeStore) {
		m_typeStoreOwnerRef = new TypeStore;
		typeStore = m_typeStoreOwnerRef;
	}
	m_typeStore = typeStore;
}

TypeDescWriter::~TypeDescWriter() {
	// does not own it's elements
	delete m_typeStoreOwnerRef;
}

DescriptorType *TypeDescWriter::type(int8_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int8_t";
	constexpr auto PT = PrimitiveType::SignedInteger;
	constexpr auto Bytes = 1;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(int16_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int16_t";
	constexpr auto PT = PrimitiveType::SignedInteger;
	constexpr auto Bytes = 2;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(int32_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int32_t";
	constexpr auto PT = PrimitiveType::SignedInteger;
	constexpr auto Bytes = 4;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(int64_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int64_t";
	constexpr auto PT = PrimitiveType::SignedInteger;
	constexpr auto Bytes = 8;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(uint8_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint8_t";
	constexpr auto PT = PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 1;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(uint16_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint16_t";
	constexpr auto PT = PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 2;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(uint32_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint32_t";
	constexpr auto PT = PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 4;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(uint64_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint64_t";
	constexpr auto PT = PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 8;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(const char*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:string";
	constexpr auto PT = PrimitiveType::String;
	return getType(TypeName, PT, 0, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(McStr, bool *alreadyExisted) {
	constexpr auto TypeName = "B:string";
	constexpr auto PT = PrimitiveType::String;
	return getType(TypeName, PT, 0, alreadyExisted);
}

DescriptorType *TypeDescWriter::type(bool*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:bool";
	constexpr auto PT = PrimitiveType::Bool;
	constexpr auto Bytes = 0;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

void TypeDescWriter::setTypeInfo(const char *name, int) {
	auto &t = m_typeStore->at(name);
	if (!t) {
		t = new DescriptorType;
	}
	m_type = t;
	m_type->typeName = name;
	m_type->primitiveType = PrimitiveType::Struct;
}

DescriptorType *TypeDescWriter::getType(TypeName tn, PrimitiveType pt, int b, bool *alreadyExisted) {
	if (m_typeStore->contains(tn)) {
		*alreadyExisted = true;
		auto type = m_typeStore->at(tn);
		oxAssert(type != nullptr, "TypeDescWriter::getType returning null DescriptorType");
		return type;
	} else {
		*alreadyExisted = false;
		auto &t = m_typeStore->at(tn);
		if (!t) {
			t = new DescriptorType;
		}
		t->typeName = tn;
		t->primitiveType = pt;
		t->length = b;
		return t;
	}
}

}
