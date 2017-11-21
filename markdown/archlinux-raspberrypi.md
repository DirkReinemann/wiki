# archlinux raspberrypi

## display

```bash
sudo vi /boot/config.txt

vim> lcd_rotate=2
```

# install system

```bash
sudo pacman -S vim sudo
```

## sudo

```bash
visudo

vim> %wheel ALL=(ALL) ALL
```

## xorg

```bash
sudo pacman -S bash-completion vim xorg xorg-drivers xorg-server xorg-server-utils xorg-xinit sudo git ttf-dejavu
```

## autologin

```bash
sudo cp /usr/lib/systemd/system/getty@.service /usr/lib/systemd/system/autologin@.service
sudo vi /usr/lib/systemd/system/autologin@.service

vim > ExecStart=-/sbin/agetty --noclear -a alarm %I %TERM

sudo systemctl disable getty@tty1
sudo systemctl enable autologin@tty1
sudo systemctl start autologin@tty1
```

## Pulseaudio

```bash
sudo pacman -S alsa-utils alsa-plugins gst-libav gst-plugins-good qt-gstreamer gstreamermm gst-plugins-ugly gstreamer-vaapi gstreamer

# set amixer output to automatic (0=automatic, 1=analog, 2=HDMI)
amixer cset numid=3 0
```
