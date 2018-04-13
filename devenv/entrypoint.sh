#!/bin/bash

# script from Deni Bertovic
# https://denibertovic.com/posts/handling-permissions-with-docker-volumes/

# Add local user
# Either use the LOCAL_USER_ID if passed in at runtime or
# fallback

USER_ID=${LOCAL_USER_ID:-9001}

if [[ $(id -u user 2> /dev/null) != $USER_ID ]]; then
	useradd --shell /bin/bash -u $USER_ID -o -c "" -m user
	export HOME=/home/user
	echo "set -o vi" >> $HOME/.bashrc
	echo "export PATH=\${PATH}:/opt/devkitPro/devkitARM/bin/" >> $HOME/.bashrc
	echo "export DEVKITPRO=/opt/devkitPro" >> $HOME/.bashrc
	echo "export DEVKITARM=/opt/devkitPro/devkitARM" >> $HOME/.bashrc
	echo "export CC=/usr/bin/clang" >> $HOME/.bashrc
	echo "export CXX=/usr/bin/clang++" >> $HOME/.bashrc
fi

exec /usr/local/bin/gosu user "$@"
