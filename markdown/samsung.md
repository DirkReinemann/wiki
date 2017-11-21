<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [samsung](#samsung)
	- [install recovery image and custom rom on s3 mini](#install-recovery-image-and-custom-rom-on-s3-mini)

<!-- /TOC -->

# samsung

## install recovery image and custom rom on s3 mini

* [rom](https://www.androidfilehost.com/?w=files&flid=126317)
* [twrp and recovery](https://www.androidfilehost.com/?w=files&flid=15009)
* [xda](https://forum.xda-developers.com/galaxy-s3-mini/development/rom-cyanogenmod-14-1-gt-i8190-t3520752)

1. copy rom.zip on sdcard
2. turn phone off
3. go into download mode (VOL- & HOME & POWER)
4. install recovery image

```bash
# detect phone (download mode)
sudo heimdall detect

# print phone partitions (download mode)
heimdall print-pit

# install recovery image
heimdall flash --Kernel2 recovery.img
```

5. restart and go into recovery mode (VOL+ & HOME & POWER)
6. wipe all data
7. install rom.zip from sdcard
8. restart phone
