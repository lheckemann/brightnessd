{ pkgs, ... }:
let brightnessd = pkgs.callPackage ./brightnessd.nix {}; in {
  systemd.services.brightnessd = {
    wantedBy = ["multi-user.target"];
    serviceConfig.ExecStart = "${brightnessd}/bin/brightness.sh";
  };
}
