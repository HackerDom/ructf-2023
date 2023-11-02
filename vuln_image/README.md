# RuCTF vuln image

* [Add new service to vuln_image](#Add-new-service-to-vuln-image)
* [Add new service in CI](#Add-new-service-in-CI)
* [Build image with CI](#Build-image-with-CI)
* [Build image with packer](#Build-image-with-packer)

## Add new service to vuln image

- Add service to [list](https://github.com/HackerDom/ructf-2023/blob/3c03d9c57e90b20869a1416f1b40edecf534aaa3/vuln_image/generate_packer_config.py#L3)

## Add new service in CI

1. Commit service sources in master branch
1. Add service name in [generate_workflow.py](https://github.com/HackerDom/ructf-2023/blob/39cadaa2dd9e7803249c85a9df2f6be1303e0e3e/.github/workflows/generate_workflows.py#L4) to SERVICES list (and to CHECKERS and SPLOITS if you have)
1. Run `generate_workflow.py` script
    ```shell
    cd .github/workflows && ./generate_workflows.py && cd -
    ```
1. Commit changes

## Fail fast | best effort on service build

Use `docker-compose build` for fail fast mode and `docker-compose build || true` for best effort in [service build](https://github.com/HackerDom/ructf-2023/blob/3c03d9c57e90b20869a1416f1b40edecf534aaa3/vuln_image/generate_packer_config.py#L134).

## Build image with CI

Run `Rebuild image` [workflow](https://github.com/HackerDom/ructf-2023/actions/workflows/rebuild_image.yml)

## Build image with packer

1. Install packer 1.7.0 or higher: https://learn.hashicorp.com/tutorials/packer/get-started-install-cli#
1. Run `packer init image.pkr.hcl`
1. Get the API Token for Digital Ocean: https://cloud.digitalocean.com/settings/applications
1. Run
```shell
export DO_TOKEN=<YOUR_API_TOKEN>
packer build -var "api_token=$DO_TOKEN" image.pkr.hcl
```
