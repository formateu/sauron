# Sauron accept testing

### Preparation
1. Install _docker_ and _docker-compose_.
1. Create docker image named _tin:sauron_.
  - Build client and server
  - Copy executables to _image_ folder
  - Change _image/config.yaml_ and docker__compose.yml to
    set up required network (if necessary).
  - `docker build -t "tin:sauron" .`

### Execution
Running network:
`docker-compose -f network.yml up`

You will have access to virtual network via virtual interface.

Shutting down:
`docker-compose -f network.yml down`

Note: remember to rebuild docker image after changing _image/config.yaml_.
