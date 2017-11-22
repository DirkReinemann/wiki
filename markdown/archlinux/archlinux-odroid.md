# archlinux odroid

## install image

* [installation instructions](https://archlinuxarm.org/platforms/armv7/samsung/odroid-xu4)

```bash
sudo dd if=/dev/zero of=/dev/mmcblk0 bs=1M count=8
sudo fdisk /dev/mmcblk0
fdisk> o
fdisk> p
fdisk> n
fdisk> p
fdisk> 1
fdisk> 4096
fdisk> w
sudo mkfs.ext4 /dev/mmcblk0
mkdir root
sudo mount /dev/mmcblk0 root
wget http://os.archlinuxarm.org/os/ArchLinuxARM-odroid-xu3-latest.tar.gz
sudo bsdtar -xpf ArchLinuxARM-odroid-xu3-latest.tar.gz -C root
cd root/boot
sudo ./sd_fusing.sh /dev/mmcblk0
```

## install system

| username  | password |
| --------- | -------- |
| root      | root     |
| alarm     | alarm    |

### home

```bash
ssh-copy-id odroid
ssh odroid
```

### odroid

```bash
passwd
su
root> passwd
root> pacman -Syu
root> pacman -S vim sudo git ctags bash-completion
root> cp /usr/bin/vim /usr/bin/vi
root> reboot
```

### home

```bash
scp .bashrc odroid:
scp .vimrc odroid:
scp .inputrc odroid:
ssh odroid
```

### odroid

```bash
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
vim
vim> :PluginInstall

su
root> visudo

vim> %wheel ALL=(ALL) ALL

root> exit
```

```bash
sudo vi /etc/locale.gen

de_DE.UTF-8 UTF-8
de_DE ISO-8859-1
en_US.UTF-8 UTF-8
en_US ISO-8859-1
```

```bash
sudo locale-gen
echo LANG=en_US.UTF-8 | sudo tee -a /etc/locale.conf
sudo ln -sf /usr/share/zoneinfo/Europe/Berlin /etc/localtime
sudo bash -c "echo odroid > /etc/hostname"
sudo bash -c "echo KEYMAP=de-latin1-nodeadkeys > /etc/vconsole.conf"
sudo usermod -a -G audio,lp,optical,storage,video,games,power,scanner,wheel alarm
sudo pacman -S cronie dbus ntp avahi
sudo systemctl enable cronie ntpd avahi-daemon
sudo vi /etc/ntp.conf

vim> server de.pool.ntp.org

sudo ntpd -gq
sudo pacman -S xorg xorg-server xorg-xinit xorg-drivers i3 dmenu rofi dunst terminator xterm python-dbus python2-dbus
```


```bash
vi ~/.bash_profile

#
# ~/.bash_profile
#

[[ -f ~/.bashrc ]] && . ~/.bashrc

if [ -z "$DISPLAY" ] && [ "$(fgconsole 2>/dev/null || echo 0)" -eq 1 ]; then
    exec startx
fi
```

vi ~/.xprofile
```
#!/bin/sh

xrandr --output HDMI-1 --primary --mode 1280x800 --pos 0x0 --rotate normal
```


```bash
vi ~/.xinitrc

#!/bin/sh

[ -f ~/.xprofile ] && . ~/.xprofile

exec i3
```


```bash
sudo vi /etc/X11/xorg.conf.d/00-keyboard.conf

Section "InputClass"
        Identifier "system-keyboard"
        MatchIsKeyboard "on"
        Option "XkbLayout" "de"
        Option "XkbModel" "pc105"
        Option "XkbVariant" "nodeadkeys"
EndSection
```

## autologin

```bash
sudo cp /usr/lib/systemd/system/getty@.service /usr/lib/systemd/system/autologin@.service
sudo vi /usr/lib/systemd/system/autologin@.service
vim> ExecStart=-/sbin/agetty --noclear -a alarm %I %TERM

sudo systemctl disable getty@tty1
sudo systemctl enable autologin@tty1
sudo systemctl start autologin@tty1
```

```
sudo pacman -S rhythmbox chromium arandr lxappearance thunar mpv higan blueman ffmpeg alsa-utils alsa-plugins pulseaudio-alsa pulseaudio gst-libav gst-plugins-good gst-plugins-ugly dex playerctl zip unzip wget make gcc htop
sudo systemctl enable bluetooth.service

sudo vi /etc/pulse/client.conf

vim> autospawn = yes

sudo poweroff
```

## bluetooth configuration

TODO

## i3 configuration

### home

```bash
scp config odroid:.config/i3/
scp i3blocks-top.conf  odroid:.config/i3/
scp i3blocks-bottom.conf odroid:.config/i3/
scp autostart.sh odroid:.config/i3/
ssh odroid "mkdir .config/i3/blocks"
zip -R9 blocks.zip * && scp blocks.zip odroid:.config/i3/blocks/
ssh odroid "unzip .config/i3/blocks/blocks.zip && rm .config/i3/blocks/blocks.zip"
ssh -t odroid "sudo poweroff"
```

## fontawesome

```bash
(github)[https://github.com/FortAwesome/Font-Awesome/blob/master/fonts/fontawesome-webfont.ttf]
sudo cp fontawesome-webfont.ttf /usr/share/fonts/TTF/
```

# gtk theme

```bash
sudo pacman -S gnome-themes-standard arc-icon-theme feh
lxappearance
lxappearance> Monospace 12, Adwaita Theme, Arc Icons
```

## vnc

```bash
sudo pacman -S x11vnc
```

## dunst and rofi

```bash
mkdir -p ~/.config/dunst
mkdir -p ~/.config/rofi
scp rofi/config odroid:.config/rofi/
scp dunst/dunstrc  odroid:.config/dunst/
```

## ssh forward application

```bash
ssh-keygen -t rsa -b 4096
ssh-keygen -t rsa -b 4096

ssh-keygen> $HOME/.ssh/id_rsa_insecure

ssh-copy-id -i ~/.ssh/id_rsa_insecure.pub dirk@192.168.0.90
```

```bash
sudo vi ~/.ssh/config

ForwardX11 yes
Host dirk
Hostname 192.168.0.80
Port 22
User dirk
```

## pulseaudio sound forward

```bash
sudo pacman -S pulseaudio-zeroconf paprefs avahi
sudo systemctl enable avahi-daemon
sudo systemctl start avahi-daemon
```

### odroid

1. open paprefs
2. enable network access to local sound devices
3. allow other machines on the lan to discover local sound devices
4. don't require authentification
5. restart pulseaudio

```bash
# restart pulseaudio
pulseaudio -k
```

## notebook

1. open paprefs
2. make discoverable pulseaudio network sound devices available locally
3. restart pulseaudio

```bash
# restart pulseaudio
pulseaudio -k
```

## chromium

```bash
sudo pacman -S upower
sudo vi $HOME/.config/chromium-flags.conf

vim> --disable-namespace-sandbox --use-gl=egl --ignore-gpu-blacklist --disable-accelerated-2d-canvas --num-raster-threads=2

sudo ln -s /usr/bin/libGLESv2.so /usr/lib/chromium/libGLESv2.so
sudo ln -s /usr/lib/libEGL.so /usr/lib/chromium/libEGL.so
```
