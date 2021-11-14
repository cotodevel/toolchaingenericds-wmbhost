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


/release folder has the latest binary precompiled for your convenience.

Latest stable release: https://bitbucket.org/Coto88/ToolchainGenericDS-wmbhost/get/TGDS1.65.zip

Coto
