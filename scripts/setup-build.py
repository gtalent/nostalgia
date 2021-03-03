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
    parser.add_argument('--target', help='Platform target',
                        default='{:s}-{:s}'.format(sys.platform, platform.machine()))
    parser.add_argument('--build_type', help='Build type (asan,debug,release)', default='release')
    parser.add_argument('--build_tool', help='Build tool (default,xcode)', default='')
    parser.add_argument('--toolchain', help='Path to CMake toolchain file')
    parser.add_argument('--current_build', help='Indicates whether or not to make this the active build', default=1)
    args = parser.parse_args()

    if args.build_type == 'asan':
        build_type_arg = 'Debug'
        sanitizer_status = 'ON'
    elif args.build_type == 'debug':
        build_type_arg = 'Debug'
        sanitizer_status = 'OFF'
    elif args.build_type == 'release':
        build_type_arg = 'Release'
        sanitizer_status = 'OFF'
    else:
        print('Error: Invalid build tool')
        sys.exit(1)

    if args.build_tool == 'xcode':
        build_config = '{:s}-{:s}'.format(args.target, args.build_tool)
    else:
        build_config = '{:s}-{:s}'.format(args.target, args.build_type)

    if 'QTDIR' in os.environ:
        qt_path = '-DQTDIR={:s}'.format(os.environ['QTDIR'])
    else:
        qt_path = ''

    if args.build_tool == '' or args.build_tool == 'default':
        if shutil.which('ninja') is None:
            build_tool = ''
        else:
            build_tool = '-GNinja'
    elif args.build_tool == 'xcode':
        build_tool = '-GXcode'
    else:
        print('Error: Invalid build tool')
        sys.exit(1)

    project_dir = os.getcwd()
    build_dir = '{:s}/build/{:s}'.format(project_dir, build_config)
    rm(build_dir)
    mkdir(build_dir)
    subprocess.run(['cmake', '-S', project_dir, '-B', build_dir, build_tool,
                    '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
                    '-DCMAKE_TOOLCHAIN_FILE={:s}'.format(args.toolchain),
                    '-DCMAKE_BUILD_TYPE={:s}'.format(build_type_arg),
                    '-DUSE_ASAN={:s}'.format(sanitizer_status),
                    '-DBUILDCORE_BUILD_CONFIG={:s}'.format(build_config),
                    '-DBUILDCORE_TARGET={:s}'.format(args.target),
                    qt_path,
                    ])

    mkdir('dist')
    if int(args.current_build) != 0:
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
