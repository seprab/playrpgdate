import os
import argparse
import numpy as np
from PIL import Image, ImageOps

def convert_to_1bit(input_folder, output_folder):
    os.makedirs(output_folder, exist_ok=True)

    for filename in os.listdir(input_folder):
        if filename.endswith(('.png', '.jpg', '.jpeg')):
            # Open image
            img = Image.open(os.path.join(input_folder, filename))

            # Convert to grayscale
            gray_img = img.convert('L')

            # Apply Floyd-Steinberg dithering
            bw_img = gray_img.convert('1', dither=Image.FLOYDSTEINBERG)

            # Save 1-bit image
            bw_img.save(os.path.join(output_folder, filename))

def main():
    parser = argparse.ArgumentParser(description='Convert PNG images to grayscale')
    parser.add_argument('input', help='Input folder path')
    parser.add_argument('output', help='Output folder path')
    
    args = parser.parse_args()
    convert_to_1bit(args.input, args.output)

if __name__ == '__main__':
    main()


# python3 convert-to-grayscale.py /path/to/input/folder /path/to/output/folder