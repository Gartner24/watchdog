# Threat model

## What this system does

`watchdog` detects and mitigates resource exhaustion caused by a controlled fork bomb launched from an isolated attacker VM. This is an educational demonstration of OS scheduling, concurrency, and monitoring algorithms under stress.

## What phantom is allowed to do

- Launch a fork bomb that rapidly creates child processes on the server.
- Consume CPU and memory until watchdog responds.
- Remain connected to the server through the Tailscale tailnet.

## What phantom cannot do

- Access the public internet (the VM has no outbound NAT to external networks).
- Perform network-level attacks (port scanning, brute force, DoS) — out of scope for this project.
- Persist beyond the demo session. phantom is always run manually and stopped manually.

## What watchdog does NOT defend against

This list is intentional — these are not bugs, they are non-goals.

- Kernel exploits or privilege escalation.
- Container escapes.
- Real malware or ransomware.
- Persistent threats or rootkits.
- Attacks originating from outside the tailnet.
- Disk exhaustion or network floods.

## Security boundary

The tailnet (Tailscale) is the security boundary for the demo. The Tailscale ACL ensures phantom can reach only the server, and no other devices on the tailnet can communicate with phantom.

See [infra/tailscale.md](../infra/tailscale.md) for the ACL configuration.
