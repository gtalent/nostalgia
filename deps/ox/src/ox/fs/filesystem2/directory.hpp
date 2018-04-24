/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filesystem/pathiterator.hpp>
#include <ox/fs/filestore.hpp>
#include <ox/ptrarith/nodebuffer.hpp>
#include <ox/std/std.hpp>

namespace ox::fs {

template<typename InodeId_t>
struct __attribute__((packed)) DirectoryEntry {

	// NodeBuffer fields
	ox::LittleEndian<std::size_t> prev = 0;
	ox::LittleEndian<std::size_t> next = 0;

	// DirectoryEntry fields
	ox::LittleEndian<InodeId_t> inode = 0;
	BString<255> name;

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

	public:
		Directory(uint8_t *buff, std::size_t size);

		/**
		 * Initializes Directory.
		 */
		Error init() noexcept;

		Error write(PathIterator it, InodeId_t inode) noexcept;

		Error rm(PathIterator it) noexcept;

		ValErr<InodeId_t> find(PathIterator it) const noexcept;

};

template<typename InodeId_t>
Directory<InodeId_t>::Directory(uint8_t *buff, std::size_t size) {
	m_size = size;
	m_buff = reinterpret_cast<decltype(m_buff)>(buff);
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
Error Directory<InodeId_t>::write(PathIterator it, InodeId_t inode) noexcept {
	if (it.hasNext()) {
		return write(it + 1, inode);
	} else {
		auto current = find(it);
		if (current.ok()) {
		} else {
		}
	}
	return 1;
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
