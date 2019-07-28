/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <string_view>
#include <vector>

#include "imgconv.hpp"
#include "pack.hpp"

namespace nostalgia {

namespace {

[[nodiscard]] static constexpr bool endsWith(std::string_view str, std::string_view ending) {
	return str.size() >= ending.size() && str.substr(str.size() - ending.size()) == ending;
}

/**
 * Convert path references to inodes to save space
 * @param buff buffer holding file
 * @return error
 * stub for now
 */
[[nodiscard]] ox::Error pathToInode(std::vector<char>*) {
	return OxError(0);
}

// stub for now
[[nodiscard]] ox::Error toMetalClaw(std::vector<char>*) {
	return OxError(0);
}

// claw file transformations are broken out because path to inode
// transformations need to be done after the copy to the new FS is complete
[[nodiscard]] ox::Error transformClaw(ox::FileSystem32 *dest, std::string path) {
	// copy
	return dest->ls(path.c_str(), [dest, path](const char *name, ox::InodeId_t) {
		auto [stat, err] = dest->stat(path.c_str());
		oxReturnError(err);
		if (stat.fileType == ox::FileType_Directory) {
			const auto dir = path + name + '/';
			oxReturnError(transformClaw(dest, dir));
		} else {
			// do transforms
			if (endsWith(path, ".claw")) {
				// load file
				std::vector<char> buff(stat.size);
				oxReturnError(dest->read(path.c_str(), buff.data(), buff.size()));
				// do transformations
				oxReturnError(pathToInode(&buff));
				oxReturnError(toMetalClaw(&buff));
				// write file to dest
				oxReturnError(dest->write(path.c_str(), buff.data(), buff.size()));
			}
		}
		return OxError(0);
	});
}

[[nodiscard]] ox::Error verifyFile(ox::FileSystem32 *fs, const std::string &path, const std::vector<char> &expected) noexcept {
	std::vector<char> buff(expected.size());
	oxReturnError(fs->read(path.c_str(), buff.data(), buff.size()));
	return OxError(buff == expected ? 0 : 1);
}

[[nodiscard]] ox::Error copy(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path) {
	std::cout << "copying directory: " << path << '\n';
	// copy
	return src->ls(path.c_str(), [src, dest, path](const char *name, ox::InodeId_t) {
		std::cout << "reading " << name << '\n';
		auto currentFile = path + name;
		auto [stat, err] = src->stat((currentFile).c_str());
		oxReturnError(err);
		if (stat.fileType == ox::FileType_Directory) {
			oxReturnError(dest->mkdir(currentFile.c_str(), true));
			oxReturnError(copy(src, dest, currentFile + '/'));
		} else {
			std::vector<char> buff;
			// do transforms
			constexpr std::string_view PngExt = ".png";
			constexpr std::string_view GbagExt = ".ng";
			if (endsWith(currentFile, PngExt)) {
				// load file from full path and transform
				const auto fullPath = src->basePath() + currentFile;
				buff = pngToGba(fullPath.c_str(), 0, 0);
				currentFile = currentFile.substr(0, currentFile.size() - PngExt.size()) + GbagExt.data();
				if (!buff.size()) {
					return OxError(1);
				}
			} else {
				// load file
				buff.resize(stat.size);
				oxReturnError(src->read(currentFile.c_str(), buff.data(), buff.size()));
			}
			// write file to dest
			std::cout << "writing " << currentFile << '\n';
			oxReturnError(dest->write(currentFile.c_str(), buff.data(), buff.size()));
			oxReturnError(verifyFile(dest, currentFile, buff));
		}
		return OxError(0);
	});
}

}

[[nodiscard]] ox::Error pack(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path) {
	oxReturnError(copy(src, dest, path));
	oxReturnError(transformClaw(dest, path));
	return OxError(0);
}

}
