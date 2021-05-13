/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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
	friend Error parseField(const DescriptorField &field, ReaderBase *rdr, DataWalker<ReaderBase, FH> *walker) noexcept;

	private:
		Vector<const DescriptorType*> m_typeStack;
		T m_fieldHandler;
		Vector<FieldName> m_path;
		Vector<TypeName> m_typePath;

	public:
		DataWalker(DescriptorType *type, T fieldHandler) noexcept;

		Result<const DescriptorType*> type() const noexcept;

		Error read(const DescriptorField&, Reader *rdr) noexcept;

	protected:
		void pushNamePath(const FieldName &fn) noexcept;

		void popNamePath() noexcept;

		void pushType(const DescriptorType *type) noexcept;

		void popType() noexcept;

};

template<typename Reader, typename T>
DataWalker<Reader, T>::DataWalker(DescriptorType *type, T fieldHandler) noexcept: m_fieldHandler(fieldHandler) {
	m_typeStack.push_back(type);
}

template<typename Reader, typename T>
Result<const DescriptorType*> DataWalker<Reader, T>::type() const noexcept {
	oxRequire(out, m_typeStack.back());
	return out;
}

template<typename Reader, typename T>
Error DataWalker<Reader, T>::read(const DescriptorField &f, Reader *rdr) noexcept {
	// get const ref of paths
	const auto &pathCr = m_path;
	const auto &typePathCr = m_typePath;
	return m_fieldHandler(pathCr, typePathCr, f, rdr);
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::pushNamePath(const FieldName &fn) noexcept {
	m_path.push_back(fn);
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::popNamePath() noexcept {
	m_path.pop_back();
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::pushType(const DescriptorType *type) noexcept {
	m_typeStack.push_back(type);
}

template<typename Reader, typename T>
void DataWalker<Reader, T>::popType() noexcept {
	m_typeStack.pop_back();
}

template<typename Reader, typename FH>
static Error parseField(const DescriptorField &field, Reader *rdr, DataWalker<Reader, FH> *walker) noexcept {
	walker->pushNamePath(field.fieldName);
	if (field.subscriptLevels) {
		// add array handling
		oxRequire(arrayLen, rdr->arrayLength(field.fieldName.c_str(), true));
		auto child = rdr->child(field.fieldName.c_str());
		child.setTypeInfo(field.fieldName.c_str(), arrayLen);
		DescriptorField f(field); // create mutable copy
		--f.subscriptLevels;
		String subscript;
		for (std::size_t i = 0; i < arrayLen; i++) {
			subscript = "[";
			subscript += static_cast<uint64_t>(i);
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
			case PrimitiveType::Union:
				if (rdr->fieldPresent(field.fieldName.c_str())) {
					auto child = rdr->child(field.fieldName.c_str());
					walker->pushType(field.type);
					oxReturnError(model(&child, walker));
					walker->popType();
					rdr->nextField();
				} else {
					// skip and discard absent field
					int discard;
					oxReturnError(rdr->field(field.fieldName.c_str(), &discard));
				}
				break;
		}
	}
	walker->popNamePath();
	return OxError(0);
}

template<typename Reader, typename FH>
constexpr Error model(Reader *rdr, DataWalker<Reader, FH> *walker) noexcept {
	oxRequire(type, walker->type());
	auto typeName = type->typeName.c_str();
	auto &fields = type->fieldList;
	rdr->setTypeInfo(typeName, fields.size());
	for (const auto &field : fields) {
		oxReturnError(parseField(field, rdr, walker));
	}
	return OxError(0);
}

template<typename Reader, typename Handler>
Error walkModel(DescriptorType *type, char *data, std::size_t dataLen, Handler handler) noexcept {
	DataWalker<Reader, Handler> walker(type, handler);
	Reader rdr(bit_cast<uint8_t*>(data), dataLen);
	return model(&rdr, &walker);
}

}
