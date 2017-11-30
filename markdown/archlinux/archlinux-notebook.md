# archlinux notebook

## gtk

### remove recently used files

```bash
rm $HOME/.local/share/recently-used.xbel
```

### set recent file limit

```bash
vi $HOME/.config/gtk-3.0/settings.ini

[Settings]
gtk-recent-files-limit=0
```

## power management thinkpad

[wiki](http://linrunner.de/en/tlp/docs/tlp-configuration.html)

```bash
sudo pacman -S tlp acpi_call
sudo systemctl enable tlp.service
sudo systemctl enable tlp-sleep.service
sudo systemctl disable systemd-rfkill.service
```

```bash
sudo vi /etc/default/tlp

# starts charging at 40 percent and stops at 90 percent
START_CHARGE_THRESH_BAT0=40
STOP_CHARGE_THRESH_BAT0=90
```

```bash
sudo reboot
```

## manpage highlighting

```bash
vi $HOME/.bashrc

man()
{
    LESS_TERMCAP_md=$'\e[01;31m' \
    LESS_TERMCAP_me=$'\e[0m' \
    LESS_TERMCAP_se=$'\e[0m' \
    LESS_TERMCAP_so=$'\e[01;44;33m' \
    LESS_TERMCAP_ue=$'\e[0m' \
    LESS_TERMCAP_us=$'\e[01;32m' \
    command man "$@"
}
```

## change qt theme

```bash
qtconfig-qt4
```

```bash
vi $HOME/.config/Trolltech.conf

[Qt]
style=adwaita
```

## vim ctags

```bash
vi $HOME/.vimrc

Plugin 'craigemery/vim-autotag'
```

## packages

* wd719x-firmware (AUR)
* aic94xx-firmware (AUR)

## change thinkfan level

```bash
echo level 7 | sudo tee /proc/acpi/ibm/fan
```
