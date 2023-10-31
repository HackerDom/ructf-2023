resource "digitalocean_droplet" "gh-workflow" {
  image              = "ubuntu-20-04-x64"
  name               = "gh-workflow"
  region             = var.region
  size               = "s-2vcpu-4gb"
  private_networking = true
  vpc_uuid           = digitalocean_vpc.jury.id
  ssh_keys = [
    data.digitalocean_ssh_key.cs.id,
    data.digitalocean_ssh_key.cerebralobserver.id,
    data.digitalocean_ssh_key.dvl.id,
    data.digitalocean_ssh_key.lololozhkin.id,
  ]
}