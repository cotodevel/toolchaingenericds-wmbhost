![ToolchainGenericDS](img/TGDS-Logo.png)

NTR/TWL SDK: TGDS1.65

master: Development branch. Use TGDS1.65: branch for stable features.

This is the Toolchain Generic wmbhost project:

Compile Toolchain: To compile this project you will need to follow the steps at https://bitbucket.org/Coto88/toolchaingenericds : Then simply extract the project somewhere.

Compile this project: Open msys, through msys commands head to the directory your extracted this project. Then write: make clean make

After compiling, run the project in NDS.

Project Specific description: 
ToolchainGenericDS-wmbhost sends a Download Play file to a flashme'd Nintendo DS.


What it does:
Button (X) 		->	(A): 	Download Play Host: Send any file, regardless of size, from SD (Press X to retry if sending fails)

WMB Host
Version 0.1

Proof of Concept

By Juglak

------

This program will run on a DS, and transmit NDS Binaries to another DS which has been flashed with FlashMe (so that it ignores WMB signatures).

To try this program, simply run it on your DS. This will broadcast a built in demo program.

If you are using a flashcart compatible with gba_nds_fat, you can place a file named
"test.nds" in the root of your media, and it will try to load and broadcast that file.

I've tested this using a few DS Lite's and a single older DS. I had about 90% success.

Sometimes the host will hang... simply press X to restart the host.

If the Wifi hardware is not initialized correctly, you will have to restart the DS. (Power cycle)

---

This is just a proof of concept and is not a finished program.

Hopefully this can be built upon to aid in making wireless multiplayer homebrew games and such.

---

Enjoy.

-J



____Remoteboot____
Also, it's recommended to use the remoteboot feature. It allows to send the current TGDS Project over wifi removing the necessity
to take out the SD card repeteadly and thus, causing it to wear out and to break the SD slot of your unit.

Usage:
- Make sure the wifi settings in the NintendoDS are properly set up, so you're already able to connect to internet from it.

- Get a copy of ToolchainGenericDS-multiboot: https://bitbucket.org/Coto88/ToolchainGenericDS-multiboot/get/TGDS1.65.zip
Follow the instructions there and get either the TWL or NTR version. Make sure you update the computer IP address used to build TGDS Projects, 
in the file: toolchaingenericds-multiboot-config.txt of said repository before moving it into SD card.

For example if you're running NTR mode (say, a DS Lite), you'll need ToolchainGenericDS-multiboot.nds, tgds_multiboot_payload_ntr.bin
and toolchaingenericds-multiboot-config.txt (update here, the computer's IP you use to build TGDS Projects) then move all of them to root SD card directory.

- Build the TGDS Project as you'd normally would, and run these commands from the shell.
<make clean>
<make>

- Then if you're on NTR mode:
<remoteboot ntr_mode computer_ip_address>

- Or if you're on TWL mode:
<remoteboot twl_mode computer_ip_address>

- And finally boot ToolchainGenericDS-multiboot, and press (X), wait a few seconds and TGDS Project should boot remotely.
  After that, everytime you want to remoteboot a TGDS Project, repeat the last 2 steps. ;-)



/release folder has the latest binary precompiled for your convenience.

Latest stable release: https://bitbucket.org/Coto88/ToolchainGenericDS-wmbhost/get/TGDS1.65.zip

Coto
