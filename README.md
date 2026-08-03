# STM32 UART Bootloader

A custom UART bootloader for the **STM32F411RE**. The project enables reliable in-field firmware updates over UART using a custom packet protocol, CRC32 validation, and a Python host application.

---

# Getting Started

## Bootloader Mode

To enter the bootloader:

1. Press and hold the **User Button**.
2. Press the **Reset** button.
3. Release the **Reset** button.
4. Release the **User Button**.

The bootloader indicates it is active by turning **ON the onboard LED** and starts listening for UART commands from the host application.

![Bootloader Mode](<WhatsApp Image 2026-08-03 at 12.43.24.jpeg>)

If the **User Button is not pressed** during reset, the bootloader validates the user application and automatically jumps to the **Default Application**.

---
## Using the Host Application

The Python host utility (`host_py.py`) provides a command-line interface for communicating with the bootloader over UART.

![Python Host Application](image.png)

## Bootloader Commands

| Command | Description |
|---------|-------------|
| BL_GET_VER | Get bootloader version |
| BL_GET_HELP | Get supported commands |
| BL_GET_CID | Read MCU Chip ID |
| BL_GET_RDP_STATUS | Read Read-Out Protection level |
| BL_GO_TO_ADDR | Jump to user application |
| BL_FLASH_ERASE | Flash sector/mass erase |
| BL_MEM_WRITE | Program application into Flash |
| BL_EN_R_W_PROTECT | Enable WRP/PCROP |
| BL_READ_SECTOR_STATUS | Read sector protection status |
| BL_DIS_R_W_PROTECT | Disable protection |


---

## Flashing a User Application

<video controls src="20260803-0756-43.1234396.mp4" title="Title"></video>


To program and run a new firmware image:

1. Build your application project.
2. Copy the generated **`.bin`** file into the same directory as `host_py.py`.
3. Rename the binary to:

```text
user_app.bin
```

4. Enter **Bootloader Mode** by holding the **User Button** while pressing **Reset**.
5. Run the Python host application.
 
```bash
python host_py.py
```

6. Select **BL_MEM_WRITE** from the menu.
7. Enter the application start address (default: `0x08008000`).
8. The host transfers the firmware image over UART while the bootloader programs it into the internal Flash memory.
9. After the programming is complete, select **BL_GO_TO_ADDR** from the menu.
10. Enter the application start address (`0x08008000` by default).
11. The bootloader validates the application, updates the Main Stack Pointer (MSP), deinitializes the bootloader peripherals, and transfers execution to the user application.

> **Note:** Ensure that the application is linked to the same start address (e.g., `0x08008000`) specified during programming and execution. An incorrect address or linker configuration will prevent the application from starting correctly.

---

# Memory Layout

| Region | Start Address |
|---------|---------------|
| Bootloader | `0x08000000` |
| User Application | `0x08008000` |
| Default Application | `0x08004000` |
| SRAM | `0x20000000` |

---

# Project Structure

```text
STM32-Bootloader
│
├── STM32Bootloader/
│   ├── Core/
│   ├── Drivers/
│   ├── Inc/
│   ├── Src/
│   └── Startup/
│
├── Application 1/
│
├── Default_Application/
│
├── Documents/
│
└── host_py.py
```

---

# Project Demonstration

## Bootloader Architecture

> *(Add architecture diagram here)*

```markdown
![Architecture](Documents/architecture.png)
```

---

## UART Communication Protocol

> *(Add packet format image here)*

```markdown
![Protocol](Documents/packet_protocol.png)
```

---

## Flash Memory Layout

> *(Add flash memory layout image here)*

```markdown
![Memory Map](Documents/memory_map.png)
```

---

## Bootloader Host Application

> *(Add terminal screenshot here)*

```markdown
![Host](Documents/host_terminal.png)
```


---

# Future Improvements

- Secure Boot
- AES Encrypted Firmware Updates
- CAN Bootloader
- USB DFU Bootloader
- OTA Firmware Updates
- Dual-Image Bootloader