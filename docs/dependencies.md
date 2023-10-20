# Running a Program

This guide will walk you through the steps to run a program on your system, including what you need to install and configure. Additionally, we'll cover what's required to port the code.

## What do I need to install?

Before you can run a program, ensure you have the following prerequisites installed on your system:

- **Operating System**: Windows 10 with Windows Subsystem for Linux (WSL) using Ubuntu 20.04.

- **Software Programs**:
  - **gcc**: The GNU Compiler Collection.
  - **ld**: The GNU Linker.
  - **tree**: The utilitu to explore folders.
  - **qemu (on Windows host)**: A virtual machine emulator.
  - **gdb**: The GNU Debugger.
  - **gnumakefile**: A makefile for building projects.
  - **nasm**: The Netwide Assembler.
  - **git**: A version control system.
  - **sgdisk**: A utility for manipulating GPT partitions.
  - **fdisk**: A command-line disk partitioning tool.
  - **mkfs.fat**: A utility for creating FAT file systems.
  - **mkfs.ext2**: A utility for creating ext2 file systems.
  - **tree**: A utility for displaying directory trees.
  - **partprobe**: A tool for informing the OS kernel of partition table changes.

## What and what do I need to configure?

To configure the environment, you will need to edit certain variables within the GNUMakefile:

- **Edit the GNUMakefile**:
  - The primary variables to configure are:
    - **QEMU**: This variable is used to specify the path to the QEMU emulator. Ensure it points to the correct QEMU executable on your system.
    - **GDB**: Optionally, if you need to configure the GDB debugger, set this variable to the path of your GDB executable.

## What do I need to implement if I want to port the code?

If you intend to port the code to a different environment or system, you may need to implement certain functions in the `source/dependencies.c` file. These functions are typically indicated by comments in the codebase, suggesting where modifications or adaptations may be necessary to ensure compatibility with your target environment.

Make sure to carefully review the comments in the `source/dependencies.c` file and make any necessary code changes as instructed to successfully port the code to your desired system.

That's it! With the required installations and configurations in place and any necessary code modifications for porting, you should be ready to run the program on your system.
