import os
import shutil
from pathlib import Path

from PIL.PngImagePlugin import PngImageFile

def main():
    # Variables
    src_dir = './'
    src_extension = '.png'
    dst_dir = 'output/'
    dst_extension = '.jpg'
    index = 1

    # Get a list of files
    files_and_dirs = os.listdir(src_dir)
    files = [elem for elem in files_and_dirs if os.path.isfile(os.path.join(src_dir, elem))]
    files.sort()

    # Make output dir if not exist
    Path(dst_dir).mkdir(parents=True, exist_ok=True)

    # Renaming
    for file in files:
        name, extension = os.path.splitext(file)
        if extension != src_extension:
            continue

        src = file
        dst = f'{dst_dir}/{index:03}{dst_extension}'
        
        print(f'copying "{src}" to "{dst}"')
        image = PngImageFile(src)
        image = image.convert('RGB')
        image.save(dst)
        #shutil.copyfile(src, dst)
 
        index += 1
    print('Done.')

if __name__ == '__main__':
    main()

