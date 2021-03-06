# GBK/IBM1388 Charset Conversion
GBK is the main simplified Chinese charset used on PC, while IBM1388 is on mainframe.
The 2 charsets are not directly interchangeable by manipulating bits like what Unicode could do between UTF-8 and UTF-16;
and they are base on different encoding (ASCII/EBCDIC) with different order of Chinese Characters.
ICU (http://site.icu-project.org/) supports both charsets, but has a huge footprint.
Here is a small footprint fast solution which eliminates dependency to ICU.

## GBK Encoding
- 1 byte - `0xxx xxxx` - `0x00 ~ 0x7f` - Compatible with ASCII
- 2 bytes - `1xxx xxxx  1xxx xxxx` - Chinese
- The most significant bit is used to identify 1 or 2 bytes encoding.

## IBM1388 Encoding
- 1 byte - Compatible with EBCDIC
- 2 bytes - Chinese
- SO(Shift Out)/`0x0E` and SI(Shift In)/`0x0F` is used to identify the start and end of 2 bytes encoding.

## License
- The map files are generated by ICU. So you probably have to obey ICU license (http://source.icu-project.org/repos/icu/icu/trunk/license.html)
