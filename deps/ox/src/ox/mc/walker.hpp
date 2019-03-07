/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>

#include "deftypes.hpp"
#include "read.hpp"

namespace ox {

template<typename T>
class MetalClawWalker {
	template<typename FH>
	friend ox::Error ioOp(class MetalClawReader*, MetalClawWalker<FH>*);

	template<typename FH>
	friend ox::Error parseField(const mc::Field &field, MetalClawReader *rdr, MetalClawWalker<FH> *walker);

	private:
		Vector<const mc::Type*> m_typeStack;
		T m_fieldHandler;
		Vector<mc::FieldName> m_path;
		Vector<mc::TypeName> m_typePath;

	public:
		MetalClawWalker(mc::Type *type, T fieldHandler);

		[[nodiscard]] const mc::Type *type() const noexcept;

		ox::Error read(const mc::Field&, MetalClawReader *rdr);

	protected:
		void pushNamePath(mc::FieldName fn);

		void popNamePath();

		void pushType(const mc::Type *type);

		void popType();

};

template<typename T>
MetalClawWalker<T>::MetalClawWalker(mc::Type *type, T fieldHandler): m_fieldHandler(fieldHandler) {
	m_typeStack.push_back(type);
}

template<typename T>
const mc::Type *MetalClawWalker<T>::type() const noexcept {
	return m_typeStack.back();
}

template<typename T>
ox::Error MetalClawWalker<T>::read(const mc::Field &f, MetalClawReader *rdr) {
	// get const ref of paths
	const auto &pathCr = m_path;
	const auto &typePathCr = m_typePath;
	return m_fieldHandler(pathCr, typePathCr, f, rdr);
}

template<typename T>
void MetalClawWalker<T>::pushNamePath(mc::FieldName fn) {
	m_path.push_back(fn);
}

template<typename T>
void MetalClawWalker<T>::popNamePath() {
	m_path.pop_back();
}

template<typename T>
void MetalClawWalker<T>::pushType(const mc::Type *type) {
	m_typeStack.push_back(type);
}

template<typename T>
void MetalClawWalker<T>::popType() {
	m_typeStack.pop_back();
}

template<typename FH>
static ox::Error parseField(const mc::Field &field, MetalClawReader *rdr, MetalClawWalker<FH> *walker) {
	walker->pushNamePath(field.fieldName);
	if (field.subscriptLevels) {
		// add array handling
		const auto arrayLen = rdr->arrayLength(true);
		auto child = rdr->child();
		child.setTypeInfo(field.fieldName.c_str(), arrayLen);
		mc::Field f(field); // create mutable copy
		--f.subscriptLevels;
		BString<100> subscript;
		for (ArrayLength i = 0; i < arrayLen; i++) {
			subscript = "[";
			subscript += i;
			subscript += "]";
			walker->pushNamePath(subscript);
			oxReturnError(parseField(f, &child, walker));
			walker->popNamePath();
		}
		rdr->nextField();
	} else {
		switch (field.type->primitiveType) {
			case mc::PrimitiveType::UnsignedInteger:
			case mc::PrimitiveType::SignedInteger:
			case mc::PrimitiveType::Bool:
			case mc::PrimitiveType::String:
				oxReturnError(walker->read(field, rdr));
				break;
			case mc::PrimitiveType::Struct:
				if (rdr->fieldPresent()) {
					auto child = rdr->child();
					walker->pushType(field.type);
					oxReturnError(ioOp(&child, walker));
					walker->popType();
					rdr->nextField();
				} else {
					// skip and discard absent field
					int discard;
					oxReturnError(rdr->op("", &discard));
				}
				break;
		}
	}
	walker->popNamePath();
	return OxError(0);
}

template<typename FH>
ox::Error ioOp(MetalClawReader *rdr, MetalClawWalker<FH> *walker) {
	auto type = walker->type();
	if (!type) {
		return OxError(1);
	}
	auto typeName = type->typeName.c_str();
	auto &fields = type->fieldList;
	rdr->setTypeInfo(typeName, fields.size());
	for (std::size_t i = 0; i < fields.size(); i++) {
		auto &field = fields[i];
		if (field.type->primitiveType == mc::PrimitiveType::Struct) {
		}
		oxReturnError(parseField(field, rdr, walker));
	}
	return OxError(0);
}

template<typename Handler>
ox::Error walkMC(mc::Type *type, uint8_t *data, std::size_t dataLen, Handler handler) {
	MetalClawWalker walker(type, handler);
	MetalClawReader rdr(data, dataLen);
	return ioOp(&rdr, &walker);
}

}
