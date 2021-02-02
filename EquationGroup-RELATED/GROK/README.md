# EquationGroup GROK Rootkit


Demonstrates GROK's use of manipulating the stack in order to make "hidden" function calls, in a similar fashion (although GROK's is far more intricate) to certain exploits which "find themselves"(IP) in memory. It also demonstrates how GROK manually maps a compressed driver in such a way, that transfering execution to that compressed driver is done via some pretty slick stack manipulation techniques, making it appear as though an "address switch" has taken place.  But it's system space, so ... not really!  Still, very impressive.

The driver (and its client, which loads it) presented above does, in principle, the same thing as GROK,
although it uses a user-mode client and a driver dispatch routine to fascilitate transferring the 
compressed driver image to the driver, which will manually map it and transfer execution to it via these
techniques discussed above.

The order in which GROK does it is as follows: 
* decompresses a driver buffer and calls its entry point
  * above uses a user-mode client, for convenience, to do this, storing the driver in it's resources
* uses an ISR routine to locate the kernel's imagebase (here a service routine is used instead)
  * has to do something of the sort since no imports have been resolved, and no DRIVER_OBJECT exists,
    which can be used to go through the nt!PsLoadedModuleList through the DriverSection field
* parses the exports, with a hash, to get the desired routines which become the "hidden" function calls:
  * ExAlloc/FreePool, ZwQuerySystemInformation
  * these are used to further gather driver information legitimately
    (here we do the exact same thing, and just display the loaded module info for poc)
* once relocations are performed, it manipulates the return address to point accurately to the same routine,
  albeit in the new memory region
* it manipulates the stack again to execute the exact same next instruction, but in the new memory region
* then it encrypts its "new self" (but not anything that would disrupt its functionality) and eventually returns 
  execution to the driver that launched it, becoming only accessible through a function pointer in a global buffer at offset +0x40

The VS solution file is in GROK_Explorations, with the "hidden" functionality implemented in **asmRoutines.asm** and **gGrok.cpp**, and *ASM_HiddenCall* and *GROK::CreateHiddenDriver* being the routines of interest.


![Alt Text](demo.gif)
