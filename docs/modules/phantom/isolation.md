# VM isolation

## Why isolation is required

phantom launches a real fork bomb. If the attacker VM has unrestricted internet access, the fork bomb process table exhaustion could inadvertently affect other systems, and the tailnet ACL would not contain the blast.

The VM must be isolated: it can only talk to the server through Tailscale. No other outbound traffic is allowed.

## Required VM network configuration

The host hypervisor must configure the VM with:
- No NAT gateway to the public internet.
- No bridge to the host's LAN (unless the LAN is the tailnet).
- Tailscale installed and joined to the tailnet.

Verify isolation from inside the VM:

```sh
# This must FAIL (no internet access)
curl --max-time 5 https://example.com
# Expected: curl: (28) Connection timed out

# This must SUCCEED (tailnet works)
tailscale ping <server-tailnet-hostname>
# Expected: pong from server in Xms
```

## Tailscale ACL for phantom

The tailnet ACL must allow phantom → server traffic only. It must NOT allow phantom to reach any other tailnet device (operator devices, other servers).

See [infra/tailscale.md](../../infra/tailscale.md) for the ACL fragment.

## What happens if isolation breaks

If phantom gains internet access or the fork bomb escapes the tailnet, stop the VM immediately:

```sh
# On the hypervisor host
virsh destroy phantom-vm    # or equivalent for your hypervisor
```

Then audit the network configuration before continuing.
