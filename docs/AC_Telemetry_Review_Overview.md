# AC Telemetry Review — Project Overview

## Purpose

AC Telemetry Review is a post-session analysis tool for Assetto Corsa. The tool captures live telemetry data during a driving session. It links this data to short video clips of key moments. A web dashboard then shows the data and video together, so the driver can review each session in detail.

The project targets local network use. It runs on an existing self-hosted machine and connects through Tailscale.

## Problem Statement

Live telemetry alone has limited value. A driver cannot watch numbers and drive at the same time. Numbers also lose meaning after several laps, since the driver may not remember the exact moment a value was recorded.

This project solves both issues. Telemetry review happens after the session, not during it. Each flagged event links to a short video clip, so the driver sees the exact moment the data describes.

## Core Components

**Telemetry capture (C++)**
A native Windows program reads live telemetry from the game's shared memory. It logs speed, RPM, throttle, brake, steering angle, and per-wheel data. It also detects key events during the session, such as tyre lock-ups and oversteer.

**Video handling**
Screen recording runs alongside the game. After the session, short clips are cut around each flagged event. Full recordings are then deleted, so only small clips remain.

**Backend (FastAPI)**
The backend receives telemetry data and event data from the capture tool. It stores this data in PostgreSQL. It also serves data and clip file paths to the frontend.

**Frontend (React)**
A browser dashboard shows session lists, lap summaries, and full telemetry graphs. Flagged events open a synced view, with video and graph playback side by side. Clicking a point on the graph moves the video to that exact moment.

## Key Features

- Live telemetry capture from Assetto Corsa
- Automatic detection of lock-ups, oversteer, and understeer
- Braking point consistency tracking across laps
- Theoretical best lap, built from the best sector times across a session
- Synced video and telemetry playback
- Ghost lap comparison, showing two laps on one graph
- Local clip storage with an automatic retention policy

## Technology Stack

| Layer | Technology |
|---|---|
| Telemetry capture | C++, native Windows build |
| Video capture and processing | OBS, ffmpeg |
| Backend | FastAPI |
| Database | PostgreSQL |
| Frontend | React with Vite |
| Charting | Recharts or Chart.js |
| Hosting | Self-hosted machine |
| Network access | Tailscale |
| Process management | NSSM |

## Deployment Approach

The backend runs as a background service on the self-hosted machine, using NSSM. PostgreSQL runs on the same machine. The frontend build output is served directly by FastAPI, so no separate web server is needed. Access happens over Tailscale, so no public DNS or SSL setup is required.

## Build Order

1. C++ shared memory reader, with basic console output
2. FastAPI ingest endpoint and PostgreSQL schema
3. Basic frontend, showing a single lap's telemetry graph
4. Event detection logic, starting with lock-up detection
5. Screen recording integration and clip cutting
6. Synced video and graph playback in the frontend
7. Theoretical best lap and braking consistency features
8. Retention policy script for clip storage
9. Stretch goals, including ghost lap overlay and a live in-game overlay

## Out of Scope for Version 1

- Live in-session overlay
- Support for other racing simulators
- Public internet deployment, DNS, and SSL configuration
- Multi-user accounts or shared session libraries

# Researched Links

https://www.jeremyong.com/winapi/io/2024/11/03/windows-memory-mapped-file-io/
https://learn.microsoft.com/en-us/windows/win32/memory/file-mapping
https://learn.microsoft.com/en-us/windows/win32/api/WinBase/nf-winbase-createfilemappinga
https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile
https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-openfilemappinga

