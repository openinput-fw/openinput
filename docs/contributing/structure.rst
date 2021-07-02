*********
Structure
*********

There are two hardware specific components in this firmware, platforms and
targets, and a couple generic components.


.. code::

   src
   ├── driver
   ├── hal
   ├── platform
   ├── protocol
   ├── targets
   └── util


``platform``
------------

Platforms contain all the architecture specific helper code required to write a
target.

``platform`` contains a subdirectory for each platform. The platform HAL
implementations should be under a subdirectory named ``hal`` in the platform
directory (eg. ``src/platforms/stm32f1/hal``).


``target``
------------

Targets are the entry point of the firmware, they contain the ``main.c`` and
define all the low level task-specific logic.

``target`` contains a subdirectory for each target. The subdirectory must
include a ``main.c`` file, which will be the entry point of the firmware.


``protocol``
------------

``protocol`` contains the `openinput protocol`_ implementation.


``hal``
------------

``hal`` contains `Hardware Abstraction Layers (HALs)`_ definition.


``util``
--------

``util`` contains generic utility code.


.. toctree::


.. _openinput protocol: https://openinput.readthedocs.io/projects/protocol
.. _Hardware Abstraction Layers (HALs): ../reference/hals.html
