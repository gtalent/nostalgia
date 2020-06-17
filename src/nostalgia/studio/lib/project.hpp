/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include <QSharedPointer>

#include <ox/fs/fs.hpp>
#include <ox/mc/mc.hpp>
#include <qnamespace.h>

#include <ox/claw/claw.hpp>
#include <ox/fs/filesystem/passthroughfs.hpp>
#include <ox/model/descwrite.hpp>

#include "nostalgiastudio_export.h"

namespace nostalgia::studio {

enum class ProjectEvent {
	FileAdded,
	// FileRecognized is triggered for all matching files upon a new
	// subscription to a section of the project and upon the addition of a file.
	FileRecognized,
	FileDeleted,
	FileUpdated,
};

[[nodiscard]] QString filePathToName(QString path, QString prefix, QString suffix);

class NOSTALGIASTUDIO_EXPORT Project: public QObject {
	Q_OBJECT

	private:
		QString m_path = "";
		mutable ox::PassThroughFS m_fs;

	public:
		Project(QString path);

		~Project();

		void create();

		ox::PassThroughFS *romFs();

		void mkdir(QString path) const;

		/**
		 * Writes a MetalClaw object to the project at the given path.
		 */
		template<typename T>
		void writeObj(QString path, T *obj) const;

		template<typename T>
		std::unique_ptr<T> loadObj(QString path) const;

		ox::FileStat stat(QString path) const;

		bool exists(QString path) const;

		void subscribe(ProjectEvent e, QObject *tgt, const char *slot) const;

		template<typename Functor>
		void subscribe(ProjectEvent e, QObject *tgt, Functor &&slot) const;

	private:
		void writeBuff(QString path, uint8_t *buff, size_t buffLen) const;

		std::vector<char> loadBuff(QString path) const;

		void procDir(QStringList &paths, QString path) const;

		QStringList listFiles(QString path = "") const;

	signals:
		void fileEvent(ProjectEvent, QString path) const;
		void fileAdded(QString) const;
		// FileRecognized is triggered for all matching files upon a new
		// subscription to a section of the project and upon the addition of a
		// file.
		void fileRecognized(QString) const;
		void fileDeleted(QString) const;
		void fileUpdated(QString) const;

};

template<typename T>
void Project::writeObj(QString path, T *obj) const {
	// write MetalClaw
	auto [buff, err] = ox::writeClaw(obj, ox::ClawFormat::Metal);
	oxThrowError(err);
	// write to FS
	writeBuff(path, ox::bit_cast<uint8_t*>(buff.data()), buff.size());

	// write type descriptor
	const auto type = ox::buildTypeDef(obj);
	auto typeOut = ox::writeClaw(type.value, ox::ClawFormat::Organic);
	oxThrowError(typeOut);
	// replace garbage last character with new line
	typeOut.value.back() = '\n';
	// write to FS
	QString descPath = "/.nostalgia/type_descriptors/";
	const auto typePath = descPath + type.value->typeName.c_str();
	mkdir(descPath);
	writeBuff(typePath, ox::bit_cast<uint8_t*>(typeOut.value.data()), typeOut.value.size());
	emit fileUpdated(path);
}

template<typename T>
std::unique_ptr<T> Project::loadObj(QString path) const {
	auto obj = std::make_unique<T>();
	auto buff = loadBuff(path);
	oxThrowError(ox::readClaw<T>(buff.data(), buff.size(), obj.get()));
	return obj;
}

template<typename Functor>
void Project::subscribe(ProjectEvent e, QObject *tgt, Functor &&slot) const {
	switch (e) {
		case ProjectEvent::FileAdded:
			connect(this, &Project::fileAdded, tgt, (slot));
			break;
		case ProjectEvent::FileRecognized:
		{
			for (auto f : listFiles()) {
				slot(f);
			}
			connect(this, &Project::fileRecognized, tgt, (slot));
			break;
		}
		case ProjectEvent::FileDeleted:
			connect(this, &Project::fileDeleted, tgt, (slot));
			break;
		case ProjectEvent::FileUpdated:
			connect(this, &Project::fileUpdated, tgt, (slot));
			break;
	}
}

}
