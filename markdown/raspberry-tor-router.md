# raspberry tor router

Um zu überprüfen, ob der WLAN-Adapter die Konfiguration unterstützt, kann folgender Befehl aufgerufen werden.

```bash
iw list
```

Die Ausgabe muss folgende Zeilen beinhalten.

```bash
software interface modes (can always be added):
  * AP/VLAN
  * monitor
```

## install

Zuerst müssen die folgenden drei Pakete installiert werden.

```bash
sudo apt-get install tor hostapd dnsmasq
```

Als nächstes können die einzelnen Dienste angehalten werden. Dieser Schritt ist optional.

```bash
sudo service tor stop
sudo service hostapd stop
sudo service dnsmasq stop
```

## hostapd

Zuerst muss die Datei ''/etc/init.d/hostapd'' mit folgender Änderung versehen werden.

```bash
DAEMON_CONF=/etc/hostapd/hostapd.conf
```

Nun wird die gerade angegebene Konfigurationsdatei ''/etc/hostapd/hostapd.conf'' erstellt und mit den Einstellungen versehen.

```bash
interface=wlan0
driver=nl80211
ssid=intranet-anonym
channel=3
ignore_broadcast_ssid=0
country_code=DE
ieee80211d=1
hw_mode=g
beacon_int=100
dtim_period=2
max_num_sta=10
logger_syslog=-1
logger_syslog_level=2
logger_stdout=-1
logger_stdout_level=2
dump_file=/tmp/hostapd.dump
ctrl_interface=/var/run/hostapd
ctrl_interface_group=0
auth_algs=3
wmm_enabled=0
wpa=2
rsn_preauth=1
rsn_preauth_interfaces=wlan0
wpa_key_mgmt=WPA-PSK
rsn_pairwise=CCMP
wpa_group_rekey=600
wpa_ptk_rekey=600
wpa_gmk_rekey=86400
wpa_passphrase=<PASSPHRASE>

# macaddr_acl=1 # mac filter
# accept_mac_file=/etc/hostapd/hostapd.accept # mac filter list
```

Zum Testen der Konfiguration kann folgender Befehl aufgerufen werden.

```bash
sudo hostapd -dd /etc/hostapd/hostapd.conf
```

## dnsmasq

Zuerst entfernen wir alle Kommentare aus der dnsmasq Konfigurationsdatei.

```bash
sudo sed -i '/^#/d' /etc/dnsmasq.conf
```

Anschließend muss die folgende Konfiguration in die Datei ''/etc/dnsmasq.conf'' eingetragen werden.

```bash
interface=wlan0
no-dhcp-interface=eth0
dhcp-range=interface:wlan0,192.168.10.100,192.168.10.110,infinite
```

## interfaces

Zuletzt muss noch die Datei ''/etc/network/interfaces'' bearbeitet werden.

```bash
auto wlan0
iface wlan0 inet static
address 192.168.10.1
netmask 255.255.255.0
broadcast 192.168.10.255
```

## start

Um alle Dienste zu starten, müssen die folgenden Befehle ausgeführt werden.

```bash
sudo ifdown wlan0
sudo ifup wlan0
sudo service hostapd start
sudo service dnsmasq start
```
