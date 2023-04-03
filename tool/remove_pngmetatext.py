import sys

from PIL.PngImagePlugin import PngImageFile

def main():
    if len(sys.argv) <= 1:
        print('Error: specify input_file')
        return

    file_name = sys.argv[1]
    image = PngImageFile(file_name)
    image.text.clear()
    image.save(file_name)

if __name__ == '__main__':
    main()

