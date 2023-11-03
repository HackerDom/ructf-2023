data "digitalocean_ssh_key" "cerebralobserver" {
  name = "cerebralobserver"
}

data "digitalocean_ssh_key" "dvl" {
  name = "dvl"
}

data "digitalocean_ssh_key" "lololozhkin" {
  name = "lololozhkin"
}

data "digitalocean_ssh_key" "cs" {
  name = "cs"
}

data "digitalocean_ssh_key" "vaspahomov" {
  name = "vaspahomov"
}

locals {
  all_ssh_keys = [
    data.digitalocean_ssh_key.cs.id,
    data.digitalocean_ssh_key.cerebralobserver.id,
    data.digitalocean_ssh_key.dvl.id,
    data.digitalocean_ssh_key.lololozhkin.id,
    data.digitalocean_ssh_key.vaspahomov.id,
  ]
}