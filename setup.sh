#!/bin/sh
VERSION=x86_64-elf-7.3.0-Linux-x86_64
wget http://newos.org/toolchains/${VERSION}.tar.xz
tar xf ${VERSION}.tar.xz
rm ${VERSION}.tar.xz
echo export PATH=$(pwd)/${VERSION}/bin/:$PATH >> .settings
echo export CROSS_COMPILE=x86_64-elf- >> .settings
# Notes: Uncommenting the lines bellow will install llvm-13 toolchain.
# You must also create symlinks (clang -> clang-13, ld.lld -> ld.lld-13
# and so on...). clang-12 also should work.
#echo "Installing llvm-13..."
#wget https://apt.llvm.org/llvm.sh
#chmod +x llvm.sh
#sudo ./llvm.sh 13
#echo "Execute 'source .settings' before using the framework (not necessary for clang)"
