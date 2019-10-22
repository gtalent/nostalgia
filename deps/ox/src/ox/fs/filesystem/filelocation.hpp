/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>
#include <ox/model/types.hpp>

namespace ox {

enum class FileAddressType: int8_t {
	None = -1,
	Path,
	ConstPath,
	Inode,
};

class FileAddress {

	template<typename T>
	friend ox::Error modelWrite(T*, FileAddress*);

	public:
		static constexpr auto Fields = 2;

	protected:
		FileAddressType m_type = FileAddressType::None;
		union {
			char *path;
			const char *constPath;
			uint64_t inode;
		} m_data;

	public:
		FileAddress();

		FileAddress(uint64_t inode);

		FileAddress(char *path);

		FileAddress(const char *path);

		~FileAddress();

		[[nodiscard]] constexpr FileAddressType type() const noexcept {
			switch (m_type) {
				case FileAddressType::Path:
				case FileAddressType::ConstPath:
					return FileAddressType::Path;
				default:
					return m_type;
			}
		}

		[[nodiscard]] constexpr ValErr<uint64_t> getInode() const noexcept {
			switch (m_type) {
				case FileAddressType::Inode:
					return m_data.inode;
				default:
					return OxError(1);
			}
		}

		[[nodiscard]] constexpr ValErr<const char*> getPath() const noexcept {
			switch (m_type) {
				case FileAddressType::Path:
					return m_data.path;
				case FileAddressType::ConstPath:
					return m_data.constPath;
				default:
					return OxError(1);
			}
		}

};

template<typename T>
ox::Error modelWrite(T *io, FileAddress *fa) {
	io->setTypeInfo("ox::FileAddress", FileAddress::Fields);
	switch (fa->m_type) {
		case FileAddressType::Path:
		case FileAddressType::ConstPath:
		{
			decltype(fa->m_data.inode) blank = 0;
			const auto strSize = ox_strlen(fa->m_data.constPath) + 1;
			auto path = ox_malloca(strSize, char, 0);
			memcpy(path.get(), fa->m_data.constPath, strSize);
			oxReturnError(io->field("path", SerStr(path.get(), strSize - 1)));
			oxReturnError(io->field("inode", &blank));
			break;
		}
		case FileAddressType::Inode:
		{
			char blankPath[1] = "";
			oxReturnError(io->field("path", SerStr(blankPath, 0)));
			oxReturnError(io->field("inode", &fa->m_data.inode));
			break;
		}
		case FileAddressType::None:
		{
			char blankPath[1] = "";
			decltype(fa->m_data.inode) blankInode = 0;
			oxReturnError(io->field("path", SerStr(blankPath, 0)));
			oxReturnError(io->field("inode", &blankInode));
			break;
		}
	}
	return OxError(0);
}

}
