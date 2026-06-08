# Project instructions for Codex

This is a Zephyr RTOS learning project.

Rules:
- Explain Zephyr concepts clearly before changing code.
- Prefer Zephyr best practices over quick hacks.
- Do not make large refactors unless explicitly requested.
- Before editing, inspect CMakeLists.txt, prj.conf, and relevant source files.
- For build errors, explain the root cause first.
- Use minimal changes.
- When suggesting code, explain why each change is needed.
- This project targets embedded/RTOS learning, so do not hide important details.

## Engineering style
- Prefer industry-standard embedded/Zephyr practices over quick hacks.
- When multiple solutions exist, explain which one is typically used in real production projects and why.
- Clearly distinguish between quick workaround, learning/demo code, and production-quality solution.

Project structure:
- src/main.c: application entry
- src/threadA.c: Thread A logic
- src/threadB.c: Thread B logic
- prj.conf: Zephyr kernel/project configuration
- CMakeLists.txt: source inclusion