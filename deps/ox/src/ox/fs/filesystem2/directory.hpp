/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filesystem/pathiterator.hpp>
#include <ox/fs/filestore/filestore.hpp>
#include <ox/ptrarith/nodebuffer.hpp>
#include <ox/std/std.hpp>

namespace ox::fs {

template<typename InodeId_t>
struct __attribute__((packed)) DirectoryEntry {

	// NodeBuffer fields
	LittleEndian<InodeId_t> prev = 0;
	LittleEndian<InodeId_t> next = 0;

	// DirectoryEntry fields
	LittleEndian<InodeId_t> inode = 0;
	BString<MaxFileNameLength> name;

	DirectoryEntry() = default;

	explicit DirectoryEntry(const char *name) {
		this->name = name;
	}

	/**
	 * @return the size of the data + the size of the Item type
	 */
	InodeId_t fullSize() const {
		return offsetof(DirectoryEntry, name) + name.size();
	}

	InodeId_t size() const {
		return fullSize() - offsetof(DirectoryEntry, inode);
	}

	void setSize(InodeId_t) {
		// ignore set value
	}

	ptrarith::Ptr<uint8_t, InodeId_t> data() {
		return ptrarith::Ptr<uint8_t, InodeId_t>(this, this->size(), sizeof(*this), this->size() - sizeof(*this));
	}

};


template<typename InodeId_t>
class Directory {

	private:
		using Buffer = ptrarith::NodeBuffer<InodeId_t, DirectoryEntry<InodeId_t>>;
		std::size_t m_size = 0;
		Buffer *m_buff = nullptr;
		FileStore *m_fs = nullptr;

	public:
		Directory(fs::FileStore *fs, InodeId_t inode);

		/**
		 * Initializes Directory.
		 */
		Error init() noexcept;

		Error write(PathIterator it, InodeId_t inode) noexcept;

		Error rm(PathIterator it) noexcept;

		ValErr<InodeId_t> find(PathIterator it) const noexcept;

};

template<typename InodeId_t>
Directory<InodeId_t>::Directory(fs::FileStore *fs, InodeId_t) {
	m_fs = fs;
	//m_size = size;
	//m_buff = reinterpret_cast<decltype(m_buff)>(buff);
}

template<typename InodeId_t>
Error Directory<InodeId_t>::init() noexcept {
	if (m_size >= sizeof(Buffer)) {
		new (m_buff) Buffer(m_size);
		return 0;
	}
	return 1;
}

template<typename InodeId_t>
Error Directory<InodeId_t>::write(PathIterator path, InodeId_t inode) noexcept {
	// find existing entry and update if exists

	if (!path.hasNext()) {
		BString<MaxFileNameLength> name;
		path.next(&name);

		auto val = m_buff->malloc(name.size());
		if (val.valid()) {
			new (val) DirectoryEntry<InodeId_t>(name.data());
			val->name = name;
			val->inode = inode;
			return 0;
		}
		return 1;
	}

	// TODO: get sub directory and call its write instead of recursing on this directory
	return write(path + 1, inode);
}

template<typename InodeId_t>
Error Directory<InodeId_t>::rm(PathIterator) noexcept {
	return 1;
}

template<typename InodeId_t>
ValErr<InodeId_t> Directory<InodeId_t>::find(PathIterator it) const noexcept {
	auto size = it.nextSize();
	char name[size + 1];
	it.next(name, size);
	for (auto i = m_buff->iterator(); i.hasNext(); i.next()) {
		if (i->name == name) {
			return static_cast<InodeId_t>(i->inode);
		}
	}
	return {0, 1};
}


extern template class Directory<uint16_t>;
extern template class Directory<uint32_t>;

extern template struct DirectoryEntry<uint16_t>;
extern template struct DirectoryEntry<uint32_t>;

using Directory16 = Directory<uint16_t>;
using Directory32 = Directory<uint32_t>;

}
