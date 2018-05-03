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

	struct __attribute__((packed)) DirectoryEntryData {
		// DirectoryEntry fields
		LittleEndian<InodeId_t> inode = 0;
		BString<MaxFileNameLength> name;
	};

	// NodeBuffer fields
	LittleEndian<InodeId_t> prev = 0;
	LittleEndian<InodeId_t> next = 0;

	DirectoryEntry() = default;

	explicit DirectoryEntry(InodeId_t inode, const char *name) {
		auto d = data();
		if (d.valid()) {
			d->inode = inode;
			d->name = name;
		}
	}

	ptrarith::Ptr<DirectoryEntryData, InodeId_t> data() {
		return ptrarith::Ptr<DirectoryEntryData, InodeId_t>(this, this->fullSize(), sizeof(*this), this->size());
	}

	const ptrarith::Ptr<DirectoryEntryData, InodeId_t> data() const {
		return ptrarith::Ptr<DirectoryEntryData, InodeId_t>(const_cast<DirectoryEntry*>(this), this->fullSize(), sizeof(*this), this->size());
	}

	/**
	 * @return the size of the data + the size of the Item type
	 */
	InodeId_t fullSize() const {
		auto d = data();
		if (d.valid()) {
			return sizeof(*this) + offsetof(DirectoryEntryData, name) + d->name.size();
		}
		return 0;
	}

	InodeId_t size() const {
		return fullSize() - offsetof(DirectoryEntryData, inode);
	}

	void setSize(InodeId_t) {
		// ignore set value
	}

	static std::size_t spaceNeeded(std::size_t chars) {
		return sizeof(DirectoryEntry) + chars;
	}

};


template<typename InodeId_t>
class Directory {

	private:
		using Buffer = ptrarith::NodeBuffer<InodeId_t, DirectoryEntry<InodeId_t>>;

		InodeId_t m_inodeId = 0;
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
Directory<InodeId_t>::Directory(fs::FileStore *fs, InodeId_t id) {
	m_fs = fs;
	m_inodeId = id;
	auto buff = fs->read(id).template to<Buffer>();
	if (buff.valid()) {
		m_size = buff.size();
		m_buff = buff;
	}
}

template<typename InodeId_t>
Error Directory<InodeId_t>::init() noexcept {
	constexpr auto Size = sizeof(Buffer);
	m_fs->write(m_inodeId, nullptr, Size);
	auto buff = m_fs->read(m_inodeId);
	if (buff.valid()) {
		new (buff) Buffer(Size);
		return 0;
	}
	return 1;
}

template<typename InodeId_t>
Error Directory<InodeId_t>::write(PathIterator path, InodeId_t inode) noexcept {
	// find existing entry and update if exists

	if (!path.hasNext()) {
		Error err = 1;
		BString<MaxFileNameLength> name;
		path.next(&name);

		// find existing version of directory
		auto old = m_fs->read(m_inodeId);
		if (old.valid()) {
			const auto newSize = m_size + DirectoryEntry<InodeId_t>::spaceNeeded(name.size());
			auto cpy = new (ox_malloca(newSize)) Buffer(old);
			cpy->setSize(newSize);
			auto val = cpy->malloc(name.size());
			if (val.valid()) {
				new (val) DirectoryEntry<InodeId_t>(inode, name.data());
				err = m_fs->write(m_inodeId, cpy, cpy->size());
			}
			ox_freea(newSize, cpy);
		}
		return err;
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
	ValErr<InodeId_t> retval = {0, 1};
	auto size = it.nextSize();
	auto name = reinterpret_cast<char*>(ox_alloca(size + 1));
	it.next(name, size);
	auto buff = m_fs->read(m_inodeId).template to<Buffer>();
	for (auto i = buff->iterator(); i.hasNext(); i.next()) {
		auto data = i->data();
		if (data.valid() && data->name == name) {
			retval = static_cast<InodeId_t>(data->inode);
		}
	}
	return retval;
}


extern template class Directory<uint16_t>;
extern template class Directory<uint32_t>;

extern template struct DirectoryEntry<uint16_t>;
extern template struct DirectoryEntry<uint32_t>;

using Directory16 = Directory<uint16_t>;
using Directory32 = Directory<uint32_t>;

}
