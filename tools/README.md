
# tools

This directory contains a number of tools for experimenting with deflating:

* **hashtester** is an experimental implementation of the ideas behind *ajflate*. It can use several hash strategies and window sizes for finding repeated patterns in the raw input. It can filter stdin with ***-f*** or experiment with a variety of options over a directory tree with ***-a***.
* **gzdump** decodes a gzip file, printing out lots of verbose state information along the way. The output format can be fed into **encode**.
* **encode** takes as input a text file containing 1's and 0's, and writes out raw binary. (Other text is treated as comments)
* **bindump** is the reverse of **encode** - converts raw binary to textual 1's and 0's, one byte per line.
* **raw2c** converts raw binary into a C array.
* **analyzedump.pl** reads **gzdump**'s output and prints some stats about the gzip stream.
* **evenbytes** prints out information about combinations of repetition length and distance under the standard "fixed Huffman encoded" which occupy a multiple of 8 bits. *ajflate* actually doesn't end up using this.

The directory also contains *deflate_block_header.in* which defines the deflate header used by *ajflate*.

### Walk-through

If you have some data in sample.dat, you can:

    # Compress it with hashtester (then gunzip to confirm you get the same data back out)
    ./hashtester -f zlib-1 < sample.dat > sample.gz
    gunzip < sample.gz > sample.dat.2
    md5sum sample.*
    
    # Dump the binary out, both naively (with bindump) and with lots of info about the gzip data (gzdump)
    ./gzdump < sample.gz > sample.dump.smart
    ./bindump < sample.gz > sample.dump.naive
    
    # Print some stats about the compressed data in sample.gz
    perl ./analyzedump.pl < sample.dump.smart
    
    # Confirm that you can turn both dumps back into the gzip
    ./encode < sample.dump.smart > sample.gz.1
    ./encode < sample.dump.naive > sample.gz.2
    md5sum sample.gz*
    
### License

Copyright (C) Grumpy Panda Pty Ltd.

The code in this directory is licensed under the GNU General Public License, Version 2. See LICENSE.GPL.


