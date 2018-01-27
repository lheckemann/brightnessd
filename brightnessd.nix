{ runCommandCC, runCommand, coreutils }:
let
  bin = runCommandCC "brightnessd-bin" {} ''
    gcc -o $out ${./brightness.c}
  '';
  script = runCommand "brightnessd" {} ''
    mkdir -p $out/bin
    substitute ${./brightness.sh} $out/bin/brightness.sh --replace @brightnessd@ "${bin}" --replace @coreutils@ "${coreutils}"
    chmod +x $out/bin/brightness.sh
  '';
in script
