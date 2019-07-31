/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filesystem/pathiterator.hpp>
#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/ptrarith/nodebuffer.hpp>
#include <ox/std/byteswap.hpp>

namespace ox {

template<typename InodeId_t>
struct __attribute__((packed)) DirectoryEntry {

	public:
		struct __attribute__((packed)) DirectoryEntryData {
			// DirectoryEntry fields
			LittleEndian<InodeId_t> inode = 0;
			char name[MaxFileNameLength];

			static constexpr std::size_t spaceNeeded(std::size_t chars) {
				return offsetof(DirectoryEntryData, name) + chars;
			}

		};

		// NodeBuffer fields
		LittleEndian<InodeId_t> prev = 0;
		LittleEndian<InodeId_t> next = 0;


	private:
		LittleEndian<InodeId_t> m_bufferSize = sizeof(DirectoryEntry);

	public:
		DirectoryEntry() = default;

		[[nodiscard]] ox::Error init(InodeId_t inode, const char *name, InodeId_t bufferSize) {
			m_bufferSize = bufferSize;
			auto d = data();
			if (d.valid()) {
				d->inode = inode;
				ox_strncpy(d->name, name, ox::min(bufferSize, static_cast<InodeId_t>(MaxFileNameLength)));
				return OxError(0);
			}
			return OxError(1);
		}

		ptrarith::Ptr<DirectoryEntryData, InodeId_t> data() {
			oxTrace("ox::fs::DirectoryEntry::data") << this->fullSize() << sizeof(*this) << this->size();
			return ptrarith::Ptr<DirectoryEntryData, InodeId_t>(this, this->fullSize(), sizeof(*this), this->size(), this->size());
		}

		/**
		 * @return the size of the data + the size of the Item type
		 */
		InodeId_t fullSize() const {
			return m_bufferSize;
		}

		InodeId_t size() const {
			return fullSize() - sizeof(*this);
		}

		void setSize(InodeId_t) {
			// ignore set value
		}

		static constexpr std::size_t spaceNeeded(std::size_t chars) {
			return sizeof(DirectoryEntry) + offsetof(DirectoryEntryData, name) + chars;
		}

};


template<typename FileStore, typename InodeId_t>
class Directory {

	private:
		using Buffer = ptrarith::NodeBuffer<InodeId_t, DirectoryEntry<InodeId_t>>;

		InodeId_t m_inodeId = 0;
		std::size_t m_size = 0;
		FileStore m_fs;

	public:
		Directory() = default;

		Directory(FileStore fs, InodeId_t inode);

		/**
		 * Initializes Directory.
		 */
		[[nodiscard]] ox::Error init() noexcept;

		[[nodiscard]] ox::Error mkdir(PathIterator path, bool parents, FileName *nameBuff = nullptr);

		/**
		 * @param parents indicates the operation should create non-existent directories in the path, like mkdir -p
		 */
		[[nodiscard]] ox::Error write(PathIterator it, InodeId_t inode, FileName *nameBuff = nullptr) noexcept;

		[[nodiscard]] ox::Error remove(PathIterator it, FileName *nameBuff = nullptr) noexcept;

		template<typename F>
		[[nodiscard]] ox::Error ls(F cb) noexcept;

		[[nodiscard]] ValErr<typename FileStore::InodeId_t> findEntry(const FileName &name) const noexcept;

