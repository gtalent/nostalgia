/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iomanip>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <ox/std/strops.hpp>
#include <ox/fs/filesystem.hpp>

#include "toollib.hpp"

// suppress warnings about using fopen
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace ox;
using namespace std;

const static auto oxfstoolVersion = "1.4.0";
const static auto usage = "usage:\n"
"\toxfs format [16,32,64] <size> <path>\n"
"\toxfs read <FS file> <inode>\n"
"\toxfs write <FS file> <inode> <insertion file>\n"
"\toxfs write-expand <FS file> <inode> <insertion file>\n"
"\toxfs rm <FS file> <inode>\n"
"\toxfs compact <FS file>\n"
"\toxfs walk <FS file>\n"
"\toxfs version\n";

size_t bytes(const char *str) {
	auto size = ::ox_strlen(str);
	const auto lastChar = str[size-1];
	auto multiplier = 1;
	char copy[size + 1];
	ox_memcpy(copy, str, size + 1);
	// parse size unit
	if (lastChar < '0' || lastChar > '9') {
		copy[size-1] = 0;
		switch (lastChar) {
			case 'k':
			case 'K':
				multiplier = 1024;
				break;
			case 'm':
			case 'M':
				multiplier = 1024 * 1024;
				break;
			case 'g':
			case 'G':
				multiplier = 1024 * 1024 * 1024;
				break;
			default:
				multiplier = -1;
		}
	}
	return ox_atoi(copy) * multiplier;
}

