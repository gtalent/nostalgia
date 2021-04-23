/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "filelocation.hpp"

namespace ox {

FileAddress::FileAddress() noexcept {
	m_data.inode = 0;
	m_type = FileAddressType::Inode;
}

FileAddress::FileAddress(const FileAddress &other) noexcept {
	operator=(other);
}

FileAddress::FileAddress(std::nullptr_t) noexcept {
}

FileAddress::FileAddress(uint64_t inode) noexcept {
	m_data.inode = inode;
	m_type = FileAddressType::Inode;
}

FileAddress::FileAddress(char *path) noexcept {
	auto pathSize = ox_strlen(path) + 1;
	m_data.path = new char[pathSize];
	memcpy(m_data.path, path, pathSize);
	m_type = FileAddressType::Path;
}

FileAddress::FileAddress(const char *path) noexcept {
	m_data.constPath = path;
	m_type = FileAddressType::ConstPath;
}

FileAddress::~FileAddress() noexcept {
	if (m_type == FileAddressType::Path) {
		delete[] m_data.path;
		m_data.path = nullptr;
	}
}

const FileAddress &FileAddress::operator=(const FileAddress &other) noexcept {
	m_type = other.m_type;
	switch (m_type) {
		case FileAddressType::Path:
		{
			auto strSize = ox_strlen(other.m_data.path) + 1;
			m_data.path = new char[strSize];
			ox_memcpy(m_data.path, other.m_data.path, strSize);
			break;
		}
		case FileAddressType::ConstPath:
		case FileAddressType::Inode:
			m_data = other.m_data;
			break;
		case FileAddressType::None:
			break;
	}
	return *this;
}

}
