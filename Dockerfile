FROM wombatant/devenv:latest

ENV DEVKITPRO /opt/devkitPro
ENV DEVKITARM ${DEVKITPRO}/devkitARM

RUN dnf install -y findutils ninja-build

# Install Powershell
RUN rpm --import https://packages.microsoft.com/keys/microsoft.asc
RUN curl https://packages.microsoft.com/config/rhel/7/prod.repo | sudo tee /etc/yum.repos.d/microsoft.repo
RUN dnf update -y
RUN dnf install -y compat-openssl10
RUN dnf install -y powershell