int format(int argc, char **args) {
	printf("Creating file system...\n");
	auto err = 0;
	if (argc >= 5) {
		auto type = ox_atoi(args[2]);
		auto size = bytes(args[3]);
		auto path = args[4];
		auto buff = new uint8_t[size];


		if (size < sizeof(FileStore64)) {
			err = 1;
			cerr <<  "File system size " << size << " too small, must be at least " << sizeof(FileStore64) << endl;
		}

		if (!err) {
			// format
			switch (type) {
				case 16:
					FileSystem16::format(buff, (FileStore16::FsSize_t) size, true);
					break;
				case 32:
					FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
					break;
				case 64:
					FileSystem64::format(buff, size, true);
					break;
				default:
					err = 1;
			}

			if (!err) {
				auto file = fopen(path, "wb");
				if (file) {
					err = fwrite(buff, size, 1, file) != 1;
					err |= fclose(file);
					if (err) {
						fprintf(stderr, "Could not write to file: %s.\n", path);
					}
				} else {
					fprintf(stderr, "Could not open file: %s.\n", path);
				}
			}
		}

		delete []buff;

		if (err == 0) {
			cerr <<  "Created file system " << path << endl;
			cerr <<  "        type " << type << endl;
			cerr <<  "        wrote " << size << " bytes\n";
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}

	return err;
}

int read(int argc, char **args) {
	auto err = 1;
	if (argc >= 4) {
		auto fsPath = args[2];
		auto inode = ox_atoi(args[3]);
		size_t fsSize;
		size_t fileSize;

		auto fsBuff = loadFileBuff(fsPath, &fsSize);

		if (fsBuff) {
			auto fs = createFileSystem(fsBuff, fsSize);

			if (fs) {
				auto output = fs->read(inode, &fileSize);

				if (output) {
					fwrite(output, fileSize, 1, stdout);
					delete []output;
					err = 0;
				}

				delete fs;
				delete []fsBuff;
			} else {
				fprintf(stderr, "Invalid file system type: %d.\n", *(uint32_t*) fsBuff);
			}
		} else {
			fprintf(stderr, "Could not open file: %s\n", fsPath);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int write(int argc, char **args, bool expand) {
	auto err = 0;
	if (argc >= 5) {
		auto fsPath = args[2];
		auto inode = ox_atoi(args[3]);
		auto srcPath = args[4];
		size_t srcSize;

		auto fsFile = fopen(fsPath, "rb");
		if (fsFile) {
			fseek(fsFile, 0, SEEK_END);

			auto fsSize = (size_t) ftell(fsFile);
			rewind(fsFile);
			auto fsBuff = new uint8_t[fsSize];
			auto itemsRead = fread(fsBuff, fsSize, 1, fsFile);
			fclose(fsFile);

			if (itemsRead) {
				auto srcBuff = loadFileBuff(srcPath, &srcSize);
				if (srcBuff) {
					auto expanded = false;
					auto fs = createFileSystem(fsBuff, fsSize);
					if (fs) {
						if (expand && fs->available() <= srcSize) {
							auto needed = fs->size() + fs->spaceNeeded(srcSize);
							fsSize = needed;
							fs = expandCopyCleanup(fs, needed);
							fsBuff = fs->buff();
						}
						err |= fs->write(inode, srcBuff, srcSize);

						// compact the file system if it was expanded
						if (expanded) {
							fs->resize();
						}

						if (err) {
							fprintf(stderr, "Could not write to file system.\n");
						}
						delete fs;
					} else {
						fprintf(stderr, "Invalid file system type: %d.\n", *(uint32_t*) fsBuff);
						err = 1;
					}

					if (!err) {
						fsFile = fopen(fsPath, "wb");

						if (fsFile) {
							err = fwrite(fsBuff, fsSize, 1, fsFile) != 1;
							err |= fclose(fsFile);
							if (err) {
								fprintf(stderr, "Could not write to file system file.\n");
							}
						} else {
							err = 1;
						}
					}
					delete []srcBuff;
				} else {
					err = 1;
					fprintf(stderr, "Could not load source file: %s.\n", srcPath);
				}
			}

			delete []fsBuff;
		} else {
			fprintf(stderr, "Could not open file system\n");
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int compact(int argc, char **args) {
	auto err = 1;
	if (argc >= 2) {
		auto fsPath = args[2];
		size_t fsSize;

		auto fsBuff = loadFileBuff(fsPath, &fsSize);
		if (fsBuff) {
			auto fs = createFileSystem(fsBuff, fsSize);

			if (fs) {
				fs->resize();
			} else {
				fprintf(stderr, "Invalid file system.\n");
			}

			// write back to file
			auto fsFile = fopen(fsPath, "wb");
			if (fsFile) {
				err = fwrite(fsBuff, fs->size(), 1, fsFile) != 1;
				err |= fclose(fsFile);
				if (err) {
					fprintf(stderr, "Could not write to file system file.\n");
				}
			} else {
				err = 1;
			}

			delete fs;
			delete []fsBuff;
		} else {
			fprintf(stderr, "Could not open file: %s\n", fsPath);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int remove(int argc, char **args) {
	auto err = 1;
	if (argc >= 4) {
		auto fsPath = args[2];
		auto inode = ox_atoi(args[3]);
		size_t fsSize;

		auto fsBuff = loadFileBuff(fsPath, &fsSize);
		if (fsBuff) {
			auto fs = createFileSystem(fsBuff, fsSize);

			if (fs) {
				err = fs->remove(inode);
			} else {
				fprintf(stderr, "Invalid file system.\n");
			}

			if (err) {
				fprintf(stderr, "Could not write to file system.\n");
			} else {
				auto fsFile = fopen(fsPath, "wb");
				if (fsFile) {
					err = fwrite(fsBuff, fsSize, 1, fsFile) != 1;
					err |= fclose(fsFile);
					if (err) {
						fprintf(stderr, "Could not write to file system file.\n");
					}
				} else {
					err = 1;
				}
			}

			delete fs;
			delete []fsBuff;
		} else {
			fprintf(stderr, "Could not open file: %s\n", fsPath);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int walk(int argc, char **args) {
	int err = 0;
	size_t fsSize;
	auto fsPath = args[2];
	auto fsBuff = loadFileBuff(fsPath, &fsSize);
	if (fsBuff) {
		auto fs = createFileSystem(fsBuff, fsSize);
		if (fs) {
			cout << setw(9) << "Type |";
			cout << setw(10) << "Start |";
			cout << setw(10) << "End |";
			cout << setw(8) << "Size";
			cout << endl;
			cout << "-------------------------------------";
			cout << endl;
			fs->walk([](const char *type, uint64_t start, uint64_t end) {
				cout << setw(7) << type << " |";
				cout << setw(8) << start << " |";
				cout << setw(8) << end << " |";
				cout << setw(8) << (end - start);
				cout << endl;
				return 0;
			});
			delete fs;
		} else {
			cerr << "Invalid file system.\n";
			err = 1;
		}
		delete []fsBuff;
	} else {
		err = 2;
	}
	return err;
}

int help(int, char**) { 
	cout << usage << endl;
	return 0;
}

int version(int, char**) { 
	cout << "oxfstool version " << oxfstoolVersion << endl;
	cout << "oxfs format version " << FileStore16::VERSION << endl;
	return 0;
}

int main(int argc, char **args) {
	auto err = 0;
	map<string, int(*)(int, char**)> cmdMap = {
		{ "format", format },
		{ "read", read },
		{ "write", [](int argc, char **args) { return write(argc, args, false); } },
		{ "write-expand", [](int argc, char **args) { return write(argc, args, true); } },
		{ "compact", compact },
		{ "rm", remove },
		{ "walk", walk },
		{ "help", help },
		{ "version", version },
	};

	if (argc > 1) {
		auto cmd = args[1];
		auto f = cmdMap[cmd];
		if (f) {
			err = f(argc, args);
		} else {
			cout << "Command '" << cmd << "' not recognized." << endl;
			err = 1;
		}
	} else {
		help(argc, args);
	}

	return err;
}
