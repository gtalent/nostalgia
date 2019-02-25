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

	private:
		mc::Type *m_type = nullptr;
		T *m_fieldHandler = nullptr;
		Vector<mc::FieldName> m_path;
		Vector<mc::TypeName> m_typePath;

	public:
		MetalClawWalker(T *fieldHandler);

		[[nodiscard]] const mc::Type *type() const noexcept;

		void read(const mc::Field&, MetalClawReader *rdr);

	protected:
		void pushNamePath(mc::FieldName fn);

		void popNamePath();

};

template<typename T>
MetalClawWalker<T>::MetalClawWalker(T *fieldHandler) {
	m_fieldHandler = fieldHandler;
}

template<typename T>
const mc::Type *MetalClawWalker<T>::type() const noexcept {
	return m_type;
}

template<typename T>
void MetalClawWalker<T>::read(const mc::Field &f, MetalClawReader *rdr) {
	const auto &pathCr = m_path;
	m_fieldHandler->read(pathCr, f, rdr);
}

template<typename FH>
static ox::Error parseField(mc::Field field, MetalClawReader *rdr, MetalClawWalker<FH> *walker) {
	ox::Error err = 0;
	walker->pushNamePath(field.fieldName);
	if (field.subscriptLevels) {
		// add array handling
		auto child = rdr->child();
		const auto arrayLen = rdr->arrayLength();
		auto f = field;
		--f.subscriptLevels;
		rdr->setTypeInfo(field.type->typeName.c_str(), arrayLen);
		BString<100> subscript;
		for (ArrayLength i = 0; i < arrayLen; i++) {
			subscript = "[";
			subscript += i;
			subscript += "]";
			walker->pushNamePath(subscript);
			err |= parseField(f, &child, walker);
			walker->pophNamePath();
		}
	} else {
		switch (field.type->primitiveType) {
			case mc::PrimitiveType::UnsignedInteger:
			case mc::PrimitiveType::SignedInteger:
			case mc::PrimitiveType::Bool:
			case mc::PrimitiveType::Float:
			case mc::PrimitiveType::String:
				err |= walker->read(field, rdr);
				break;
			case mc::PrimitiveType::Struct:
				if (rdr->fieldPresent()) {
					auto child = rdr->child();
					rdr->setTypeInfo(field.type->typeName.c_str(), field.type->fieldList.size());
					err |= ioOp(&child, walker);
				} else {
					// skip and discard absent field
					int discard;
					err |= rdr->op("", &discard);
				}
				break;
		}
	}
	walker->popNamePath();
	return err;
}

template<typename FH>
ox::Error ioOp(MetalClawReader *rdr, MetalClawWalker<FH> *walker) {
	Error err = 0;
	auto &fields = walker->type()->fieldList;
	for (std::size_t i = 0; i < fields.size(); i++) {
		auto &field = fields[i];
		parseField(field, rdr, walker);
	}
	return OxError(err);
}

}
