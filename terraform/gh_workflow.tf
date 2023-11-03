resource "digitalocean_droplet" "gh-workflow" {
  image    = "ubuntu-20-04-x64"
  name     = "gh-workflow"
  region   = var.region
  size     = "s-2vcpu-4gb"
  vpc_uuid = digitalocean_vpc.jury.id
  ssh_keys = local.all_ssh_keys
}