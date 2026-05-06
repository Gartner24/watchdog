# Fork bomb

## What a fork bomb is

A fork bomb is a denial-of-service technique that exploits the `fork()` system call to create processes exponentially. The classic shell one-liner:

```sh
:(){ :|:& };:
```

Defines a function `:` that calls itself twice in the background, then invokes it. Each call spawns two more, and so on. The process table fills up, the scheduler is overwhelmed, and the system becomes unresponsive.

## The C variant used by phantom

The shell fork bomb is immediately uncontrollable. `phantom` uses a C implementation with depth and timing controls:

```c
void bomb(int depth) {
    if (depth <= 0) { sleep(duration_s); return; }
    pid_t pid = fork();
    if (pid == 0) {
        bomb(depth - 1);  // child recurses
        _exit(0);
    }
    bomb(depth - 1);  // parent also recurses
    waitpid(pid, NULL, 0);
}
```

At `--depth 6`, this spawns at most 2^6 = 64 concurrent processes — enough to stress a small server without permanently crashing it.

## Knobs

| Parameter | Effect |
|---|---|
| `--depth` | Maximum fork depth. Each +1 doubles max processes. Safe range: 4-7. |
| `--sleep-ms` | Delay between fork waves. Lower = more aggressive. |
| `--duration` | Leaf processes sleep this long before exiting. Controls how long load stays elevated. |

## Kill switch

phantom has a built-in self-termination after `--duration` seconds. For manual abort:

```sh
# On the attacker VM
kill -SIGTERM $(pgrep -x phantom)

# On the server (emergency)
sudo pkill -9 phantom
```

## Code map

| Source file | Contents |
|---|---|
| `src/phantom/fork_bomb.c` | `bomb()` function, main(), CLI arg parsing |

## Related docs

- [isolation.md](isolation.md) — why the VM must be isolated before running this
- [run-demo.md](../../operations/run-demo.md) — how to trigger and observe safely
