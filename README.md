# defect-studio-cxx
## Background and Objectives
Molecular dynamics is a computational technique used to simulate the properties of atomic systems comprising up to hundreds of billions of atoms. It is broadly divided into two categories: ab initio molecular dynamics (AIMD) and classical molecular dynamics (MD). In general, molecular dynamics involves solving Newton’s equations of motion at each timestep, and, therefore, it is necessary to quantify the forces acting between atoms in the many chemical environments that they may encounter. The difference between MD and AIMD lies in how these forces are calculated. As the name suggests, AIMD involves obtaining the forces between atoms from first principles – i.e., from quantum mechanics, typically via density functional theory (DFT). Classical MD, on the other hand, uses an interatomic potential (IAP), a mathematical description of the N-body interactions between the elements of interest.

The use of interatomic potentials is what enables MD to be performed on systems of millions or even billions of atoms. However, the ability to perform MD on complex, multicomponent systems is limited by the availability of interatomic potentials incorporating those components (chemical elements). Traditionally, the creation of an IAP would involve thousands of DFT simulations exploring the range of chemical environments and combinations. However, setting up these DFT simulations by hand is tedious, if not outright intractable for complex systems containing 3 or more elements.

Due to the nature of nuclear fusion, the IAPs employed in the MD simulations of fusion materials must be able to predict their properties in abnormal conditions accurately. Whilst solid materials are never perfect and always contain some concentration of point defects in their crystal structures, the challenging neutron and chemical environment in a fusion reactor means that many such defects, and the effects of radiation damage, are of special interest. To construct IAPs which accurately predict these, a wide range of systems incorporating point defects must be simulated at the DFT level of theory, in addition to much larger structural features including (but not limited to) surfaces and grain boundaries.

Therefore, DefectStudio (DS) aims to simplify the generation of input files for the DFT simulations, which form part of the ‘training set’ for machine-learned IAPs (MLIPs).

## Installation
Clone the repository, then install using the usual cmake workflow:

> mkdir build && cd build
> cmake .. -DCMAKE_BUILD_TYPE=Release
> make
> sudo make install

## Usage
The **TASKS** file is where you define the tasks (duh) that DS will run through. The current dev repository contains a TASKS file with some tasks already defined (and others commented out with a # at the start of the each line). Each task consists of a command (the first word in the task line) and the command arguments that follow it. For instance,

> TIS INPUT_FILE POSCAR RANDOM W PERCENT 1 ELEMENT H DIM 8 OUTPUT_DIR WH_6750

will add a number of H atoms at tetrahedral interstitial sites (TIS) near randomly-selected W atoms in the input file ('POSCAR'). 1% of the W atoms in the input file will be targetted. The output file will be placed in a folder called WH_6750.

A full list of commands and command arguments will be made available soon(tm).