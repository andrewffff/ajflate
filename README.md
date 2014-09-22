# ajflate

*ajflate* is a specialized implementation of the *deflate* compression algorithm. *deflate* is used by protocols such as SSH and HTTP, and file formats like PNG, GZ and ZIP.

The *deflate* format involves two kinds of compression:
* Lempel-Ziv encoding, which can replace repeated content with references to one copy.
* Huffman encoding, which can encode symbols using a variable number of bits. (Compare to regular 8-bit ASCII, where 'e' and 'z' always take up 8 bits each, even though the former is often far more common).

Most compression algorithms designed for speed only do Lempel-Ziv. Some examples are  [Snappy](https://code.google.com/p/snappy/), [LZ4](http://fastcompression.blogspot.com/2011/05/lz4-explained.html) and [LZJB](http://en.wikipedia.org/wiki/LZJB). They tend to be byte-orientated, processing data in multiples of 8 bits.

Variable-length encodings tend to be slower. Not only does determining the right Huffman table for particular content take time, but operating on individual bits is more work. The *deflate* specification describes one "fixed" Huffman table which most encoders use on their "fast" setting, but the encoder still has to shunt bits around.

*ajflate* does one new thing - it produces compressed content using a tailored Huffman encoding which outputs only 1-byte and 3-byte data fragments. This allows the encoder to work one byte at a time, more closely resembling the aforementioned fast encoders than a typical *deflate* encoder. In principle, it can be very fast.

There are tradeoffs:
* The *deflate* / *zlib* format really wasn't designed for this. It's shoehorned in. The encoder will never be quite as fast as something like LZO, and will always compress a bit worse.
* Only 7-bit values (ie those values under 128) can be encoded. This is true for many English web pages and Javascript files. It's not true for PNG images, EXE downloads, non-English text or many other things. The encoder will break out of compression mode to handle this content, so it'll "work", but the encoding will be slow and the resulting "compressed" data will be large.

### About the implementation

* **lib/** contains a standalone implementation. It's not as optimized as it could be.
* **example/** contains a simple demo of the library.
* **tools/** contains a slower experimental implementation, and other tools to decode and explore GZ files.
* **contrib/** contains utility code from elsewhere.

### License

The contents of **tools/** are separately licensed under the GNU General Public License, Version 2.

The contents of **contrib/** are derived from third-party code; see the files in that directory for specific license detail.

Everything else is licensed under the below 2-clause BSD license. For the removal of doubt, everything in **lib/** is BSD licensed, even if a copy also appears in the more-restrictively-licensed **tools/** directory.

*ajflate* was written by Andrew Francis.

#### BSD license

Copyright (C) Grumpy Panda Pty Ltd as trustee for the AJF Family Trust. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

