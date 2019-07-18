FROM fedora:30

RUN dnf update -y

###############################################################################
# Install gosu

RUN dnf install -y curl gnupg
RUN gpg --keyserver ha.pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4
RUN curl -o /usr/local/bin/gosu -SL "https://github.com/tianon/gosu/releases/download/1.10/gosu-amd64" && \
    curl -o /usr/local/bin/gosu.asc -SL "https://github.com/tianon/gosu/releases/download/1.10/gosu-amd64.asc" && \
    gpg --verify /usr/local/bin/gosu.asc && \
    rm /usr/local/bin/gosu.asc && \
    chmod +x /usr/local/bin/gosu

###############################################################################
# Install dev tools

RUN dnf install -y clang
RUN dnf install -y llvm
RUN dnf install -y libasan
RUN dnf install -y mingw64-gcc-c++
RUN dnf install -y cmake
RUN dnf install -y make
RUN dnf install -y git
RUN dnf install -y vim
RUN dnf install -y sudo
RUN dnf install -y fuse-devel
RUN dnf install -y qt5-devel
RUN dnf install -y findutils
RUN dnf install -y ninja-build
RUN dnf install -y libcxx-devel libcxxabi-devel

###############################################################################
# Install devkitARM

#RUN dnf install -y lbzip2
#RUN curl -o /tmp/devkitArm.tar.bz2 -SL https://phoenixnap.dl.sourceforge.net/project/devkitpro/devkitARM/devkitARM_r47/devkitARM_r47-x86_64-linux.tar.bz2
#WORKDIR /opt
#RUN tar xf /tmp/devkitArm.tar.bz2
#ENV DEVKITARM /opt/devkitARM

###############################################################################
# Setup sudoers

ADD devenv/sudoers /etc/sudoers

###############################################################################
# Setup working directory

RUN mkdir /usr/src/project
WORKDIR /usr/src/project

###############################################################################
# Setup entrypoint

ADD devenv/entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]

ENV CC clang
ENV CXX clang++
