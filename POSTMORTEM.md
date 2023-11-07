# RuCTF 2023 | Post-mortem

Although the competition went smoothly enough, there were some problems before and during the competition.

## Tokens issues

This year we had no resources to create a fully-functional website for the competition. So we decided to use Google Docs for the registration.

Before the game we need to send cloud tokens by email, and do it by hands. So we write a simple Python script which sends emails using Gmail. But we did some mistakes and send invalid tokens twice.

1. Gmail throttles consecutive sends, but accepts emails by batches. We ran our script some times, but forgot to update mapping (team to email). It lead to problem: first N participants got first N tokens, next N participants got the same N tokens and so on.

2. During the second attempt we tried to fix this, but failed. We added updates to mapping (team to email), but did it wrong. So the teams got invalid tokens again.

3. The third attempt was success, we send correct tokens to teams.

When we realized that tokens are invalid, we closed our VMs cloud and restarted it. It took some time, so we decided to publish services in Discord: participants could start to solve.

Timeline:

```
10:00 UTC - cloud open
10:40 UTC - we realized the mistake, cloud closed
10:44 UTC - services was published to Discord
11:15 UTC - we started to resend correct tokens
11:30 UTC - cloud open
12:00 UTC - network open
```

The conclusion is simple: prepare such critical scripts earlier and review it harder.

## Checkers issues

We started the game with 3 machines for checkers, each have 16 CPU cores (3x16). Shortly after the network was opened there turned out a problem: the checkers was stopped to run. Checksystem had utilize all available cores, and CPU resources was exhausted.

The first assumption regarding the root cause was that one of the checkers was consuming excessive resources. We disabled this checker, but it did not resolve the issue. We also suspected that the checking queue on the checksystem's master was full of jobs, so we attempted to restart the workers, but this action also had not fixed the issue. Then we attempted to upscale the checker's workers' pool size to 5, but it took a while. Simultaneously, we increased the size of the initial workers to 48 cores, the maximum available on DigitalOcean, which finaly helped and services goes "green" on scoreboard. However, immediately after this fix, the checksystem manager (main coordinator) went down due to a misconfiguration issue during the setup of the new workers in ansible. Fix missconfiguration had been fixed asap as well as 2 new workes was added into the pool.

We could restart the entire game, but we did not want to do this, because the game was already started and some teams already got flag points for attacks. Now we think that it was not a good solution: restarting the entire game was better, because it also would refresh SLA (decreased by not-working checkers).

Timeline:

```
12:00 UTC - network open
12:11 UTC - we realized that checkers is not working
12:20 UTC - we understood that the problem is caused by exhausted CPU
12:41 UTC - we upscaled checkers machines
12:48 UTC - checkers was ressurected
```

Next time we will use more powerful machines for checkers and stress test the game with more load.

## VPN issues

We used [DigitalOcean](https://www.digitalocean.com/) cloud provider to build our game infrastructure.

Since Russian government bans a lot of foreign IPs, some participants from Russia could not connect to their VPN, and therefore have no access to VMs.

During the game we did not manage to find out the solution, but some people setup their own VPNs, and connected to game network through their own tunnels.

We don't know the good solution here, since many countries have a practice to ban external IPs. Next time we will try to use a different provider.

## Service `werk` issues

First we planned to release a service called `werk`, it should have big codebase and implement complex logic. Unfortunately, we was not on time and did not success to finish it before the competition.

In 12 hours before the game there are only 5 services, and we thought it will not be enough, it seemed to us that 5 services will be solved quite fast (later it turned out that only 3/5 services was solved). So we made a decision to write an entirely new service, which would be simple, in order to reach 6 services.

This was a bad decision, because the new service `werk` was not well-tested, and the intended solution required too much bruteforce. Luckily, the service was solvable, and there was another solution, found by participants (see [writeup](/writeups/werk/)).

Now we understand that release less services is better than release unfinished service.
