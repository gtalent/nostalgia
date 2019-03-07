/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/typeinfo.hpp>

#include "defwriter.hpp"

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


MetalClawDefWriter::MetalClawDefWriter(mc::TypeStore *typeStore) {
	if (!typeStore) {
		m_typeStoreOwnerRef = new mc::TypeStore;
		typeStore = m_typeStoreOwnerRef;
	}
	m_typeStore = typeStore;
}

MetalClawDefWriter::~MetalClawDefWriter() {
	// does not own it's elements
	delete m_typeStoreOwnerRef;
}

mc::Type *MetalClawDefWriter::type(int8_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int8_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 8;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(int16_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int16_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 16;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(int32_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int32_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 32;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(int64_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int64_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 64;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(uint8_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint8_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 8;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(uint16_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint16_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 16;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(uint32_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint32_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 32;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(uint64_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint64_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 64;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(const char*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:string";
	constexpr auto PT = mc::PrimitiveType::String;
	return getType(TypeName, PT, 0, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(McStr, bool *alreadyExisted) {
	constexpr auto TypeName = "B:string";
	constexpr auto PT = mc::PrimitiveType::String;
	return getType(TypeName, PT, 0, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(bool*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:bool";
	constexpr auto PT = mc::PrimitiveType::Bool;
	constexpr auto Bytes = 0;
	return getType(TypeName, PT, Bytes, alreadyExisted);
}

void MetalClawDefWriter::setTypeInfo(const char *name, int) {
	auto &t = m_typeStore->at(name);
	if (!t) {
		t = new mc::Type;
	}
	m_type = t;
	m_type->typeName = name;
	m_type->primitiveType = mc::PrimitiveType::Struct;
}

mc::Type *MetalClawDefWriter::getType(mc::TypeName tn, mc::PrimitiveType pt, int b, bool *alreadyExisted) {
	if (m_typeStore->contains(tn)) {
		*alreadyExisted = true;
		auto type = m_typeStore->at(tn);
		oxAssert(type != nullptr, "MetalClawDefWriter::getType returning null Type");
		return type;
	} else {
		*alreadyExisted = false;
		auto &t = m_typeStore->at(tn);
		if (!t) {
			t = new mc::Type;
		}
		t->typeName = tn;
		t->primitiveType = pt;
		t->length = b;
		return t;
	}
}

}
