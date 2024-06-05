FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -y build-essential git cmake libcmocka-dev clang-tidy valgrind strace ltrace sudo libcap2-bin

ENV USERNAME=dev
RUN useradd -G sudo -m -s /bin/bash $USERNAME && echo ${USERNAME}:${USERNAME}123 | chpasswd
RUN echo "$USERNAME ALL=(ALL) NOPASSWD: /usr/sbin/setcap, /usr/bin/ctest" > /etc/sudoers.d/dev_rules

USER dev
