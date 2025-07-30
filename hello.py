#!/usr/bin/env python3

def greet(name="World"):
    """Return a greeting message."""
    return f"Hello, {name}!"

def main():
    """Main function to demonstrate greeting."""
    print(greet())
    print(greet("GitHub"))
    print(greet("Linear"))

if __name__ == "__main__":
    main()