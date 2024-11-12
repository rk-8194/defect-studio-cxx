import yaml
import argparse
import os

def convert_to_xyz(yaml_file, xyz_file):
    with open(yaml_file, 'r') as file:
        data = yaml.safe_load(file)

    configurations = data.get('configurations', {})
    structure = data.get('structure', {})
    coords = structure.get('coords', [])
    lattice = structure.get('lattice', [])
    
    # Ensure we have the required data
    if not isinstance(configurations, dict) or not isinstance(coords, list) or not isinstance(lattice, list):
        print("Error: Invalid data structure in YAML file.")
        return
    
    # Prepare species list from the first configuration
    first_key = next(iter(configurations))
    species_list = configurations[first_key]
    
    # Check that species is a list
    if not isinstance(species_list, list):
        print(f"Error: Configuration for {first_key} is not a list.")
        return
    
    # Open the output XYZ file for writing
    with open(xyz_file, 'w') as xyz:
        # Write the number of atoms
        xyz.write(f"{len(species_list)}\n")  # Number of atoms
        xyz.write("Generated from YAML file\n")  # Comment line

        # Write the atomic coordinates in Cartesian format
        for i, coord in enumerate(coords):
            # Convert fractional coordinates to Cartesian
            cartesian_coord = [
                coord[0] * lattice[0][0] + coord[1] * lattice[1][0] + coord[2] * lattice[2][0],
                coord[0] * lattice[0][1] + coord[1] * lattice[1][1] + coord[2] * lattice[2][1],
                coord[0] * lattice[0][2] + coord[1] * lattice[1][2] + coord[2] * lattice[2][2],
            ]
            species = species_list[i]
            xyz.write(f"{species} {' '.join(map(str, cartesian_coord))}\n")

    print(f"Successfully wrote XYZ to {xyz_file}.")

def main():
    parser = argparse.ArgumentParser(description='Convert YAML file to XYZ format with Cartesian coordinates.')
    parser.add_argument('yaml_file', type=str, help='Path to the input YAML file')
    parser.add_argument('xyz_file', type=str, help='Path to the output XYZ file')

    args = parser.parse_args()

    if not os.path.isfile(args.yaml_file):
        print(f"Error: The file {args.yaml_file} does not exist.")
        return

    convert_to_xyz(args.yaml_file, args.xyz_file)

if __name__ == "__main__":
    main()
