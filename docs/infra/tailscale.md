# Tailscale

Tailscale provides the VPN tunnel that connects the attacker VM to the server. It also allows the operator to observe logs from any device during the demo.

## Install (server and attacker VM)

```sh
curl -fsSL https://tailscale.com/install.sh | sh
sudo tailscale up
```

Authenticate in the browser when prompted. Repeat on the attacker VM.

## Verify connectivity

```sh
# From the attacker VM
tailscale ping <server-machine-name>
# pong from server in Xms via ...
```

## MagicDNS

Enable MagicDNS in the Tailscale admin console so that `phantom` can reach the server by hostname (`server`) instead of IP. This makes demo configs portable.

## ACL for this project

In the Tailscale admin console under **Access controls**, add:

```json
{
  "acls": [
    {
      "action": "accept",
      "src": ["tag:phantom"],
      "dst": ["tag:server:*"]
    },
    {
      "action": "accept",
      "src": ["tag:operator"],
      "dst": ["tag:server:22,2222,80,443"]
    }
  ],
  "tagOwners": {
    "tag:phantom": ["autogroup:admin"],
    "tag:server":  ["autogroup:admin"],
    "tag:operator":["autogroup:admin"]
  }
}
```

This allows:
- `phantom` (attacker VM) → server: all ports (fork bomb traffic is arbitrary).
- `operator` devices → server: SSH and HTTP/HTTPS only.
- `phantom` → operator devices: **blocked** (phantom cannot reach anything except the server).

Apply the `phantom` tag to the attacker VM and the `server` tag to the host in the Tailscale machines list.

## Verify isolation

From the attacker VM, confirm the operator device is NOT reachable:

```sh
tailscale ping <operator-device-name>
# Should timeout or return "no route"
```

## Related docs

- [isolation.md](../modules/phantom/isolation.md) — VM-level isolation requirements
- [firewall.md](firewall.md) — UFW rules that also gate Tailscale port
