# centos-watchdog
Example on how to patch and compile watchdog kernel module

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

# Check status
sudo wdctl

# Test watchdog (Warning, system will do hard reset)
gcc watchdog.c -o wdtest
sudo ./wdtest

```


