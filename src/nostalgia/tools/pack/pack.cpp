/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/claw/read.hpp>

#include "pack.hpp"

namespace nostalgia {

namespace {

/**
 * Convert path references to inodes to save space
 * @param buff buffer holding file
 * @return error
 * stub for now
 */
static ox::Error pathToInode(ox::Vector<uint8_t>*) noexcept {
	return OxError(0);
}

// just strip header for now...
static ox::Error toMetalClaw(ox::Vector<uint8_t> *buff) noexcept {
	auto [mc, err] = ox::stripClawHeader(ox::bit_cast<char*>(buff->data()), buff->size());
	oxReturnError(err);
	buff->resize(mc.size());
	ox_memcpy(buff->data(), mc.data(), mc.size());
	return OxError(0);
}

// claw file transformations are broken out because path to inode
// transformations need to be done after the copy to the new FS is complete
static ox::Error transformClaw(ox::FileSystem32 *dest, ox::String path) noexcept {
	// copy
	oxTrace("pack::transformClaw") << "path:" << path.c_str();
	return dest->ls(path.c_str(), [dest, path](const ox::String &name, ox::InodeId_t) {
		auto filePath = path + name;
		auto [stat, err] = dest->stat(filePath.c_str());
		oxReturnError(err);
		if (stat.fileType == ox::FileType_Directory) {
			const auto dir = path + name + '/';
			oxReturnError(transformClaw(dest, dir));
		} else {
			// do transforms
			if (name.endsWith(".ng") || name.endsWith(".npal")) {
				// load file
				ox::Vector<uint8_t> buff(stat.size);
				oxReturnError(dest->read(filePath.c_str(), buff.data(), buff.size()));
				// do transformations
				oxReturnError(pathToInode(&buff));
				oxReturnError(toMetalClaw(&buff));
				// write file to dest
				oxReturnError(dest->write(filePath.c_str(), buff.data(), buff.size()));
			}
		}
		return OxError(0);
	});
}

static ox::Error verifyFile(ox::FileSystem32 *fs, const ox::String &path, const ox::Vector<uint8_t> &expected) noexcept {
	ox::Vector<uint8_t> buff(expected.size());
	oxReturnError(fs->read(path.c_str(), buff.data(), buff.size()));
	return OxError(buff == expected ? 0 : 1);
}

struct VerificationPair {
	ox::String path;
	ox::Vector<uint8_t> buff;
};

static ox::Error copy(ox::PassThroughFS *src, ox::FileSystem32 *dest, ox::String path) noexcept {
	oxOutf("copying directory: {}\n", path);
	ox::Vector<VerificationPair> verificationPairs;
	// copy
	oxReturnError(src->ls(path.c_str(), [&verificationPairs, src, dest, path](const char *name, ox::InodeId_t) noexcept {
		auto currentFile = path + name;
		if (currentFile == "/.nostalgia") {
			return OxError(0);
		}
		oxOutf("reading {}\n", name);
		auto [stat, err] = src->stat((currentFile).c_str());
		oxReturnError(err);
		if (stat.fileType == ox::FileType_Directory) {
			oxReturnError(dest->mkdir(currentFile.c_str(), true));
			oxReturnError(copy(src, dest, currentFile + '/'));
		} else {
			ox::Vector<uint8_t> buff;
			// load file
			buff.resize(stat.size);
			oxReturnError(src->read(currentFile.c_str(), buff.data(), buff.size()));
			// write file to dest
			oxOutf("writing {}\n", currentFile);
			oxReturnError(dest->write(currentFile.c_str(), buff.data(), buff.size()));
			oxReturnError(verifyFile(dest, currentFile, buff));
			verificationPairs.push_back({currentFile, buff});
		}
		return OxError(0);
	}));
	// verify all at once in addition to right after the files are written
	for (auto i = 0u; i < verificationPairs.size(); ++i) {
		const auto &v = verificationPairs[i];
		oxReturnError(verifyFile(dest, v.path, v.buff));
	}
	return OxError(0);
}

}

ox::Error pack(ox::PassThroughFS *src, ox::FileSystem32 *dest) noexcept {
	oxReturnError(copy(src, dest, "/"));
	oxReturnError(transformClaw(dest, "/"));
	return OxError(0);
}

}
