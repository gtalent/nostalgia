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

[[nodiscard]] static constexpr bool endsWith(std::string_view str, std::string_view ending) {
	return str.size() >= ending.size() && str.substr(str.size() - ending.size()) == ending;
}

// stub for now
ox::Error pathToInode(std::vector<char>*) {
	return OxError(0);
}

// stub for now
ox::Error toMetalClaw(std::vector<char>*) {
	return OxError(0);
}

ox::Error copy(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path) {
	// copy
	src->ls(path.c_str(), [src, dest, path](const char *name, ox::InodeId_t) {
		auto stat = src->stat(path.c_str());
		oxReturnError(stat.error);
		if (stat.value.fileType == ox::FileType_Directory) {
			const auto dir = path + name + '/';
			oxReturnError(dest->mkdir(dir.c_str()));
			oxReturnError(copy(src, dest, dir));
		} else {
			std::vector<char> buff(stat.value.size);
			// load file
			oxReturnError(src->read(path.c_str(), buff.data(), buff.size()));
			// do transforms
			if (endsWith(path, ".claw")) {
				oxReturnError(pathToInode(&buff));
				oxReturnError(toMetalClaw(&buff));
			} else if (endsWith(path, ".png")) {
				const auto fullPath = src->basePath() + path;
				oxReturnError(pngToGba(fullPath.c_str(), 0, 0));
			}
			// write file to dest
			oxReturnError(dest->write(path.c_str(), buff.data(), buff.size()));
		}
		return OxError(0);
	});
	return OxError(0);
}

}
