> [!WARNING]
> This driver is very basic and highly detectable by anti-cheat systems. It should not be used in any paid software.

# IoCTL-Kernel-Cheat-Driver

A simple UserMode-KernelMode driver with communication implemented via IOCTL codes.

## Usage

To load the driver, you can use [KdMapper](https://github.com/TheCruZ/kdmapper) (or any other driver loading tool). Keep in mind that this project was tested solely with a basic bunnyhop for CS2, and may require further adaptation for other use cases.

## Sources

[Defining IoCtl codes](https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes)
[IoStackLocation](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_stack_location)
[PsLoadedModuleList Unlinking](https://gist.github.com/muturikaranja/b7d4b59c72611e76aed94b2f0bf33aa2)
[PsLoadedModuleList Unlinking](https://github.com/DErDYAST1R/PsLoadedModuleList-Dkom-Unlinking)
[Cs2 Entity Finding](https://github.com/roflmuffin/CounterStrikeSharp/blob/main/managed/CounterStrikeSharp.API/Modules/Entities/EntitySystem.cs)
