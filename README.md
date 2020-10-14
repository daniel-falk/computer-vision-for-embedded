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

The SDK is distributed in two diffenrent docker images, one for the tools, e.g. compilers and one for the libraries. To compile the next applications we need both the libraries and the compiler (altough it would probably work with the upstream compiler as well..). The first way of doing this is to use the docker image with the libraries to copy them to our local source folder. This could look like:

```bash
docker run --rm -v `pwd`:/src axisecp/acap-api:3.1-armv7hf cp -r /opt/axis/sdk/temp/sysroots/cortexa9hf-neon-poky-linux-gnueabi/ /src/sysroot
docker run --rm -v `pwd`:/src axisecp/acap-toolchain:3.1-armv7hf /usr/bin/arm-linux-gnueabihf-gcc --sysroot /src/sysroot /src/up_down_detect.c -lcapture -o /src/up_down

scp up_down 192.168.0.90:/tmp
ssh -t 192.168.0.90 /tmp/up_down
```

A more reproducable way is to create a new container image which combines the tools and the libraries. We can build a new image (named `camera-sdk`) using the specification in the file `Dockerfile`:
```bash
docker build -t camera-sdk .

docker run --rm -v `pwd`:/src camera-sdk /usr/bin/arm-linux-gnueabihf-gcc --sysroot /sysroot /src/up_down_detect.c -lcapture -o /src/up_down
```

If you are not comfortable using Docker, or can't use it in your developer machine, the SDK is also available as `.deb` packages. Take a look at the [Axis website for ACAP](https://www.axis.com/sv-se/products/analytics/acap).
