#!/usr/bin/env python3
"""
SVG to PNG Converter

This script converts all .svg icon files in a specified directory
to PNG format with configurable sizes.

Requirements:
    cairosvg>=2.5.0
    Pillow>=9.0.0

Usage:
    python svg_to_png.py [--input <icons_dir>] [--output <output_dir>] [--size <size>]

Example:
    python svg_to_png.py --input ../icons --output ../icons_png --size 64
"""

import os
import argparse
from pathlib import Path

try:
    import cairosvg
except ImportError:
    print("Error: cairosvg is required. Install with: pip install cairosvg")
    exit(1)

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow is required. Install with: pip install Pillow")
    exit(1)


def convert_svg_to_png(svg_path: Path, png_path: Path, size: int):
    """
    Convert a single SVG file to PNG.
    
    Args:
        svg_path: Path to input SVG file
        png_path: Path to output PNG file
        size: Output size (width and height in pixels)
    """
    # CairoSVG outputs at the SVG's native size, so we need to:
    # 1. Convert to PNG bytes
    # 2. Resize using Pillow
    
    # Read SVG and convert to PNG bytes
    png_data = cairosvg.svg2png(
        url=str(svg_path),
        output_width=size,
        output_height=size
    )
    
    # Write directly to file
    with open(png_path, 'wb') as f:
        f.write(png_data)


def convert_all_svgs(input_dir: Path, output_dir: Path, size: int):
    """
    Convert all SVG files in a directory to PNG.
    
    Args:
        input_dir: Directory containing SVG files
        output_dir: Directory to save PNG files
        size: Output size (width and height in pixels)
    """
    svg_files = sorted(input_dir.glob("*.svg"))
    
    if not svg_files:
        print(f"No SVG files found in {input_dir}")
        return 0
    
    # Create output directory
    output_dir.mkdir(parents=True, exist_ok=True)
    
    converted = 0
    for svg_file in svg_files:
        png_file = output_dir / f"{svg_file.stem}.png"
        
        try:
            convert_svg_to_png(svg_file, png_file, size)
            print(f"Converted: {svg_file.name} -> {png_file.name} ({size}x{size})")
            converted += 1
        except Exception as e:
            print(f"Error converting {svg_file.name}: {e}")
    
    return converted


def main():
    parser = argparse.ArgumentParser(
        description="Convert SVG icons to PNG format"
    )
    parser.add_argument(
        '--input', '-i',
        type=str,
        default='../icons',
        help='Input directory containing SVG files (default: ../icons)'
    )
    parser.add_argument(
        '--output', '-o',
        type=str,
        default='../icons_png',
        help='Output directory for PNG files (default: ../icons_png)'
    )
    parser.add_argument(
        '--size', '-s',
        type=int,
        default=64,
        help='Output icon size in pixels (default: 64)'
    )
    parser.add_argument(
        '--sizes',
        type=str,
        default=None,
        help='Comma-separated list of sizes to generate (e.g., "16,24,32,64")'
    )
    
    args = parser.parse_args()
    
    input_dir = Path(args.input).resolve()
    output_dir = Path(args.output).resolve()
    
    if not input_dir.exists():
        print(f"Error: Input directory '{input_dir}' does not exist")
        return 1
    
    if args.sizes:
        # Generate multiple sizes
        sizes = [int(s.strip()) for s in args.sizes.split(',')]
        total_converted = 0
        
        for size in sizes:
            size_output_dir = output_dir / f"{size}x{size}"
            print(f"\nGenerating {size}x{size} icons...")
            total_converted += convert_all_svgs(input_dir, size_output_dir, size)
        
        print(f"\nTotal icons converted: {total_converted}")
    else:
        # Single size
        converted = convert_all_svgs(input_dir, output_dir, args.size)
        print(f"\nTotal icons converted: {converted}")
    
    return 0


if __name__ == "__main__":
    exit(main())
