<!-- toc -->
  * [linux](#linux)
    * [ffmpeg](#ffmpeg)
      * [trim audio file and save it to file](#trim-audio-file-and-save-it-to-file)
      * [capture audio from default output and encode it to mp3 and save it to file](#capture-audio-from-default-output-and-encode-it-to-mp3-and-save-it-to-file)
    * [pulseaudio](#pulseaudio)
      * [list audio sources](#list-audio-sources)
    * [putty](#putty)
      * [ssh key to pkk](#ssh-key-to-pkk)
    * [sudoers](#sudoers)
      * [enable systemd service for group](#enable-systemd-service-for-group)
    * [mkfsdos](#mkfsdos)
      * [create fat32 partition](#create-fat32-partition)
    * [shred](#shred)
      * [wipe disk with zeros](#wipe-disk-with-zeros)
    * [xephyr](#xephyr)
      * [start virtual xserver](#start-virtual-xserver)
    * [xrandr](#xrandr)
      * [add virtual display](#add-virtual-display)
      * [change display resolution](#change-display-resolution)
    * [oneliners](#oneliners)
      * [killa all processes by name](#killa-all-processes-by-name)
      * [check if file is a valid pdf file](#check-if-file-is-a-valid-pdf-file)
    * [apache](#apache)
      * [logfile format in bash](#logfile-format-in-bash)
    * [ssh](#ssh)
      * [ssh port forwarding](#ssh-port-forwarding)
      * [ssh tunnel](#ssh-tunnel)
      * [mount directory over ssh via sshfs](#mount-directory-over-ssh-via-sshfs)
    * [firefox](#firefox)
      * [bookmark backup file](#bookmark-backup-file)
      * [trust certificate permanently](#trust-certificate-permanently)
    * [journalctl](#journalctl)
      * [remove journal logs and keep minimum](#remove-journal-logs-and-keep-minimum)
      * [print all logs from date](#print-all-logs-from-date)
    * [steam](#steam)
      * [change cs fonts](#change-cs-fonts)
    * [thunderbird](#thunderbird)
      * [read thunderbird addressbook from mozilla MORK format](#read-thunderbird-addressbook-from-mozilla-mork-format)
    * [rhythmbox](#rhythmbox)
      * [extract radio stations](#extract-radio-stations)
    * [pandoc](#pandoc)
      * [convert markdown to html with code highlighting](#convert-markdown-to-html-with-code-highlighting)
    * [cracklib](#cracklib)
      * [check password quality](#check-password-quality)
    * [steghide](#steghide)
      * [hide information inside another file](#hide-information-inside-another-file)
    * [luks](#luks)
      * [remove encryption from hard disk](#remove-encryption-from-hard-disk)
    * [pwgen](#pwgen)
      * [generate password](#generate-password)
    * [bchunk](#bchunk)
      * [bin and cue to iso file](#bin-and-cue-to-iso-file)
    * [mdf2iso](#mdf2iso)
      * [mdf to iso file](#mdf-to-iso-file)
    * [mount](#mount)
      * [mount iso](#mount-iso)
    * [xmlstarlet](#xmlstarlet)
      * [select elements](#select-elements)
    * [grep](#grep)
      * [grep everythink between quotes](#grep-everythink-between-quotes)
    * [locale](#locale)
      * [set correct locale settings](#set-correct-locale-settings)
    * [printer](#printer)
      * [find ipp url](#find-ipp-url)
      * [lpd](#lpd)
<!-- toc -->


# linux

## ffmpeg

### trim audio file and save it to file

```bash
ffmpeg -i input.mp3 -ss 00:00:10 -to 00:01:10 -c copy output.mp3
```

### capture audio from default output and encode it to mp3 and save it to file

```bash
ffmpeg -f pulse -i default -codec:a libmp3lame -qscale:a 2 output.mp3
```

## pulseaudio

### list audio sources

```bash
pactl list sources
```

## putty

### ssh key to pkk

```bash
puttygen id_rsa -o id_rsa.ppk
```

## sudoers

### enable systemd service for group

```bash
sudo visudo

vim> %wheel ALL=(ALL) NOPASSWD: /usr/bin/systemctl start bluetooth.service, /usr/bin/systemctl stop bluetooth.service (Insert after wheel)
```

## mkfsdos

### create fat32 partition

```bash
sudo mkdosfs -F 32 -I /dev/sdb
```

## shred

### wipe disk with zeros

```bash
sudo shred -vzn 0 /dev/sdb
```

## xephyr

### start virtual xserver

```bash
Xephyr -br -ac -noreset -screen 1024x768 :1
```

## xrandr

### add virtual display

```bash
xrandr --addmode VIRTUAL1 1024x768
xrandr --output VIRTUAL1 --mode 1024x768 --right-of LVDS1
```

### change display resolution

```bash
xrandr --output VGA-1 --mode 1024x768
```

## oneliners

### killa all processes by name

```bash
ps ux | grep 'exe$' | awk '{ print $2 }' | xargs -I '{}' kill {}
```

### check if file is a valid pdf file

```bash
file file.pdf | grep -o "PDF document" > /dev/null && echo 1 || echo 0
```

## apache

### logfile format in bash

```bash
date "+%d/%b/%Y:%H:%M:%S %z"
```

## ssh

### ssh port forwarding

```bash
# forwards port 8888 on remote host to port 9000 on localhost
ssh -vL 8888:localhost:9000 username@domain
```

### ssh tunnel

```bash
# socks
ssh -C2qTnN -D 8080 user@domain

# https
ssh -C2qTnN -L 8080 user@domain
```

### mount directory over ssh via sshfs

```bash
# mount
sshfs server:port:/path /mnt -o follow_symlinks

# unmount
fusermount3 -u /mnt
```

## firefox

### bookmark backup file

* [github](https://github.com/andikleen/lz4json)

```bash
# find profile directory
find $HOME/.mozilla/firefox -maxdepth 1 ! -path $HOME/.mozilla/firefox -type d | grep 'default$'

# backup folder
$HOME/.mozilla/firefox/[A-Za-z0-9]*.default/bookmarkbackups

# last bookmark backup file
ls -1 $HOME/.mozilla/firefox/[PROFILE]/bookmarkbackups | sort -r | head -1
```

### trust certificate permanently

1. open website with untrusted certificates
2. Advanced -> Add Exception... -> View... -> Details -> Export... -> Save Certificate -> Close -> Cancel
3. Preferences -> Advanced -> Certificates -> View Certificates -> Import... -> Select Downloaded Certficate -> Open -> Trust this CA to identify websites. -> OK -> OK

## journalctl

### remove journal logs and keep minimum

```bash
sudo journalctl --rotate --flush && sudo journalctl --vacuum-size=1K --vacuum-time=1s
```

### print all logs from date

```bash
# print logs from today|yesterday
journalctl -S yesterday|today

# print logs from date
journalctl -S "2017-09-09 18:30:00"
```

## steam

### change cs fonts

```bash
vi $HOME/.local/share/Steam/steamapps/common/Half-Life/platform/resource/TrackerScheme.res

vim> GameConsole_Mono
```

## thunderbird

### read thunderbird addressbook from mozilla MORK format

[wiki](https://wiki.mozilla.org/Mork)

```bash
# find profile directory
find $HOME/.thunderbird -maxdepth 1 ! -path $HOME/.thunderbird -type d | grep 'default$'

# default addressbook file
$HOME/.thunderbird/[PROFILE]/abook.mab

RELATIONS: awk '{ if ($0 ~ /^[ \t]*\[[0-9]+\(\^/ || $0 ~ /^[ \t]*\(\^/) { sub(/^[ \t]+/, "", $0); sub(/}$/, "", $0); printf $0; if ($0 ~ /\]/ ) { printf "\n" } } }' abook.mab

IFS=$'\n'
for I in $(awk '{ if ($0 ~ /^[ \t]*\[[0-9]+\(\^/ || $0 ~ /^[ \t]*\(\^/) { sub(/^[ \t]+/, "", $0); sub(/}$/, "", $0); printf $0; if ($0 ~ /\]/ ) { printf "\n" } } }' abook.mab); do
	echo $0
done
```

## rhythmbox

### extract radio stations

```bash
# database file
$HOME/.local/share/rhythmbox/rhythmdb.xml

cat rhythmdb.xml | xmlstarlet ed -d '/rhythmdb/entry[not(@type="iradio")]' | xmlstarlet ed -u '/rhythmdb/entry/play-count' -v '' -u '/rhythmdb/entry/last-played' -v '' -u '/rhythmdb/entry/bitrate' -v ''
```

## pandoc

### convert markdown to html with code highlighting

```bash
pandoc input.md -s --highlight-style pygments -o output.html
```

## cracklib

### check password quality

```bash
sudo pacman -S cracklib
echo "password" | cracklib-check
```

## steghide

### hide information inside another file

* [website](http://steghide.sourceforge.net/)
* supported formats are jpeg, bmp, wav, au

```bash
sudo pacman -S steghide

# embed
steghide embed -cf picture.jpg -ef secret.txt

# extract
steghide extract -sf picture.jpg
```

## luks

### remove encryption from hard disk

```bash
sudo cryptsetup luksRemoveKey /dev/sdx
```

## pwgen

### generate password

```bash
sudo pacman -S pwgen
pwgen 16 1
```

## bchunk

### bin and cue to iso file

```bash
sudo pacman -S bchunk
bchunk input.bin input.cue output.iso
```

## mdf2iso

### mdf to iso file

```bash
sudo pacman -S mdf2iso
mdf2iso input.mdf output.iso
```

## mount

### mount iso

```bash
mkdir $HOME/iso
sudo mnount -o loop file.iso $HOME/iso
sudo umount $HOME/iso
```

## xmlstarlet

### select elements

* [syntax](https://www.w3schools.com/xml/xpath_syntax.asp)

```bash
cat file.xml | xmlstatlet -sel -t -c "//element[@attribute]"
```

## grep

### grep everythink between quotes

```bash
grep -E "(.*?)"
```

## locale

### set correct locale settings

[archlinux](https://wiki.archlinux.org/index.php/locale)

* the directory that contains the locale defintions can be found under **/usr/share/i18n/locales**

```bash
# display installes locales
locale -a
localectl list-locales

# display locale settings
locale

# denable and disable locales
sudo vi /etc/locale.gen
sudo locale-gen

# change locale settings
sudo vi /etc/locale.conf

# set date format to YYYY-MM-DD
LC_TIME=en_DK.UTF-8
```

## printer

* check ports for
	* lpd: 515/tcp
	* ipp: 631/tcp

```
sudo nmap -p1-1024 [PRINTER IP ADDRESS]
```

### find ipp url

```bash
ippfind

ipp://IP:631/RESOURCE
```

### lpd

```
lpd://IP:515/PASSTHRU
```
