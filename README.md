# it8786 watchdog
Example on how to patch and compile watchdog kernel module

## Example usage (After compilation and installation)
```bash
# Check status
sudo wdctl

# Test watchdog (Warning, system will do hard reset)
gcc watchdog.c -o wdtest
sudo ./wdtest
```

## centos
``` bash
$ cat /etc/centos-release
CentOS Linux release 8.1.1911 (Core)
$ uname -r
4.18.0-147.el8.x86_64

#Install dependencies
sudo yum groupinstall -y "Development Tools"
sudo yum install -y ncurses-devel
sudo yum install -y hmaccalc zlib-devel binutils-devel elfutils-libelf-devel
mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
echo '%_topdir %(echo $HOME)/rpmbuild' > ~/.rpmmacros

# Get kernel source
rpm -i http://vault.centos.org/8.1.1911/BaseOS/Source/SPackages/kernel-4.18.0-147.el8.src.rpm

# Install additional kernel dependencies
cd rpmbuild/
cd ~/rpmbuild/SPECS
for pkg in `rpmbuild -bp --target=$(uname -m) kernel.spec 2>&1 | grep 'is needed by' |awk '{print $1}' | tr '\n' ' '`;do sudo yum install $pkg -y;done

# Unpack and patch source
rpmbuild -bp --target=$(uname -m) kernel.spec
cd ~/rpmbuild/BUILD/kernel-*/linux-*/
patch -p1 < ../it8786_wdt.patch

# Compile
make oldconfig
make prepare
make modules_prepare
make -j 4 M=drivers/watchdog

# Load module
sudo insmod drivers/watchdog/it87_wdt.ko
```
## openSUSE
``` bash
$ cat /etc/os-release
NAME="openSUSE Leap"
VERSION="15.1"
ID="opensuse-leap"
ID_LIKE="suse opensuse"
VERSION_ID="15.1"
PRETTY_NAME="openSUSE Leap 15.1"
ANSI_COLOR="0;32"
CPE_NAME="cpe:/o:opensuse:leap:15.1"
BUG_REPORT_URL="https://bugs.opensuse.org"
HOME_URL="https://www.opensuse.org/"

$ uname -r
4.12.14-lp151.28.36-default

# Install kernel source and dependencies
sudo zypper install kernel-source
sudo zypper install patch

# Patch source
cd /usr/src/linux-4.12.14-lp151.28.36
sudo patch -p1 < ~/it8786_wdt.patch # Hunk #2 will fail but is OK

# Build module
sudo make -C /lib/modules/4.12.14-lp151.28.36-default/build/ oldconfig
sudo make -C /lib/modules/4.12.14-lp151.28.36-default/build/ prepare
sudo make -C /lib/modules/4.12.14-lp151.28.36-default/build/ modules_prepare
sudo make -C /lib/modules/4.12.14-lp151.28.36-default/build/ SUBDIRS=drivers/watchdog modules

# Install module
sudo cp -v /usr/src/linux-4.12.14-lp151.28.36-obj/x86_64/default/drivers/watchdog/it87_wdt.ko /lib/modules/4.12.14-lp151.28.36-default/kernel/drivers/watchdog/it87_wdt.ko
sudo depmod -a

# Load module
sudo modprobe it87_wdt
```
