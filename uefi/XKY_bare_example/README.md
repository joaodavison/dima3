# XKY Example

This is an XKY example with one bare partition.
The partition will:
- be scheduled
- print message
- enter in IDLE_MODE

## Compile XKY Module

To compile this XKY Example Module, we assume the user has access to the xmf_make tool and the XKY OS pre-compiled.
This can be provided by contacting GMV at xky_support@gmv.com.

Inside the current folder, run the xmf_make (it should be available from the user PATH):

`# xmf_make`

This will generate inetrmediary files that will be helpful to link all XKY elements together.

Then run:

`# make`


At this point, a folder 'executable' will be available with XKYAPP.bin file.
This file is what needs to be uploaded to TFTP Server with name 'XKY_JETSON_APP.bin', so that the UEFI APPLICATION is able to detect the application and run it.

