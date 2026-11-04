# Using the three servers

How to read the servers' windows and how to operate them, from the source (`MsgServerDlg.cpp`, `NpcServerDlg.cpp`,
`AccountServerDlg.cpp`, `WorldKernel.cpp`, the map group's `MsgTalk.cpp`). Setting them up is `docs/LOCALHOST.md`. Every
command here was read from the code, not tried; where the code's behaviour is odd, it says so (those are the 2008
server's own, kept on purpose: `docs/ORIGINAL_BUGS.md`).

## 1. GMs and PMs

There is no GM flag in the database: the **character name** decides.

- **GM**: the name contains `[GM]` or `[PM]`. **PM** (the stronger rank): the name contains `[PM]`.
- Players cannot type `[` or `]` in a new name, so a GM is made in the database: create the character normally, then,
  while it is offline, `UPDATE cq_user SET name='[GM]Hero' WHERE name='Hero';` (names are at most 15 bytes).
- The world also has a looser test (a name containing `GM` or `PM`, no brackets) for one feature: when `cq_config` type
  11009 lists IP addresses, such names may log in only from them. `cq_merged` has no 11009 rows, so it is off.
- Every word a GM says, commands included, is logged to `gmlog\gm <date>.log`.

## 2. The windows

### MsgServer

| part | what it shows or does |
|---|---|
| left pane | `start server` / `current`: start and clock time. `socket thread`, `world thread`, `map group 0 thread`: average (and max) milliseconds one round of that thread takes; the number in `[ ]` after the map group is the last message type it processed (negative: an internal message). Map groups 1..4 stay 0 with `MAPGROUP_SIZE=1`. `debug [...]`: internal counters. Refreshed every 5 s; every 5 minutes the panes are also written to the log. |
| right pane | `players`: online now (max since start); `login` / `logout` in the last 5 minutes; `socket bytes`: client traffic per second (packets per second); `npc socket bytes`: the NpcServer link's. |
| text box | the server's messages (start-up steps, warnings, the echo of console commands). |
| **Talk** box and button | the operator console: section 3. It is not chat. |
| **Close** | asks "Are you sure?", then stops all threads and exits at once, with no announcement and no kick (use the console first: section 5). |

The window title is `<game title> - <server name> - <build date>`. The file `Game server online.txt` beside it holds the
same two panes, rewritten every 5 s.

### NpcServer

Two status panes (the shell's: `kernel(ms): average/max` of a round; the kernel's: its monsters and the link), a text
box, and two buttons:

- **Close**: asks "Are you CLOSE server new?" (sic), then stops the kernel thread and exits in order.
- **no use**: despite its name, an abrupt exit. It is the default button, so **Enter** in this window triggers it: the
  program ends without stopping its thread or releasing its link. Avoid it.

NpcServer takes no typed commands.

### AccountServer

| part | what it shows |
|---|---|
| left pane | `start server`, `running` (uptime); `[c]point thread` and `[c]login thread`: the spinning character shows the thread is alive, the bar its load; `server state`: one character per game-server slot (20 per row) showing which game servers are logged in; `connections` and `database time`: bars of the connection count and database load. |
| right pane | a 5-minute progress bar; `login (5 min)` and `point (5 min)`: accepted logins and point charges in the last 5 minutes; `point total`; `online table`: players online; `max online` (and when); `service`: the slowest and average handling time. |
| text box | its messages: "Server running...", each game server logging in ("Game server #1 "Seifuku" logged in") or closing, socket errors. |
| **Close** | asks "Close the server?". |
| About box (the window's system menu, top-left icon) | holds **Allow login**: logins are refused for `ENABLELOGINDELAY` seconds after start, so the game servers can report who is still online; the button ends that wait early. "Never use when restarting the server": while a game server is still running, it could let a player in twice. With `ENABLELOGINDELAY=0` (the guide's config) logins are allowed at once and the button is not needed. |

## 3. The MsgServer console (the Talk box)

Type a line and press Enter or **Talk**. The line is echoed in the text box, then its first word is matched
(case-insensitive); anything else does nothing. A GM can send the same lines from inside the game with `/cmd <line>`.

| command | what it does |
|---|---|
| `broadcast <text>` | a red system message with the whole text to every player |
| `shutdown` | only **announces** "Server maintenance for thirty minutes. Please log off immediately..."; stops nothing |
| `kickoutall` | disconnects every player and refuses new connections until MsgServer restarts; no confirmation |
| `restartserver` | after a `kickoutall`: closes MsgServer and starts it again (refused before) |
| `restartwindows` | after a `kickoutall`: **reboots the machine** (refused before) |
| `setmaxplayer <n>` | the login cap: once more than n players are online, accounts with an id above 1000 are refused (default 800; not saved) |
| `kickoutcheat <n>` | a bit mask: the cheat kinds whose bit is set get the player kicked instead of only logged |

## 4. In-game GM commands

Typed in the game's chat box, starting with `/`, by a GM (section 1). A player's `/...` is ordinary chat. Many commands
want exactly the arguments shown and silently do nothing with more or fewer. "PM" marks the commands that need `[PM]`.
Replies the string file has are English; a few are Chinese literals in the code.

**Moving and looking**

