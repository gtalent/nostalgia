FROM wombatant/devenv:latest

ENV DEVKITPRO /opt/devkitPro
ENV DEVKITARM ${DEVKITPRO}/devkitARM

RUN dnf install -y qt5-devel llvm libasan

###############################################################################
# Install Ox

RUN git clone https://github.com/wombatant/ox.git /usr/local/src/ox && \
    cd /usr/local/src/ox && \
	 git checkout -b install a5166e03bbaea2f200cfc730c69579c3d50ae2a7

	 # setup build dirs
RUN mkdir -p \
             /usr/local/src/ox/build/release \
             /usr/local/src/ox/build/windows \
             /usr/local/src/ox/build/gba;

    # install Ox for native environment
RUN cd /usr/local/src/ox/build/release && \
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../ && \
    make -j install

    # install Ox for GBA
RUN cd /usr/local/src/ox/build/gba && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
          -DCMAKE_TOOLCHAIN_FILE=cmake/Modules/GBA.cmake \
          -DCMAKE_INSTALL_PREFIX=/opt/devkitPro/devkitARM \
          -DOX_USE_STDLIB=OFF ../../ && \
    make -j install

    # install Ox for Windows
RUN cd /usr/local/src/ox/build/windows && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
          -DCMAKE_TOOLCHAIN_FILE=cmake/Modules/Mingw.cmake \
          -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 \
          -DOX_BUILD_EXEC=OFF ../../ && \
    make -j install
