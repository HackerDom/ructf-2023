resource "digitalocean_droplet" "cloud" {
  image    = "ubuntu-20-04-x64"
  name     = "cloud"
  region   = var.region
  size     = "g-2vcpu-8gb"
  ssh_keys = local.all_ssh_keys
  connection {
    host        = self.ipv4_address
    user        = "root"
    type        = "ssh"
    private_key = file(var.pvt_key)
    timeout     = "2m"
  }
  provisioner "remote-exec" {
    inline = [
      "echo ${var.do_token} > /root/do-token",
    ]
  }
}
