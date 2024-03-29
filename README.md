# Presage based input predictor for the Sailfish OS
This input handler plugin provides an alternative text prediction solution to the Xt9 engine shipped by the Jolla. It could be useful if you are using community supported language pack or if you have a ported device.

## Features
The predicted words are generated by the presage library. The presage predictions coming from various plugins:
* ngram: this plugin uses a database generated from text corpuses. Basically it tries to match your typed word with sentence fragments in the database and offers the words from those segments. 
* user-ngram: this plugin works the same manner as the ngram, but it's database is expanded continously as you type. 
* hunspell: it tries to spellcheck the currently typed word with hunspell and if it found to be mistyped it will suggest the correct version as well

# Distribution, releases
The released build of this plugin (and keyboard layouts utilizing it) can be installed from Sailfish Chum repositories:
```
# enable chum repository

# search chum repos
zypper se keyboard-presage-

# install
zypper in keyboard-presage-en_US
```
or OpenRepos:
- https://openrepos.net/content/sailfishkeyboard/keyboard-presage-enus
- https://openrepos.net/content/sailfishkeyboard/maliit-plugin-presage

Range of keyboards are available.

# Development hints for using the Sailfish OS SDK
If you try to build this package on a vanilia Mer build machine you are going to get some similar errors:
```
No provider of 'libmarisa-devel' found.
No provider of 'libpresage-devel' found.
error: Failed build dependencies:
	hunspell-devel >= 1.5.1 is needed by maliit-plugin-presage-1.0-7.armv7hl
	libmarisa-devel is needed by maliit-plugin-presage-1.0-7.armv7hl
	libpresage-devel is needed by maliit-plugin-presage-1.0-7.armv7hl
	pkgconfig(sqlite3) is needed by maliit-plugin-presage-1.0-7.armv7hl
```

To overcome this you will need to add Sailfish Chum repositories to your target.

Login to your Mer VM with the following command:
```
ssh -p 2222 -i /opt/SailfishOS/vmshare/ssh/private_keys/engine/mersdk mersdk@localhost
```

Run the following commands to register Chum OBS repo and install the dependencies (replace with the corresponding SFOS release):
```
sb2 -t SailfishOS-4.2.0.21-aarch64 -m sdk-install -R zypper ar -f https://repo.sailfishos.org/obs/sailfishos:/chum/4.2.0.21_aarch64/ chum_obs
sb2 -t SailfishOS-4.2.0.21-aarch64 -m sdk-install -R zypper ref
sb2 -t SailfishOS-4.2.0.21-aarch64 -m sdk-install -R zypper in libpresage-devel libmarisa-devel
```
Now you can build and deploy the package from the Sailfish OS SDK's QtCreator

# Debugging (khm. troubleshooting) info
To debug a keyboard plugin you should run on your device:
```
pkill maliit-server; MALIIT_DEBUG=enabled maliit-server
```
(Console output is controlled by the MALIIT_DEBUG variable)
If you know how to hook this through the GDB in the SailfishOS SDK let me know.

# Thanks, inspiration, links
Matteo Vescovi who developed the Presage:
http://presage.sourceforge.net/

Hanhsuan's predictor helped me at the beginnging a lot
https://github.com/hanhsuan/jolla-chewing

Pekka Vuorela for the debugging tips
