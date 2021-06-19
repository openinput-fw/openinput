# SPDX-License-Identifier: MIT

import glob
import os
import os.path
import pickle
import subprocess

import nox


nox.options.sessions = ['test']
nox.options.reuse_existing_virtualenvs = True


def install_dependencies(session, dependencies):
    unmet = pickle.loads(
        subprocess.check_output([
            'python',
            os.path.join('tests', 'unmet-dependencies.py'),
            *dependencies,
        ], env=session.env, text=False)
    )

    if unmet:
        session.install(*unmet)
    else:
        print('no dependencies to install')


@nox.session()
def test(session):
    htmlcov_output = os.path.join(session.virtualenv.location, 'htmlcov')
    xmlcov_output = os.path.join(session.virtualenv.location, 'coverage.xml')

    if not os.path.isdir(htmlcov_output):
        os.mkdir(htmlcov_output)

    install_dependencies(session, {
        # build system
        'ninja_syntax',
        'toml',
        # tests
        'pytest',
        # coverage
        'gcovr',
    })

    # cleanup .gcda (GCC coverage) files
    for file in glob.glob(os.path.join('**', '*.gcda'), recursive=True):
        os.remove(file)

    # build testsuite
    session.run('python', 'configure.py', 'testsuite')
    session.run('ninja', external=True)

    # run tests
    session.run(
        'python', '-m', 'pytest',
        '--showlocals', '-ra', '--durations=10', '--durations-min=1.0',
        'tests/', *session.posargs,
    )

    # generate C coverage reports
    session.run(
        'gcovr', '-r', '.', '-b',
        '--xml', xmlcov_output,
        '--html-details', os.path.join(htmlcov_output, 'index.html'),
    )
