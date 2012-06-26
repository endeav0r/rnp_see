rnp_see
=======

Rainbows And Pwnies Symbolic Execution Engine. This engine is under rapid development 
does not currently completely execute its test case. I push commits when I can.

Building
========
Build requirements are g++, lua and udis86. I am currently building in the latest 
64-bit Ubuntu. udis86 is available here: http://udis86.sourceforge.net/

make.lua is currently configured to use LuaFileSystem. This will only compile objects
that are older than their sources. You can safely comment out require('lfs') and build
without this functionality.

Once you have everything set up, run: `lua make.lua`

Running
=======
`cd test/1/`  

`../../see test`

You'll probably need to hit return after running commands

  - *a* => (All) run until completion
  - *s* => step a single instruction
  - *d* => step 8 instructions
  - *f* => step 16 instructions
  - *r* => print variables corresponding to x86 registers with their ascii names
  - *v* => print all variables

Distribution & Comments
=======================

Engine is currently not for public distribution.

Comments go to endeavor@rainbowsandpwnies.com

I am not currently taking bug fixes. There are bugs. I am fixing them.