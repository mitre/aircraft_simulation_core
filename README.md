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
The public-facing surface of this codebase is large, providing interfaces (as C++ structs) and null implementations where reasonable.
Some classes are also fully-functional algorithms (e.g. [ThreeDOFDynamics]()) with [tests]().

If you need to do deeper, [read the docs](https://mitre.github.io/aircraft_simulation_core/).

## Mathematical Model

📓 This software is built using [this mathematical model](./documentation/MTR260018_PRS_Derivation%20of%20Point%20Mass%20Model%20for%20ATM%20Research.pdf) which derives the kinematics, dynamics, and control for a point-mass aircraft operating in a wind field and moving across a surface.
The mathematical model is a system of equations that connects the state variables of true airspeed, flight path angle, and heading angle.
Then, coordinate frame transformations are used to derive 3-dimensional motion on a plane and an ellipsoid, providing propogation of altitide and position.

🧮 If you need to do deeper into the maths, [read the paper](./documentation/MTR260018_PRS_Derivation%20of%20Point%20Mass%20Model%20for%20ATM%20Research.pdf).

👨‍💻 If you need the software implementation, [read the docs](./docs/README.md).

## Build

Configure and build the library with CMake:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run the public test suite:

```sh
cmake --build build --target run_simcore_tests --parallel
```

If you only need the library target and want to skip test dependencies:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSIMCORE_BUILD_TESTING=OFF
cmake --build build --target simcore --parallel
```

---

Happy Simulating!
