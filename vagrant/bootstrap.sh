#!/usr/bin/env bash
rm -rf /home/vagrant/annabell

apt-get update
apt-get install -y mc
# build tools
apt-get install -y git
apt-get install -y build-essential libtool
apt-get install -y autotools-dev
apt-get install -y autoconf

# library dependencies
apt-get install -y libpcre3-dev
apt-get install -y libgtest-dev
apt-get install -y cmake
cd /usr/src/gtest
cmake .
make
mv libg* /usr/lib/


# annabell repo
git clone https://github.com/golosio/annabell.git /home/vagrant/annabell

# annabell build
cd /home/vagrant/annabell
libtoolize
autoreconf --install
mkdir build
cd build
../configure
make

# set file permissions for user vagrant (this script runs with root by default)
chown -R vagrant:vagrant /home/vagrant/annabell

