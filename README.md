## RuCTF 2023

RuCTF 2023 is an open annual online international challenge in the information security.

The contest is driven by classic rules (Attack-Defense CTF). Each team is given a set of vulnerable services. Organizers regulary fill services with flags — private information. The goal of each team is to find vulnerabilities, fix them in their services and exploit them in order to get flags from other teams.

RuCTF 2023 was held on November 4, 2023.

## Services

| Service | Language | Checker | Sploit | Writeup | Author | CI |
|---------|----------|---------|--------|---------|--------|----|
|[hyperborea-legends](/services/hyperborea-legends/)|C# + Python|[Checker](/checkers/hyperborea-legends/)|[Sploit](/sploits/hyperborea-legends/)|[Writeup](/writeups/hyperborea-legends/)|[@AlexMoshkov](https://github.com/AlexMoshkov)|![hyperborea-legends](https://github.com/HackerDom/ructf-2023/actions/workflows/check_hyperborea-legends.yml/badge.svg)|
|[memos](/services/memos/)<br>+ [internal](/internal/memos/)|C|[Checker](/checkers/memos/)|[Sploit](/sploits/memos/)|[Writeup](/writeups/memos/)|[@keltecc](https://github.com/keltecc)|![memos](https://github.com/HackerDom/ructf-2023/actions/workflows/check_memos.yml/badge.svg)|
|[perune](/services/perune/)<br> + [internal](/internal/perune/)|C++|[Checker](/checkers/perune/)|[Sploit](/sploits/perune/)|[Writeup](/writeups/perune/)|[@revervand](https://github.com/revervand)|![perune](https://github.com/HackerDom/ructf-2023/actions/workflows/check_perune.yml/badge.svg)|
|[rusi-trainer](/services/rusi-trainer/)|TypeScript|[Checker](/checkers/rusi-trainer/)|[Sploit](/sploits/rusi-trainer/)|[Writeup](/writeups/rusi-trainer/)|[@NitroLine](https://github.com/NitroLine)|![rusi-trainer](https://github.com/HackerDom/ructf-2023/actions/workflows/check_rusi-trainer.yml/badge.svg)|
|[rustest](/services/rustest/)|Rust|[Checker](/checkers/rustest/)|[Sploit](/sploits/rustest/)|[Writeup](/writeups/rustest/)|[@lololozhkin](https://github.com/lololozhkin)|![rustest](https://github.com/HackerDom/ructf-2023/actions/workflows/check_rustest.yml/badge.svg)|
|[werk](/services/werk/)<br> + [internal](/internal/werk/)|Go|[Checker](/checkers/werk/)| [Sploit](/sploits/werk/) |[Writeup](/writeups/werk/)|[@rkhapov](https://github.com/rkhapov)<br>[@werelaxe](https://github.com/werelaxe)|![werk](https://github.com/HackerDom/ructf-2023/actions/workflows/check_werk.yml/badge.svg)|

## Infrastructure

![image](https://github.com/HackerDom/ructf-2023/actions/workflows/rebuild_image.yml/badge.svg)

- Image master: [@danilkaz](https://github.com/danilkaz)
- Checksystem master: [@1MiKHalyCH1](https://github.com/1MiKHalyCH1)
- Cloud / network master: [@1MiKHalyCH1](https://github.com/1MiKHalyCH1)
- Technical support: [@vaspahomov](https://github.com/vaspahomov)

And our awesome team leader: [@capitanbanana](https://github.com/capitanbanana)

---

(C) HackerDom with <3

## Post-mortem

This year we encountered some problems before and during the competition.

Please, read [POSTMORTEM.md](/POSTMORTEM.md) for more information.
