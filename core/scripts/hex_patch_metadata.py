#!/usr/bin/env python3
import sys, os
import hmac, hashlib
sys.path += [ os.path.join(os.path.split(__file__)[0], 'libs') ]
from intelhex import IntelHex16bit

# Plain Unsafe ops (op = bytes = word {little endian})
# ----------------------------------------------------
# ret = 0x08 0x95 = 0x9508
# reti = 0x18 0x95 = 0x9518
# ijmp = 0x09 0x94 = 0x9409
# icall = 0x09 0x95 = 0x9509
# elpm = 0xD8 0x95 = 0x95D8

# Unsafe ops with arguments
# -------------------------
# elpm Rd,Z = op & (0xFE0F) == 0x9006
# elpm Rd,Z+ = op & (0xFE0F) == 0x9007

# Conditionally unsafe ops (depending on relative target address)
# ---------------------------------------------------------------
# brbs s,k = op & (0xFC00) == 0xF000
# brbc s,k = op & (0xFC00) == 0xF400
# rjmp k = op & (0xF000) == 0xC000
# rcall k = op & (0xF000) == 0xD000

# Two word ops
# ---------------------------
# jmp = 0x0C 0x94 = 0x940C
# call = 0x0E 0x94 = 0x940E

# Opcodes are represented little-endian everywhere (toolchain/datasheet/etc)

# Unsafe ops (should never appear anywhere except as 2nd word in long calls)
unsafe_ops = [int("0x9508", 16), int("0x9518", 16), int("0x9409", 16),
        int("0x9509", 16), int("0x95D8",16)]
elpm_mask = int("0xFE0F", 16)
elpm_ops = [int("0x9006", 16), int("0x9007", 16)]

# Conditionally unsafe ops (just log when they appear as 2nd word in long call)
branch_ops_mask = int("0xFC00", 16)
branch_ops = [int("0xF000", 16), int("0xF400", 16)]
rel_ops_mask = int("0xF000", 16)
rel_ops = [int("0xC000", 16), int("0xD000", 16)]

# Two word ops
two_word_ops = [int("0x940C", 16), int("0x940E", 16)]

# Last page of app is reserved for metadata. One page is 256 bytes.
# !! Caution !! This is a word address (16bit) and is used with IntelHex16bit
# class
#metadata_offset = int("0xFD00", 16)//2 # 1Kb bootloader
#metadata_offset = int("0xFB00", 16)//2 # 2Kb bootloader
metadata_offset = int("0xF700", 16)//2 # 4Kb bootloader
#metadata_offset = int("0xEF00", 16)//2 # 8Kb bootloader

# 160 bit key
key = b'\x6e\x26\x88\x6e\x4e\x07\x07\xe1\xb3\x0f\x24\x16\x0e\x99\xb9\x12\xe4\x61\xc4\x24'

def main(argv):
   if len(argv) != 3:
      print('hex_patch_metadata.py <ihexfile> <datastart> <dataend>')
      sys.exit(2)

   # Check if hexfile exists
   hexfile = argv[0]
   if not os.path.isfile(hexfile):
      print("ERROR: File not found:", hexfile)
      sys.exit(2)

   # Parse data offset
   try:
      datastart = int(argv[1], 16) # in hex, first byte of .data. == size .text
      dataend = int(argv[2], 16) # in hex, end of .data last byte not included. == total size .text + .data
   except:
      print("ERROR: Offsets not valid:", argv[1], argv[2])
      sys.exit(2)

   # Start parsing ihex
   ih = IntelHex16bit(hexfile)

   # Look for opcodes that __can__ be unsafe as 2nd word and log them
   unsafe_2ndword = list()
   prev_op_long = 0
   for addr in range(datastart//2):
       if ih[addr] in two_word_ops and not prev_op_long:
           prev_op_long = 1
       elif prev_op_long:
           prev_op_long = 0
           if ( (ih[addr] & branch_ops_mask) in branch_ops or
                (ih[addr] & rel_ops_mask) in rel_ops or
                (ih[addr] & elpm_mask) in elpm_ops or
                 ih[addr] in unsafe_ops or
                 ih[addr] in two_word_ops ):
               print("Unsafe 2nd word, adding to list!")
               print(ih[addr])
               print(addr)
               unsafe_2ndword.append(addr)

   # Start patching hex
   ih[metadata_offset] = dataend # total .text + .data size
   ih[metadata_offset + 1] = datastart # .data start address == .text size
   ih[metadata_offset + 2] = len(unsafe_2ndword)
   for idx, addr in enumerate(unsafe_2ndword):
       ih[metadata_offset+3+idx] = addr

   # Calculate word size of metadata section
   meta_size = 3 # base size
   meta_size += len(unsafe_2ndword)

   # Calculate hmac
   hmac_gen = hmac.new(key, None, hashlib.sha1)
   hmac_gen.update(ih.tobinstr(0,dataend-1)) # tobinstr uses byteaddr, even on an IntelHex16 object
   hmac_gen.update(ih.tobinstr(metadata_offset*2, (metadata_offset + meta_size)*2 - 1))
   print(hmac_gen.hexdigest());

   # Add hmac after regular metadata (frombytes uses byteaddr, even on IntelHex16 objects)
   ih.frombytes(hmac_gen.digest(), (metadata_offset + meta_size)*2)

   # Write out file
   ih.write_hex_file(hexfile)

if __name__ == "__main__":
     main(sys.argv[1:])
