terraform {
  required_providers {
    digitalocean = {
      source  = "digitalocean/digitalocean"
      version = "1.22.2"
    }
  }

  backend "s3" {
    endpoints = {
      s3 = "https://ams3.digitaloceanspaces.com"
    }
    key                         = "terraform.tfstate"
    bucket                      = "ructf2023-tf-storage"
    region                      = "us-west-1"
    skip_requesting_account_id  = true
    skip_credentials_validation = true
    skip_metadata_api_check     = true
  }

}

provider "digitalocean" {
  token = var.do_token
}
