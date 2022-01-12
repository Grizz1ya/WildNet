# DISCLAIMER

This project should be used for authorized testing or educational purposes only.

The main objective behind creating this offensive project was to aid security researchers and to enhance the understanding of commercial TCP loader style botnets. We hope this project helps to contribute to the malware research community and people can develop efficient countermeasures.

Usage of WildNET without prior mutual consistency can be considered as an illegal activity. It is the final user's responsibility to obey all applicable local, state and federal laws. Authors assume no liability and are not responsible for any misuse or damage caused by this program.

# ABOUT
This instance is written in pure C++. The weight is ~20 kb.

The server is written in python and copes with its tasks perfectly.

The crypt adds a signature from another license file, compresses the binary file and removes all reactions from antiviruses (including Windows Defender in scan and run time).

# Screenshots
![Alt-текст](https://raw.githubusercontent.com/Grizz1ya/WildNet/main/screen.jpg "Орк")

# Commands
`ping` - check user

`url` - lets the user know that the next input will be a link to upload file (x32&x64)

`build` - lets the user know that the next input will be a link on a new build

`cancel` - end of waiting for all links

`exit` - exit

`host` -  lets the user know that the next input will be new host to reconnect

`reload` - reinstall build on PC users

# Features
Adding to auto-upload via lnk file.

Downloading and launching a file via a direct link.

Getting a backup host address if it was not possible to connect to the one specified in the build.

Automatic restart every 30 minutes.

Encryption of all strings

# Usage

### BUILD
Change host in `Main.cpp`
`auto HOST = "127.0.0.1";`

Change recovery host address in `Tools.h`
`#define BACKUPPATH TEXT(XorStr("/Grizz1ya/WildNet/main/backuphost"))`

Compile with Realease & x86

### CRYPT
Rename the file to `in.exe`, copy to the `Crypt` folder and run `Signer.bat`

### PANEL
If not localhost open `in&out` `TCP/UDP` connections with port `1234`
Start `WildServer.py`

`list' - see all bots

`cls` - clear console

etc commands from build...





