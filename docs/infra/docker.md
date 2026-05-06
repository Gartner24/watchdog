# Docker

> Adapted from [vps-setup/vps-setup.md](https://github.com/Gartner24/vps-setup).

## Scope

Docker is installed on the server for the **reverse proxy stack only** (Nginx + Certbot, see [vps-proxy](https://github.com/Gartner24/vps-proxy)). The `watchdog` daemon does **not** run in Docker.

Running watchdog in a container would hide `/proc` and cgroup v2 from the daemon, breaking the OS algorithm demonstrations that are the point of the project.

## Install Docker

```sh
# Remove any old versions
sudo apt remove docker docker-engine docker.io containerd runc

# Install via official script
curl -fsSL https://get.docker.com | sudo sh

# Add deploy user to the docker group
sudo usermod -aG docker deploy
newgrp docker

# Verify
docker --version
docker compose version
```

## Post-install

Log out and back in for the group change to take effect. Verify:

```sh
docker run --rm hello-world
```

## Related docs

- [vps-proxy](https://github.com/Gartner24/vps-proxy) — the Nginx reverse proxy stack that uses Docker
