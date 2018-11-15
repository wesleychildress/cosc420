#!/bin/bash
sudo apt-get install -f

LIST_OF_APPS="ssh ntp qt-sdk pkg-config ncurses-dev nfs-server libselinux1-dev pdsh tftp gfortran
libxml2-dev libboost-dev tk-dev apache2 libapache2-mod-perl2 tftpd-hpa debootstrap tcpdump
isc-dhcp-server curl libterm-readline-gnu-perl"

# set up permanent networking connections
mv /etc/network/interfaces /etc/network/interfaces.og
cp $( pwd )/configFiles/interfaces /etc/network/interfaces
#sudo /etc/init.d/networking restart

# set SELINUX=permissive
setenforce -P 0

#install essential build tools:
sudo apt-get install -y build-essential

# install mariadb (new, updated mysql):
sudo apt-get install -y mysql-server

# install other important packages:
sudo apt-get install -y $LIST_OF_APPS

# install warewulf
cd $( pwd )/src
echo In directory $( pwd )
sudo chmod +x install-wwdebsystem
sudo ./install-wwdebsystem 3.6
cd ..

# make copy of original config files then move these into place
mv /etc/exports /etc/exports.og
cp $( pwd )/configFiles/exports /etc/exports

mv /usr/local/libexec/warewulf/wwmkchroot/include-deb /usr/local/libexec/warewulf/wwmkchroot/include-deb.og
cp $( pwd )/configFiles/include-deb /usr/local/libexec/warewulf/wwmkchroot/include-deb

mv /usr/local/etc/warewulf/vnfs.conf /usr/local/etc/warewulf/vnfs.conf.og
cp $( pwd )/configFiles/vnfs.conf /usr/local/etc/warewulf/vnfs.conf

cp $( pwd )/configFiles/debian7.tmpl /usr/local/libexec/warewulf/wwmkchroot/debian7.tmpl

# Create directories necessary for successful chrooting:
mkdir /srv/chroots
mkdir /srv/chroots/debian7
mkdir /srv/chroots/debian7/vnfs
mkdir /srv/chroots/debian7/srv
mkdir /srv/chroots/debian7/srv/chroots

# create warewulf chroot:
wwmkchroot debian7 /srv/chroots/debian7

# MINIMIZE chroot install files:
# chroot /srv/chroots/debian7
# mount -t proc proc proc/
# apt-get remove ????
# exit

# config files
sudo mv /etc/idmapd.conf /etc/idmapd.conf.og
sudo mv /srv/chroots/debian7/etc/idmapd.conf /srv/chroots/debian7/etc/idmapd.conf.og
cp $( pwd )/configFiles/idmapd.conf /etc/idmapd.conf
cp $( pwd )/configFiles/idmapd.conf /srv/chroots/debian7/etc/idmapd.conf

sudo mv /etc/default/nfs-common /etc/default/nfs-common.og
cp $( pwd )/configFiles/nfs-common /etc/default/nfs-common

sudo mv /usr/local/etc/warewulf/vnfs.conf /usr/local/etc/warewulf/vnfs.conf.og
cp $( pwd )/configFiles/vnfs.conf /usr/local/etc/warewulf/vnfs.conf

sudo sudo mv /usr/local/etc/warewulf/defaults/node.conf /usr/local/etc/warewulf/defaults/node.conf.og
cp $( pwd )/configFiles/node.conf /usr/local/etc/warewulf/defaults/node.conf

sudo mv /usr/local/etc/warewulf/defaults/provision.conf /usr/local/etc/warewulf/defaults/provision.conf.og
cp $( pwd )/configFiles/provision.conf /usr/local/etc/warewulf/defaults/provision.conf

sudo mv /usr/local/etc/warewulf/bootstrap.conf /usr/local/etc/warewulf/bootstrap.conf.og
cp $( pwd )/configFiles/bootstrap.conf /usr/local/etc/warewulf/bootstrap.conf

sudo mv /srv/chroots/debian7/etc/fstab /srv/chroots/debian7/etc/fstab.og
cp $( pwd )/configFiles/fstab /srv/chroots/debian7/etc/fstab

sudo mv /srv/chroots/debian7/etc/rc.local /srv/chroots/debian7/etc/rc.local.og
cp $( pwd )/configFiles/rc.local /srv/chroots/debian7/etc/rc.local

# restart nfs on master node
/etc/init.d/nfs-kernel-server restart
/etc/init.d/nfs-common restart

# Restart the tftp server:
/etc/init.d/tftpd-hpa restart

wwvnfs --chroot /srv/chroots/debian7  --hybridpath=/vnfs
wwsh dhcp update

sudo mv /srv/chroots/debian7/etc/apt/sources.list /srv/chroots/debian7/etc/apt/sources.list.og
cp $( pwd )/configFiles/sources.list /srv/chroots/debian7/etc/apt/sources.list
sudo mv /srv/chroots/debian7/etc/ntp.conf /srv/chroots/debian7/etc/ntp.conf.og
cp $( pwd )/configFiles/ntp.conf /srv/chroots/debian7/etc/ntp.conf

# update debian7 vnfs (magic land)
chroot /srv/chroots/debian7
mount -t proc proc proc/
apt-get update
apt-get upgrade

wwvnfs --chroot /srv/chroots/debian7  --hybridpath=/vnfs


# update the files and everything else!!!!!
wwsh file sync
wwsh dhcp update
wwsh pxe update

# Build and install MPICH
tar zxvf mpich-3.2.1.tar.gz
cd mpich-3.2.1
./configure --enable-fc --enable-f77 --enable-romio --enable-mpe --with-pm=hydra

# make -j 4
make
make install
