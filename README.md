# Henhouse 5.0

## Development Environment
```
$ 
$ docker build -t zephyr-stm32 --build-arg TARGETARCH=amd64 .
$ docker run --name zephyr-stm32 --rm -it -p 3333:3333 -p 2222:22 -p 8800:8800 -v "$(pwd)"/workspace:/workspace -w /workspace zephyr-stm32

```

screen /dev/ttyACM0 115200

## Projects
$ west build -b nucleo_c071rb samples/basic/blinky_pwm
$ west build -b nucleo_wl55jc samples/basic/blinky
$ west build -b hs5_c071rb .
$ west build -b hs5_wl55jc .

## Styleguide
Class naming: CamelCase (SensorManager)
Functions: pascalCase (deciCelsiusToCelsius)
Variables: pascalCase (sensorSelector)
Members: m_ prefix


## Docker setup rootless

cat <<EOT | sudo tee "/etc/apparmor.d/usr.local.bin.rootlesskit"
abi <abi/4.0>,
include <tunables/global>

/home/roman/bin/rootlesskit flags=(unconfined) {
  userns,

  # Site-specific additions and overrides. See local/README for details.
  include if exists <local/usr.local.bin.rootlesskit>
}
EOT
sudo systemctl restart apparmor.service
