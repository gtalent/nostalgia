/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "nodebuffer.hpp"

namespace ox::fs {

using InodeId_t = uintptr_t;

class FileStore {

	public:
		struct StatInfo {
			InodeId_t inodeId;
			InodeId_t links;
			InodeId_t size;
			uint8_t fileType;
		};

		virtual Error format() = 0;

		virtual Error setSize(InodeId_t size) = 0;

		virtual Error write(InodeId_t id, void *data, InodeId_t dataLen, uint8_t fileType = 0) = 0;

		virtual Error incLinks(InodeId_t id) = 0;

		virtual Error decLinks(InodeId_t id) = 0;

		virtual Error read(InodeId_t id, void *data, InodeId_t *size) = 0;

		virtual Error read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size) = 0;

		virtual StatInfo stat(InodeId_t id) = 0;

		virtual InodeId_t spaceNeeded(InodeId_t size) = 0;

		virtual InodeId_t size() = 0;

		virtual InodeId_t available() = 0;

};

}
