# Wireless for Bela

Here's how to set up Bela as an access point (courtesy of Martin Borisov Minovski):

First we need to connect Bela to the Internet so we can download some software needed for the access point. This can be achieved by activating Internet sharing on your phone. I wouldn't recommend using the university's wireless network because it uses a RADIUS server (using your uni login details) which can be very tricky to configure. Assuming your phone's WiFi name (a.k.a. BSSID) is "iPhone" and the password is "wifipassword", here's what you need to do:

1. Connect Bela to your computer using a micro USB cable and wait for 30 seconds
2. Open a terminal window and type ssh 192.168.7.2
3. For Windows users, use the Putty program - download from here, then in the Host Name box, type 192.168.7.2
4. When you see "login as:", type root and hit Enter

Seeing "root@bela:~#" means that you have SSH access which will allow to do anything you want with your Bela.

* Plug in your WiFi USB dongle to the USB port. 
* `ip link set wlan0 up` - in order to activate the interface.
* `nano /etc/network/interfaces` - this will open up a text editor. Look up the piece of text that looks like "iface wlan0....". Change it so it looks like so:

```C++
allow-hotplug wlan0
iface wlan0 inet dhcp
wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
```

Note that there are no "#" signs in the beginning of each line. To save the file, press Ctrl+X, followed by Y to confirm.

Back in the command line:

* `/etc/wpa_supplicant/wpa_supplicant.conf`

Add the following in the (initially empty) file:

```C++
network={
ssid="iPhone"
psk="wifipassword"
}
```

* `/etc/init.d/networking restart` - to restart the networking service
* `ifdown wlan0`
* `ifup wlan0`

Now your Bela should have access to the Internet through your phone. Type `ping 8.8.8.8` to test it out. If it works, you can download the package needed for the access point functionality:

```C++
apt-get upgrade
apt-get update
apt-get install hostapd - press Y when prompted. This will install the service which will turn Bela into a WiFi access point.
nano /etc/default/hostapd
```

Find the line that starts with DAEMON_CONF in /etc/default/hostapd, uncomment it and change it so it looks like this:

* `DAEMON_CONF="/etc/hostapd/hostapd.conf"`

Now we've told hostapd to look for a configuration file, so we need to create it:

* `nano /etc/hostapd/hostapd.conf`

Put the following into this file:

```C++
interface=wlan0
hw_mode=g
channel=1
wmm_enabled=1
ssid=MyBelaAccessPoint
auth_algs=1
wpa=2
wpa_passphrase=wifipassword
wpa_key_mgmt=WPA-PSK
rsn_pairwise=CCMP
ieee80211d=1
country_code=DK
ieee80211n=1
```

Remember to change the `ssid=` and `wpa_passphrase=` lines. 

*`nano /etc/network/interfaces`

Now we need to change the network configuration so that Bela advertises its network instead of trying to connect to one (the internet connection that was initially set up). To do that, replace the wlan0 configuration with the following:

```C++
allow-hotplug wlan0
iface wlan0 inet static
hostapd /etc/hostapd/hostapd.conf
address 192.168.30.1
netmask 255.255.255.0
```

Now, restart Bela by typing "shutdown -r now". Once Bela reboots, you should be able to see a network called MyBelaAccessPoint. You would need static IP configuration (usually IP addresses from 192.168.30.2 to 192.168.30.254) and a subnet mask of 255.255.255.0 on each connecting client. Ideally, a DHCP server using dnsmasq would be needed so that you don't worry about IP configuration, but that hasn't been tried yet. 

Once you have connected and set up the client device's IP configuration, you are able to access Bela IDE in the browser at 192.168.30.1, as well as connect to it via SSH.
