# Racing Game Milestone 10 — Windows qualification

## Status

Blocked on one host-privileged VirtualBox action as of 2026-09-03. This is not a
completed Windows qualification and is not a CNA or Racing runtime failure.

## Live-state evidence

- Host kernel: `6.12.100+deb13-amd64`.
- VirtualBox: `7.2.8r173730`.
- Loaded host modules: `vboxdrv`, `vboxnetflt`, `vboxnetadp`.
- `/proc/misc` assigns `vboxdrv` minor 262 and `vboxdrvu` minor 263.
- `/dev/vboxdrv` and `/dev/vboxdrvu` are absent after the host reboot.
- `VBoxManage` therefore refuses VM operations before it can inspect or start the
  registered machine.
- The official `/usr/lib/virtualbox/vboxdrv.sh start` path would recreate the
  missing device, but requires the unavailable host `sudo` credential.
- An unprivileged `systemctl start vboxdrv.service` attempt timed out without
  authorization.

The VM configuration was inspected without modifying its disks. It retains:

- Windows 7 SP1, 32-bit guest and 2 GiB RAM;
- VBoxSVGA, 128 MiB VRAM and 3D acceleration;
- Guest Additions 7.2.8;
- an existing saved state and the intact `win7.vdi`;
- the offline guest state and existing local shared folders;
- the prior authentic XNA 4 content-build evidence used by Milestone 2.

## Owner action

Run this once on the host:

```sh
sudo /usr/lib/virtualbox/vboxdrv.sh start
```

This is deliberately narrower than `/sbin/vboxconfig`: all matching modules are
already installed and loaded, so rebuilding or reinstalling them is not justified.
After the device is restored, resume this milestone with a Windows `OPENGL33`
build/package, real rendering, content/XACT/storage/input checks and a complete
race-return run. Do not report Windows as supported until those gates pass.

## Continuation decision

Per the owner instruction that a failed VirtualBox task must not stop Racing Game
work, Milestone 11 Android qualification proceeds while this host-only gate is
pending.
