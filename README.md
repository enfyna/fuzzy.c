<p align="center">
  <img width=100 src=https://github.com/user-attachments/assets/318fdeb6-8315-44f8-8326-4d1be4d34b58 />
</p>
<h1 align="center">
  fuzzy.c
</h1>
<p align="center">
  <img src=https://img.shields.io/badge/-%2300599C?style=for-the-badge&logo=c&logoColor=white>
  <img src=https://img.shields.io/badge/RAYLIB-FFFFFF?style=for-the-badge&logo=raylib&logoColor=black>
</p>
<br>
<p align="center">
  Fuzzy inference system library written in C.
</p>
<br>
<p align="center">
  <img width=60% src="https://github.com/user-attachments/assets/1c38ef78-7191-464d-8eb8-73349b396b0a" />
</p>

# Build

1. Clone the repo.
2. Download raylib 5.5 and put it into the src/third-party directory.
3. Run ``make``.

# Examples
| Example | Type | Description | Preview |
|---------|---------|---------|---------|
| [Tipper](./examples/tipper.c) | Mamdani | Tipper problem with 2 inputs, 1 output and 3 rules. | <img width=40% src=https://github.com/user-attachments/assets/1c38ef78-7191-464d-8eb8-73349b396b0a /> |
| [Tipper (no gui)](./examples/tipper-no-gui.c)  | Mamdani | Tipper problem with 2 inputs, 1 output and 3 rules. | https://onlinegdb.com/MdOKWoPQg |
| [Tipper (sugeno, no gui)](./examples/tipper-sugeno-no-gui.c)  | Sugeno | Tipper problem with 2 inputs, 1 output and 4 rules. | - |
| [Mobile Robot](./examples/mobile-robot.c) | Mamdani | Mobile Robot problem with 2 inputs, 1 output and 20 rules. | <img width=40% src="https://github.com/user-attachments/assets/7d1a61d4-00c9-4037-bd72-cceaf62568a6" /> |
