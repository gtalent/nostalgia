/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if __has_include(<filesystem>)

#include <filesystem>
#include <string>

#include "filesystem.hpp"

namespace ox {

/**
 *
 */
class PassThroughFS: public FileSystem {
	private:
		std::filesystem::path m_path;

	public:
		PassThroughFS(const char *dirPath);

		~PassThroughFS();

		[[nodiscard]] std::string basePath();

		Error mkdir(const char *path, bool recursive = false) override;

		Error move(const char *src, const char *dest) override;

		Error read(const char *path, void *buffer, std::size_t buffSize) override;

		Error read(uint64_t inode, void *buffer, std::size_t size) override;

		Error read(uint64_t inode, std::size_t readStart, std::size_t readSize, void *buffer, std::size_t *size) override;

		template<typename F>
		Error ls(const char *dir, F cb);

		Error remove(const char *path, bool recursive = false) override;

		void resize(uint64_t size, void *buffer = nullptr) override;

		Error write(const char *path, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) override;

		Error write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = FileType_NormalFile) override;

		ValErr<FileStat> stat(uint64_t inode) override;

		ValErr<FileStat> stat(const char *path) override;

		uint64_t spaceNeeded(uint64_t size) override;

		uint64_t available() override;

		uint64_t size() const override;

		uint8_t *buff() override;

		Error walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) override;

		bool valid() const override;

	private:
		/**
		 * Strips the leading slashes from a string.
		 */
		const char *stripSlash(const char *path);

};

template<typename F>
Error PassThroughFS::ls(const char *dir, F cb) {
	for (auto &p : std::filesystem::directory_iterator(m_path / stripSlash(dir))) {
		if (auto err = cb(p.path().filename().c_str(), 0); err) {
			return err;
		}
	}
	return OxError(0);
}

}

#endif
