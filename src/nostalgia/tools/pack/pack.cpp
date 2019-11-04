/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <string_view>
#include <vector>

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
[[nodiscard]] ox::Error pathToInode(std::vector<uint8_t>*) {
	return OxError(0);
}

// stub for now
[[nodiscard]] ox::Error toMetalClaw(std::vector<uint8_t>*) {
	return OxError(0);
}

// claw file transformations are broken out because path to inode
// transformations need to be done after the copy to the new FS is complete
[[nodiscard]] ox::Error transformClaw(ox::FileSystem32 *dest, std::string path) {
	// copy
	oxTrace("pack::transformClaw") << "path:" << path.c_str();
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
				std::vector<uint8_t> buff(stat.size);
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

[[nodiscard]] ox::Error verifyFile(ox::FileSystem32 *fs, const std::string &path, const std::vector<uint8_t> &expected) noexcept {
	std::vector<uint8_t> buff(expected.size());
	oxReturnError(fs->read(path.c_str(), buff.data(), buff.size()));
	return OxError(buff == expected ? 0 : 1);
}

struct VerificationPair {
	std::string path;
	std::vector<uint8_t> buff;
};

[[nodiscard]] ox::Error copy(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path) {
	std::cout << "copying directory: " << path << '\n';
	std::vector<VerificationPair> verficationPairs;
	// copy
	oxReturnError(src->ls(path.c_str(), [&verficationPairs, src, dest, path](const char *name, ox::InodeId_t) {
		std::cout << "reading " << name << '\n';
		auto currentFile = path + name;
		auto [stat, err] = src->stat((currentFile).c_str());
		oxReturnError(err);
		if (stat.fileType == ox::FileType_Directory) {
			oxReturnError(dest->mkdir(currentFile.c_str(), true));
			oxReturnError(copy(src, dest, currentFile + '/'));
		} else {
			std::vector<uint8_t> buff;
			// do transforms
			//const std::string OldExt = path.substr(path.find_last_of('.'));
			// load file
			buff.resize(stat.size);
			oxReturnError(src->read(currentFile.c_str(), buff.data(), buff.size()));
			// write file to dest
			std::cout << "writing " << currentFile << '\n';
			oxReturnError(dest->write(currentFile.c_str(), buff.data(), buff.size()));
			oxReturnError(verifyFile(dest, currentFile, buff));
			verficationPairs.push_back({currentFile, buff});
		}
		return OxError(0);
	}));

	// verify all at once in addition to right after the files are written
	for (auto v : verficationPairs) {
		oxReturnError(verifyFile(dest, v.path, v.buff));
	}

	return OxError(0);
}

}

[[nodiscard]] ox::Error pack(ox::PassThroughFS *src, ox::FileSystem32 *dest) {
	oxReturnError(copy(src, dest, "/"));
	oxReturnError(dest->stat("/TileSheets/Charset.ng").error);
	oxReturnError(dest->stat("/Palettes/Charset.npal").error);
	oxReturnError(transformClaw(dest, "/"));
	return OxError(0);
}

}
