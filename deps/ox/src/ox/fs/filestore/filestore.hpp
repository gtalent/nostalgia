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

using InodeId_t = uint64_t;
using FsSize_t = std::size_t;

class FileStore {

	public:
		struct StatInfo {
			InodeId_t inode = 0;
			InodeId_t links = 0;
			FsSize_t size = 0;
			uint8_t fileType = 0;
		};

		virtual Error format() = 0;

		virtual Error setSize(InodeId_t size) = 0;

		virtual Error incLinks(InodeId_t id) = 0;

		virtual Error decLinks(InodeId_t id) = 0;

		virtual Error write(InodeId_t id, void *data, FsSize_t dataLen, uint8_t fileType = 0) = 0;

		/**
		 * @return error if the inode could not be removed or did not exist
		 */
		virtual Error remove(InodeId_t id) = 0;

		virtual Error read(InodeId_t id, void *data, FsSize_t dataSize, std::size_t *size = nullptr) = 0;

		virtual Error read(InodeId_t id, FsSize_t readStart, FsSize_t readSize, void *data, FsSize_t *size = nullptr) = 0;

		/**
		 * @return data section of the requested file
		 */
		virtual const ptrarith::Ptr<uint8_t, std::size_t> read(InodeId_t id) = 0;

		virtual ValErr<StatInfo> stat(InodeId_t id) = 0;

		virtual Error resize(std::size_t size, void *newBuff = nullptr) = 0;

		virtual InodeId_t spaceNeeded(FsSize_t size) = 0;

		virtual InodeId_t size() = 0;

		virtual InodeId_t available() = 0;

		/**
		 * @return a pointer to the buffer of the file system, or null if not
		 * applicable
		 */
		virtual uint8_t *buff() = 0;

		virtual Error walk(Error(*cb)(uint8_t, uint64_t, uint64_t)) = 0;

		virtual ValErr<InodeId_t> generateInodeId() = 0;

};

}
