<!-- toc -->
  * [arch linux](#arch-linux)
    * [yaourt](#yaourt)
    * [ssh](#ssh)
    * [bash](#bash)
    * [vim](#vim)
    * [theme](#theme)
    * [terminator](#terminator)
    * [wallpaper](#wallpaper)
    * [i3](#i3)
    * [applications](#applications)
    * [firefox](#firefox)
<!-- toc -->


# arch linux

[LUKS](https://wiki.archlinux.org/index.php/Dm-crypt/Device_encryption#Encryption_options_for_LUKS_mode)

```bash
loadkeys de-latin1
dhcpcd
sgdisk --zap-all /dev/sda
gdisk /dev/sda
gdisk> o (Y, ENTER)
gdisk> n (1, ENTER, ENTER, +1007k, ENTER, ef02, ENTER)
gdisk> n (2, ENTER, ENTER, +128M, ENTER, ENTER)
gdisk> n (3, ENTER, ENTER, +0, ENTER, 8e00, ENTER)
gdisk> p
gdisk> w (Y, ENTER)
cryptsetup -v luksFormat /dev/sda3
cryptsetup luksOpen /dev/sda3 crypto
pvcreate /dev/mapper/crypto
vgcreate crypto /dev/mapper/crypto
lvcreate -C y -L 8GB crypto -n swap
lvcreate -l +100%FREE crypto -n root
vgscan
vgchange -ay
mkfs.ext4 /dev/sda2
mkswap /dev/mapper/crypto-swap
mkfs.ext4 /dev/mapper/crypto-root
swapon /dev/mapper/crypto-swap
mount /dev/mapper/crypto-root /mnt
mkdir /mnt/boot
mount /dev/sda2 /mnt/boot
lsblk /dev/sda (show information)
pacman -Syy
pacstrap -i /mnt base base-devel
pacman -S vim
cp /usr/bin/vim /usr/bin/vi
genfstab -U -p /mnt >> /mnt/etc/fstab
cat /mnt/etc/fstab
arch-chroot /mnt /bin/bash
vi /etc/locale.gen (en_US.UTF-8 UTF-8, de_DE.UTF-8 UTF-8)
locale-gen
echo LANG=en_US.UTF-8 > /etc/locale.conf
export LANG=en_US.UTF-8
/etc/localtime
ln -s /usr/share/zoneinfo/Europe/Berlin /etc/localtime
vi /etc/hostname (archlinux)
vi /etc/hosts
```

```bash
vi /etc/vconsole.conf (KEYMAP=de-latin1)

127.0.0.1       localhost.localdomain   localhost  archlinux
::1             localhost.localdomain   localhost  archlinux
```

```bash
ip link
systemctl enable dhcpcd@INTERFACE.service
vi /etc/mkinitcpio.conf (HOOKS="base udev autodetect modconf block keymap encrypt lvm2 filesystems keyboard fsck")
cd /boot
mkinitcpio -p linux
passwd
useradd -m -g users -G audio,lp,optical,storage,video,games,power,scanner,wheel -s /bin/bash dirk
passwd dirk
pacman -S grub lvm2
vi /etc/default/grub (GRUB_CMDLINE_LINUX="cryptdevice=/dev/sda3:crypto resume=/dev/crypto/swap")
grub-install --target=i386-pc --recheck /dev/sda
grub-mkconfig -o /boot/grub/grub.cfg
visudo (%wheel ALL=(ALL) ALL)
exit
umount /mnt/boot
umount /mnt
swapoff /dev/mapper/crypto-swap
reboot
```

```bash
sudo pacman -S cronie dbus ntp avahi
sudo systemctl enable cronie ntpd avahi
sudo vi /etc/ntp.conf (server de.pool.ntp.org)
sudo ntpd -gq
sudo hwclock -w
sudo pacman -Syu
```

```bash
sudo vi /etc/pacman.conf (only on x64)

[multilib]
Include = /etc/pacman.d/mirrorlist
```

```bash
sudo pacman -Syy && sudo pacman -Syu
sudo pacman -S bash-completion git xorg-server xorg-server-utils xorg xorg-xinit xorg-drivers i3 i3-wm i3-status dmenu rofi dunst terminator xterm
```

```bash
vi ~/.xinitrc

#!/bin/sh

exec i3
```

```bash
vi ~/.bash_profile

if [ -z "$DISPLAY" ] && [ "$(fgconsole)" -eq 1 ]; then
    exec startx
fi

export EDITOR=vim
```

```bash
sudo reboot
```

## yaourt

```bash
git clone https://aur.archlinux.org/package-query.git
cd package-query
makepkg -si
cd ..
git clone https://aur.archlinux.org/yaourt.git
cd yaourt
makepkg -si
cd ..
```

## ssh

```bash
sudo pacman -S openssh
ssh-keygen -t rsa -b 4096 -C "dirk.reinemann@gmx.de"
```

## bash

* copy .inputrc and .bashrc from linux repository

## vim

* copy .vimrc from linux repository and install plugins

```bash
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
vim
vim>:PluginInstall
```

## theme

```bash
pacman -S arc-icon-theme gtk3 gnome-themes-standard
yaourt adwaita-qt4 adwaita-qt5 xcursor-human
update ~/.gtkrc-2.0 and ~/.config/gtk-3.0/settings.ini (theme adwaita, cursor human, icon arc)
```

## terminator

```bash
copy terminator conf
sudo pacman -S python-dbus python3-dbus
```

## wallpaper

```bash
sudo pacman -S feh
i3 config
```

## i3

```bash
sudo pacman -S lm_sensors xdotool pulseaudio
volumecontrol.sh from linux repository
yaourt i3blocks
```

## applications

```bash
sudo pacman -S firefox thunderbird thunar lxappearance keychain ctags meld kile rhythmbox scala chromium keepassx easytag pdfsam zeal gparted wine openttd mednafen htop libreoffice nmap wireshark-gtk filezilla
yaourt jdk intellij-idea-ultimate-edition spotify blockify sublime-text-dev skypeforlinux-bin gtkhash postman-bin
```
## firefox

* adblock plus
* better privacy
* firebug
* https everywhere
* noscript
* random agent spoofer
* self destructing cookies
* ublock origin
