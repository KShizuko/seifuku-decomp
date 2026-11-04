# Seifuku Server

Targeting patch 4351.

## What you need

- Windows, and Visual Studio 2026 (version 18) Community or above in its default folder, with "Desktop development
  with C++" (MSVC v145, Windows SDK 10.0.26100, C++ MFC for v145, C++ CMake tools for Windows).
- MariaDB 10.11 on 127.0.0.1:3306: the servers were built and tested against 10.11.16 through the bundled MariaDB
  Connector/C 3.3.19 (below); other servers and versions are untested.
- A 4351 client. The servers read four things of its: the `map` folder, and `ini\GameMap.dat`, `ini\TerrainNpc.ini`
  (the client's `ini\terrainnpc.ini`) and `ini\AdditiveSize.ini`. They must be the client's own files: the client walks
  on its maps, and a different map desynchronises it.

## Build

From a "Developer Command Prompt for VS 2026" in this folder (it puts `cmake`, `msbuild` and `vstest.console` on the
path). First the bundled MariaDB Connector/C, once per configuration, into `vendor\build`:

```
cmake -S vendor\mariadb-connector-c-3.3.19-src -B vendor\build -G "Visual Studio 18 2026" -A Win32 -DWITH_UNIT_TESTS=OFF -DWITH_CURL=OFF -DWITH_SSL=SCHANNEL -DCLIENT_PLUGIN_REMOTE_IO=OFF
cmake --build vendor\build --config Release --target mariadbclient libmariadb
```

then the solution:

```
msbuild seifuku.sln /m /p:Configuration=Release /p:Platform=Win32
```

The programs go to `bin\Release\`: `MsgServer.exe`, `NpcServer.exe`, `Lottery.dll`, `AccountServer.exe`, and the test
DLLs. Only Win32 (x86) is built. For Debug, put `Debug` in both `--config` and `Configuration`.

## Tests

```
vstest.console bin\Release\scaffold_tests.dll bin\Release\common_tests.dll bin\Release\comm_tests.dll bin\Release\mapdata_tests.dll bin\Release\aimodule_tests.dll bin\Release\npckernel_tests.dll bin\Release\msgserver_tests.dll bin\Release\accountserver_tests.dll /Platform:x86
```

runs the unit tests. The integration tests (`integration_tests.dll`) are built too but are not for this copy: they
start all three servers against the full game data, which it does not carry.

## The databases

`database/cq.sql` is the game database and `database/account.sql` the account database, each to load into an empty
database of its own. In the MariaDB client, as root:

```
CREATE DATABASE cq CHARACTER SET latin1;
CREATE DATABASE account CHARACTER SET latin1;
CREATE USER 'sfkaccount'@'localhost' IDENTIFIED BY 'sfkaccount';
GRANT ALL ON account.* TO 'sfkaccount'@'localhost';
```

then from a command prompt (latin1 and binary mode, so the game's GBK texts arrive byte for byte):

```
mariadb -u root --default-character-set=latin1 --binary-mode cq < database\cq.sql
mariadb -u root --default-character-set=latin1 --binary-mode account < database\account.sql
```

The game servers log in as root without a password (`shell.ini` `DB_USER`/`DB_PW`, `config.ini` `LoginName`/
`Password`); the AccountServer as `sfkaccount`, whose password must not be empty (`gameserver.cfg`: the player limit,
the server id, then the database host, user, password and name).

### What the game database holds

Four maps: Birth Village (1010), Twin City (1002), the Market (1036) and the Lottery Center (700). Every NPC of Birth
Village; in Twin City only the Conductress, who takes you to the Market; in the Market, LadyLuck (to the Lottery
Center: level 70 and 27 CPs) and the Market Controller (back to Twin City); in the Lottery Center, LadyLuck (the way
out) and the 21 LuckyBoxes. No monsters. Twin City is a PK map here (its "no PK" mark is cleared), so the characters
can fight each other there. The scripts kept are the ones those NPCs and the login, a death, a player kill and a
level-up reach; everything else is empty, so most features of the full game cannot be reached here.

Item types, item bonuses and the points a character gets at each level were made from the 4351 client's own files by
`tools/client-data.ps1` (`-Table itemtype`, `itemaddition`, `point_allot`; `goods` too, but no NPC kept is a shop).
The client's points have no archer, so an archer levelling up gets free points to place instead. Besides those, the
scripts, the maps and the accounts' characters, the database holds only what the servers need: the skills the
characters have or the scripts teach, the level table, the lottery's prizes and settings, and one row each of the system buyers and the fuse table, without
which MsgServer does not start.

### The accounts

| account / password | character | |
|---|---|---|
| izanami / izanami | Izanami[PM] | water taoist, female |
| izanagi / izanagi | Izanagi[GM] | warrior, male |
| sakuya / sakuya | none yet | |
| hoori / hoori | none yet | |

Izanami[PM] and Izanagi[GM] are second-rebirth characters at the top level, in Twin City, with +12 super gear (two
super gems, -7% blessing, +255 HP each), their class's skills, 100,000,000 silver and 1,000,000 CPs. `[PM]` or `[GM]`
in a name gives the GM commands of `docs/USAGE.md`. sakuya and hoori have no character: the first login creates one
in the client, which starts in Birth Village as a new player, where the scripts check for a new character's class and
level. Another account is two rows (`account`, and `point001` with `netbar_ip` and `ip_mask` empty, not NULL).

## Run

Two folders, made from `distributable/` and `bin/Release/`:

```
GameServer\      MsgServer.exe, NpcServer.exe, Lottery.dll, and distributable\GameServer's files:
                 shell.ini (MsgServer), config.ini (NpcServer), adapter.ini, ini\En_Res.ini;
                 plus the client's map\ folder and its three ini files (above)
AccountServer\   AccountServer.exe, and distributable\AccountServer's files:
                 config.ini, Account.ini, isp.ini, gameserver.cfg
```

Start MariaDB, then AccountServer.exe, then MsgServer.exe (its window shows the account server login), then
NpcServer.exe. Close them in the reverse order.

**Set `shell.ini`'s `ACCOUNT_IP` to this machine's LAN address** (not 127.0.0.1) when a client is to play, on this
machine or another. The AccountServer sends the client the address it sees MsgServer connect from, and MsgServer takes
every connection from 127.0.0.1 for NpcServer and leaves it unciphered: a client sent there connects and hangs on the
loading screen. NpcServer stays on 127.0.0.1.

The client's server list names the AccountServer's address and port 9958, and the server's name, `Seifuku` here
(`Account.ini` and `shell.ini` `SERVERNAME`, which must agree). The game port is 5816.

## Third-party code

`vendor/mariadb-connector-c-3.3.19-src` is MariaDB Connector/C 3.3.19, unmodified, under the GNU Lesser General Public
License 2.1 (its `COPYING.LIB`; the zlib it bundles under its own licence, `external/zlib/LICENSE`). The servers link it
statically; its full source is here, so they can be rebuilt against a changed copy of it. Everything else is built from
the Windows SDK, MFC and the C and C++ runtimes of Visual Studio.