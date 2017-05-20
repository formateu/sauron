# Sauron accept testing

### Preparation
1. Install _docker_ and _docker-compose_.
1. Create docker image named _sauron_.
  - Build client and server
  - Copy executables to _image_ folder
  - Change _image/config.yaml_ and docker__compose.yml to
    set up required network.

### Execution
```
docker-compose up
```

Note: remember to rebuild docker image after changing _image/config.yaml_.
