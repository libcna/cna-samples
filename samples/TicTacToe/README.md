# TicTacToe — CNA free reimplementation

> **This program is not a port of Microsoft's XNA 4.0 `TicTacToe_4_0` sample.**

The original sample has two separately built and deployed parts: a Windows Phone 7 XNA client and
a self-hosted WCF server. The server owns the game state and returns moves to registered clients
through Microsoft Push Notification Service XML messages. Neither original part is ported here,
and those retired platform services are intentionally not emulated.

This directory instead retains an owner-approved, standalone CNA game loosely inspired by the
same Tic-Tac-Toe theme. It uses local game state, local random AI, mouse/keyboard controls, a
desktop layout and CNA's CNJ loose-content format. It must be described and published as a free
reimplementation, never as an XNA sample port or a fidelity reference.

Its deliberately retained local AI is also not the original server AI. It is simplistic, can make
irrational moves and can lose consistently; this known behavior is accepted only as a property of
the free reimplementation, not as XNA sample behavior.

Controls:

- Left mouse button: place a move.
- `N`: start a new game after the current game ends.
- `Escape`: exit.
- `F1`: display the reimplementation's help overlay.

The complete original audit and the owner-approved non-port boundary are recorded in
[`missing.md`](missing.md) and the repository [`plan.md`](../../plan.md).

Verified free-reimplementation builds (not original-port artifacts) are stored at:

- `/rv/samples/SAMPLE-015-TicTacToe_4_0/cna-native-opengles3`
- `/rv/samples/SAMPLE-015-TicTacToe_4_0/cna-web-webgl2`
