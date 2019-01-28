#!/bin/bash
set -x
set -e

# /usr/lib64/ccache/{gcc,cc,g++,c++} symlinks are only created when the
# system compiler is installed. We don't use it, though, we use devtoolset-8
# gcc.
yum install -y centos-release-scl-rh epel-release rpm-build yum-utils gcc-c++
yum install -y cmake3 ccache yasm ninja-build
yum-config-manager --disable "*epel*"

# Work around https://bugs.centos.org/view.php?id=14773
yum install -y devtoolset-8-toolchain
yum remove -y centos-release-scl-rh

yum-builddep -y dcmtk.spec
sed -i -e 's/\(%dist .el[0-9]*\).centos/\1/ ' /etc/rpm/macros.dist
groupadd -g $DOCKER_GID $DOCKER_USERNAME
useradd -u $DOCKER_UID -g $DOCKER_GID -G wheel --no-create-home $DOCKER_USERNAME
su $DOCKER_USERNAME -c "scl enable devtoolset-8 'make -f Makefile-rpm rpm'"
