import string
import secrets

def write_alphanumeric_file(filename, length):
    """
    Generates a string of a specified length containing random
    alphanumeric characters and writes it to a file.

    Args:
        filename (str): The name of the file to write to.
        length (int): The number of characters to generate.
    """
    # Define the possible characters (digits, lowercase, uppercase)
    characters = string.ascii_letters + string.digits

    # Generate the random alphanumeric string
    # secrets.choice is used for cryptographically strong randomness
    random_string = ''.join(secrets.choice(characters) for _ in range(length))

    # Write the string to the file
    try:
        with open(filename, 'w') as f:
            f.write(random_string)
        print(f"Successfully wrote {length} characters to {filename}")
    except IOError as e:
        print(f"Error writing to file: {e}")

# Example usage:
file_name = "testfiles/extra_large_file2.txt"
num_characters = 100000000
write_alphanumeric_file(file_name, num_characters)