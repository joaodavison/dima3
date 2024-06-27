with open('kernel.efi', 'rb+') as f:
    f.seek(0x38)
    f.write(b'\x41')

    f.seek(0x39)
    f.write(b'\x52')

    f.seek(0x3A)
    f.write(b'\x4d')

    f.seek(0x3B)
    f.write(b'\x64')

