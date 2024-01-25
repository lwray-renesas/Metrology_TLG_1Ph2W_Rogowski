# Metrology_TLG_1Ph2W
Single phase RL78/I1C metering source code for PMOD. (R5F11TLG)

## Supported compilers:
- CC-RL 1.12.10

## Instructions e2studio:
- Open worksace anywhere on machine (except for local git repo).
- File --> Import --> General --> Existing Projects Into Workspace
- Navigate to this repo.
- Select project.
- Click Finish.

## Documentation
This section outlines the documentation shipped with this repo.

**20230418_RL78I1C HW Design Guideline.pdf**
*Design guide for the RL78/I1C*

**20230616_RL78I1C_Metrology_Preliminary_User Manual_Rev5.02_ EM_1P2W8.pdf**
*User manual for the metering libraries*

**C14069_RL78-I1C_RAA788000_1-ph_AC_PMOD_V3.0_Schematic_Diagram_07-10-22.pdf**
*Schematic for rogowski based single phase PMOD*

**R12UZ0074EG0110_ISOLATED_SHUNT_UM_RL78I1C.pdf**
*User manual for the shunt version of the single phase PMOD*

## Build Configurations
The different build configurations are:
**Debug_ROGOWSKI**
*Debug version of the project enables on-chip debug circuitry in option bytes - enables integrator on phase current measurements*

**Release_ROGOWSKI**
*Release version of the project disables on-chip debug circuitry in option bytes - enables integrator on phase current measurements*

# Notes
- For compiler v1.10.00 the following linker command must be applied: -Change_message=information=1321,1110
This is due to the bug: https://en-support.renesas.com/knowledgeBase/19936382

- The .cgp file in the doc/AP4 folder is purely the **base** file used to create the drivers for this project.
The drivers have been severly modified and any overwriting of the drivers should be checked carefully with a diff tool to ensure appropriate functionality is retained.