| command | does |
|---|---|
| `/chgmap <map> <x> <y>` | teleport to a map |
| `/fly <x> <y>` | jump on the current map |
| `/find <name>` | go to a player |
| `/pos` | your coordinates |
| `/mapdata <x> <y>` | a cell's terrain data |
| `/player all` or `/player map` | online count, or players on your map (needs an account id of 100 or less) |
| `/queryid <name>` (PM) | every role of that exact name on your map, with id and position |
| `/QueryRole <id>`, `/statuslist <id>` (PM) | a role's details, or its active statuses (`0` = yourself) |

**Players and accounts**

| command | does |
|---|---|
| `/kickout <name>` | disconnect a player |
| `/arrest <character id>`, `/arrestname <name>`, `/逮捕 <id>` | send a player to the jail (map 6002) |
| `/lock <account id>`, `/unlock <account id>` | refuse, or allow again, an account's logins (in memory until restart; only for accounts seen since start; the world matches the word case-sensitively, so type it in lower case) |
| `/lockname <name>`, `/unlockname <name>` | the same by character name; probably ineffective (original bug) |
| `/client` | send every player the client check (verify code), which the 4351 client answers |
| `/punish 1` / `/punish 0` | switch the cheat jail on or off (off at start: decisions log 87) |
| `/kickoutcheat <n>` (PM) | as the console's |
| `/setmaxplayer <n>` (PM) | as the console's |
| `/kickoutall` | as the console's: stops the server for players |
| `/cmd <console line>` | any console command (section 3), including `restartwindows` |

**Your own character** (PM unless marked; most are saved to the database)

| command | does |
|---|---|
| `/life`, `/mana` (GM) | full HP, full MP |
| `/fullxp` (GM), `/xp [n]` | XP to full, or to n |
| `/sp <n>` | stamina |
| `/uplev <n>` | raise your level by n |
| `/superman` | +500 to every attribute |
| `/pro <n>` | change your class (a wrong id can break the character) |
| `/awardmoney <n>` | silver |
| `/awarditem <item type>` | one item of that type |
| `/awarditem <item type> <plus> [bless] [hp] [gem1] [gem2]` | one equipment with its options (our addition, not 2008's: decisions log 106). Each option left off, or 0, keeps the type's default. plus: a level `cq_itemaddition` has for the item; bless (the −% typed as a positive number) and hp: 0..255; gems as socket codes: the gem's item type minus 700000 (13 a Super DragonGem), 255 an empty socket. A non-equipment or a value outside these: nothing happens. Example: `/awarditem 410339 9 7 255 13 13`, a Super BuriedBlade +9 −7 with 255 HP and two Super DragonGems. The options reach the database when the character's items are saved (at logout), as a script's item does |
| `/awardmagic <magic type> <levels>`, `/awardwskill <skill type> <level>` | learn a skill, a weapon proficiency |
| `/action <cq_action id>` | run any script as yourself |

**World and content**

| command | does |
|---|---|
| `/broadcast <word>`, `/千里传音 <word>` | a server-wide GM message; **only the first word** is sent (use the console's `broadcast` for sentences) |
| `/newbiemap <map> on` / `off` | mark a map newbie-protected (until restart) |
| `/焰火`, `/焰火2 <text>` | fireworks around you; the second spells up to 8 bytes of text |
| `/cheatwnd <word> <word>` | an anti-cheat window message to every player (what the 4351 client does with it is not known) |
| `/web <url>` | whispered to a player: tells their client to open a web page |
| `/奖品` | claim the first `cq_bonus` row of your account |
| `/查询 <name>` | a member of your guild's contribution |
| `/lotterytest <count> <type> <colour>` (PM) | test lottery draws, logged, no prize (a huge count stalls the server) |
| `/reloadaction <id>` | reload one script row; `/reloadaction 0` (PM) reloads the whole table |
| `/reloadmagictype` (PM) | reload the skill table |
| `/showaction` | toggles a flag nothing reads: no effect |

**The protection lock.** `/setlock <6..9 digits>` and `/clearlock <old password>` set and remove a character's lock
key, but sit inside the GM-only parser, so players cannot use them, and the lock's `/unlock <password>` is never reached
(the account `/unlock` above catches the word first: 2008's own order, `docs/ORIGINAL_BUGS.md`). A lock set by a script
(action 1053) therefore cannot be opened in game; clear `cq_user.lock_key` in the database instead.

## 5. Starting and stopping

Start: MariaDB, the AccountServer, MsgServer (its text box shows "Account server login OK"), NpcServer.

Stop, in order:
1. MsgServer console: `broadcast` a warning, wait, then `kickoutall`.
2. Close NpcServer (**Close**, not **no use**), then MsgServer, then the AccountServer.

A program that sets the named event `TQ ConqerOnline Exit Event` stops both game servers in order by itself:
MsgServer announces the maintenance, kicks everyone 50 seconds later and closes; NpcServer closes after 50 seconds.

## 6. Cautions

- `/reloadaction 0` and `/reloadmagictype` swap tables every map group shares without a lock: they can crash a running
  server ("for local test only"). Use them only with no players online.
- `/cmd restartwindows` lets any `[GM]` reboot the machine from inside the game. Give `[GM]` names only to people you
  trust with the host.
- `/action`, `/awarditem`, `/awardmoney`, `/uplev`, `/pro` write to the database at once.
