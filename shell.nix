{ pkgs ? import <nixpkgs> {} }:

let
in pkgs.mkShell {
  buildInputs = with pkgs; [
    clang-tools
    cmake
    gdb
    sdl3
  ];
}
