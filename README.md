mips_os
=======

A very light shell with simple filesystem and memory manager. Developed for a student project on a MIPS processor and MIPS emulator (QEMU)


COMPILE & RUN
=============

Require QEMU and mips-gcc-* tools


To execute on your computer with the real libc :

<code>
make pc && make run_pc
</code>
Will generate os_pc.


To compile for qemu (with mips-libc) :

<code>
make mips
</code>
Will generate os_mips.

To execute with qemu :
<code>make run_mips</code>



SHELL COMMANDS
==============

			man
						display a short help

			tree [path]
						display the filesystem like a tree

			ls [path]
						display children of the directory or the 
						information of a file

			mkdir dirpath
			
			touch filepath
						create empty file
			
			wtxt filepath text
						write the content into the file
						create file if it doesnt exist

			whex filepath hex
						e.g. whex file 4142430A 4142430A
			
			cat filepath
			
			rm path
						remove a file or a directory 
						(if it's not a working dir'ancestor)
			
			pwd
						print working directory
			
			cd dirpath
			
			exec filepath
						#todo - execute a file
			
			dbg
						display memory debug

			exit
