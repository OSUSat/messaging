{ pkgs ? import <nixpkgs> {} }:

let
  gccPkg = pkgs.gcc-unwrapped;
  gccVersion = gccPkg.version;
  gccStorePath = gccPkg;
  stdcxxIncludeDir = "${gccStorePath}/include/c++/${gccVersion}";
  stdcxxArchIncludeDir = "${stdcxxIncludeDir}/${pkgs.stdenv.hostPlatform.config}";
  clangBuiltins = "${pkgs.llvmPackages.clang-unwrapped.lib}/lib/clang/19/include";
in

pkgs.mkShell {
    name = "osusat-generator";

    buildInputs = with pkgs; [
        python313Packages.pyyaml
        python313Packages.crccheck
        pyright
        clang
        clang-tools
        llvmPackages_latest.lldb 
        llvmPackages_latest.libllvm
        gcc-unwrapped
    ];

    shellHook = ''
        # set CLANGD_PATH for editor tooling to use the Nix-wrapped clangd
        export CLANGD_PATH="${pkgs.clang-tools}/bin/clangd"

        CLANGD_CONFIG_FILE=".clangd"

        if [[ ! -d "${stdcxxIncludeDir}" ]]; then
            echo "❌ ERROR: GCC C++ headers not found at ${stdcxxIncludeDir}"
            find /nix/store -name "c++" -type d 2>/dev/null | head -5
        fi

        if [[ ! -d "${clangBuiltins}" ]]; then
            echo "❌ ERROR: Clang built-ins not found at ${clangBuiltins}"
            find /nix/store -path "*/clang/*/include" -type d 2>/dev/null | head -5
        fi

        if [[ -x "$CLANGD_PATH" ]]; then
            echo "✅ CLANGD found at: $CLANGD_PATH"
        else
            echo "❌ ERROR: CLANGD not found at: $CLANGD_PATH"
        fi

        echo "📁 Project root: $(pwd)"
        echo "📁 Include directory: $(pwd)/c/include"
        echo "📁 Checking if include dir exists: $(ls -la c/include/ 2>/dev/null || echo 'NOT FOUND')"

        cat > $CLANGD_CONFIG_FILE <<EOF
CompileFlags:
    Add:
    - "-I$(pwd)/c/include"
    - "-I$(pwd)/cpp/include"

Index:
    Background: Build
---
If:
    PathMatch: .*\.c$

CompileFlags:
    Add:
    - "-std=c11"
---
If:
    PathMatch: .*\.(cpp|cxx|cc|hpp|hxx)$

CompileFlags:
    Add:
    - "-std=c++14"
EOF

        echo "✅ Generated .clangd config:"
        cat $CLANGD_CONFIG_FILE
    '';
}
