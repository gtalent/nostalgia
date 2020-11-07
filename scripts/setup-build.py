#! /usr/bin/env python3

import os
import platform
import shutil
import subprocess
import sys

target = sys.argv[1]
build_type = sys.argv[2]
vcpkg_dir = sys.argv[3]

project = os.getcwd()

def mkdir(path):
    if not os.path.exists(path) and os.path.isdir(path):
        os.mkdir(path)

def rm(path):
    if os.path.exists(path) or os.path.islink(path):
        os.remove(path)

if target == 'gba':
    toolchain = '-DCMAKE_TOOLCHAIN_FILE=cmake/modules/GBA.cmake'
    nostalgia_build_type = 'GBA'
else:
    toolchain = '-DCMAKE_TOOLCHAIN_FILE={:s}/scripts/buildsystems/vcpkg.cmake'.format(vcpkg_dir)
    nostalgia_build_type = 'Native'

if build_type == 'asan':
    build_type_arg = 'Debug'
    sanitizer_status = 'ON'
elif build_type == 'debug':
    build_type_arg = 'Debug'
    sanitizer_status = 'OFF'
elif build_type == 'release':
    build_type_arg = 'Release'
    sanitizer_status = 'OFF'


build_config = '{:s}-{:s}'.format(target, build_type)

if 'NOSTALGIA_QT_PATH' in os.environ:
    qt_path = '-DNOSTALGIA_QT_PATH={:s}'.format(os.environ['NOSTALGIA_QT_PATH'])
else:
    qt_path = ''

if shutil.which('ninja') == None:
    build_tool = ''
else:
    build_tool = '-GNinja'

build_dir = '{:s}/build/{:s}'.format(project, build_config)
mkdir(build_dir)
subprocess.run(['cmake', '-S', project, '-B', build_dir, build_tool,
        '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
        '-DCMAKE_BUILD_TYPE={:s}'.format(build_type_arg),
        '-DUSE_ASAN={:s}'.format(sanitizer_status),
        '-DNOSTALGIA_IDE_BUILD=OFF',
        '-DNOSTALGIA_BUILD_CONFIG={:s}'.format(build_config),
        '-DNOSTALGIA_BUILD_TYPE={:s}'.format(nostalgia_build_type),
        qt_path,
        toolchain,
    ])

mkdir('dist')
if target != 'gba':
    cb = open('.current_build', 'w')
    cb.write(build_type)
    cb.close()

rm('compile_commands.json')
if platform.system() != 'Windows':
    os.symlink('build/{:s}/compile_commands.json'.format(build_config), 'compile_commands.json')
