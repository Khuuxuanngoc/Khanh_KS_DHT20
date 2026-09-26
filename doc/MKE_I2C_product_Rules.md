# MKE I2C Loadcell Product Rules

These rules apply when developing, modifying, or debugging the MKE_I2C_Loadcell Arduino Library and related project files.

## 1. Coding Standards & Examples
- **Memory Optimization:** Always use the `F()` macro for string literals in `Serial.print()` and `Serial.println()` to save SRAM on AVR boards (e.g., Arduino Uno). Example: `Serial.println(F("Hello World"));`
- **Code Language:** All variables, comments, and serial print strings inside `.ino`, `.cpp`, and `.h` files MUST be written entirely in **English** to ensure accessibility for international users.
- **Bilingual Documentation:** Every example sketch folder MUST contain a `README.md` file. This README must provide instructions, usage, and important notes in both **Vietnamese** (🇻🇳 Tiếng Việt) and **English** (🇬🇧 English).

## 2. Known Bugs & Successful Fixes
- **I2C Race Condition & EEPROM Write Delay:** 
  - *Bug:* The PY32 I2C Slave uses an asynchronous ring buffer. EEPROM write operations (like `ForceSaveEeprom()`) take ~30ms and disable interrupts. If the Master sends a `SET` command followed immediately by a `GET` command, the PY32 slave will be busy, causing bus lockups, timeouts, or returning stale data from the previous command.
  - *Fix:* Always append a `delay(50);` after any `SET` command (e.g., `setFilterLevel`, `setScale`, `tare`, `calibrate`) in the Arduino Master code.
- **Zero Tracking API Removal:**
  - *Bug:* `getZeroTracking` (and other GET commands) relies on reading the slave's `dataToReply` buffer without a verification header. This caused severe race conditions.
  - *Fix:* The `setZeroTracking` and `getZeroTracking` hardware APIs were completely removed from the Arduino library. Zero tracking is now strictly handled via Software on the Arduino side (e.g., `if (abs(weight) < 1.0) weight = 0.0;`).
- **Factory Reset / NVIC_SystemReset Hang on PY32:**
  - *Bug:* Calling `NVIC_SystemReset()` inside the I2C Slave firmware to apply factory reset settings caused the PY32 to freeze/hang instead of cleanly rebooting. This caused the master to lose connection, and the factory reset appeared to fail.
  - *Fix:* Removed `NVIC_SystemReset()`. Instead, perform a "soft reload" in software by directly calling `hx711Bridge.beginBridge(new_address)` and updating internal RAM variables directly from the newly saved EEPROM.

## 3. Important Notes & Architecture Constraints
- **Explicit I2C Initialization:** Never call `Wire.begin()` implicitly inside a sensor library's `begin()` function. Always require the user to explicitly call `Wire.begin()` in their Arduino `setup()` function. This prevents multiple initializations on the same bus and allows users to configure custom SDA/SCL pins on platforms like ESP32.
- **Blind I2C Reads:** The `requestData()` function delays for 5ms before reading the response. Since the PY32 slave does not echo the `Mode ID` in its 4-byte response, the Master blindly trusts the incoming data. Never call a `GET` command while the slave might still be executing a previous command.
- **I2C Packet Limit:** The slave's `rxBuffer` was increased from 8 to 80 bytes to prevent buffer overflow when the Master spams commands, but flooding the bus without delays is still strongly discouraged.

## 4. Library Distribution & Installation (MKE_ONE)
- **Centralized Installation:** MakerEdu libraries (such as `MKE_I2C_Loadcell`) are distributed as part of the **`MKE_ONE`** ecosystem. 
- **Documentation Rule:** In all future projects and library documentations (User Manuals, READMEs), always instruct the user to install the **`MKE_ONE`** library via the Arduino Library Manager instead of installing the individual sensor library directly. `MKE_ONE` acts as a meta-package that auto-installs all MakerEdu dependencies.
- **Dependency Rule:** Individual libraries should minimize external dependencies in their `library.properties` if they only use standard Arduino libraries (like `Wire.h`). Instead, add the new library's name to the `depends` list of the `MKE_ONE` library properties.

