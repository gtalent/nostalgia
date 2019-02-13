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

MetalClawDefWriter::MetalClawDefWriter(mc::TypeStore *typeStore) {
	if (!typeStore) {
		m_typeStoreOwnerRef = new mc::TypeStore;
		typeStore = m_typeStoreOwnerRef;
	}
	m_typeStore = typeStore;
}

MetalClawDefWriter::~MetalClawDefWriter() {
	if (m_typeStoreOwnerRef) {
		delete m_typeStoreOwnerRef;
	}
}

constexpr mc::Type *MetalClawDefWriter::type(int8_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int8_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 8;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(int16_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int16_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 16;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(int32_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int32_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 32;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(int64_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:int64_t";
	constexpr auto PT = mc::PrimitiveType::SignedInteger;
	constexpr auto Bytes = 64;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(uint8_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint8_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 8;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(uint16_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint16_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 16;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(uint32_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint32_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 32;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(uint64_t*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:uint64_t";
	constexpr auto PT = mc::PrimitiveType::UnsignedInteger;
	constexpr auto Bytes = 64;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

mc::Type *MetalClawDefWriter::type(McStr, bool *alreadyExisted) {
	constexpr auto TypeName = "B:string";
	constexpr auto PT = mc::PrimitiveType::String;
	return getPrimitive(TypeName, PT, 0, alreadyExisted);
}

constexpr mc::Type *MetalClawDefWriter::type(bool*, bool *alreadyExisted) {
	constexpr auto TypeName = "B:bool";
	constexpr auto PT = mc::PrimitiveType::Bool;
	constexpr auto Bytes = 0;
	return getPrimitive(TypeName, PT, Bytes, alreadyExisted);
}

constexpr void MetalClawDefWriter::setTypeInfo(const char *name, int) {
	m_type = &m_typeStore->at(name);
	m_type->typeName = name;
	m_type->primitiveType = mc::PrimitiveType::Struct;
}

constexpr mc::Type *MetalClawDefWriter::getPrimitive(mc::String tn, mc::PrimitiveType pt, int b, bool *alreadyExisted) {
	if (m_typeStore->contains(tn)) {
		*alreadyExisted = true;
		return &m_typeStore->at(tn);
	} else {
		*alreadyExisted = false;
		auto t = &m_typeStore->at(tn);
		t->typeName = tn;
		t->primitiveType = pt;
		t->length = b;
		return t;
	}
}

}
