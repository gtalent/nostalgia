/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/claw/read.hpp>
#include <ox/fs/fs.hpp>

#include "pack.hpp"

namespace nostalgia {

/**
 * Convert path references to inodes to save space
 * @param buff buffer holding file
 * @return error
 * stub for now
 */
static ox::Result<ox::Buffer> pathToInode(const ox::Buffer &buff) noexcept {
	return ox::move(buff);
}

// just strip header for now...
static ox::Result<ox::Buffer> toMetalClaw(const ox::Buffer &buff) noexcept {
	return ox::stripClawHeader(buff.data(), buff.size());
}

static ox::Error doTransformations(ox::FileSystem *dest, const ox::String &filePath) noexcept {
	if (filePath.endsWith(".ng") || filePath.endsWith(".npal")) {
		// load file
		oxRequireM(buff, dest->read(filePath.c_str()));
		// do transformations
		oxReturnError(pathToInode(buff).moveTo(&buff));
		oxReturnError(toMetalClaw(buff).moveTo(&buff));
		// write file to dest
		oxReturnError(dest->write(filePath.c_str(), buff.data(), buff.size()));
	}
	return OxError(0);
}

// claw file transformations are broken out because path to inode
// transformations need to be done after the copy to the new FS is complete
static ox::Error transformClaw(ox::FileSystem *dest, const ox::String &path) noexcept {
	// copy
	oxTracef("pack::transformClaw", "path: {}", path);
	oxRequire(fileList, dest->ls(path));
	for (const auto &name : fileList) {
		const auto filePath = path + name;
		oxRequire(stat, dest->stat(filePath.c_str()));
		if (stat.fileType == ox::FileType::Directory) {
			const auto dir = path + name + '/';
			oxReturnError(transformClaw(dest, dir));
		} else {
			oxReturnError(doTransformations(dest, filePath));
		}
	}
	return OxError(0);
}

static ox::Error verifyFile(ox::FileSystem *fs, const ox::String &path, const ox::Buffer &expected) noexcept {
	ox::Buffer buff(expected.size());
	oxReturnError(fs->read(path.c_str(), buff.data(), buff.size()));
	return OxError(buff == expected ? 0 : 1);
}

struct VerificationPair {
	ox::String path;
	ox::Buffer buff;
};

static ox::Error copy(ox::FileSystem *src, ox::FileSystem *dest, const ox::String &path) noexcept {
	oxOutf("copying directory: {}\n", path);
	ox::Vector<VerificationPair> verificationPairs;
	// copy
	oxRequire(fileList, src->ls(path));
	for (const auto &name : fileList) {
		const auto currentFile = path + name;
		if (currentFile == "/.nostalgia") {
			continue;
		}
		oxOutf("reading {}\n", name);
		oxRequire(stat, src->stat(currentFile.c_str()));
		if (stat.fileType == ox::FileType::Directory) {
			oxReturnError(dest->mkdir(currentFile.c_str(), true));
			oxReturnError(copy(src, dest, currentFile + '/'));
		} else {
			// load file
			oxRequireM(buff, src->read(currentFile.c_str()));
			// write file to dest
			oxOutf("writing {}\n", currentFile);
			oxReturnError(dest->write(currentFile.c_str(), buff.data(), buff.size()));
			oxReturnError(verifyFile(dest, currentFile, buff));
			verificationPairs.emplace_back(currentFile, ox::move(buff));
		}
	}
	// verify all at once in addition to right after the files are written
	for (const auto &v : verificationPairs) {
		oxReturnError(verifyFile(dest, v.path, v.buff));
	}
	return OxError(0);
}

ox::Error pack(ox::FileSystem *src, ox::FileSystem *dest) noexcept {
	oxReturnError(copy(src, dest, "/"));
	oxReturnError(transformClaw(dest, "/"));
	return OxError(0);
}

}
