# truck-code

This repository provides a comprehensive set of software tools required to automate a platoon of heavy-duty vehicles.

This repository is designed to support [QNX 7.0](http://blackberry.qnx.com/en/sdp7) operating systems, and is written and compiled using the C++ programming language.


## More information

- [Installation](https://github.com/AboudyKreidieh/truck-code/wiki/Setup-Instructions)
- [Documentation](https://aboudykreidieh.github.io/truck-code/)
- [GitHub Wiki](https://github.com/AboudyKreidieh/truck-code/wiki)


## Credits

This respository is supported by [California PATH](https://path.berkeley.edu/home) at UC Berkeley and the [King Abdulaziz City for Science and Technology](https://www.kacst.edu.sa/eng/Pages/default.aspx). Contributors include: Abdul Rahman Kreidieh, John Spring, Fang-Chieh Chou, and Saleh Albeik.

Several scripts in this repository are adapted from code available [here](https://github.com/jspring/rfs-volvo_cacc). The files within this repository at are adapted, and the location of the code snippts they are adapted from, is depicted in the table below.

| File in truck-code                  | Files in rfs-volvo_cacc                                                            |
|-------------------------------------|------------------------------------------------------------------------------------|
| include/can/can_dev.cpp             | can/drv_common/[can_if.c, can_init.c], can/drv_sja1000/[can_dev.c, sja1000funcs.c] |
| include/can/can_if.cpp              | can/drv_common/can_if.c                                                            |
| include/can/can_init.cpp            | can/drv_common/can_init.c                                                          |
| include/can/can_man.h               | can/drv_common/can_init.c, can/include/can_defs.h, can/jbus/include/j1939.h        |
| include/can/can.cpp                 | can/jbus/src/[can_clt.c, j1939can.c]                                               |
| include/can/das_clt.h               |  path/local/das_clt.h                                                              |
| include/can/delay.h                 | can/drv_sja1000/linux/delay.h                                                      |
| include/can/io_func.cpp             | can/drv_common/[can_init.c, io_func.c]                                             |
| include/can/sja1000.cpp             | can/drv_sja1000/sja1000funcs.c                                                     |
| include/can/sja1000.h               | can/drv_sja1000/[defs.h, sja1000.h]                                                |
| include/jbus/j1939_interpreters.cpp | can/jbus/src/j1939pdu.c                                                            |
| include/jbus/j1939_struct.h         | can/jbus/include/[j1939.h, jbus.h]                                                 |
| include/jbus/j1939_utils.cpp        | can/jbus/include/j1939pdu.h, can/jbus/src/j1939scale.c                             |
| include/jbus/j1939_utils.h          | can/jbus/include/j1939.h                                                           |
| include/jbus/jbus.cpp               | can/jbus/src/j1939can.c                                                            |
| src/rd_j1939.cpp                    | can/jbus/src/rdj1939.c                                                             |
| src/can_man.cpp                     | can/drv_common/can_man.c                                                           |
