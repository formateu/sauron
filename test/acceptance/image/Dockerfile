#
# Created by Przemyslaw Kopanski on 26.05.17
#
FROM ubuntu:17.04

# Updating OS
RUN \
  apt-get update && \
  apt-get -y upgrade
RUN apt-get -y install libstdc++6
RUN apt-get -y install libyaml-cpp0.5v5

WORKDIR /root

# Add executables and config
COPY sauron_client /root/sauron_client
COPY sauron_server /root/sauron_server
COPY config.yaml /root/config.yaml

# Launch client by default
CMD ["./sauron_client", "3000"]
