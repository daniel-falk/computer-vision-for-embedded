# Compile the first application
The most simple (but not most correct!) way to compile an application for an AXIS M3106-L Mk II network camera is to install the upstream armv7 compiler. You can either install it locally:
```bash
apt install gcc-arm-linux-gnueabihf -y
```

and then compile the program:
```bash
arm-linux-gnueabihf-gcc hello_world.c -o my-prog
```

or you can install and run it in a container to avoid conflicts with other software installed in your computer:
```bash
docker run --rm -v `pwd`:/src ubuntu:xenial-20200903 bash -c "apt update && apt install gcc-arm-linux-gnueabihf -y && arm-linux-gnueabihf-gcc /src/hello_world.c -o /src/my-prog"
```

using this way you will reinstall the compiler every time you build, but don't worry - we will get to better methods soon.

# Run the first application
In the current working directory should now be a binary file called my-prog. This file is compiled for the armv7 architecture and will probably not run on your host computer (unless your host computer has an ARM CPU or if you e.g. has enabled transparent emulation using QEMU). You can try:
```bash
./my-prog  # This will fail with an error
```

We can however use ssh/scp to copy the binary to the camera and try it there:
```bash
scp my-prog 192.168.0.90:/tmp
ssh 192.168.0.90 /tmp/my-prog
# output: Hello World!
```
If the above does not work, verify the camera IP and that ssh is enabled in the cameras plain config.

# Using the correct SDK and compiler

...todo
