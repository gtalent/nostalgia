#! /usr/bin/env python3

#
#  Copyright 2016 - 2021 gary@drinkingtea.net
#
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# "Python Busy Box" - adds cross platform equivalents to Unix commands that
#                     don't translate well to that other operating system

import os
import shutil
import subprocess
import sys


def mkdir(path):
    if not os.path.exists(path) and os.path.isdir(path):
        os.mkdir(path)

# this exists because Windows is utterly incapable of providing a proper rm -rf
def rm(path):
    if (os.path.exists(path) or os.path.islink(path)) and not os.path.isdir(path):
        os.remove(path)
    elif os.path.isdir(path):
        shutil.rmtree(path)

def cmake_build(base_path, target):
    if not os.path.isdir(base_path):
        # nothing to build
        return 0
    for d in os.listdir(base_path):
        args = ['cmake', '--build', os.path.join(base_path, d), '--target']
        if target is not None:
            args.append(target)
        err = subprocess.run(args).returncode
        if err != 0:
            return err

def main():
    if sys.argv[1] == 'mkdir':
        mkdir(sys.argv[2])
    elif sys.argv[1] == 'rm':
        for i in range(2, len(sys.argv)):
            rm(sys.argv[i])
    elif sys.argv[1] == 'cmake-build':
        err = cmake_build(sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else None)
        sys.exit(err)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(1)

