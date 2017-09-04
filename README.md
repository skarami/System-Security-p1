Secure Computer Systems: Project 1
==========================================

GDB tips
--------

Notice the `disassemble` and `stepi` commands.

You may find the `x` command useful to examine memory (and the
different ways you can print the contents such as `/a` `/i`
after `x`). The `info register` command is helpful in printing
out the contents of registers.

A useful way to run GDB is to use the `-e` and `-s` command line flags;
for example, the command `gdb -e sploit3 -s /tmp/target3` in the VM
tells GDB to execute `sploit3` and use the symbol file in `target3`.
These flags let you trace the execution of the `target3` after the
sploit's memory image has been replaced with the target's through the
execve system call.

When running GDB using these command line flags, you should follow
the following procedure for setting breakpoints and debugging memory:

1. Tell GDB to notify you on `exec`, by issuing the command `catch exec`;
2. Run the program. GDB will execute the sploit until the `execve`
   syscall, then return control to you;
3. Set any breakpoints you want in the target; and
4. Resume execution by telling GDB `continue` (or just `c`).

If you try to set breakpoints before the exec boundary, you will
get a segfault.

If you wish, you can instrument the target code with arbitrary
assembly using the `__asm__()` pseudofunction, to help with debugging.
Be sure, however, that your final exploits work against the unmodified
targets, since these we will use these in grading.

Suggested reading in [Phrack](http://www.phrack.org)

- Aleph One, "Smashing the Stack for Fun and Profit," Phrack 49 #14.
- klog, "The Frame Pointer Overwrite," Phrack 55 #08.
- Bulba and Kil3r, "Bypassing StackGuard and StackShield, Phrack 56 #0x05.
- Silvio Cesare, "Shared Library Call Redirection via ELF PLT Infection," Phrack 56 #0x07.
- Michel Kaempf, "Vudo - An Object Superstitiously Believed to Embody Magical Powers," Phrack 57 #0x08.
- Anonymous, "Once Upon a free()...," Phrack 57 #0x09.
- Nergal, "The Advanced Return-into-lib(c) Exploits: PaX Case Study," Phrack 58 #0x04.
- Gera and Riq, "Advances in Format String Exploiting," Phrack 59 #0x04.
- Anonymous, "Bypassing PaX ASLR Protection," Phrack 59 #0x09.
- blexim, "Basic Integer Overflows," Phrack 60 #0x10.

Other Books
-----------

- W. Richard Stevens, _Advanced Programming in the Unix Environment_.  Addison-Wesley, 1993.
- AEleen Frisch, _Essential System Administration_, second edition.  O'Reilly, 1995.

- Dean Elsner, Jay Fenlason, et al., _Using AS_.  FSF, 1994.
- Richard Stallman et al., _Using the GNU Compiler Collection_. FSF, 2002.
- Richard Stallman, Roland Pesch, Stan Shebs, et al. _Debugging with GDB_.  FSF, 2001.

These are [online](http://www.gnu.org/manual/manual.html).

