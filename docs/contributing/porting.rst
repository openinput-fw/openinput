*******
Porting
*******


Porting to a new platform
-------------------------

To port the ``onpeninput`` firmware to a new platform, you will need to do the
following:

- Add a new platform (family) to the build system (``config/families``)
- Add the linker scripts to the build system (``config/linker``)
- Add platform specific code to ``src/platform`` (see Structure_ for more information)
   - Implement the HID HAL
   - Optionally implement the other relevant HALs


Adding a new target
-------------------

To port the ``onpeninput`` firmware to a new platform, you will need to do the
following:

- Add a new target to the build system (``config/targets``)
- Add or symlink a linker script for your target (``config/linker``)
- Add a the target code to ``src/targets`` (see Structure_ for more information)
   - Must include a ``main.c``, which will be the entry point of the firmware


.. todo::

   Add documentation on how to add new dependencies to the build system and link it here.


.. toctree::


.. _Structure: structure.html
