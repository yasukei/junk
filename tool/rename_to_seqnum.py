import os
import shutil
from pathlib import Path

def main():
    # Variables
    dir_path = './'
    outputdir_path = 'output/'
    target_extension = '.png'
    index = 1

    # Get a list of files
    files_and_dirs = os.listdir(dir_path)
    files = [elem for elem in files_and_dirs if os.path.isfile(os.path.join(dir_path, elem))]
    files.sort()

    # Make output dir if not exist
    Path(outputdir_path).mkdir(parents=True, exist_ok=True)

    # Renaming
    src_files = list()
    dst_files = list()
    for file in files:
        name, extension = os.path.splitext(file)
        if extension != target_extension:
            continue

        src = file
        dst = f'{outputdir_path}/{index:03}{extension}'
        print(f'copying "{src}" to "{dst}"')
        shutil.copyfile(src, dst)
 
        index += 1
    print('Done.')

if __name__ == '__main__':
    main()

