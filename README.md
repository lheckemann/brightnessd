# brightnessd

silly little daemon that manages my screen brightness. Bash and C. Bash script
reads config changes from a FIFO, C program manages the actual brightness. The
C program is there mostly because I couldn't find a way to read events from an
input device neatly in shell.
