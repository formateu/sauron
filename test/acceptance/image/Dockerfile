FROM ubuntu:16.04

# Updating OS
RUN \
  apt-get update && \
  apt-get -y upgrade

WORKDIR /root

# Add executables and config
COPY sauron_client /root/sauron_client
COPY sauron_server /root/sauron_server
COPY config.yaml /root/config.yaml

# Launch client by default
CMD ["sauron_client"]
