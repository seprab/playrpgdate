import os
import argparse
from PIL import Image

def convert_to_grayscale(input_folder, output_folder):
    os.makedirs(output_folder, exist_ok=True)
    
    for filename in os.listdir(input_folder):
        if filename.endswith('.png'):
            img = Image.open(os.path.join(input_folder, filename))
            grayscale_img = img.convert('L')
            grayscale_img.save(os.path.join(output_folder, filename))

def main():
    parser = argparse.ArgumentParser(description='Convert PNG images to grayscale')
    parser.add_argument('input', help='Input folder path')
    parser.add_argument('output', help='Output folder path')
    
    args = parser.parse_args()
    convert_to_grayscale(args.input, args.output)

if __name__ == '__main__':
    main()


# python3 convert-to-grayscale.py /path/to/input/folder /path/to/output/folder