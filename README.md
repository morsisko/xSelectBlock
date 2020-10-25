# xSelectBlock
Plugin for x64dbg that allows you to select block of data in dump widget easier

# Features

## GUI inspired by HxD
![](https://i.imgur.com/KE353Tp.gif)

## Commands

### xrange
Allows you to select bytes in range of `start`-`stop`

`xrange start, stop`

Example:

`xrange 76F31000, 76F31013`

### xlen
Allows you to select bytes starting at `start` with length `len`

`xlen start, len`

Example:

`xlen 76F31000, 13`

### xext
Allows you to extend/shrink currect selection by `len`

`xext len`

Example:

`xext 2`

### xset
Allows you to set current selection to specific `len`

`xset len`

Example:

`xset 2`
