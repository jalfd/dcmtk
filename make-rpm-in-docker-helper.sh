#!/bin/bash
set -x
set -e

os_version=$1

sed -i -e 's/\(%dist .el[0-9]*\).centos/\1/ ' /etc/rpm/macros.dist
groupadd -g $DOCKER_GID $DOCKER_USERNAME
useradd -u $DOCKER_UID -g $DOCKER_GID -G wheel --no-create-home $DOCKER_USERNAME

if [[ $os_version == el7 ]]; then
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
    su $DOCKER_USERNAME -c "scl enable devtoolset-8 'make -f Makefile-rpm rpm'"
fi

if [[ $os_version == el8 ]]; then
    dnf install -y epel-release
    dnf --enablerepo=PowerTools install -y rpm-build gcc-c++ cmake ccache make ninja-build 'dnf-command(config-manager)'
    dnf config-manager --disable "*epel*"

    dnf --enablerepo=PowerTools builddep -y dcmtk.spec

    su $DOCKER_USERNAME -c "make -f Makefile-rpm rpm"
fi
