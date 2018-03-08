/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "filestore.hpp"
#include "linkedlist.hpp"

namespace ox::fs {

template<typename size_t>
class FileStoreTemplate: public FileStore {

	private:
		struct __attribute__((packed)) FileStoreData {
			ox::LittleEndian<size_t> rootNode = sizeof(LinkedList<size_t, Item>);
		};

		size_t m_buffSize = 0;
		ox::fs::LinkedList<size_t, Item> *m_linkedList = nullptr;

	public:
		FileStoreTemplate(void *buff, size_t buffSize);

		Error format();

		Error setSize(InodeId_t buffSize);

		Error write(InodeId_t id, void *data, InodeId_t dataLen, uint8_t fileType = 0);

		Error incLinks(InodeId_t id);

		Error decLinks(InodeId_t id);

		Error read(InodeId_t id, void *data, InodeId_t *size);

		Error read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size);

		StatInfo stat(InodeId_t id);

		InodeId_t spaceNeeded(InodeId_t size);

		InodeId_t size();

		InodeId_t available();

	private:
		FileStoreData &fileStoreData();

};

template<typename size_t>
FileStoreTemplate<size_t>::FileStoreTemplate(void *buff, size_t buffSize) {
	m_buffSize = buffSize;
	m_linkedList = static_cast<ox::fs::LinkedList<size_t, Item>*>(buff);
	if (!m_linkedList->valid(buffSize)) {
		m_buffSize = 0;
		m_linkedList = nullptr;
	}
}

template<typename size_t>
Error FileStoreTemplate<size_t>::format() {
	Error err = 0;
	auto data = m_linkedList->malloc(sizeof(FileStoreData));
	if (data.valid()) {
		new (data->data()) FileStoreData;
	} else {
		err = 1;
	}
	return err;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::setSize(InodeId_t size) {
	Error err = 0;
	if (m_buffSize >= size) {
		err |= m_linkedList->setSize(size);
	} else {
		err = 1;
	}
	return err;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::write(InodeId_t id, void *data, InodeId_t dataLen, uint8_t fileType) {
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::incLinks(InodeId_t id) {
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::decLinks(InodeId_t id) {
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, void *data, InodeId_t *size) {
	return 1;
}

template<typename size_t>
Error FileStoreTemplate<size_t>::read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size) {
	return 1;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::StatInfo FileStoreTemplate<size_t>::stat(InodeId_t id) {
	return {};
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::spaceNeeded(InodeId_t size) {
	return 1;
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::size() {
	return 1;
}

template<typename size_t>
InodeId_t FileStoreTemplate<size_t>::available() {
	return 1;
}

template<typename size_t>
typename FileStoreTemplate<size_t>::FileStoreData &FileStoreTemplate<size_t>::fileStoreData() {
	return *reinterpret_cast<FileStoreData*>(m_linkedList->firstItem());
}

using FileStore16 = FileStoreTemplate<uint16_t>;
using FileStore32 = FileStoreTemplate<uint32_t>;
using FileStore64 = FileStoreTemplate<uint64_t>;

}
