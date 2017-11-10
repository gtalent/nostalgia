FROM wombatant/devenv:latest

ENV DEVKITPRO /opt/devkitPro
ENV DEVKITARM ${DEVKITPRO}/devkitARM

RUN dnf install -y findutils ninja-build
