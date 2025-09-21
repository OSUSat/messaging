# messaging
Messaging standards and protocols for OSUSat/SCRT

## Messaging Spec

- The messaging specification can be found in spec/SPEC.md
- The current revision of the messaging specification implementation is located at spec/messages.yml

## Code Generation

- A simple code generation pipeline is found in generator/gen.py
- It outputs C, C++, and Python headers/codecs for messages and encoding/decoding functionality

## Project Integration

### C/CPP integration

- Add this repository as a subdirectory of your project
    - git submodule add https://github.com/osusat/messaging.git lib/
- Use one of the example CMakeLists.txt files to link the C/CPP libraries
    - You may need to modify the subdirectory that CMake targets for the library files. Change `add_subdirectory(../../ ...` -> `add_subdirectory(lib/messaging ...`, for example.
        - Make sure it targets the submodule you added

### Python Integration

- Install the package via PyPi: `pip install osusat-messaging`

## Examples

- Example codec/packet usage can be found under the `examples` directory

## Developing

### Development with UNIX/Windows systems

```
# Generator Development

cd generator
python -m venv .venv/ # create a python virtual environment for package isolation
source .venv/bin/activate # activate the virtual environment you just created
pip install -r requirements.txt # install generator dependencies

# C/CPP Codec Development
mkdir build && cd build
cmake ..
make
```

### Development with NixOS systems

```
nix-shell # good to go :)
```
