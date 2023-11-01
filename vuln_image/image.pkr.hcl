packer {
  required_plugins {
    digitalocean = {
      version = ">= 1.0.0"
      source  = "github.com/hashicorp/digitalocean"
    }
  }
}

variable "api_token" {
  type = string
}

source "digitalocean" "vuln_image" {
  droplet_name  = "ructf-2023-{{timestamp}}"
  snapshot_name = "ructf-2023-{{timestamp}}"
  api_token     = var.api_token
  image         = "ubuntu-20-04-x64"
  region        = "ams3"
  size          = "s-4vcpu-8gb"
  ssh_username  = "root"
}

build {
  sources = ["source.digitalocean.vuln_image"]

  provisioner "shell" {
    inline_shebang = "/bin/sh -ex"
    environment_vars = [
      "DEBIAN_FRONTEND=noninteractive",
    ]
    inline = [
      # Wait apt-get lock
      "while ps -opid= -C apt-get > /dev/null; do sleep 1; done",
      "apt-get clean",
      # apt-get update sometime may fail
      "for i in `seq 1 3`; do apt-get update && break; sleep 10; done",

      # Wait apt-get lock
      "while ps -opid= -C apt-get > /dev/null; do sleep 1; done",

      "apt-get dist-upgrade -y -o Dpkg::Options::='--force-confdef' -o Dpkg::Options::='--force-confold'",
      "for i in `seq 1 3`; do apt-get update && break; sleep 10; done",
      "apt-get upgrade -y -q -o Dpkg::Options::='--force-confdef' -o Dpkg::Options::='--force-confold'",

      # Install docker and docker-compose
      "apt-get install -y -q apt-transport-https ca-certificates nfs-common",
      "curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -",
      "add-apt-repository \"deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable\"",
      "for i in `seq 1 3`; do apt-get update && break; sleep 10; done",
      "apt-get install -y -q docker-ce",
      "curl -L \"https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)\" -o /usr/local/bin/docker-compose",
      "chmod +x /usr/local/bin/docker-compose",

      # Install haveged, otherwise docker-compose may hang: https://stackoverflow.com/a/68172225/1494610
      "apt-get install -y -q haveged",

      # Add users for services
      "useradd -m -s /bin/bash example",
      "useradd -m -s /bin/bash hyperborea-legends",
      "useradd -m -s /bin/bash rusi-trainer",
      "useradd -m -s /bin/bash rustest",
      "useradd -m -s /bin/bash sqlfs",
      "useradd -m -s /bin/bash werk",
    ]
  }

  ### Ructf motd
  provisioner "shell" {
    inline = [
      "rm -rf /etc/update-motd.d/*",
    ]
  }
  provisioner "file" {
    source = "motd/ructf-banner.txt"
    destination = "/ructf-banner.txt"
  }
  provisioner "file" {
    source = "motd/00-header"
    destination = "/etc/update-motd.d/00-header"
  }
  provisioner "file" {
    source = "motd/10-help-text"
    destination = "/etc/update-motd.d/10-help-text"
  }
  provisioner "shell" {
    inline = [
      "chmod +x /etc/update-motd.d/*",
    ]
  }

  ## Onboot docker-compose run service
  provisioner "file" {
    source = "service-boot/ructf-service@.service"
    destination = "/etc/systemd/system/ructf-service@.service"
  }

  # Copy services
  provisioner "file" {
    source = "../services/example/"
    destination = "/home/example/"
  }
  provisioner "file" {
    source = "../services/hyperborea-legends/"
    destination = "/home/hyperborea-legends/"
  }
  provisioner "file" {
    source = "../services/rusi-trainer/"
    destination = "/home/rusi-trainer/"
  }
  provisioner "file" {
    source = "../services/rustest/"
    destination = "/home/rustest/"
  }
  provisioner "file" {
    source = "../services/sqlfs/"
    destination = "/home/sqlfs/"
  }
  provisioner "file" {
    source = "../services/werk/"
    destination = "/home/werk/"
  }

  # Build and run services for the first time
  provisioner "shell" {
    inline = [
      "cd ~example",
      "docker-compose build || true",
      "cd ~hyperborea-legends",
      "docker-compose build || true",
      "cd ~rusi-trainer",
      "docker-compose build || true",
      "cd ~rustest",
      "docker-compose build || true",
      "cd ~sqlfs",
      "docker-compose build || true",
      "cd ~werk",
      "docker-compose build || true",

      "systemctl daemon-reload",

      "systemctl enable ructf-service@example",
      "systemctl enable ructf-service@hyperborea-legends",
      "systemctl enable ructf-service@rusi-trainer",
      "systemctl enable ructf-service@rustest",
      "systemctl enable ructf-service@sqlfs",
      "systemctl enable ructf-service@werk",
    ]
  }

  # Fix some internal digitalocean+cloud-init scripts to be compatible with our cloud infrastructure
  provisioner "shell" {
    script = "digital_ocean_specific_setup.sh"
  }

  # cleanup after multistage build
  provisioner "shell" {
    inline = [
      "docker image prune -f || true",
    ]
  }
}
