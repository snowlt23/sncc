
rm /etc/localtime
ln -s /usr/share/zoneinfo/Asia/Tokyo /etc/localtime

apt update
apt install -y build-essential
apt install -y libc6:i386
apt install -y gcc-multilib
