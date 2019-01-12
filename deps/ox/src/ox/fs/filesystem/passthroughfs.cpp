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
	if (recursive) {
		success = std::filesystem::create_directories(m_path / path);
	} else {
		success = std::filesystem::create_directory(m_path / path);
	}
	return OxError(success ? 0 : 1);
}

Error PassThroughFS::move(const char *src, const char *dest) {
	std::filesystem::rename(m_path / src, m_path / dest);
	return OxError(0);
}

Error PassThroughFS::read(const char *path, void *buffer, std::size_t buffSize) {
	auto file = fopen((m_path / path).c_str(), "r");
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
		return OxError(std::filesystem::remove_all(m_path / path) != 0);
	} else {
		return OxError(std::filesystem::remove(m_path / path) != 0);
	}
}

void PassThroughFS::resize(uint64_t, void*) {
	// unsupported
}

Error PassThroughFS::write(const char *path, void *buffer, uint64_t size, uint8_t) {
	auto f = fopen(path, "w");
	auto err = OxError(fwrite(buffer, size, 1, f) == size ? 0 : 1);
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
	auto size = std::filesystem::file_size(path, ec);
	return {{.size = size}, OxError(ec.value())};
}

uint64_t PassThroughFS::spaceNeeded(uint64_t size) {
	return size;
}

uint64_t PassThroughFS::available() {
	std::error_code ec;
	auto s = std::filesystem::space(path, ec);
	return s.available;
}

uint64_t PassThroughFS::size() const {
	std::error_code ec;
	auto s = std::filesystem::space(path, ec);
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

}

#endif
