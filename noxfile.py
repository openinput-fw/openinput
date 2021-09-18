# SPDX-License-Identifier: MIT

import contextlib
import glob
import os
import os.path
import pathlib
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


@contextlib.contextmanager
def save_path(path):
    if os.path.exists(path):
        os.rename(path, f'{path}.bak')
        try:
            yield
        finally:
            os.rename(f'{path}.bak', path)
    else:
        yield


@contextlib.contextmanager
def cd(*path_parts):
    old_cwd = os.getcwd()
    os.chdir(os.path.join(*path_parts))
    try:
        yield
    finally:
        os.chdir(old_cwd)


@nox.session()
def test(session):
    htmlcov_output = os.path.join(session.virtualenv.location, 'htmlcov')
    htmlcov_index_output = os.path.join(htmlcov_output, 'index.html')
    xmlcov_output = os.path.join(session.virtualenv.location, 'coverage.xml')

    if not os.path.isdir(htmlcov_output):
        os.mkdir(htmlcov_output)

    install_dependencies(session, {
        # build system
        'ninja_syntax',
        'tomli',
        # tests
        'pytest',
        # coverage
        'gcovr',
    })

    # cleanup .gcda (GCC coverage) files
    for file in glob.glob(os.path.join('**', '*.gcda'), recursive=True):
        os.remove(file)

    # build testsuite
    with save_path('build.ninja'):
        session.run('python', 'configure.py', 'testsuite')
        session.run('ninja', external=True)
    out_path = os.path.abspath(os.path.join('build', 'testsuite', 'out'))

    # build and install python testsuite wrapper
    with cd('tests', 'wrapper'):
        session.run('python', 'setup.py', 'develop', env={
            'LDFLAGS': f'-L{out_path}',
        })

    try:
        # run tests
        session.run(
            'python', '-m', 'pytest',
            '--showlocals', '-ra', '--durations=10', '--durations-min=1.0',
            'tests/', *session.posargs,
            env={
                'LD_LIBRARY_PATH': out_path,
            }
        )
    finally:
        # generate C coverage reports
        session.run(
            'gcovr', '-r', '.', '-b',
            '--xml', xmlcov_output,
            '--html-details', htmlcov_index_output,
        )
        print(f'coverage report available at: file://{os.path.abspath(htmlcov_index_output)}')

        # print C coverage report
        session.run('gcovr', '-r', '.', '-b')


@nox.session()
@nox.parametrize('sanitizer', ('address', 'memory', 'undefined'))
def fuzz(session, sanitizer):
    install_dependencies(session, {
        # build system
        'ninja_syntax',
        'tomli',
    })

    # build testsuite
    with save_path('build.ninja'):
        session.run(
            'python', 'configure.py',
            '--compiler', 'clang',
            'fuzz',
            f'--engine=-fsanitize=fuzzer,{sanitizer}',
        )
        session.run('ninja', external=True)
    fuzz_exe = pathlib.Path('build', 'fuzz', 'out', 'fuzz').absolute()

    # run fuzzer
    session.run(
        os.fspath(fuzz_exe),
        '-seed=1', '-max_total_time=60',
        '-print_pcs=1', '-print_final_stats=1',
        external=True,
    )
