{ runCommandCC, runCommand }:
let
  bin = runCommandCC "brightnessd-bin" {} ''
    gcc -o $out ${./brightness.c}
  '';
  script = runCommand "brightnessd" {} ''
    mkdir -p $out/bin
    substitute ${./brightness.sh} $out/bin/brightness.sh --replace @brightnessd@ "${bin}"
    chmod +x $out/bin/brightness.sh
  '';
in script
