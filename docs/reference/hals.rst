**********************************
Hardware Abstraction Layers (HALs)
**********************************

There are several abstraction layers used to make some hardware feature(s)
available to the generic firmware code. They allow us to implement certain
functionality (eg. the protocol) without having to rewrite specific code for
each platform.

HALs are implemented as C structs and contain functions pointers for the task
implementation, some might also include data fields if supporting data is
needed. All function pointers receive the HAL itself as the first argument,
making all of the data and functionality provided by the HAL available to it.

All HALs have a void pointer ``drv_data`` field, which can be used by the
implementer to store internal data, which may be needed to perform tasks.


BLOCKDEV
********

The ``BLOCKDEV`` HAL provides the functionality required to interact with storage
devices with a block device topology.


.. literalinclude:: ../../src/hal/blockdev.h
   :language: c
   :lines: 10-


HID
***

The ``HID`` HAL provides the functionality required to interact with the host as
a HID device. See `USB HID`_ for information about the protocol.


.. literalinclude:: ../../src/hal/hid.h
   :language: c
   :lines: 10-


.. admonition:: Why is there no ``receive``?
   :class: hint

   The HID HAL only provides a ``send`` command because the receive the
   functionality should be handled by the target in the main loop, who should
   call ``protocol_dispatch`` on received data.


SPI
***

The ``SPI`` HAL provides the functionality required to interact with SPI
peripherals. See `SPI introduction`_ for information about the protocol.


.. literalinclude:: ../../src/hal/spi.h
   :language: c
   :lines: 10-


Ticks
*****

The ticks HAL provides the functionality to interact with the system clock.


.. literalinclude:: ../../src/hal/ticks.h
   :language: c
   :lines: 10-


.. _USB HID: https://www.usb.org/hid
.. _SPI introduction: https://www.corelis.com/education/tutorials/spi-tutorial/
