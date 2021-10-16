#!/usr/bin/env python3

import argparse
import zipfile
from pathlib import Path
from zipfile import ZipFile

import call_wrapper

def path_basename_tuple(path):
    return (path, path.name)

def zipdir(zip_file, path, arc_path=None):
    assert(path.exists() and path.is_dir())

    for file in path.rglob("*"):
        if (file.name.startswith(".")):
            # skip `hidden` files
            continue
        if (arc_path):
            file_arc_path = f"{arc_path}/{file.relative_to(path)}"
        else:
            file_arc_path = file.relative_to(path)
        zip_file.write(file, file_arc_path)

def pack(is_debug = False):
    cur_dir = Path(__file__).parent.absolute()
    root_dir = cur_dir.parent
    result_machine_dir = root_dir/"_result"/("x86_Debug" if is_debug else "x86_Release")
    assert(result_machine_dir.exists() and result_machine_dir.is_dir())

    output_dir = result_machine_dir
    output_dir.mkdir(parents=True, exist_ok=True)

    component_zip = output_dir/"foo_dotnet_component_host.fb2k-component"
    component_zip.unlink(missing_ok=True)

    with ZipFile(component_zip, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        zipdir(z, root_dir/"licenses", "licenses")
        z.write(*path_basename_tuple(root_dir/"LICENSE"))
        z.write(*path_basename_tuple(root_dir/"CHANGELOG.md"))
        
        files_to_pack = [
            'Ijwhost.dll',
            'dotnet_component_interface.deps.json',
            'dotnet_component_interface.dll',
            'foo_dotnet_component_host.deps.json',
            'foo_dotnet_component_host.dll',
            'foo_dotnet_component_host.runtimeconfig.json'
        ]
        
        for f in files_to_pack:
            z.write(*path_basename_tuple(result_machine_dir/'bin'/f))

    print(f"Generated file: {component_zip}")

    if (not is_debug):
        # Release pdbs are packed in a separate package
        pdb_zip = output_dir/"foo_dotnet_component_host_pdb.zip"
        if (pdb_zip.exists()):
            pdb_zip.unlink()

        with ZipFile(pdb_zip, "w", zipfile.ZIP_DEFLATED) as z:
            z.write(*path_basename_tuple(result_machine_dir/"bin"/"dotnet_component_interface.pdb"))
            z.write(*path_basename_tuple(result_machine_dir/"dbginfo"/"foo_dotnet_component_host.pdb"))

        print(f"Generated file: {pdb_zip}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Pack component to .fb2k-component')
    parser.add_argument('--debug', default=False, action='store_true')

    args = parser.parse_args()

    call_wrapper.final_call_decorator(
        "Packing component",
        "Packing component: success",
        "Packing component: failure!"
    )(
    pack
    )(
        args.debug
    )
