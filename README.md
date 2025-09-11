
# ShitOS

The ShitOS project is an operating system targeting x86_64. 
ShitOS is my first real attempt at doing OSDev, so it's bad.
This project serves as a learning project for me, I hope to stick with it.



## Installation

To clone/build this project, follow these instructions:

```
  git clone https://github.com/Poptart69420/shit-os.git
  cd shit-os
  git clone https://github.com/Poptart69420/gcc.git cross_compiler/gcc
  git clone https://github.com/Poptart69420/binutils-gdb.git cross_compiler/binutils-gdb
  ./cross_compile.sh
  ./build.sh
```

Then you can run it with one of my premade qemu scrips with:

```

./qemu-run.sh

```

Or:

```

./qemu-run-with-debugger.sh

```

To run it with the debugging output.
## To-do (Git Repo)

- Add licensing (whatever provide the most FLOSS)

- Add a list of dependancies (trying to keep the list minimal)

- Add some documentation


## To-do (Actual project)

- Finish paging

- Expanded memory management

- Add scheduling

- And a fucking wholeeee lot more
