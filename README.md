# 🎲🖥️ RAIInet

**RAIInet** is a multiplayer strategic board game implemented entirely in C++ with both **terminal** and **graphical (X11)** user interfaces. Designed for 2–4 players, the game operates on a Linux environment and demonstrates clean, modular software architecture using OOP and MVC principles.

---

## 🎮 Demo

Here are screenshots of both the terminal and GUI versions of the game in action:

### 🧾 Terminal Version

<p float="left">
  <img width="533" alt="Screenshot 2025-03-17 at 11 09 35 PM" src="https://github.com/user-attachments/assets/088697c5-1f39-48f5-ba96-82675e158e01" />
</p>

---

### 🖼️ Graphical Version

<p float="left">
  <img width="478" alt="Screenshot 2025-03-17 at 11 09 27 PM" src="https://github.com/user-attachments/assets/3aecedec-a8de-489a-be5e-d082346e0cb0" />
</p>

---

## 💡 Inspiration

The idea behind **RAIInet** was to combine strategic gameplay with robust software architecture. We aimed to simulate a physical board game digitally, focusing not only on game logic but also on maintainability and clean software design. Supporting both CLI and GUI modes provided valuable experience in cross-interface development and abstraction.

---

## 🛠️📋 How It Works

### 🎲 Gameplay

- Designed for **2–4 players**
- Players take turns placing, connecting, and capturing strategic network pieces on a shared grid.
- Victory is determined by achieving unique connection paths or territory control (custom ruleset).

### 🧑‍💻 Terminal Mode

- Uses Linux terminal (bash) for input/output
- Efficient grid rendering using C++ I/O and ANSI codes
- Debug-friendly and lightweight

### 🪟 Graphical Mode (X11/XQuartz)

- Built using the **X11 graphics library**
- Interactive UI components with real-time event handling
- Custom rendering of board and pieces
- Observer pattern used to dynamically update the board view as state changes

---

## 🧱 Architecture

- ✅ **Language:** C++  
- 🧩 **Modules:** 15 modular components, 3000+ lines of code  
- 🧠 **Design Patterns:** MVC, SOLID (SRP), Observer  
- 📐 **Design Tools:** UML for planning structure and relationships  
- 📦 **Version Control:** Git  
- 🐧 **OS/Runtime:** Linux with Bash, tested with XQuartz on macOS  

---

## 🏆 Key Features & Highlights

1. 🧠 **Modular MVC Design:** Each component of the game is separated using the Model-View-Controller paradigm, making the codebase extensible and testable.
2. 🕹️ **Dual Interfaces:** Seamless switching between terminal-based and graphical interaction.
3. 👁️ **Observer Pattern:** Efficient state management and view updates in the X11 graphical UI.
4. ⚙️ **Scalability:** Designed to accommodate future AI or networked multiplayer.
5. 📊 **Planned with UML:** Class diagrams and sequence diagrams guided the development from the ground up.

---

## 🚀 Getting Started

### Prerequisites

- Linux environment (or macOS with XQuartz)
- C++ compiler (g++)
- Bash terminal
- X11/XQuartz (for graphics mode)

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/RAIInet.git
   cd RAIInet
   make
   ./raiinet
