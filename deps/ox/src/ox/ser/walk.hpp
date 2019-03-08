/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>

#include "desctypes.hpp"

namespace ox {

template<typename Reader, typename T>
class DataWalker {
	template<typename ReaderBase, typename FH>
	friend ox::Error parseField(const DescriptorField &field, ReaderBase *rdr, DataWalker<ReaderBase, FH> *walker);

	private:
		Vector<const DescriptorType*> m_typeStack;
		T m_fieldHandler;
		Vector<FieldName> m_path;
		Vector<TypeName> m_typePath;

	public:
		DataWalker(DescriptorType *type, T fieldHandler);

		[[nodiscard]] const DescriptorType *type() const noexcept;

		ox::Error read(const DescriptorField&, Reader *rdr);

	protected:
		void pushNamePath(FieldName fn);

		void popNamePath();

		void pushType(const DescriptorType *type);

		void popType();

};

template<typename Reader, typename T>
DataWalker<Reader, T>::DataWalker(DescriptorType *type, T fieldHandler): m_fieldHandler(fieldHandler) {
	m_typeStack.push_back(type);
}

template<typename Reader, typename T>
const DescriptorType *DataWalker<Reader, T>::type() const noexcept {
	return m_typeStack.back();
}

template<typename Reader, typename T>
ox::Error DataWalker<Reader, T>::read(const DescriptorField &f, Reader *rdr) {
	// get const ref of paths
	const auto &pathCr = m_path;
	const auto &typePathCr = m_typePath;
	return m_fieldHandler(pathCr, typePathCr, f, rdr);
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::pushNamePath(FieldName fn) {
	m_path.push_back(fn);
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::popNamePath() {
	m_path.pop_back();
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::pushType(const DescriptorType *type) {
	m_typeStack.push_back(type);
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::popType() {
	m_typeStack.pop_back();
}

template<typename Reader, typename FH>
static ox::Error parseField(const DescriptorField &field, Reader *rdr, DataWalker<Reader, FH> *walker) {
	walker->pushNamePath(field.fieldName);
	if (field.subscriptLevels) {
		// add array handling
		const auto arrayLen = rdr->arrayLength(true);
		auto child = rdr->child();
		child.setTypeInfo(field.fieldName.c_str(), arrayLen);
		DescriptorField f(field); // create mutable copy
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
			case PrimitiveType::UnsignedInteger:
			case PrimitiveType::SignedInteger:
			case PrimitiveType::Bool:
			case PrimitiveType::String:
				oxReturnError(walker->read(field, rdr));
				break;
			case PrimitiveType::Struct:
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

template<typename Reader, typename FH>
ox::Error ioOp(Reader *rdr, DataWalker<Reader, FH> *walker) {
	auto type = walker->type();
	if (!type) {
		return OxError(1);
	}
	auto typeName = type->typeName.c_str();
	auto &fields = type->fieldList;
	rdr->setTypeInfo(typeName, fields.size());
	for (std::size_t i = 0; i < fields.size(); i++) {
		auto &field = fields[i];
		if (field.type->primitiveType == PrimitiveType::Struct) {
		}
		oxReturnError(parseField(field, rdr, walker));
	}
	return OxError(0);
}

}
