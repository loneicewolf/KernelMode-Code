# KernelMode-Code 
#### and ( soon coming ) Advanced stuff


2021 Updated (still updating) - original repo: https://github.com/i-nino/KernelMode-Code

### Still adding POCS and Improvements as well as Docs


Variety of windows kernel-mode driver code, from an x64 keylogger, re-implementing reversed notable (some not so notable) rootkit components, coded solutions to various reversing malware challenges, internal functionality of windows, etc.

### Tree-View:

        APCs
        
        BlockingDriverFromLoading
        
        FestiRootkit
                   SysrootHook 
                              POCS
                                  video.mp4 <-- coming soon
        
        GROK_Explorations
        
        IRPsOverFileApis
        
        KKeylogger
        
        SCSI
        
        SYS-Dlls
        
        malware_research
                      Practical_Reverse_Engineering
                                                  SAMPLES
                                                          
        README.md
        
        
**EquationGroup-Related**

GROK

and some additional txts/docs and zips/pocs.

more coming soon, and that is quickly!

---------------------------------------------
        
        



---------------------------------------------




===================================================================================================

## WHAT MAKES THE EQUATION GROUP UNIQUE?


---------------------------------------------------------



### Ultimate persistence and invisibility


---------------------------------------------------------



GReAT has been able to recover two modules which allow reprogramming of the hard drive firmware of more than a dozen of the popular HDD brands. This is perhaps the most powerful tool in the Equation group’s arsenal and the first known malware capable of infecting the hard drives.



---------------------------------------------------------


## By reprogramming the hard drive firmware (i.e. rewriting the hard drive’s operating system), the group achieves two purposes:

           An extreme level of persistence that helps to survive disk formatting and OS reinstallation. If the malware gets into the firmware, it is available to “resurrect” itself forever. It may prevent the deletion of a certain disk sector or substitute it with a malicious one during system boot.
              “Another dangerous thing is that once the hard drive gets infected with this malicious payload, it is impossible to scan its firmware. To put it simply: for most hard drives there are functions to write into the hardware firmware area, but there are no functions to read it back. It means that we are practically blind, and cannot detect hard drives that have been infected by this malware” – warns Costin Raiu, Director of the Global Research and Analysis Team at Kaspersky Lab.
              The ability to create an invisible, persistent area hidden inside the hard drive. It is used to save exfiltrated information which can be later retrieved by the attackers. Also, in some cases it may help the group to crack the encryption: “Taking into account the fact that their GrayFish implant is active from the very boot of the system, they have the ability to capture the encryption password and save it into this hidden area,” explains Costin Raiu.
              
          
          
#### Ability to retrieve data from isolated networks


---------------------------------------------------------


Sample of fanny.bmp (aka DWE for short, or  DementiaWheel)

https://github.com/loneicewolf/fanny.bmp

The **Fanny** worm stands out from all the attacks performed by the Equation group. Its main purpose was to map air-gapped networks, in other words – to understand the topology of a network that cannot be reached, and to execute commands to those isolated systems. For this, it used a unique USB-based command and control mechanism which allowed the attackers to pass data back and forth from air-gapped networks.

In particular, an infected USB stick with a hidden storage area was used to collect basic system information from a computer not connected to the Internet and to send it to the C&C when the USB stick was plugged into a computer infected by Fanny and having an Internet connection. If the attackers wanted to run commands on the air-gapped networks, they could save these commands in the hidden area of the USB stick. When the stick was plugged into the air-gapped computer, Fanny recognized the commands and executed them.

Note: to explain the USB-Hidden areas:
Fanny.bmp is making use of a FileSystem driver (FAT 16/32) and from there, creates a covert, storage (within the FAT) as well as using a rootkit.
Here, information is stored, encrypted.


### Fanny used two zero-days to replicate.
https://securelist.com/a-fanny-equation-i-am-your-father-stuxnet/68787/



---------------------------------------------------------


           Name 	nls_933w.dll
           MD5 	11fb08b9126cdb4668b3f5135cf7a6c5
           Type 	HDD reprogramming module
           Compiled 	Tue Jun 15 20:23:37 2010
           
        
           
        
---------------------------------------------------------

Refs and further readings:

- https://github.com/loneicewolf/HDD-Firmware-RE

- https://www.kaspersky.com/about/press-releases/2015_equation-group-the-crown-creator-of-cyber-espionage

- https://securelist.com/equation-the-death-star-of-malware-galaxy/68750/

- https://github.com/loneicewolf/fanny.bmp

- https://securelist.com/a-fanny-equation-i-am-your-father-stuxnet/68787/
