```
devmem2.c: Simple program to read/write from/to any location in memory.
```
Originally written by, and hosted at, www.lartmaker.nl/lartware/.
Put on Github by [Radii](http://www.lart.tudelft.nl/).

Forked version with additional features and a display similar to [devmem](https://elixir.bootlin.com/busybox/1.36.0/source/miscutils/devmem.c).

New features : 
- Add the possibility to read/write different files.
- Add an option to open a file in read-only mode.
- Write/dump memory regions.

Usage : 
```
root@qemuarm64:~# ./devmem2
Usage:  ./devmem2 [ options ] { address } [ type / length [ data ] ]
        address       : memory address to act upon
        type / length : access operation type : [b]yte, [h]alfword, [w]ord, or access length
        data          : data to be written
Options:
        --file, -f      : file to map (/dev/mem by default)
        --read-only, -r : open file in read-only mode
        --verbose, -v   : display more informations
        --help, -h      : display this help and exit
```

Examples :

```
root@qemuarm64:~# ./devmem2 0x4ff7f000
0x00000000

root@qemuarm64:~# ./devmem2 0x4ff7f000 b
0x00

root@qemuarm64:~# ./devmem2 0x4ff7f000 h
0x0000

root@qemuarm64:~# ./devmem2 0x4ff7f000 w
0x00000000

root@qemuarm64:~# ./devmem2 0x4ff7f000 32
00000000: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000010: 0000 0000 0000 0000 0000 0000 0000 0000  ................

root@qemuarm64:~# ./devmem2 0x4ff7f000 32 0x31

root@qemuarm64:~# ./devmem2 0x4ff7f000 32
00000000: 3131 3131 3131 3131 3131 3131 3131 3131  1111111111111111
00000010: 3131 3131 3131 3131 3131 3131 3131 3131  1111111111111111

root@qemuarm64:~# ./devmem2 0x4ff7f000 128
00000000: 3131 3131 3131 3131 3131 3131 3131 3131  1111111111111111
00000010: 3131 3131 3131 3131 3131 3131 3131 3131  1111111111111111
00000020: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000030: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000040: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000050: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000060: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000070: 0000 0000 0000 0000 0000 0000 0000 0000  ................

root@qemuarm64:~# ./devmem2 0x4ff7f020 w 0x31323334

root@qemuarm64:~# ./devmem2 0x4ff7f000 128
00000000: 3131 3131 3131 3131 3131 3131 3131 3131  1111111111111111
00000010: 3131 3131 3131 3131 3131 3131 3131 3131  1111111111111111
00000020: 3433 3231 0000 0000 0000 0000 0000 0000  4321............
00000030: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000040: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000050: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000060: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000070: 0000 0000 0000 0000 0000 0000 0000 0000  ................

root@qemuarm64:~# echo "Hello world!" > hello.txt

root@qemuarm64:~# ./devmem2 -f ./hello.txt 0x00000000 12
00000000: 4865 6c6c 6f20 776f 726c 6421  Hello world!

root@qemuarm64:~# ./devmem2 --file ./hello.txt --verbose 0x00000000 12
./hello.txt opened.
Memory mapped at address 0x7fb1450000.
00000000: 4865 6c6c 6f20 776f 726c 6421  Hello world!

root@qemuarm64:~# ./devmem2 --file ./hello.txt 0x00000006 1 0x57

root@qemuarm64:~# ./devmem2 --file ./hello.txt 0x00000000 12
00000000: 4865 6c6c 6f20 576f 726c 6421  Hello World!
```


