<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:1a1814,50:c8860a,100:1a1814&height=220&section=header&text=DevLog&fontSize=90&fontColor=f5f0e8&fontAlignY=42&desc=CLI%20Developer%20Journal%20%2B%20HTML%20Reports&descAlignY=64&descColor=c8860a&descSize=20" />

<br/>

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Frontend](https://img.shields.io/badge/Reports-HTML%2FCSS-E34F26?style=for-the-badge&logo=html5&logoColor=white)](https://developer.mozilla.org/en-US/docs/Web/HTML)
[![License](https://img.shields.io/badge/License-MIT-c8860a?style=for-the-badge)](LICENSE)
[![Status](https://img.shields.io/badge/Status-In%20Development-27c93f?style=for-the-badge)]()
[![FOSS](https://img.shields.io/badge/Hackathon-FOSS%202026-7a3d9e?style=for-the-badge)]()

<br/>

*A C++ command-line journal for developers that tracks daily coding sessions and auto-generates beautiful HTML progress reports.*

<br/>

</div>

---

## Overview

Most developers forget what they built two weeks ago. DevLog fixes that by giving you a simple terminal prompt each day — what you worked on, what you learned, what blocked you — and turning all of it into a self-contained HTML report you can share or keep for yourself.

No external libraries. No internet required. Just C++17 and a browser.

---

## Getting Started

**Requirements:** `g++` with C++17 support (standard on Linux and macOS)

```bash
git clone https://github.com/anshrjpt/DEV-LOG.git
cd DEV-LOG
make
./devlog
```

---

## Commands

```
  ██████╗ ███████╗██╗   ██╗██╗      ██████╗  ██████╗
  ██╔══██╗██╔════╝██║   ██║██║     ██╔═══██╗██╔════╝
  ██║  ██║█████╗  ██║   ██║██║     ██║   ██║██║  ███╗
  ██║  ██║██╔══╝  ╚██╗ ██╔╝██║     ██║   ██║██║   ██║
  ██████╔╝███████╗ ╚████╔╝ ███████╗╚██████╔╝╚██████╔╝

  Developer Journal · v0.1 · FOSS
```

| Command | Description |
|:---|:---|
| `./devlog new` | Log today's dev session |
| `./devlog list` | View all past entries |
| `./devlog search <keyword>` | Search entries by keyword |
| `./devlog report` | Generate a full HTML report |
| `./devlog week` | Show this week's summary |
| `./devlog edit` | Edit the last entry |
| `./devlog help` | Show all commands |

---

## How It Works

```
┌──────────────────────────────────────────────────────────────────┐
│                                                                    │
│   You answer 5        C++ saves each       C++ reads all logs     │
│   prompts in    →     entry as JSON    →   and computes stats     │
│   the terminal        to /logs/                  |                │
│                                                  v                │
│                       A self-contained HTML report is generated   │
│                       and saved to /reports/                       │
│                                                                    │
└──────────────────────────────────────────────────────────────────┘
```

Each daily entry stores: what you worked on, what you learned, what blocked you, tags, and a mood score (1-5).

The generated HTML report includes a stats bar, entry timeline, activity heatmap, top topics, skill breakdown, and mood chart — all in a single file with no external dependencies.

---

## Project Structure

```
dev-journal/
├── src/
│   └── main.cpp          ← entry point and command router
├── logs/                 ← daily entries stored as JSON
├── reports/              ← generated HTML reports
├── Makefile
└── README.md
```

---

## Build Log

This project is being built live at **1 hour per day** as part of a FOSS hackathon.

| Phase | Days | Status |
|:---|:---|:---|
| Foundation & Setup | Days 1-5 | In Progress |
| Core C++ Logic | Days 6-12 | Upcoming |
| HTML Report Generator | Days 13-19 | Upcoming |
| Extra Features | Days 20-22 | Upcoming |
| Polish & Submit | Days 23-25 | Upcoming |

---

## Contributing

Contributions, issues, and feature requests are welcome.

```bash
git checkout -b feature/your-feature-name
git commit -m "feat: describe your change"
git push origin feature/your-feature-name
```

Open a Pull Request with a clear description of what you changed and why.

---

## License

MIT License — free to use, modify, and distribute. See [LICENSE](LICENSE) for details.

---

<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:1a1814,50:c8860a,100:1a1814&height=100&section=footer" />

Built for the FOSS community · If this helped you, consider giving it a star

</div>
