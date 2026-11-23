# KernelMode-Code

> Status: Active Fork! Still adding PoCs, improvements, and documentation.  
> Original Repository: [i-nino/KernelMode-Code](https://github.com/i-nino/KernelMode-Code) (archived/outdated)

---

A collection of Windows kernel-mode driver code, featuring an x64 keylogger, re-implementations of reversed rootkit components, solutions to malware reversing challenges, and explorations of Windows internal functionality.

---

## Repository Structure

```text
KernelMode-Code/
├── APCs/
├── BlockingDriverFromLoading/
├── EquationGroup-Related/
│   └── GROK/
├── FestiRootkit/
│   └── SysrootHook/
│       └── POCS/
│           └── video.mp4          # Coming soon
├── GROK_Explorations/
├── IRPsOverFileApis/
├── KKeylogger/
├── malware_research/
│   └── Practical_Reverse_Engineering/
│       └── SAMPLES/
├── SCSI/
│   └── POCS/
│       └── SCSI_TEST_1.mp4
├── SYS-Dlls/
└── README.md
```

*Note: Additional text files, documentation, ZIP archives, and PoCs are included throughout the repository, with more updates coming soon.*

---

## Research Notes: Equation Group

### What Makes the Equation Group Unique?

#### 1. Ultimate Persistence and Invisibility
Kaspersky's Global Research and Analysis Team (GReAT) recovered two modules capable of reprogramming the hard drive firmware of over a dozen popular HDD brands. This represents one of the most powerful tools in the Equation Group’s arsenal and the first known malware capable of infecting hard drive firmware.

By reprogramming the hard drive firmware (rewriting the drive's internal operating system), the group achieves two primary objectives:

* **Extreme Persistence:** The malware survives disk formatting and full OS reinstallation. Once embedded in the firmware, it can "resurrect" itself indefinitely, preventing the deletion of specific disk sectors or substituting them with malicious sectors during system boot.
  > *"Another dangerous thing is that once the hard drive gets infected with this malicious payload, it is impossible to scan its firmware. To put it simply: for most hard drives there are functions to write into the hardware firmware area, but there are no functions to read it back. It means that we are practically blind, and cannot detect hard drives that have been infected by this malware."*  
  > — **Costin Raiu**, Director of GReAT at Kaspersky Lab

* **Covert Storage & Encryption Cracking:** It creates an invisible, persistent storage area hidden inside the hard drive to save exfiltrated data for later retrieval. In certain scenarios, it also aids in bypassing disk encryption:
  > *"Taking into account the fact that their GrayFish implant is active from the very boot of the system, they have the ability to capture the encryption password and save it into this hidden area."*  
  > — **Costin Raiu**

**Recovered Module Sample:**

| Property | Value |
| :--- | :--- |
| **Name** | `nls_933w.dll` |
| **MD5** | `11fb08b9126cdb4668b3f5135cf7a6c5` |
| **Type** | HDD reprogramming module |
| **Compiled** | `Tue Jun 15 20:23:37 2010` |

---

#### 2. Data Retrieval from Air-Gapped Networks (`fanny.bmp`)
* **Sample Repository:** [loneicewolf/fanny.bmp](https://github.com/loneicewolf/fanny.bmp) *(aka DWE / DementiaWheel)*
* **Replication:** Utilized two zero-day exploits to propagate.

The **Fanny** worm stands out for its ability to map air-gapped networks—understanding the topology of isolated systems and executing commands on them. It relies on a specialized USB-based Command and Control (C&C) mechanism to bridge air-gapped and internet-connected machines:

1. **Data Collection:** An infected USB drive containing a hidden storage area collects basic system telemetry when plugged into an air-gapped machine.
2. **Exfiltration:** When that same USB drive is later connected to an internet-enabled machine infected with Fanny, the collected telemetry is transmitted to the C&C server.
3. **Command Execution:** Attackers can stage commands inside the USB drive's hidden partition; once plugged back into the air-gapped target, Fanny parses and executes them.

> **Technical Note on USB Hidden Areas:**  
> `fanny.bmp` leverages a File System driver (`FAT16`/`FAT32`) alongside a rootkit component to carve out covert, encrypted storage directly within the File Allocation Table (FAT).

---

## References & Further Reading

* [HDD Firmware Reverse Engineering (loneicewolf)](https://github.com/loneicewolf/HDD-Firmware-RE)
* [Fanny.bmp Sample Repository (loneicewolf)](https://github.com/loneicewolf/fanny.bmp)
* [Equation Group: The Crown Creator of Cyber-Espionage (Kaspersky Press Release)](https://www.kaspersky.com/about/press-releases/2015_equation-group-the-crown-creator-of-cyber-espionage)
* [Equation: The Death Star of Malware Galaxy (Securelist)](https://securelist.com/equation-the-death-star-of-malware-galaxy/68750/)
* [A Fanny Equation: "I am your father, Stuxnet" (Securelist)](https://securelist.com/a-fanny-equation-i-am-your-father-stuxnet/68787/)
