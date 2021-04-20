:hide-toc:

*********
openinput
*********

*openinput* is a project that provides firmware for input devices like mice,
keyboards, etc.
The main focus of the project is to provide a good end-user experience, from
configuration drivers to firmware updates to overall reliability.

We hope to enable the community share and develop their creativity by allowing
them to build and tinker with devices without having to worry about troublesome
technical issues like software support, etc. *openinput* provides the underlying
framework required to make all this possible.


State of the project
====================

Currently, we have a working target implementation using the Linux UHID API,
which allows you to have full software testing of the protocol using real
drivers.
We are working on the STM32F1 target, which should provide official firmware
support for the Steelseries Rival 310.

The protocol is also a work in progress. The base format and basic function
discovery functions are defined, implemented in the firmware and implemented in
a `libratbag`_ driver.


.. admonition:: Roadmap

   - Linux UHID target |:+1:|
   - Steelseries Rival 310 target |:hammer:|
   - Custom hardware target (supporting USB full speed - 8kHz report rate)
   - Custom wireless target (device + receiver)

   - libratbag_ driver (allows use in piper_) |:+1:|
   - fwupd_ plugin (firmware updates)

   - Linux driver for native functions (battery reporting, etc.)
   - Linux driver for multiplexing devices (multiple devices connected to a single receiver)

   ========== ================
   |:+1:|     Done!
   |:hammer:| Work in progress
   ========== ================


Getting started
===============

Building the firmware
---------------------

To build the *openinput* firmware you will first need to install the
dependencies required to run the build system.

- ninja_syntax_ (``pip install ninja_syntax``)
- ninja_ (``pacman -S ninja``)
- pkgconf_ (``pacman -S pkgconf``) (for native targets)

You will also need a copy of the GCC compiler, binutils and a standard C library
for your target architecture.

- Native (``pacman -S gcc binutils glibc``)
   - Linux UHID (``linux-uhid``)
- ``arm-none-eabi`` (``pacman -S arm-none-eabi-gcc arm-none-eabi-bintuils arm-none-eabi-newlib``)
   - STM32F1 (``stm32f1-generic``)
      - Bluepill
      - Steelseries Rival 310


After having all requirements met we can build! First we will run
``configure.py`` to generate a ``build.ninja`` for our target and then we will
invoke ``ninja`` to actually do the building.

``configure.py`` will clone the necessary submodules by default, you can pass the
``--no-fetch``/``-n`` option to disable this behavior.


.. code-block:: console

   $ ./configure.py linux-uhid
   $ ninja


After this, you should have you shiny new object in ``build/out``.


.. toctree::
   :hidden:

   Source Code <https://github.com/openinput-fw/openinput/>
   Issue Tracker <https://github.com/openinput-fw/openinput/issues>
   Protocol Definition <https://github.com/openinput-fw/openinput-protocol>


.. _libratbag: https://github.com/libratbag/libratbag
.. _piper: https://github.com/libratbag/piper
.. _fwupd: https://github.com/fwupd/fwupd

.. _ninja_syntax: https://pypi.org/project/ninja_syntax
.. _ninja: https://github.com/ninja-build/ninja
.. _pkgconf: https://github.com/pkgconf/pkgconf