## 5. Comprehensive Testing (Test All APIs)
- **Feature Completeness:** When creating or finalizing an I2C sensor/module project, you MUST create a comprehensive test sketch (similar to `UnoCodeTest_GetSet_I2C_info_Gemini_I2C_HX710B.ino`) that systematically tests ALL available APIs and parameters (Get/Set operations, EEPROM saving, modes, etc.) to ensure complete library coverage.
- **Verification:** This test file serves as a final quality assurance step to verify that the Arduino Library correctly interfaces with all features provided by the Slave Firmware.

## 6. Document Styling (PDF Export)
- **Print Styles:** All Markdown documents serving as manuals or official documentation (e.g., `UserManual`, `FactoryManual`) MUST include a specific HTML `<style>` block at the very top of the file to ensure correct formatting and page breaks when exported to PDF.
- **Required Style Block:**
```html
<details>
  <summary><b>PDF Export Style (Hidden on web)</b></summary>
  <style>
    @media print {
      @page {
        size: A4;
        margin: 16mm 15mm 16mm 15mm;
      }
      body {
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
        font-size: 13.5px;
        line-height: 1.55;
      }
      h1, h2, h3, h4 {
        page-break-after: avoid !important;
        break-after: avoid !important;
      }
      pre, code, table, blockquote {
        page-break-inside: avoid !important;
        break-inside: avoid !important;
      }
      tr {
        page-break-inside: avoid !important;
        page-break-after: auto;
      }
    }
    .page-break {
      page-break-after: always;
      break-after: page;
    }
  </style>
</details>
```

## 7. Mermaid Diagrams & Render Workarounds
- **Bilingual Diagrams:** When creating Mermaid diagrams (like wiring diagrams), always provide both a Vietnamese version (e.g., `Wiring_mermaid_ai.md`) and an English version (e.g., `Wiring_mermaid_ai_en.md`).
- **Subgraph Titles Clipping:** Do not use HTML tags like `<br/>` inside `subgraph` titles. It causes text clipping. Always write titles on a single line: `subgraph MCU["Microcontroller (Arduino)"]`.
- **Node Bounding Box Bug:** For very short text nodes (like "5V", "S-", "GND"), the Mermaid rendering engine often miscalculates the bounding box, cutting off characters like `-`. To prevent this, always pad short labels with HTML non-breaking spaces `&nbsp;`. Example: use `S_MINUS["&nbsp;S-&nbsp;"]` instead of `S_MINUS["S-"]`.
- **Link Length:** Use longer links (e.g., `-----`) to increase the visual distance between modules and prevent overcrowding.

## 8. Advanced I2C Synchronization & RFID State Management
- **Robust 7-Byte I2C Packet with Checksum:** 
  - *Rule:* Always use a strictly defined packet structure with an XOR Checksum for Master-Slave communication. E.g., `[Address, Mode, Val0, Val1, Val2, Val3, Checksum]`. The Slave ISR MUST validate the Checksum before enqueuing the packet. This completely prevents corrupted packets from crashing the state machine.
- **Closed-Loop Polling & Stale ACK Prevention:**
  - *Bug:* If Master sends a command and polls for an acknowledgment (Mode ID echo), it might falsely read a "stale ACK" from a previously identical command, causing the Master to proceed before the Slave has processed the new command.
  - *Fix:* The Slave MUST implement an asynchronous `clearAcknowledge()` method that sets `lastRequestedMode = 0` **immediately** inside the `receiveEvent` ISR. This forces the Master to wait until the `loop()` task fully processes the pending command and manually re-asserts the Mode ID.
