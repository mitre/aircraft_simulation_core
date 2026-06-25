# Aircraft Simulation Core

> This is the copyright work of The MITRE Corporation, and was produced for the U. S. Government under Contract Number 693KA8-22-C-00001, and is subject
to Federal Aviation Administration Acquisition Management System Clause 3.5-13, Rights In Data-General (Oct. 2014), Alt. III and Alt. IV (Jan. 2009).  No other use other than that granted to the U. S. Government, or to those acting on behalf of the U. S. Government, under that Clause is authorized without the express written permission of The MITRE Corporation. For further information, please contact The MITRE Corporation, Contracts Management Office, 7515 Colshire Drive, McLean, VA  22102-7539, (703) 983-6000.
>
> (c) 2026 The MITRE Corporation. All Rights Reserved.
>
> Approved for Public Release; Distribution Unlimited. 15-1482

This project contains content developed by The MITRE Corporation.
If this code is used in a deployment or embedded within another project, it is requested that you send an email to [opensource@mitre.org](mailto:opensource@mitre.org) in order to let us know where this software is being used.

![Apache 2.0](https://img.shields.io/badge/license-Apache%20License%202.0-blue?style=for-the-badge)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)

## Licensing

[Apache 2.0](https://github.com/mitre/FMACM/blob/master/LICENSE)

## Documentation

This codebase is _not_ an application; it is a library.
We use it to enable a larger simulation framework (e.g., the [MOPS Aircraft and Control Model](https://github.com/mitre/FMACM)).
The public-facing surface is large, providing interfaces (as C++ structs) and null implementations where reasonable.
Some classes also fully-functional algorithms (e.g. [ThreeDOFDynamics]()) and are tested.

TODO fill out a separate area of markdown for how to use this code. Make sure those are part of pages.
- Aircraft Performance needed...leave some implementation placeholder ideas/examples
- 

## Mathematical Model

📓 This software is built using [this mathematical model](./documentation/MTR260018_PRS_Derivation%20of%20Point%20Mass%20Model%20for%20ATM%20Research.pdf) which derives the kinematics, dynamics, and control for a point-mass aircraft operating in a wind field and moving along a WGS84 ellipsoid.

TODO Quickly define the key mathematical points.

TODO basic mermaid block diagram

---

Happy Simulating!
