
# GoshkikOS

The GoshkikOS project is an operating system targeting x86_64.
GoshkikOS is my first real attempt at doing OSDev, so it's bad.
This project serves as a learning project for me, I hope to stick with it.

The project is licensed under WTFPL (DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE)

# Installation

To clone/build this project, follow these instructions:

```
  git clone https://github.com/Poptart69420/GoshkikOS.git
  cd GoshkikOS
  ./cross_compile.sh gcc x86_64-goshkikos
  ./build.sh
```

Then you can run it with one of my premade qemu scripts with:

```

./qemu-run.sh

```

Or:

```

./qemu-run-with-debugger.sh

```

To run it with the debugging output.


# Completed (Git Repo)

- Added a proper readme (still being updated over time)

- Added an installation/build guide

- Added a license that reflects the FLOSS (Free Libre Open Source Software) ideals of this project


## - To-Do (Git Repo)

- Add a list of dependencies (trying to keep the list minimal)

- Add some documentation


# Completed (Actual Project)

- Cross compiler setup

- Build scripts

- Run scripts

- GDT setup

- ISR/IDT/IRQ setup (minimal)

- Spinlock

- Virtual terminal (minimal)

- PS2 keyboard handling (minimal)

- Finished paging (memmap, pmm, vmm)

- Scheduling

## - To-Do (Actual Project)

- VFS

- Tmpfs

- Tarfs

- Modular device handling

- Modular driver loader

- Persistant file systems

- Expanded memory management

- Userspace setup

- Actually setting up syscalls

- Reorganizing the codebase

- VGA driver

- MSR table

- Re-writing some of the Assembly

- PS2 mouse handling

- Expanded PS2 keyboard handling

- Distro-agnostic depencency installation

- USB driver

- Audio drivers

- Network drivers

- And a fucking wholeeee lot more

## - Long Term Goals

- Learn more low level programming

- Get to program more in Assembly (eventually wanting to rewrite some of the C code as Assembly)

- Port and/or create a userspace libc. I would like to have it be somewhat easy to use different standard libaries

- Port (and/or maybe create, but unlikely) a display server

- Port and/or create a window manager

- Port some programs (Fastfetch, Emacs, Vim, Firefox, etc)

- Port GCC

- Port BinUtils

- Eventually be able to work on this project within the OS itself

- Setup a custom website and host my own online git thing. Maybe Forgejo?

## - WTFPL License

```
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.

```

## Goshkik Sleepy

![Goshkik Sleepy](readme_images/goshkik-sleepy.png)

## Goshkik WHAT

![Goshkik WHAT](readme_images/goshkik-what.png)

## Drawing Of Goshkik

![Goshkik purrito drawing](readme_images/goshkik-purrito.png)

## WTFPL Logo

![WTFPL logo](readme_images/wtfpl.png)

## WTFPL Comic Strip

![WTFPL comic strip](readme_images/wtfpl-strip.jpg)
