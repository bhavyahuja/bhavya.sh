# bhavya.sh

![Language](https://img.shields.io/badge/language-C-00599C)
![Platform](https://img.shields.io/badge/platform-Linux-yellowgreen)
![Standard](https://img.shields.io/badge/standard-POSIX-orange)
![Architecture](https://img.shields.io/badge/architecture-modular%20%2F%2020%2B%20files-blue)

A fully-featured Unix shell built from scratch in C. Implements foreground/background job control, arbitrary pipe chaining, I/O redirection, persistent command history, recursive file search, raw-mode terminal I/O, `/proc` filesystem introspection, and live man-page fetching over raw TCP sockets — all without `popen`, `system()`, or any third-party libraries.

---

## Features

### Built-in Commands

| Command | Description |
|---------|-------------|
| `hop [path...]` | `cd` replacement — supports `~`, `-`, `..`, `.`, relative and absolute paths; chained multi-hop |
| `reveal [-a] [-l] [path]` | `ls` replacement — lexicographic listing with hidden-file and long-format flags; color-coded output (blue = dirs, green = executables, white = files) |
| `seek [-d\|-f] [-e] <name> [dir]` | Recursive prefix-match search across a directory tree; `-e` auto-opens a single file or `cd`s into a single directory match |
| `proclore [pid]` | Reads `/proc/<pid>/status` and `/proc/<pid>/exe` to display PID, status (R/S/Z + foreground flag), process group, virtual memory, and executable path |
| `log` | Persistent command history (max 15, cross-session via `log.txt`); deduplicates consecutive identical commands |
| `log execute <n>` | Re-executes the nth most-recent command |
| `log purge` | Clears history |
| `activities` | Lists all shell-spawned background processes in lexicographic order with PID and state (Running / Stopped) |
| `ping <pid> <signal>` | Sends an arbitrary signal to a process (`signal % 32` applied); wraps `kill(2)` |
| `fg <pid>` | Pulls a stopped or background process to the foreground and hands it terminal control |
| `bg <pid>` | Resumes a stopped background process in the background |
| `neonate -n <t>` | Polls `/proc` every `t` seconds to print the most recently created system PID; exits on `x` keypress using raw terminal mode (`termios`) |
| `iMan <cmd>` | Fetches and renders the man page for `<cmd>` from `man.he.net` over a raw TCP socket (no `curl`, no `libcurl`) |

### Shell Mechanics

- **Prompt** — `<user@host:~/relative/path>` with foreground-process timing: `<user@host:~ sleep : 5s>` when a process takes > 2 s
- **Input parsing** — handles `;` and `&` separated command lists with arbitrary whitespace and tabs
- **Background execution** — `cmd &` spawns a background process, prints its PID, and asynchronously notifies on exit/stop via `SIGCHLD`
- **I/O redirection** — `>` (overwrite), `>>` (append), `<` (input); file created with permissions `0644` if absent
- **Pipes** — arbitrary-depth pipe chaining (`cmd1 | cmd2 | ... | cmdN`); pipes and redirection compose correctly
- **Signals** — `Ctrl-C` → SIGINT to foreground process; `Ctrl-Z` → SIGTSTP, pushes foreground to background as Stopped; `Ctrl-D` → SIGKILL all children, exit shell
- **`.myshrc`** — startup config supporting single-word aliases (`alias ll = reveal -l`) and composite functions (`mk_hop`, `hop_seek`)

---

## Architecture

Each feature is isolated in its own `.c` / `.h` pair. `main.c` wires signal handlers and runs the read-eval loop.

```
bhavya.sh/
├── Makefile
├── .myshrc              # startup aliases and functions
├── src/
│   ├── main.c           # entry point — signal setup, REPL loop
│   ├── display.c/h      # prompt rendering, home directory resolution
│   ├── tokenizing.c/h   # semicolon + ampersand tokenizer
│   ├── hop.c/h          # directory navigation
│   ├── reveal.c/h       # directory listing
│   ├── seek.c/h         # recursive file/directory search
│   ├── log.c/h          # persistent command history
│   ├── proclore.c/h     # /proc-based process inspection
│   ├── isValid.c/h      # command dispatch, fork/exec, bg/fg tracking
│   ├── validHelper.c/h  # PATH-aware command existence check
│   ├── redirection.c/h  # I/O redirection (>, >>, <)
│   ├── piping.c/h       # pipe chaining
│   ├── signals.c/h      # signal handlers (SIGINT, SIGTSTP, SIGCHLD, ping)
│   ├── fgbg.c/h         # fg / bg / bring_to_foreground / continue_bg_process
│   ├── neonate.c/h      # latest-PID monitor, raw terminal keypress
│   ├── iMan.c/h         # raw TCP socket man-page fetcher
│   ├── myshrc.c/h       # alias + function config parser
│   ├── globals.c/h      # shared state (homeDir, prevDir, activities[], fg_proc)
│   └── header.h         # common includes
└── docs/
    └── screenshots/     # feature demo screenshots
```

---

## Build & Run

**Requirements:** `gcc`, `make`, Linux (uses `/proc`, `POSIX`, `termios`)

```bash
make          # produces a.out
./a.out       # starts the shell
```

```bash
make clean    # removes a.out
```

---

## Implementation Highlights

**`/proc` introspection** — `proclore` and `neonate` read directly from the Linux `/proc` virtual filesystem (`/proc/<pid>/status`, `/proc/<pid>/exe`, `/proc` directory mtime) without any helper utilities.

**Raw TCP networking** — `iMan` opens a TCP socket to `man.he.net:80`, sends a hand-crafted HTTP GET request, reads the response, strips HTML tags, and prints the man page body — no `curl`, no `libcurl`, no `popen`.

**Raw terminal mode** — `neonate` switches the terminal into non-canonical, no-echo mode using `tcgetattr` / `tcsetattr` to detect a single `x` keypress without blocking the print loop.

**Pipe + redirection composition** — `handle_piping` forks one child per pipe segment; each child independently calls `handle_io_redirection` before `execvp`, so `cat < in.txt | wc | cat > out.txt` works correctly end-to-end.

**Persistent history** — command log survives shell restarts via `log.txt`; consecutive duplicates are deduplicated; log commands are never stored in the log itself.

**Signal safety** — `SIGCHLD` is handled with `SA_RESTART | SA_NOCLDSTOP` via `sigaction`; `waitpid(WNOHANG | WUNTRACED | WCONTINUED)` reaps background children and updates the `activities[]` table without blocking the REPL.
