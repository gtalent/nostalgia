/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "passthroughfs.hpp"

#if defined(OX_HAS_PASSTHROUGHFS)

#include <fstream>

namespace ox {

PassThroughFS::PassThroughFS(const char *dirPath) {
	m_path = dirPath;
}

PassThroughFS::~PassThroughFS() {
}

std::string PassThroughFS::basePath() {
	return m_path.string();
}

Error PassThroughFS::mkdir(const char *path, bool recursive) {
	bool success = false;
	const auto p = m_path / stripSlash(path);
	const auto u8p = p.u8string();
	oxTrace("ox::fs::PassThroughFS::mkdir") << u8p.c_str();
	if (recursive) {
		if (std::filesystem::is_directory(p)) {
			success = true;
		} else {
			success = std::filesystem::create_directories(p);
		}
	} else {
		success = std::filesystem::create_directory(p);
	}
	return OxError(success ? 0 : 1);
}

Error PassThroughFS::move(const char *src, const char *dest) {
	std::filesystem::rename(m_path / stripSlash(src), m_path / stripSlash(dest));
	return OxError(0);
}

Error PassThroughFS::read(const char *path, void *buffer, std::size_t buffSize) {
	try {
		std::ifstream file((m_path / stripSlash(path)), std::ios::binary | std::ios::ate);
		const std::size_t size = file.tellg();
		file.seekg(0, std::ios::beg);
		if (size > buffSize) {
			oxTrace("ox::fs::PassThroughFS::read::error") << "Read failed: Buffer too small:" << path;
			return OxError(1);
		}
		file.read(static_cast<char*>(buffer), buffSize);
	} catch (const std::fstream::failure&) {
		oxTrace("ox::fs::PassThroughFS::read::error") << "Read failed:" << path;
		throw OxError(2);
	}
	return OxError(0);
}

ValErr<uint8_t*> PassThroughFS::read(const char*) {
	return OxError(1);
}

Error PassThroughFS::read(uint64_t, void*, std::size_t) {
	// unsupported
	return OxError(1);
}

Error PassThroughFS::read(uint64_t, std::size_t, std::size_t, void*, std::size_t*) {
	// unsupported
	return OxError(1);
}

ValErr<uint8_t*> PassThroughFS::read(uint64_t) {
	return OxError(1);
}

Error PassThroughFS::remove(const char *path, bool recursive) {
	if (recursive) {
		return OxError(std::filesystem::remove_all(m_path / stripSlash(path)) != 0);
	} else {
		return OxError(std::filesystem::remove(m_path / stripSlash(path)) != 0);
	}
}

ox::Error PassThroughFS::resize(uint64_t, void*) {
	// unsupported
	return OxError(1);
}

Error PassThroughFS::write(const char *path, void *buffer, uint64_t size, uint8_t) {
	auto p = (m_path / stripSlash(path));
	try {
		std::ofstream f(p, std::ios::binary);
		f.write(static_cast<char*>(buffer), size);
	} catch (const std::fstream::failure&) {
		oxTrace("ox::fs::PassThroughFS::write::error") << "Write failed:" << path;
		throw OxError(1);
	}
	return OxError(0);
}

Error PassThroughFS::write(uint64_t, void*, uint64_t, uint8_t) {
	// unsupported
	return OxError(1);
}

ValErr<FileStat> PassThroughFS::stat(uint64_t) {
	// unsupported
	return {{}, OxError(1)};
}

ValErr<FileStat> PassThroughFS::stat(const char *path) {
	std::error_code ec;
	const auto p = m_path / stripSlash(path);
	uint8_t type = std::filesystem::is_directory(p, ec) ?
		FileType_Directory : FileType_NormalFile;
	oxTrace("PassThroughFS::stat") << ec.message().c_str() << path;
	uint64_t size = type == FileType_Directory ? 0 : std::filesystem::file_size(p, ec);
	oxTrace("PassThroughFS::stat") << ec.message().c_str() << path;
	oxTrace("PassThroughFS::stat::size") << path << size;
	return {{0, 0, size, type}, OxError(ec.value())};
}

uint64_t PassThroughFS::spaceNeeded(uint64_t size) {
	return size;
}

uint64_t PassThroughFS::available() {
	std::error_code ec;
	auto s = std::filesystem::space(m_path, ec);
	return s.available;
}

uint64_t PassThroughFS::size() const {
	std::error_code ec;
	auto s = std::filesystem::space(m_path, ec);
	return s.capacity;
}

char *PassThroughFS::buff() {
	return nullptr;
}

Error PassThroughFS::walk(Error(*)(uint8_t, uint64_t, uint64_t)) {
	return OxError(1);
}

bool PassThroughFS::valid() const {
	return std::filesystem::is_directory(m_path);
}

const char *PassThroughFS::stripSlash(const char *path) {
	auto pathLen = ox_strlen(path);
	for (decltype(pathLen) i = 0; i < pathLen && path[0] == '/'; i++) {
		path++;
	}
	return path;
}

}

#endif
