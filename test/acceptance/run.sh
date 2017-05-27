#!/bin/sh

cd image
./copy.sh
docker build -t 'tin:sauron' .
cd ..
docker-compose -f network.yml up
