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
		ox::fs::LinkedList<size_t> *m_linkedList = nullptr;

	public:
		FileStoreTemplate(void *buff);

		bool valid(InodeId_t size);

		void resize(InodeId_t size = 0);

		Error write(InodeId_t id, void *data, InodeId_t dataLen, uint8_t fileType = 0);

		Error incLinks(InodeId_t id);

		Error decLinks(InodeId_t id);

		Error read(InodeId_t id, void *data, InodeId_t *size);

		Error read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size);

		StatInfo stat(InodeId_t id);

		InodeId_t spaceNeeded(InodeId_t size);

		InodeId_t size();

		InodeId_t available();

};

template<typename size_t>
FileStoreTemplate<size_t>::FileStoreTemplate(void *buff) {
	m_linkedList = static_cast<ox::fs::LinkedList<size_t>*>(buff);
}

template<typename size_t>
bool FileStoreTemplate<size_t>::valid(size_t size) {
	return false;
}

void FileStoreTemplate<size_t>::resize(InodeId_t size = 0) {
}

Error FileStoreTemplate<size_t>::write(InodeId_t id, void *data, InodeId_t dataLen, uint8_t fileType = 0) {
	return 1;
}

Error FileStoreTemplate<size_t>::incLinks(InodeId_t id) {
	return 1;
}

Error FileStoreTemplate<size_t>::decLinks(InodeId_t id) {
	return 1;
}

Error FileStoreTemplate<size_t>::read(InodeId_t id, void *data, InodeId_t *size) {
	return 1;
}

Error FileStoreTemplate<size_t>::read(InodeId_t id, InodeId_t readStart, InodeId_t readSize, void *data, InodeId_t *size) {
	return 1;
}

StatInfo FileStoreTemplate<size_t>::stat(InodeId_t id) {
	return {};
}

InodeId_t FileStoreTemplate<size_t>::spaceNeeded(InodeId_t size) {
	return 1;
}

InodeId_t FileStoreTemplate<size_t>::size() {
	return 1;
}

InodeId_t FileStoreTemplate<size_t>::available() {
	return 1;
}

}
