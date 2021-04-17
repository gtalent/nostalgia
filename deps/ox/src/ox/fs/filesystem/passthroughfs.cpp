/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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

ox::String PassThroughFS::basePath() {
	return m_path.string().c_str();
}

Error PassThroughFS::mkdir(const char *path, bool recursive) noexcept {
	bool success = false;
	const auto p = m_path / stripSlash(path);
	const auto u8p = p.u8string();
	oxTrace("ox::fs::PassThroughFS::mkdir") << u8p.c_str();
	if (recursive) {
		std::error_code ec;
		auto isDir = std::filesystem::is_directory(p, ec);
		oxReturnError(OxError(ec.value(), "PassThroughFS: mkdir failed"));
		if (isDir) {
			success = true;
		} else {
			success = std::filesystem::create_directories(p, ec);
			oxReturnError(OxError(ec.value(), "PassThroughFS: mkdir failed"));
		}
	} else {
		std::error_code ec;
		success = std::filesystem::create_directory(p, ec);
		oxReturnError(OxError(ec.value(), "PassThroughFS: mkdir failed"));
	}
	return OxError(success ? 0 : 1);
}

Error PassThroughFS::move(const char *src, const char *dest) noexcept {
	std::error_code ec;
	std::filesystem::rename(m_path / stripSlash(src), m_path / stripSlash(dest), ec);
	if (ec.value()) {
		return OxError(1);
	}
	return OxError(0);
}

Error PassThroughFS::read(const char *path, void *buffer, std::size_t buffSize) noexcept {
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
		return OxError(2);
	}
	return OxError(0);
}

Result<const uint8_t*> PassThroughFS::read(const char*) noexcept {
	return OxError(1);
}

Error PassThroughFS::read(uint64_t, void*, std::size_t) noexcept {
	// unsupported
	return OxError(1);
}

Error PassThroughFS::read(uint64_t, std::size_t, std::size_t, void*, std::size_t*) noexcept {
	// unsupported
	return OxError(1);
}

Result<const uint8_t*> PassThroughFS::read(uint64_t) noexcept {
	return OxError(1);
}

Result<Vector<String>> PassThroughFS::ls(const char *dir) noexcept {
	Vector<String> out;
	std::error_code ec;
	const auto di = std::filesystem::directory_iterator(m_path / stripSlash(dir), ec);
	oxReturnError(OxError(ec.value(), "PassThroughFS: ls failed"));
	for (auto &p : di) {
		auto u8p = p.path().filename().u8string();
		out.emplace_back(ox::bit_cast<const char*>(u8p.c_str()));
	}
	return ox::move(out);
}

Error PassThroughFS::remove(const char *path, bool recursive) noexcept {
	if (recursive) {
		return OxError(std::filesystem::remove_all(m_path / stripSlash(path)) != 0);
	} else {
		return OxError(std::filesystem::remove(m_path / stripSlash(path)) != 0);
	}
}

Error PassThroughFS::resize(uint64_t, void*) noexcept {
	// unsupported
	return OxError(1);
}

Error PassThroughFS::write(const char *path, void *buffer, uint64_t size, uint8_t) noexcept {
	auto p = (m_path / stripSlash(path));
	try {
		std::ofstream f(p, std::ios::binary);
		f.write(static_cast<char*>(buffer), size);
	} catch (const std::fstream::failure&) {
		oxTrace("ox::fs::PassThroughFS::write::error") << "Write failed:" << path;
		return OxError(1);
	}
	return OxError(0);
}

Error PassThroughFS::write(uint64_t, void*, uint64_t, uint8_t) noexcept {
	// unsupported
	return OxError(1);
}

Result<FileStat> PassThroughFS::stat(uint64_t) noexcept {
	// unsupported
	return OxError(1);
}

Result<FileStat> PassThroughFS::stat(const char *path) noexcept {
	std::error_code ec;
	const auto p = m_path / stripSlash(path);
	uint8_t type = std::filesystem::is_directory(p, ec) ?
		FileType_Directory : FileType_NormalFile;
	oxTrace("ox::fs::PassThroughFS::stat") << ec.message().c_str() << path;
	uint64_t size = type == FileType_Directory ? 0 : std::filesystem::file_size(p, ec);
	oxTrace("ox::fs::PassThroughFS::stat") << ec.message().c_str() << path;
	oxTrace("ox::fs::PassThroughFS::stat::size") << path << size;
	oxReturnError(OxError(ec.value()));
	return FileStat{0, 0, size, type};
}

uint64_t PassThroughFS::spaceNeeded(uint64_t size) noexcept {
	return size;
}

Result<uint64_t> PassThroughFS::available() noexcept {
	std::error_code ec;
	auto s = std::filesystem::space(m_path, ec);
	oxReturnError(OxError(ec.value(), "PassThroughFS: could not get FS size"));
	return s.available;
}

Result<uint64_t> PassThroughFS::size() const noexcept {
	std::error_code ec;
	auto s = std::filesystem::space(m_path, ec);
	oxReturnError(OxError(ec.value(), "PassThroughFS: could not get FS size"));
	return s.capacity;
}

char *PassThroughFS::buff() noexcept {
	return nullptr;
}

Error PassThroughFS::walk(Error(*)(uint8_t, uint64_t, uint64_t)) noexcept {
	return OxError(1);
}

bool PassThroughFS::valid() const noexcept {
	std::error_code ec;
	auto out = std::filesystem::is_directory(m_path, ec);
	if (!ec.value()) {
		return out;
	}
	return false;
}

const char *PassThroughFS::stripSlash(const char *path) noexcept {
	auto pathLen = ox_strlen(path);
	for (decltype(pathLen) i = 0; i < pathLen && path[0] == '/'; i++) {
		path++;
	}
	return path;
}

}

#endif
