# watchdog — documentation index

## Reading order

**Students** (understanding what was built and why):

1. [Architecture overview](architecture/overview.md) — system topology
2. [Data flow](architecture/data-flow.md) — what happens during an attack
3. [Threat model](architecture/threat-model.md) — scope and non-goals
4. [watchdog module](modules/watchdog/README.md) — daemon internals
5. [phantom module](modules/phantom/README.md) — attacker internals

**Operators** (deploying and running the system):

1. [Infrastructure](infra/README.md) — server setup
2. [Deploy](operations/deploy.md) — step-by-step provisioning
3. [Run demo](operations/run-demo.md) — live demo runbook
4. [Troubleshooting](operations/troubleshooting.md)

---

## Section index

| Section | Contents |
|---|---|
| [architecture/](architecture/) | System topology, data flow, threat model |
| [modules/watchdog/](modules/watchdog/) | Daemon: scheduler, semaphores, monitoring, logging |
| [modules/phantom/](modules/phantom/) | Attacker: fork bomb, isolation |
| [infra/](infra/) | Server hardening, firewall, Docker, Tailscale, systemd |
| [operations/](operations/) | Deploy runbook, demo runbook, troubleshooting |
