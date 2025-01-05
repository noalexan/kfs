FROM ubuntu:24.04

ENV IN_DOCKER=1
ENV MAKEFLAGS=-j32

RUN apt update

# Install Python3.12
RUN apt install -y software-properties-common python3-launchpadlib \
&& add-apt-repository ppa:deadsnakes/ppa \
&& apt update && apt install -y python3.12

# Install build dependencies
RUN apt install -y build-essential flex bison pkg-config m4 libtool automake autoconf autopoint gettext texinfo libfreetype-dev gawk xorriso curl git

RUN mkdir -pv /sources /tools && chown 1000:1000 /sources /tools

USER 1000:1000

# Download sources
WORKDIR /sources
RUN curl -#O https://ftp.gnu.org/gnu/binutils/binutils-2.43.tar.gz
RUN curl -#O https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.gz
RUN curl -#O https://ftp.gnu.org/gnu/gmp/gmp-6.3.0.tar.gz
RUN curl -#O https://ftp.gnu.org/gnu/mpc/mpc-1.3.1.tar.gz
RUN curl -#O https://ftp.gnu.org/gnu/mpfr/mpfr-4.2.1.tar.gz
RUN git clone --depth=1 https://git.savannah.gnu.org/git/grub.git

# Install binutils
RUN tar xvf /sources/binutils-2.43.tar.gz -C /sources
WORKDIR /sources/binutils-2.43
RUN ./configure --prefix=/tools --target=i386-linux-gnu && make && make install

# Install GCC
RUN tar xvf /sources/gcc-14.2.0.tar.gz -C /sources
WORKDIR /sources/gcc-14.2.0
RUN tar xvf /sources/gmp-6.3.0.tar.gz && ln -sv gmp-6.3.0 gmp && tar xvf /sources/mpc-1.3.1.tar.gz && ln -sv mpc-1.3.1 mpc && tar xvf /sources/mpfr-4.2.1.tar.gz && ln -sv mpfr-4.2.1 mpfr
RUN mkdir -v build && cd build && ../configure --prefix=/tools --target=i386-linux-gnu --with-glibc-version=2.40 --with-newlib --without-headers --enable-default-pie --enable-default-ssp --disable-nls --disable-shared --disable-multilib --disable-threads --disable-libatomic --disable-libgomp --disable-libquadmath --disable-libssp --disable-libvtv --disable-libstdcxx --enable-languages=c,c++ && make && make install

# Install GRUB
WORKDIR /sources/grub
RUN ./bootstrap && ./configure --prefix=/tools --target=i386-linux-gnu && make && make install

# Cleanup
RUN rm -rf /sources/*

WORKDIR /kfs
CMD [ "make" ]
