DexPwn: DexProtector Utility
======================================================================

This utility can either steal protected files from DexProtector and copy them to a specified path, or it can be used to replace one of those files at runtime with an altered version.

DexProtector encrypts the original application DEX file and decrypts it at runtime. After decryption DexProtector must write the file to disk in order to load the DexFile into memory. By utilizing inotify we can be alerted to when these files are saved and then operate on them.

Credits
----------------------------------------------------------------------
This project makes use of code that was provided by mkttanabe with their inotifywait-for-Android. I have removed items that we not required by DexPwn and extended on the example.c that was provided in the libinotifytools folder.

I claim no copyright to any files with the exception of dexpwn.c

Copyright
----------------------------------------------------------------------
dexpwn.c is licensed under the GNU GPL Public license. For more information please see the LICENSE file.