- **RFID State Machine & Crypto1 Session Protection:**
  - *Bug:* Continuously polling an active MIFARE card with unencrypted `REQA` commands destroys the active Crypto1 session, causing subsequent `Auth`, `Read`, or `Write` commands to fail with `Timeout (Status: 3)`.
  - *Fix:* Implement an explicit `isCardSelected` state latch in the Slave. When a card is successfully detected and UID is read, set `isCardSelected = true` and **STOP ALL BACKGROUND POLLING**. Only resume polling (`isCardSelected = false`) when the Master explicitly sends a `HaltCard` command, when any standard command (Auth/Read) times out/fails, or when the Master queries `isCardPresent` while the cache is empty (which indicates a Master reboot/reset).

## 9. ISR Safety & Buffer Management
- **Atomic Operations:** Any variables or buffers shared between the main `loop()` and an Interrupt Service Routine (ISR) MUST be declared with the `volatile` keyword. When modifying multi-byte buffers (like the I2C Ring Buffer) or shared counters (like `rxHead`) from outside the ISR, the operations MUST be wrapped in `noInterrupts();` and `interrupts();` to prevent Race Conditions and memory corruption.
- **Buffer Scalability:** Never hardcode buffer sizes (e.g., `rxBuffer[8]`) as magic numbers scattered across multiple functions. Always define a central macro (e.g., `#define VNEHC_RX_BUFFER_SIZE 16`) to ensure the buffer size can be safely and consistently scaled up without risking array out-of-bounds errors.

## 10. Module ID & I2C Address Allocation Policy (Single Source of Truth)
- **Centralized Registry (`listMakerEdu_I2C_address.h`):** The `idModuleEnum_List` and default I2C Address macros are central to the entire MakerEdu PY32 ecosystem. To prevent ID collisions, AI agents and developers MUST NOT randomly assign or guess `id_module` values.
- **ID Reservation:** Whenever a new module project begins, you MUST first verify the chronological order and explicitly add its ID to `listMakerEdu_I2C_address.h` to "reserve" the slot (e.g., `idModuleEnum_Loadcell = 6`, `idModuleEnum_RFID_RC522 = 7`). 
- **Consistency Enforcement:** Because `listMakerEdu_I2C_address.h` is currently copied into individual project folders rather than being a Git Submodule, if you update this file in the current repository, you MUST prompt the user to manually synchronize these changes across all other PY32 module repositories (e.g., MP3, Loadcell) to ensure the ecosystem remains in sync.

## 11. Admin Mode Requirements for Critical Settings
- **Rule:** Core configuration commands (such as `SetAddress`, `Set_ID_Module`, `Set_FW_Version`, `Set_ProductCode`, and `Factory_Reset`) MUST be protected by Admin Mode in the PY32 Slave firmware.
- **Master Implementation:** When writing Arduino Master examples that modify these critical settings (e.g., changing the I2C address or performing a factory reset), the Master MUST first use the Advanced class (e.g., `MKE_I2C_RFID_Advanced`) to call `unlockAdminMode()` (which sends `modeIdEnum_Set_Admin_Mode` with the `VNEHC_KEY_ADMIN_MODE` key) before sending the configuration command. Otherwise, the PY32 Slave will silently ignore the request.

## 12. MakeCode Object-Oriented Event Bug (Hoisting Failure)
- **Bug:** When creating custom MakeCode extensions, defining top-level Event Blocks (e.g., `//% block="on event"`) as instance methods for Objects (e.g., `rfid1.onCardDetected()`) causes catastrophic variable hoisting issues. When users switch from Blocks back to JavaScript, MakeCode's compiler aggressively hoists the event handler to the top of the file but completely drops or misplaces the object instantiation (`let rfid1 = MKE_RFID.create()`), resulting in a "Block-scoped variable used before declaration" typescript error.
- **Fix (The Industry Standard):** DO NOT use Object-Oriented event blocks for class instances in MakeCode. Instead, expose boolean polling methods (e.g., `isCardPresent()`) and instruct users to place them inside standard `basic.forever` `if` loops. This approach perfectly integrates with MakeCode's cooperative threading, bypasses the buggy OOP block compiler, and guarantees 100% stability when switching between Blocks and Code, even with multiple instances (e.g. `rfid1`, `rfid2`).
