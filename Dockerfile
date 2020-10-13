FROM axisecp/acap-api:3.1-armv7hf as api
FROM axisecp/acap-toolchain:3.1-armv7hf as toolchain

COPY --from=api /opt/axis/sdk/temp/sysroots/cortexa9hf-neon-poky-linux-gnueabi/ /sysroot
