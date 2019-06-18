/*
 * Copyright 2015 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
ox::Error pathToInode(std::vector<char>*) {
	return OxError(0);
}

// stub for now
ox::Error toMetalClaw(std::vector<char>*) {
	return OxError(0);
}

// claw file transformations are broken out because path to inode
// transformations need to be done after the copy to the new FS is complete
ox::Error transformClaw(ox::FileSystem32 *dest, std::string path) {
	// copy
	dest->ls(path.c_str(), [dest, path](const char *name, ox::InodeId_t) {
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
	return OxError(0);
}

ox::Error copy(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path) {
	// copy
	src->ls(path.c_str(), [src, dest, path](const char *name, ox::InodeId_t) {
		auto [stat, err] = src->stat(path.c_str());
		oxReturnError(err);
		if (stat.fileType == ox::FileType_Directory) {
			const auto dir = path + name + '/';
			oxReturnError(dest->mkdir(dir.c_str()));
			oxReturnError(copy(src, dest, dir));
		} else {
			std::vector<char> buff;
			// do transforms
			if (endsWith(path, ".png")) {
				// load file from full path and transform
				const auto fullPath = src->basePath() + path;
				buff = pngToGba(fullPath.c_str(), 0, 0);
				if (!buff.size()) {
					return OxError(1);
				}
			} else {
				// load file
				buff.resize(stat.size);
				oxReturnError(src->read(path.c_str(), buff.data(), buff.size()));
			}
			// write file to dest
			oxReturnError(dest->write(path.c_str(), buff.data(), buff.size()));
		}
		return OxError(0);
	});
	return OxError(0);
}

}

ox::Error pack(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path) {
	oxReturnError(copy(src, dest, path));
	oxReturnError(transformClaw(dest, path));
	return OxError(0);
}

}
