/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if __has_include(<filesystem>)

#include <stdio.h>

#include "passthroughfs.hpp"

namespace ox {

PassThroughFS::PassThroughFS(const char *dirPath) {
	m_path = dirPath;
}

PassThroughFS::~PassThroughFS() {
}

Error PassThroughFS::mkdir(const char *path, bool recursive) {
	bool success = false;
	const auto p = m_path / stripSlash(path);
	oxTrace("ox::fs::PassThroughFS::mkdir") << p.c_str();
	if (recursive) {
		success = std::filesystem::create_directories(p);
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
	auto file = fopen((m_path / stripSlash(path)).c_str(), "r");
	if (file) {
		fseek(file, 0, SEEK_END);
		const std::size_t size = ftell(file);
		if (size <= buffSize) {
			rewind(file);
			auto itemsRead = fread(buffer, buffSize, 1, file);
			fclose(file);
			return OxError(itemsRead == 1 ? 0 : 1);
		}
	}
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

Error PassThroughFS::remove(const char *path, bool recursive) {
	if (recursive) {
		return OxError(std::filesystem::remove_all(m_path / stripSlash(path)) != 0);
	} else {
		return OxError(std::filesystem::remove(m_path / stripSlash(path)) != 0);
	}
}

void PassThroughFS::resize(uint64_t, void*) {
	// unsupported
}

Error PassThroughFS::write(const char *path, void *buffer, uint64_t size, uint8_t) {
	auto p = (m_path / stripSlash(path));
	auto f = fopen(p.c_str(), "w");
	auto err = OxError(fwrite(buffer, size, 1, f) == 1 ? 0 : 1);
	fclose(f);
	return err;
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
	return {{.size = size, .fileType = type}, OxError(ec.value())};
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

uint8_t *PassThroughFS::buff() {
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