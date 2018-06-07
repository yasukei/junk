#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import io
import struct

class ELDHeaderIdentifier:
    length = 16

    def __init__(self, byte_array):
        if len(byte_array) != self.length:
            return False
        ids = struct.unpack('<16B', byte_array)
        self.MAG0       = ids[0]
        self.MAG1       = ids[1]
        self.MAG2       = ids[2]
        self.MAG3       = ids[3]
        self.CLASS      = ids[4]
        self.DATA       = ids[5]
        self.VERSION    = ids[6]
        self.OSABI      = ids[7]
        self.ABIVERSION = ids[8]
        self.PAD        = ids[9:]

    def is_valid(self):
        if self.MAG0 == 0x7f and self.MAG1 == 0x45 and self.MAG2 == 0x4c and self.MAG3 == 0x46:
            return True
        else:
            return False

    def is_for_32bit(self):
        return True if self.CLASS == 1 else False

    def is_little_endian(self):
        return True if self.DATA == 1 else False

    def get_str_of_CLASS(self):
        which = '32' if self.is_for_32bit() else '64'
        return which + '-bit format'

    def get_str_of_DATA(self):
        which = 'little' if self.is_little_endian() else 'big'
        return which + ' endian'

    def get_str_of_OSABI(self):
        dic = {
                0x00 : 'System V',
                0x01 : 'HP-UX',
                0x02 : 'NetBSD',
                0x03 : 'Linux',
                0x04 : 'GNU Hurd',
                0x06 : 'Solaris',
                0x07 : 'AIX',
                0x08 : 'IRIX',
                0x09 : 'FreeBSD',
                0x0A : 'Tru64',
                0x0B : 'Novell Modesto',
                0x0C : 'OpenBSD',
                0x0D : 'OpenVMS',
                0x0E : 'NonStop Kernel',
                0x0F : 'AROS',
                0x10 : 'Fenix OS',
                0x11 : 'CloudABI',
                0x53 : 'Sortix',
                }
        return dic.get(self.OSABI, 'unknown')

    def print(self):
        print('MAG0       = 0x{:02x}'.format(self.MAG0))
        print('MAG1       = 0x{:02x}'.format(self.MAG1))
        print('MAG2       = 0x{:02x}'.format(self.MAG2))
        print('MAG3       = 0x{:02x}'.format(self.MAG3))
        print('CLASS      = 0x{:02x} ({:s})'.format(self.CLASS, self.get_str_of_CLASS()))
        print('DATA       = 0x{:02x} ({:s})'.format(self.DATA, self.get_str_of_DATA()))
        print('VERSION    = 0x{:02x}'.format(self.VERSION))
        print('OSABI      = 0x{:02x} ({:s})'.format(self.OSABI, self.get_str_of_OSABI()))
        print('ABIVERSION = 0x{:02x}'.format(self.ABIVERSION))
        #print('PAD        = {:d}'.format(self.PAD))

class ELDHeaderOthers:
    def __init__(self, byte_array, bit32=True, little_endian=True):
        if bit32 and len(byte_array) != 36:
            return False
        elif len(byte_array) != 48:
            return False

        endian = '<' if little_endian else '>'
        entries = struct.unpack(endian + 'HHI', byte_array[0:8])
        self.e_type          = entries[0]
        self.e_machine       = entries[1]
        self.e_version       = entries[2]

        if bit32:
            entries = struct.unpack(endian + 'III', byte_array[8:20])
        else:
            entries = struct.unpack(endian + 'QQQ', byte_array[8:32])
        self.e_entry    = entries[0]
        self.e_phoff    = entries[1]
        self.e_shoff    = entries[2]

        if bit32:
            entries = struct.unpack(endian + 'IHHHHHH', byte_array[20:36])
        else:
            entries = struct.unpack(endian + 'IHHHHHH', byte_array[32:48])
        self.e_flags        = entries[0]
        self.e_ehsize       = entries[1]
        self.e_phentsize    = entries[2]
        self.e_phnum        = entries[3]
        self.e_shentsize    = entries[4]
        self.e_shnum        = entries[5]
        self.e_shstrndx     = entries[6]

    def print(self):
        print('type       = 0x{:02x}'.format(self.e_type))
        print('machine    = 0x{:02x}'.format(self.e_machine))
        print('version    = 0x{:04x}'.format(self.e_version))
        print('entry      = 0x{:04x}'.format(self.e_entry))
        print('phoff      = 0x{:04x}'.format(self.e_phoff))
        print('shoff      = 0x{:04x}'.format(self.e_shoff))
        print('flags      = 0x{:04x}'.format(self.e_flags))
        print('ehsize     = 0x{:02x}'.format(self.e_ehsize))
        print('phentsize  = 0x{:02x}'.format(self.e_phentsize))
        print('phnum      = 0x{:02x}'.format(self.e_phnum))
        print('shentsize  = 0x{:02x}'.format(self.e_shentsize))
        print('shnum      = 0x{:02x}'.format(self.e_shnum))
        print('shstrndx   = 0x{:02x}'.format(self.e_shstrndx))

class Elem:
    def __init__(self, offset, size, name, byte_array):
        self.offset = offset
        self.size = size
        self.name = name

class ELDHeader:
    def __init__(self, byte_array):
        if len(byte_array) < ELDHeaderIdentifier.length:
            return False

        self.e_ident = ELDHeaderIdentifier(byte_array[0:16])
        if self.e_ident.is_for_32bit:
            self.e_others = ELDHeaderOthers(byte_array[16:52]) 
        else:
            self.e_others = ELDHeaderOthers(byte_array[16:64]) 
	#uint16_t e_type;
	#uint16_t e_machine;
	#uint32_t e_version;

	#uint64_t e_entry;
	#uint64_t e_phoff;
	#uint64_t e_shoff;

	#uint16_t e_flags;
	#uint16_t e_ehsize;
	#uint16_t e_phetsize;
	#uint16_t e_phnum;
	#uint16_t e_shentsize;
	#uint16_t e_shnum;
	#uint16_t e_shstrndx;

def main():
    print("hello")
    with io.open('hello', 'rb') as elffile:
        ELDHeader(elffile.read(16))
    pass

if __name__ == '__main__':
    main()

