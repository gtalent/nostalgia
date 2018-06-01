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

	public:
		struct __attribute__((packed)) DirectoryEntryData {
			// DirectoryEntry fields
			LittleEndian<InodeId_t> inode = 0;
			char name[MaxFileNameLength];
		};

		// NodeBuffer fields
		LittleEndian<InodeId_t> prev = 0;
		LittleEndian<InodeId_t> next = 0;


	private:
		LittleEndian<InodeId_t> m_bufferSize = sizeof(DirectoryEntry);

	public:
		DirectoryEntry() = default;

		DirectoryEntry(InodeId_t inode, const char *name, InodeId_t bufferSize) {
			auto d = data();
			if (d.valid()) {
				d->inode = inode;
				ox_strncpy(d->name, name, MaxFileNameLength);
				m_bufferSize = bufferSize;
			}
		}

		ptrarith::Ptr<DirectoryEntryData, InodeId_t> data() {
			return ptrarith::Ptr<DirectoryEntryData, InodeId_t>(this, this->fullSize(), sizeof(*this), this->size());
		}

		/**
		 * @return the size of the data + the size of the Item type
		 */
		InodeId_t fullSize() const {
			return m_bufferSize;
		}

		InodeId_t size() const {
			return fullSize() - offsetof(DirectoryEntryData, inode);
		}

		void setSize(InodeId_t) {
			// ignore set value
		}

		static std::size_t spaceNeeded(std::size_t chars) {
			return sizeof(DirectoryEntry) + offsetof(DirectoryEntryData, name) + chars;
		}

};


template<typename InodeId_t>
class Directory {

	private:
		using Buffer = ptrarith::NodeBuffer<InodeId_t, DirectoryEntry<InodeId_t>>;

		InodeId_t m_inodeId = 0;
		std::size_t m_size = 0;
		FileStore *m_fs = nullptr;

	public:
		Directory(FileStore *fs, InodeId_t inode);

		/**
		 * Initializes Directory.
		 */
		Error init() noexcept;

		Error write(PathIterator it, InodeId_t inode) noexcept;

		Error rm(PathIterator it) noexcept;

		ValErr<InodeId_t> find(const BString<MaxFileNameLength> &name) const noexcept;

};

template<typename InodeId_t>
Directory<InodeId_t>::Directory(FileStore *fs, InodeId_t inodeId) {
	m_fs = fs;
	m_inodeId = inodeId;
	auto buff = fs->read(inodeId).template to<Buffer>();
	if (buff.valid()) {
		m_size = buff.size();
	}
}

template<typename InodeId_t>
Error Directory<InodeId_t>::init() noexcept {
	constexpr auto Size = sizeof(Buffer);
	m_fs->write(m_inodeId, nullptr, Size);
	auto buff = m_fs->read(m_inodeId);
	if (buff.valid()) {
		new (buff) Buffer(Size);
		m_size = Size;
		return OxError(0);
	}
	m_size = 0;
	return OxError(1);
}

template<typename InodeId_t>
Error Directory<InodeId_t>::write(PathIterator path, InodeId_t inode) noexcept {
	InodeId_t nextChild = 0;
	if ((path + 1).hasNext()) {
		oxTrace("ox::fs::Directory::write") << "Attempting to write to next sub-Directory";

		// read the name and pop it off the stack as soon as possible to help
		// avoid a stack overflow in the recursive calls
		{
			BString<MaxFileNameLength> name;
			path.get(&name);
			nextChild = find(name);
		}

		// It's important for the recursive call to be outside the block where name
		// lived. This avoids allocation several BString<MaxFileNameLength>s on the
		// stack at once, while also avoiding the use of heap memory.
		if (nextChild) {
			return Directory(m_fs, nextChild).write(path + 1, inode);
		}
	} else {
		// insert the new entry on this directory

		// get the name
		BString<MaxFileNameLength> name;
		path.next(&name);

		// find existing version of directory
		auto old = m_fs->read(m_inodeId);
		if (old.valid()) {
			const auto newSize = m_size + DirectoryEntry<InodeId_t>::spaceNeeded(name.size());
			auto cpy = ox_malloca(newSize, Buffer, old);
			if (cpy != nullptr) {
				// TODO: look for old version of this entry and delete it

				cpy->setSize(newSize);
				auto val = cpy->malloc(name.size());
				if (val.valid()) {
					oxTrace("ox::fs::Directory::write") << "Attempting to write Directory to FileStore";
					new (val) DirectoryEntry<InodeId_t>(inode, name.data(), newSize);
					return m_fs->write(m_inodeId, cpy, cpy->size());
				} else {
					oxTrace("ox::fs::Directory::write::fail") << "Could not allocate memory for new directory entry";
				}
			} else {
				oxTrace("ox::fs::Directory::write::fail") << "Could not allocate memory for copy of Directory";
			}
		} else {
			oxTrace("ox::fs::Directory::write::fail") << "Could not read existing version of Directory";
		}
	}
	return OxError(1);
}

template<typename InodeId_t>
Error Directory<InodeId_t>::rm(PathIterator) noexcept {
	return OxError(1);
}

template<typename InodeId_t>
ValErr<InodeId_t> Directory<InodeId_t>::find(const BString<MaxFileNameLength> &name) const noexcept {
	oxTrace("ox::fs::Directory::find") << name.c_str();
	auto buff = m_fs->read(m_inodeId).template to<Buffer>();
	if (buff.valid()) {
		oxTrace("ox::fs::Directory::find") << "Found directory buffer.";
		for (auto i = buff->iterator(); i.valid(); i.next()) {
			auto data = i->data();
			oxTrace("ox::fs::Directory::find::name") << name.c_str();
			if (data.valid() && data->name == name.c_str()) {
				return static_cast<InodeId_t>(data->inode);
			}
		}
		oxTrace("ox::fs::Directory::find::fail");
	} else {
		oxTrace("ox::fs::Directory::find::fail") << "Could not find directory buffer";
	}
	return {0, OxError(1)};
}


extern template class Directory<uint16_t>;
extern template class Directory<uint32_t>;

extern template struct DirectoryEntry<uint16_t>;
extern template struct DirectoryEntry<uint32_t>;

using Directory16 = Directory<uint16_t>;
using Directory32 = Directory<uint32_t>;

}
