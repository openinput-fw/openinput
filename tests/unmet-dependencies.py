# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>

import importlib.metadata
import pickle
import sys


unmet = set()

for dep in sys.argv[1:]:
    try:
        importlib.metadata.distribution(dep)
    except importlib.metadata.PackageNotFoundError:
        unmet.add(dep)

sys.stdout.buffer.write(pickle.dumps(unmet))
