# UEFI

Because GRUB does not allow allocating memory to specific regions, we have developed an UEFI Application that is able to:
- Find 'XKY_JETSON_APP.bin' in any filesystem
- Allocate space in DRAM to place the file into memory
- Place the file contents into 0x80000000
- Move program counter to 0x80000000

## Setup Instructions

In order to run the UEFI APP, we need:
1- Enter in UEFI SHELL

    - Go to the Boot Manager by pressing F11

    - Select 'UEFI Shell' from the Boot Menu.

2- Configure eth0
  
    - run:
    `Shell> ifconfig -s eth0 static <IP_ADDR> <NET_MASK> <IP_GATEWAY>`

3- Change to a Filesystem (preferably, the EFI Filesystem that contains the directory /EFI/BOOT)

    - In our environment, which is the provided by default, it's the FS3. Change to FS3 by running:
    `Shell>FS3:`


4- Load the XKYEfiApp.efi from TFTP server or other medium
    
    - Ensure XKYEfiApp.efi is available at TFTP Server
    
    - Run command to download from TFTP server:
    `FS3:>tftp <SERVER_IP_ADDR> XKYEfiApp.efi`

5- Load the XKY_JETSON_APP.bin from the TFTP server or other medium

    - Ensure XKY_JETSON_APP.bin is available at TFTP Server
    
    - Run command to download from TFTP server:
    `FS3:>tftp <SERVER_IP_ADDR> XKY_JETSON_APP.bin`

6- Run the XKYEfiApp.efi UEFI APP
   
   - Run command to run UEFI APP:
    `FS3:>XKYEfiApp.efi`

