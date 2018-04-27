/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/ptrarith/nodebuffer.hpp>

namespace ox::fs {

using InodeId_t = uintptr_t;
using FsSize_t = uintptr_t;

class FileStore {

	public:
		struct StatInfo {
			InodeId_t inodeId = 0;
			InodeId_t links = 0;
			InodeId_t size = 0;
			uint8_t fileType = 0;
		};

		virtual Error format() = 0;

		virtual Error setSize(InodeId_t size) = 0;

		virtual Error write(InodeId_t id, void *data, FsSize_t dataLen, uint8_t fileType = 0) = 0;

		virtual Error incLinks(InodeId_t id) = 0;

		virtual Error decLinks(InodeId_t id) = 0;

		virtual Error read(InodeId_t id, void *data, FsSize_t dataSize, FsSize_t *size) = 0;

		virtual Error read(InodeId_t id, FsSize_t readStart, FsSize_t readSize, void *data, FsSize_t *size) = 0;

		virtual ValErr<const uint8_t*> read(InodeId_t id) = 0;

		virtual StatInfo stat(InodeId_t id) = 0;

		virtual InodeId_t spaceNeeded(FsSize_t size) = 0;

		virtual InodeId_t size() = 0;

		virtual InodeId_t available() = 0;

};

}
