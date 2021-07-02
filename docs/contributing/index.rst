************
Contributing
************


Contributing can be highly rewarding, but it can also be frustrating at times.
It takes time to review patches, and as this is an open source project, that
sometimes can take a while. The reviewing process depends on the availability
of the maintainers, who may not be always available. Please try to be
understanding throught the process.

There a few guidelines you need to keep in mind when contributing. Please have
a look at them as that will make the contribution process easier for all
parties.


Commits
-------

Git history is very important to the *openinput* project. The git history is not
only a code versioning tool, but also documentation. It should describe how the
code changed and why. Someone should be able to relatively easily undertand the
code progress by reading the git history.

For this reason, we will not accept big "code-dump" patches. Every commit should
be an unique logical change, and should be acompanied with an appropriate
description.


.. admonition:: Hint
   :class: hint

   Look at the current git history to have a better idea of what is supposed to
   look like!


Format
~~~~~~

We adopt the following commit format:

.. code:: none

   component: description

``component`` is the code component (``docs``, ``readme``, ``platform/stm32f1``,
``target/stm32f1-generic``, etc.). If your commit touches several components,
use the one the original logical change belongs to.

``description`` is a descriptive summary of the change, why it is needed, and
any non-obvious missing context.


.. admonition:: Hint
   :class: hint

   If you are not sure what to use as the ``component`` prefix, look at the git
   history for the file in question.


If the commit is not trivial, or involves some context, you **should** write a
summary in the commit body.

See the following `example <https://github.com/openinput-fw/openinput/commit/b46ddd1e99fb74ccfbee195c4d7fa8af40d11e2f>`_:


.. code:: none

   hal: pass hid_interface_t as void* to hid_send

      hid_interface_t may be an incomplete type and the compiler does not seem
      happy about that.

      Signed-off-by: Filipe Laíns <lains@riseup.net>


.. admonition:: Hint
   :class: hint

   When unsure how to structure your commit or what description to use, please
   ask a maintainer.


Workflow
~~~~~~~~

While working on *openinput*, we recommend using the following workflow.


Creating a commit
*****************

After writing your code, you should you stage the relevant changes using:


.. code:: shell

   git add -p


This will allow you to select which changes you actually want to include in the
commit. You will be prompt with code hunk and a menu to select what you want to
do. There are several options:


.. code:: none

   y - stage this hunk
   n - do not stage this hunk
   q - quit; do not stage this hunk or any of the remaining ones
   a - stage this hunk and all later hunks in the file
   d - do not stage this hunk or any of the later hunks in the file
   s - split the current hunk into smaller hunks
   e - manually edit the current hunk
   ? - print help



After selecting all relevent bits of the code, you should commit.


.. code:: shell

   git commit


Fixing a commit
***************

If you made any mistake in a commit, we recommend using `fixup commits`_. It
goes as follows. You make the changes you want to add to the commit and then
stage them.


.. code:: shell

   git add -p


After this, instead of commiting this change as a separate commit, we commit it
as a fixup commit for the commit you want to fix (in the example, ``HEAD~1``,
it can be replaced by a commit hash or other kind of commit reference).


.. code:: shell

   git commit --fixup HEAD~1


After this we can squash the fixup commit with the original commit by rebasing.


.. code:: shell

   git rebase --autosquash --interactive origin/main


.. admonition:: Hint
   :class: hint

   If you are new to rebases, we recommend you to have a look at the `Rebasing
   page`_ of the git documentation.


.. _fixup commits: https://git-scm.com/docs/git-commit#Documentation/git-commit.txt---fixupamendrewordltcommitgt
.. _Rebasing page: https://git-scm.com/book/en/v2/Git-Branching-Rebasing