		[[nodiscard]] ValErr<typename FileStore::InodeId_t> find(PathIterator name, FileName *nameBuff = nullptr) const noexcept;

};

template<typename FileStore, typename InodeId_t>
Directory<FileStore, InodeId_t>::Directory(FileStore fs, InodeId_t inodeId) {
	m_fs = fs;
	m_inodeId = inodeId;
	auto buff = m_fs.read(inodeId).template to<Buffer>();
	if (buff.valid()) {
		m_size = buff.size();
	}
}

template<typename FileStore, typename InodeId_t>
ox::Error Directory<FileStore, InodeId_t>::init() noexcept {
	constexpr auto Size = sizeof(Buffer);
	oxTrace("ox::fs::Directory::init") << "Initializing Directory with Inode ID:" << m_inodeId;
	oxReturnError(m_fs.write(m_inodeId, nullptr, Size));
	auto buff = m_fs.read(m_inodeId).template to<Buffer>();
	if (buff.valid()) {
		new (buff) Buffer(Size);
		m_size = Size;
		return OxError(0);
	}
	m_size = 0;
	return OxError(1);
}

template<typename FileStore, typename InodeId_t>
ox::Error Directory<FileStore, InodeId_t>::mkdir(PathIterator path, bool parents, FileName *nameBuff) {
	if (path.valid()) {
		oxTrace("ox::fs::Directory::mkdir") << path.fullPath();
		// reuse nameBuff if it has already been allocated, as it is a rather large variable
		if (nameBuff == nullptr) {
			nameBuff = reinterpret_cast<FileName*>(ox_alloca(sizeof(FileName)));
		}

		// determine if already exists
		auto name = nameBuff;
		path.get(name);
		auto childInode = find(name->c_str());
		if (!childInode.ok()) {
			// if this is not the last item in the path and parents is disabled,
			// return an error
			if (!parents && path.hasNext()) {
				return OxError(1);
			}
			childInode = m_fs.generateInodeId();
			oxTrace("ox::fs::Directory::mkdir") << "Generated Inode ID:" << childInode.value;
			oxLogError(childInode.error);
			oxReturnError(childInode.error);

			// initialize the directory
			Directory<FileStore, InodeId_t> child(m_fs, childInode.value);
			oxReturnError(child.init());

			auto err = write(name->c_str(), childInode.value);
			if (err) {
				oxLogError(err);
				// could not index the directory, delete it
				oxLogError(m_fs.remove(childInode.value));
				return err;
			}
		}

		Directory<FileStore, InodeId_t> child(m_fs, childInode.value);
		if (path.hasNext()) {
			oxReturnError(child.mkdir(path.next(), parents, nameBuff));
		}
	}
	return OxError(0);
}

template<typename FileStore, typename InodeId_t>
ox::Error Directory<FileStore, InodeId_t>::write(PathIterator path, InodeId_t inode, FileName *nameBuff) noexcept {
	// reuse nameBuff if it has already been allocated, as it is a rather large variable
	if (nameBuff == nullptr) {
		nameBuff = reinterpret_cast<FileName*>(ox_alloca(sizeof(FileName)));
	}
	auto name = nameBuff;

	if (path.next().hasNext()) { // not yet at target directory, recurse to next one
		oxReturnError(path.get(name));

		oxTrace("ox::fs::Directory::write") << "Attempting to write to next sub-Directory: "
			<< name->c_str() << " of " << path.fullPath();

		auto [nextChild, err] = findEntry(*name);
		oxReturnError(err);

		oxTrace("ox::fs::Directory::write") << name->c_str() << ": " << nextChild;

		if (nextChild) {
			// reuse name because it is a rather large variable and will not be used again
			// be attentive that this remains true
			name = nullptr;
			return Directory(m_fs, nextChild).write(path.next(), inode, nameBuff);
		} else {
			oxTrace("ox::fs::Directory::write") << name->c_str()
				<< "not found and not allowed to create it.";
			return OxError(1);
		}
	} else {
		// insert the new entry on this directory

		// get the name
		path.next(name);

		oxTrace("ox::fs::Directory::write") << "Attempting to write Directory to FileStore";

		// find existing version of directory
		oxTrace("ox::fs::Directory::write") << "Searching for inode" << m_inodeId;
		auto old = m_fs.read(m_inodeId);
		if (old.valid()) {
			const auto entrySize = DirectoryEntry<InodeId_t>::spaceNeeded(name->len());
			const auto entryDataSize =DirectoryEntry<InodeId_t>::DirectoryEntryData::spaceNeeded(name->len());
			const auto newSize = m_size + entrySize;
			auto cpy = ox_malloca(newSize, Buffer, old);
			if (cpy != nullptr) {
				// TODO: look for old version of this entry and delete it

				oxReturnError(cpy->setSize(newSize));
				auto val = cpy->malloc(entryDataSize);
				if (val.valid()) {
					oxTrace("ox::fs::Directory::write") << "Attempting to write Directory entry:" << name->data();
					oxTrace("ox::fs::Directory::write") << "Attempting to write Directory to FileStore";
					oxReturnError(val->init(inode, name->data(), entrySize));
					return m_fs.write(m_inodeId, cpy, cpy->size());
				} else {
					oxTrace("ox::fs::Directory::write::fail") << "Could not allocate memory for new directory entry";
					return OxError(1);
				}
			} else {
				oxTrace("ox::fs::Directory::write::fail") << "Could not allocate memory for copy of Directory";
				return OxError(1);
			}
		} else {
			oxTrace("ox::fs::Directory::write::fail") << "Could not read existing version of Directory";
			return OxError(1);
		}
	}
	return OxError(1);
}

template<typename FileStore, typename InodeId_t>
ox::Error Directory<FileStore, InodeId_t>::remove(PathIterator path, FileName *nameBuff) noexcept {
	// reuse nameBuff if it has already been allocated, as it is a rather large variable
	if (nameBuff == nullptr) {
		nameBuff = reinterpret_cast<FileName*>(ox_alloca(sizeof(FileName)));
	}
	auto &name = *nameBuff;
	oxReturnError(path.get(&name));

	oxTrace("ox::fs::Directory::remove") << name.c_str();
	auto buff = m_fs.read(m_inodeId).template to<Buffer>();
	if (buff.valid()) {
		oxTrace("ox::fs::Directory::remove") << "Found directory buffer.";
		for (auto i = buff->iterator(); i.valid(); i.next()) {
			auto data = i->data();
			if (data.valid()) {
				if (ox_strncmp(data->name, name.c_str(), name.len()) == 0) {
					oxReturnError(buff->free(i));
				}
			} else {
				oxTrace("ox::fs::Directory::remove") << "INVALID DIRECTORY ENTRY";
			}
		}
	} else {
		oxTrace("ox::fs::Directory::remove::fail") << "Could not find directory buffer";
		return OxError(1);
	}
	return OxError(0);
}

template<typename FileStore, typename InodeId_t>
template<typename F>
ox::Error Directory<FileStore, InodeId_t>::ls(F cb) noexcept {
	oxTrace("ox::fs::Directory::ls");
	auto buff = m_fs.read(m_inodeId).template to<Buffer>();
	if (!buff.valid()) {
		oxTrace("ox::fs::Directory::ls::fail") << "Could not directory buffer";
		return OxError(1);
	}

	oxTrace("ox::fs::Directory::ls") << "Found directory buffer.";
	for (auto i = buff->iterator(); i.valid(); i.next()) {
		auto data = i->data();
		if (data.valid()) {
			oxReturnError(cb(data->name, data->inode));
		} else {
			oxTrace("ox::fs::Directory::ls") << "INVALID DIRECTORY ENTRY";
		}
	}

	return OxError(0);
}

template<typename FileStore, typename InodeId_t>
ValErr<typename FileStore::InodeId_t> Directory<FileStore, InodeId_t>::findEntry(const FileName &name) const noexcept {
	oxTrace("ox::fs::Directory::findEntry") << name.c_str();
	auto buff = m_fs.read(m_inodeId).template to<Buffer>();
	if (!buff.valid()) {
		oxTrace("ox::fs::Directory::findEntry::fail") << "Could not findEntry directory buffer";
		return {0, OxError(2)};
	}
	oxTrace("ox::fs::Directory::findEntry") << "Found directory buffer, size:" << buff.size();
	for (auto i = buff->iterator(); i.valid(); i.next()) {
		auto data = i->data();
		if (data.valid()) {
			oxTrace("ox::fs::Directory::findEntry").del("") << "Comparing \"" << name.c_str() << "\" to \"" << data->name << "\"";
			if (ox_strncmp(data->name, name.c_str(), name.len()) == 0) {
				oxTrace("ox::fs::Directory::findEntry").del("") << "\"" << name.c_str() << "\" match found.";
				return static_cast<InodeId_t>(data->inode);
			}
		} else {
			oxTrace("ox::fs::Directory::findEntry") << "INVALID DIRECTORY ENTRY";
		}
	}
	oxTrace("ox::fs::Directory::findEntry::fail") << "Entry not present";
	return {0, OxError(1)};
}

template<typename FileStore, typename InodeId_t>
ValErr<typename FileStore::InodeId_t> Directory<FileStore, InodeId_t>::find(PathIterator path, FileName *nameBuff) const noexcept {
	// reuse nameBuff if it has already been allocated, as it is a rather large variable
	if (nameBuff == nullptr) {
		nameBuff = reinterpret_cast<FileName*>(ox_alloca(sizeof(FileName)));
	}

	// determine if already exists
	auto name = nameBuff;
	if (path.get(name)) {
		return {0, OxError(1)};
	}

	auto v = findEntry(name->c_str());
	if (!v.error) {
		return v;
	}
	name = nullptr;
	v = find(path.next(), nameBuff);
	if (!v.error) {
		return v;
	}

	return {0, OxError(1)};
}


extern template class Directory<FileStore16, uint16_t>;
extern template class Directory<FileStore32, uint32_t>;

extern template struct DirectoryEntry<uint16_t>;
extern template struct DirectoryEntry<uint32_t>;

using Directory16 = Directory<FileStore16, uint16_t>;
using Directory32 = Directory<FileStore32, uint32_t>;

}
