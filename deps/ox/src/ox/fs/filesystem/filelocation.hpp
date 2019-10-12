/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/std.hpp>

namespace ox {

enum class FileAddressType {
	None  = -1,
	Path,
	ConstPath,
	Inode,
};

class FileAddress {
	private:
		FileAddressType m_type = FileAddressType::None;
		union {
			char *path;
			const char *constPath;
			uint64_t inode;
		} m_data;

	public:
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

}
