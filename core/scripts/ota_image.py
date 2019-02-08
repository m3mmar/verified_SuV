#!/usr/bin/env python3
import sys, os, struct
sys.path += [ os.path.join(os.path.split(__file__)[0], 'libs') ]
from intelhex import IntelHex16bit

# Last page of app is reserved for metadata. One page is 256 bytes.
# !! Caution !! This is a word address (16bit) and is used with IntelHex16bit
# class
#metadata_offset = int("0xFD00", 16)//2 # 1Kb bootloader
#metadata_offset = int("0xFB00", 16)//2 # 2Kb bootloader
metadata_offset = int("0xF700", 16)//2 # 4Kb bootloader
#metadata_offset = int("0xEF00", 16)//2 # 8Kb bootloader

def main(argv):
   if len(argv) != 2:
      print('ota_image.py <ihexfile> <binfile>')
      sys.exit(2)

   # Check if hexfile exists
   hexfile = argv[0]
   if not os.path.isfile(hexfile):
      print("ERROR: File not found:", hexfile)
      sys.exit(2)

   # Target binfile
   binfile = argv[1]

   # Start parsing ihex
   ih = IntelHex16bit(hexfile)

   # Get data end == total .text + .data size
   dataend = ih[metadata_offset]

   # Calculate word size of metadata section
   meta_size = 3 # base size
   meta_size += ih[metadata_offset+2] # len(unsafe_2ndword)
   meta_size += 10 # HMAC

   # Start filling file
   f = open(binfile, 'wb')

   # First write metadata to beginning of file (!!Word addresses!!)
   ih.tobinfile(f, metadata_offset*2, (metadata_offset + meta_size)*2 - 1)
   # Write .text + .data after
   ih.tobinfile(f, 0, dataend-1)

   f.close()

if __name__ == "__main__":
     main(sys.argv[1:])
