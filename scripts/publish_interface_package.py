#!/usr/bin/env python3

import argparse
import glob
import subprocess
from pathlib import Path

import call_wrapper

def generate(is_debug = False):
    cur_dir = Path(__file__).parent.absolute()
    root_dir = cur_dir.parent
    
    result_any_machine_dir = root_dir/'_result'/('AnyCPU_Debug' if is_debug else 'AnyCPU_Release')
    assert result_any_machine_dir.exists() and result_any_machine_dir.is_dir()

    package_files = glob.glob(str(result_any_machine_dir/'bin'/'dotnet_component_interface*.nupkg'))
    assert len(package_files) == 1

    package_path = package_files[0]

    subprocess.check_call(['dotnet', 'nuget',
                           'push',
                           str(package_path),
                           '--source', 'github'],
                           cwd=root_dir, shell=True)
    print(f'Package published')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Publish interface package to GitHub')
    parser.add_argument('--debug', default=False, action='store_true')

    args = parser.parse_args()

    call_wrapper.final_call_decorator(
        'Publishing interface package',
        'Publishing interface package: success',
        'Publishing interface package: failure!'
    )(
    generate
    )(
        args.debug
    )
