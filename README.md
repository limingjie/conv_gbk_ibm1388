# GBK/IBM1388 charset conversion
GBK is the main simplified Chinese charset used on PC, while IBM1388 is on mainframe.
The 2 charsets are not directly interchangable by manipulating bits like what Unicode could do between UTF-8 and UTF-16;
and they are base on different encoding (ASCII/EBCDIC) with different way to compitable with their bases.
ICU (http://site.icu-project.org/) supports both charsets, but has a huge footprint.
Here is a small footprint fast solution.

## GBK encoding
- 1 byte - 0xxx xxxx - 0x00 ~ 0x7f - Compatible with ASCII
- 2 bytes - 1xxx xxxx  1xxx xxxx - Chinese
The most significant bit is used to identify 1 or 2 bytes encoding.

## IBM1388 encoding
- 1 byte - Compatible with EBCDIC
- 2 bytes - Chinese
- SO(Shift Out)/0x0E and SI(Shift In)/0x0F is used to identify the start and end of 2 bytes encoding.

## License
- The maps files are generated by ICU. So you probably have to obey ICU license (http://source.icu-project.org/repos/icu/icu/trunk/license.html)
- No license on c functions.
