import os
import shutil
from pathlib import Path

from PIL.PngImagePlugin import PngImageFile

def main():
    # Variables
    dir_path = './'
    target_extension = '.png'

    # Get a list of files
    files_and_dirs = os.listdir(dir_path)
    files = [elem for elem in files_and_dirs if os.path.isfile(os.path.join(dir_path, elem))]

    # Remove PngInfo
    for file in files:
        name, extension = os.path.splitext(file)
        if extension != target_extension:
            continue

        print(f'removing pnginfo of {file}')
        image = PngImageFile(file)
        image.text.clear()
        image.info.clear()
        image.save(file)
    print('Done.')

if __name__ == '__main__':
    main()

