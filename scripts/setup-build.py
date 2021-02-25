#! /usr/bin/env python3

import argparse
import os
import platform
import shutil
import subprocess
import sys

from pybb import mkdir, rm

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--target', help='Platform target ({OS}-{Arch},gba)', default='{:s}-{:s}'.format(sys.platform, platform.machine()))
    parser.add_argument('--build_type', help='Build type (asan,debug,release)', default='release')
    parser.add_argument('--build_tool', help='Build tool (default,xcode)', default='')
    parser.add_argument('--vcpkg_dir', help='Path to VCPKG')
    args = parser.parse_args()

    if args.target == 'gba':
        toolchain = '-DCMAKE_TOOLCHAIN_FILE=cmake/modules/GBA.cmake'
        nostalgia_build_type = 'GBA'
    else:
        toolchain = '-DCMAKE_TOOLCHAIN_FILE={:s}/scripts/buildsystems/vcpkg.cmake'.format(args.vcpkg_dir)
        nostalgia_build_type = 'Native'

    if args.build_type == 'asan':
        build_type_arg = 'Debug'
        sanitizer_status = 'ON'
    elif args.build_type == 'debug':
        build_type_arg = 'Debug'
        sanitizer_status = 'OFF'
    elif args.build_type == 'release':
        build_type_arg = 'Release'
        sanitizer_status = 'OFF'

    if args.build_tool == 'xcode':
        build_config = '{:s}-{:s}'.format(args.target, args.build_tool)
    else:
        build_config = '{:s}-{:s}'.format(args.target, args.build_type)

    if 'NOSTALGIA_QT_PATH' in os.environ:
        qt_path = '-DNOSTALGIA_QT_PATH={:s}'.format(os.environ['NOSTALGIA_QT_PATH'])
    else:
        qt_path = ''

    if args.build_tool == '' or args.build_tool == 'default':
        if shutil.which('ninja') == None:
            build_tool = ''
        else:
            build_tool = '-GNinja'
    elif args.build_tool == 'xcode':
        build_tool = '-GXcode'

    project_dir = os.getcwd()
    build_dir = '{:s}/build/{:s}'.format(project_dir, build_config)
    rm(build_dir)
    mkdir(build_dir)
    subprocess.run(['cmake', '-S', project_dir, '-B', build_dir, build_tool,
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
    if args.target != 'gba':
        cb = open('.current_build', 'w')
        cb.write(args.build_type)
        cb.close()

    rm('compile_commands.json')
    if platform.system() != 'Windows':
        os.symlink('build/{:s}/compile_commands.json'.format(build_config), 'compile_commands.json')

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(1)

