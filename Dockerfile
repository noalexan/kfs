FROM debian:latest

ENV IN_DOCKER=1

RUN apt update && apt install -y binutils-i686-linux-gnu gcc-i686-linux-gnu g++-i686-linux-gnu make grub2 xorriso

WORKDIR /kfs
ENTRYPOINT [ "make" ]
