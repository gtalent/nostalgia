/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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
	friend Error model(T*, FileAddress*);

	public:
		static constexpr auto TypeName = "net.drinkingtea.ox.FileAddress";
		static constexpr auto Fields = 2;

		union Data {
			static constexpr auto TypeName = "net.drinkingtea.ox.FileAddress.Data";
			static constexpr auto Fields = 3;
			char *path;
			const char *constPath;
			uint64_t inode;
		};

	protected:
		FileAddressType m_type = FileAddressType::None;
		Data m_data;

	public:
		FileAddress();

		FileAddress(const FileAddress &other);

		FileAddress(std::nullptr_t);

		FileAddress(uint64_t inode);

		FileAddress(char *path);

		FileAddress(const char *path);

		~FileAddress();

		const FileAddress &operator=(const FileAddress &other);

		[[nodiscard]] constexpr FileAddressType type() const noexcept {
			switch (m_type) {
				case FileAddressType::Path:
				case FileAddressType::ConstPath:
					return FileAddressType::Path;
				default:
					return m_type;
			}
		}

		Result<uint64_t> getInode() const noexcept {
			switch (m_type) {
				case FileAddressType::Inode:
					return m_data.inode;
				default:
					return OxError(1);
			}
		}

		Result<const char*> getPath() const noexcept {
			switch (m_type) {
				case FileAddressType::Path:
					return m_data.path;
				case FileAddressType::ConstPath:
					return m_data.constPath;
				default:
					return OxError(1);
			}
		}

		operator bool() const {
			return m_type != FileAddressType::None;
		}

};

template<typename T>
Error model(T *io, FileAddress::Data *obj) {
	io->template setTypeInfo<FileAddress::Data>();
	oxReturnError(io->field("path", SerStr(&obj->path)));
	oxReturnError(io->field("constPath", SerStr(&obj->path)));
	oxReturnError(io->field("inode", &obj->inode));
	return OxError(0);
}

template<typename T>
Error model(T *io, FileAddress *fa) {
	io->template setTypeInfo<FileAddress>();
	oxReturnError(io->field("type", bit_cast<int8_t*>(&fa->m_type)));
	oxReturnError(io->field("data", UnionView(&fa->m_data, static_cast<int>(fa->m_type))));
	return OxError(0);
}

}
